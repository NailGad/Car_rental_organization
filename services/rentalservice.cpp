#include "rentalservice.h"
#include "../database/databasemanager.h"
#include "../patterns/carstatusobserver.h"
#include "../utils/dateutils.h"
#include <QDebug>

RentalService::RentalService()
    : m_dbManager(nullptr), m_statusSubject(nullptr), m_statusObserver(nullptr)
{
    m_dbManager = &DatabaseManager::getInstance();
    
    // Настраиваем Observer для обновления статуса автомобилей
    m_statusSubject = new CarStatusSubject();
    m_statusObserver = new CarStatusObserver();
    m_statusSubject->attachObserver(m_statusObserver);
}

bool RentalService::createRental(int carId, int userId, const QDate& startDate, const QDate& endDate, double totalCost)
{
    if (!m_dbManager) {
        qDebug() << "DatabaseManager не инициализирован!";
        return false;
    }
    
    // Проверяем доступность автомобиля
    if (!isCarAvailable(carId, startDate, endDate)) {
        qDebug() << "Автомобиль недоступен в указанный период!";
        return false;
    }
    
    // Создаем аренду
    Rental rental(0, carId, userId, startDate, endDate, totalCost, false);
    
    if (m_dbManager->addRental(rental)) {
        // Обновляем статус автомобиля через Observer
        updateCarStatusOnRentalStart(carId);
        qDebug() << "Аренда успешно создана!";
        return true;
    }
    
    return false;
}

bool RentalService::completeRental(int rentalId, const QDate& actualReturnDate)
{
    if (!m_dbManager) {
        return false;
    }
    
    Rental rental = m_dbManager->getRentalById(rentalId);
    if (rental.getId() == 0) {
        return false;
    }

    // Пересчитываем стоимость аренды по фактическому количеству дней
    Car car = m_dbManager->getCarById(rental.getCarId());
    if (car.getId() != 0) {
        int days = rental.getStartDate().daysTo(actualReturnDate) + 1;
        if (days < 1) {
            days = 1;
        }
        double newTotalCost = car.getDailyPrice() * days;
        rental.setTotalCost(newTotalCost);
    }

    rental.setActualReturnDate(actualReturnDate);
    rental.setCompleted(true);
    
    if (m_dbManager->updateRental(rental)) {
        // Обновляем статус автомобиля через Observer
        updateCarStatusOnRentalComplete(rental.getCarId());
        qDebug() << "Аренда завершена!";
        return true;
    }
    
    return false;
}

RentalService::CompleteRentalResult RentalService::completeRentalWithFine(int rentalId, const QDate& actualReturnDate, double fineMultiplier)
{
    CompleteRentalResult result;
    result.success = false;
    result.wasOverdue = false;
    result.fineAmount = 0.0;
    result.overdueDays = 0;
    
    if (!m_dbManager) {
        return result;
    }
    
    Rental rental = m_dbManager->getRentalById(rentalId);
    if (rental.getId() == 0) {
        return result;
    }
    
    // Проверяем просрочку
    QDate plannedReturnDate = rental.getEndDate();
    bool isOverdue = actualReturnDate > plannedReturnDate;
    
    if (isOverdue) {
        result.wasOverdue = true;
        result.overdueDays = plannedReturnDate.daysTo(actualReturnDate);
        
        // Рассчитываем и начисляем штраф
        Fine fine = calculateFine(rentalId, actualReturnDate, fineMultiplier);
        result.fineAmount = fine.getAmount();
        
        if (fine.getAmount() > 0) {
            applyFine(fine);
        }
    }
    
    // Завершаем аренду
    result.success = completeRental(rentalId, actualReturnDate);
    
    return result;
}

Fine RentalService::calculateFine(int rentalId, const QDate& actualReturnDate, double fineMultiplier)
{
    if (!m_dbManager) {
        return Fine();
    }
    
    Rental rental = m_dbManager->getRentalById(rentalId);
    if (rental.getId() == 0) {
        return Fine();
    }
    
    QDate plannedReturnDate = rental.getEndDate();
    int overdueDays = plannedReturnDate.daysTo(actualReturnDate);
    
    if (overdueDays <= 0) {
        return Fine(); // Нет просрочки
    }
    
    Car car = m_dbManager->getCarById(rental.getCarId());
    double basePrice = car.getDailyPrice();
    double fineAmount = basePrice * fineMultiplier * overdueDays;
    
    Fine fine(0, rentalId, fineAmount, DateUtils::currentDate(), 
              QString("Просрочка возврата на %1 день(ей)").arg(overdueDays));
    
    return fine;
}

