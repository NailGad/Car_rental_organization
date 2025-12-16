#include "reportmanager.h"
#include <QMap>
#include <QDebug>
#include <algorithm>

ReportManager::ReportManager()
    : m_dbManager(nullptr)
{
    m_dbManager = &DatabaseManager::getInstance();
}

RevenueReport ReportManager::generateRevenueReport(const QDate& startDate, const QDate& endDate)
{
    RevenueReport report;
    report.totalRevenue = 0.0;
    report.totalFines = 0.0;
    report.totalRentals = 0;
    report.activeRentals = 0;
    report.averageRentalDuration = 0.0;
    report.fleetUtilization = 0.0;
    
    if (!m_dbManager) {
        return report;
    }
    
    // Получаем все аренды за период
    QList<Rental> allRentals = m_dbManager->getAllRentals();
    QList<Rental> periodRentals;
    
    for (const Rental& rental : allRentals) {
        if (rental.getStartDate() >= startDate && rental.getStartDate() <= endDate) {
            periodRentals.append(rental);
        }
    }
    
    report.totalRentals = periodRentals.size();
    
    // Рассчитываем доход от аренд
    report.totalRevenue = calculateTotalRevenue(periodRentals);
    
    // Рассчитываем штрафы
    report.totalFines = calculateTotalFines(startDate, endDate);
    
    // Активные аренды
    QList<Rental> activeRentals = m_dbManager->getActiveRentals();
    report.activeRentals = activeRentals.size();
    
    // Средняя длительность аренды
    if (!periodRentals.isEmpty()) {
        int totalDays = 0;
        for (const Rental& rental : periodRentals) {
            totalDays += rental.getDaysRented();
        }
        report.averageRentalDuration = static_cast<double>(totalDays) / periodRentals.size();
    }
    
    // Загруженность парка
    QList<Car> allCars = m_dbManager->getAllCars();
    int totalCars = allCars.size();
    int rentedCars = 0;
    
    for (const Car& car : allCars) {
        if (car.getStatus() == CarStatus::Rented) {
            rentedCars++;
        }
    }
    
    if (totalCars > 0) {
        report.fleetUtilization = (static_cast<double>(rentedCars) / totalCars) * 100.0;
    }
    
    return report;
}

QList<CarStatistics> ReportManager::getCarStatistics(const QDate& startDate, const QDate& endDate)
{
    QList<CarStatistics> statistics;
    
    if (!m_dbManager) {
        return statistics;
    }
    
    QList<Car> allCars = m_dbManager->getAllCars();
    QList<Rental> allRentals = m_dbManager->getAllRentals();
    
    for (const Car& car : allCars) {
        CarStatistics stats;
        stats.carId = car.getId();
        stats.carName = car.getFullName();
        stats.rentalCount = 0;
        stats.totalRevenue = 0.0;
        
        // Подсчитываем аренды для этого автомобиля
        for (const Rental& rental : allRentals) {
            if (rental.getCarId() == car.getId() &&
                rental.getStartDate() >= startDate &&
                rental.getStartDate() <= endDate) {
                stats.rentalCount++;
                stats.totalRevenue += rental.getTotalCost();
            }
        }
        
        statistics.append(stats);
    }
    
    return statistics;
}

QList<CarStatistics> ReportManager::getPopularCars(const QDate& startDate, const QDate& endDate, int limit)
{
    QList<CarStatistics> allStats = getCarStatistics(startDate, endDate);
    
    // Сортируем по количеству аренд
    std::sort(allStats.begin(), allStats.end(),
              [](const CarStatistics& a, const CarStatistics& b) {
                  return a.rentalCount > b.rentalCount;
              });
    
    // Возвращаем топ N
    if (allStats.size() > limit) {
        allStats = allStats.mid(0, limit);
    }
    
    return allStats;
}

QMap<CarStatus, int> ReportManager::getCarStatusStatistics()
{
    QMap<CarStatus, int> statistics;
    
    if (!m_dbManager) {
        return statistics;
    }
    
    QList<Car> allCars = m_dbManager->getAllCars();
    
    for (const Car& car : allCars) {
        statistics[car.getStatus()]++;
    }
    
    return statistics;
}

QMap<QDate, double> ReportManager::getDailyRevenue(const QDate& startDate, const QDate& endDate)
{
    QMap<QDate, double> dailyRevenue;
    
    if (!m_dbManager) {
        return dailyRevenue;
    }
    
    QList<Rental> allRentals = m_dbManager->getAllRentals();
    
    for (const Rental& rental : allRentals) {
        if (rental.getStartDate() >= startDate && rental.getStartDate() <= endDate) {
            QDate rentalDate = rental.getStartDate();
            dailyRevenue[rentalDate] += rental.getTotalCost();
        }
    }
    
    return dailyRevenue;
}

double ReportManager::calculateTotalRevenue(const QList<Rental>& rentals)
{
    double total = 0.0;
    for (const Rental& rental : rentals) {
        total += rental.getTotalCost();
    }
    return total;
}

double ReportManager::calculateTotalFines(const QDate& startDate, const QDate& endDate)
{
    if (!m_dbManager) {
        return 0.0;
    }
    
    QList<Fine> allFines = m_dbManager->getAllFines();
    double total = 0.0;
    
    for (const Fine& fine : allFines) {
        if (fine.getDate() >= startDate && fine.getDate() <= endDate) {
            total += fine.getAmount();
        }
    }
    
    return total;
}

