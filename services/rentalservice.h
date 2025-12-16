#ifndef RENTALSERVICE_H
#define RENTALSERVICE_H

#include "../models/rental.h"
#include "../models/car.h"
#include "../models/fine.h"
#include "../patterns/carstatusobserver.h"
#include <QDate>
#include <QList>

class DatabaseManager;
class CarStatusSubject;

class RentalService
{
public:
    RentalService();
    
    // Создать новую аренду
    bool createRental(int carId, int userId, const QDate& startDate, const QDate& endDate, double totalCost);
    
    // Завершить аренду
    bool completeRental(int rentalId, const QDate& actualReturnDate);
    
    // Завершить аренду с автоматическим начислением штрафа при просрочке
    // Возвращает структуру с информацией о результате
    struct CompleteRentalResult {
        bool success;
        bool wasOverdue;
        double fineAmount;
        int overdueDays;
    };
    CompleteRentalResult completeRentalWithFine(int rentalId, const QDate& actualReturnDate, double fineMultiplier = 1.5);
    
    // Рассчитать штраф за просрочку
    Fine calculateFine(int rentalId, const QDate& actualReturnDate, double fineMultiplier = 1.5);
    
    // Применить штраф к аренде
    bool applyFine(const Fine& fine);
    
    // Получить активные аренды
    QList<Rental> getActiveRentals() const;
    
    // Получить аренды пользователя
    QList<Rental> getUserRentals(int userId) const;
    
    // Получить аренду по ID
    Rental getRentalById(int rentalId) const;
    
    // Получить штрафы по ID аренды
    QList<Fine> getFinesByRentalId(int rentalId) const;
    
    // Проверить доступность автомобиля в период
    bool isCarAvailable(int carId, const QDate& startDate, const QDate& endDate) const;
    
    // Автоматическая проверка просроченных аренд и начисление штрафов
    void checkAndApplyOverdueFines();

private:
    DatabaseManager* m_dbManager;
    CarStatusSubject* m_statusSubject;
    CarStatusObserver* m_statusObserver;
    
    void updateCarStatusOnRentalStart(int carId);
    void updateCarStatusOnRentalComplete(int carId);
};

#endif // RENTALSERVICE_H