bool RentalService::applyFine(const Fine& fine)
{
    if (!m_dbManager) {
        return false;
    }
    
    return m_dbManager->addFine(fine);
}

QList<Rental> RentalService::getActiveRentals() const
{
    if (!m_dbManager) {
        return QList<Rental>();
    }
    
    return m_dbManager->getActiveRentals();
}

QList<Rental> RentalService::getUserRentals(int userId) const
{
    if (!m_dbManager) {
        return QList<Rental>();
    }
    
    return m_dbManager->getRentalsByUserId(userId);
}

Rental RentalService::getRentalById(int rentalId) const
{
    if (!m_dbManager) {
        return Rental();
    }
    
    return m_dbManager->getRentalById(rentalId);
}

QList<Fine> RentalService::getFinesByRentalId(int rentalId) const
{
    if (!m_dbManager) {
        return QList<Fine>();
    }
    
    return m_dbManager->getFinesByRentalId(rentalId);
}

bool RentalService::isCarAvailable(int carId, const QDate& startDate, const QDate& endDate) const
{
    if (!m_dbManager) {
        return false;
    }
    
    Car car = m_dbManager->getCarById(carId);
    if (car.getId() == 0) {
        return false;
    }
    
    // Проверяем статус автомобиля
    if (car.getStatus() != CarStatus::Available && car.getStatus() != CarStatus::Reserved) {
        return false;
    }
    
    // Проверяем пересечение с существующими арендами
    QList<Rental> activeRentals = m_dbManager->getActiveRentals();
    for (const Rental& rental : activeRentals) {
        if (rental.getCarId() == carId) {
            // Проверяем пересечение дат
            if (!(endDate < rental.getStartDate() || startDate > rental.getEndDate())) {
                return false; // Есть пересечение
            }
        }
    }
    
    return true;
}

void RentalService::updateCarStatusOnRentalStart(int carId)
{
    if (m_statusSubject) {
        m_statusSubject->notifyStatusChanged(carId, CarStatus::Rented);
    }
}

void RentalService::updateCarStatusOnRentalComplete(int carId)
{
    if (m_statusSubject) {
        m_statusSubject->notifyStatusChanged(carId, CarStatus::Available);
    }
}

void RentalService::checkAndApplyOverdueFines()
{
    if (!m_dbManager) {
        return;
    }
    
    // Получаем все активные аренды
    QList<Rental> activeRentals = m_dbManager->getActiveRentals();
    QDate currentDate = DateUtils::currentDate(); // Текущая дата в системной временной зоне региона
    
    for (const Rental& rental : activeRentals) {
        // Проверяем, просрочена ли аренда (текущая дата больше даты возврата)
        if (currentDate > rental.getEndDate()) {
            // Получаем существующие штрафы для этой аренды
            QList<Fine> existingFines = m_dbManager->getFinesByRentalId(rental.getId());
            
            // Ищем штраф, начисленный сегодня или позже даты возврата
            Fine existingFine;
            bool fineExists = false;
            for (const Fine& fine : existingFines) {
                if (fine.getDate() >= rental.getEndDate()) {
                    existingFine = fine;
                    fineExists = true;
                    break;
                }
            }
            
            // Рассчитываем актуальный штраф на текущую дату
            Fine calculatedFine = calculateFine(rental.getId(), currentDate, 1.5);
            
            if (calculatedFine.getAmount() > 0) {
                if (!fineExists) {
                    // Штраф еще не начислен - начисляем новый
                    applyFine(calculatedFine);
                    qDebug() << "Автоматически начислен штраф за аренду #" << rental.getId() 
                             << "в размере" << calculatedFine.getAmount() << "руб (просрочка на" 
                             << rental.getEndDate().daysTo(currentDate) << "дней)";
                } else if (existingFine.getAmount() != calculatedFine.getAmount()) {
                    // Штраф уже есть, но сумма изменилась (прошло больше дней) - обновляем
                    existingFine.setAmount(calculatedFine.getAmount());
                    existingFine.setDate(currentDate);
                    existingFine.setReason(QString("Просрочка возврата на %1 день(ей)").arg(
                        rental.getEndDate().daysTo(currentDate)));
                    m_dbManager->updateFine(existingFine);
                    qDebug() << "Обновлен штраф за аренду #" << rental.getId() 
                             << "до" << calculatedFine.getAmount() << "руб";
                }
            }
        }
    }
}

