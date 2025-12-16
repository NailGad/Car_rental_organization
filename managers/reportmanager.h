#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include "../models/rental.h"
#include "../models/car.h"
#include "../models/fine.h"
#include "../database/databasemanager.h"
#include <QDate>
#include <QList>
#include <QMap>

struct RevenueReport {
    double totalRevenue;
    double totalFines;
    int totalRentals;
    int activeRentals;
    double averageRentalDuration;
    double fleetUtilization; // Загруженность парка в %
};

struct CarStatistics {
    int carId;
    QString carName;
    int rentalCount;
    double totalRevenue;
};

class ReportManager
{
public:
    ReportManager();
    
    // Отчет по доходу за период
    RevenueReport generateRevenueReport(const QDate& startDate, const QDate& endDate);
    
    // Статистика по автомобилям
    QList<CarStatistics> getCarStatistics(const QDate& startDate, const QDate& endDate);
    
    // Получить популярные автомобили
    QList<CarStatistics> getPopularCars(const QDate& startDate, const QDate& endDate, int limit = 10);
    
    // Получить статистику по статусам автомобилей
    QMap<CarStatus, int> getCarStatusStatistics();
    
    // Получить доход по дням
    QMap<QDate, double> getDailyRevenue(const QDate& startDate, const QDate& endDate);

private:
    DatabaseManager* m_dbManager;
    
    double calculateTotalRevenue(const QList<Rental>& rentals);
    double calculateTotalFines(const QDate& startDate, const QDate& endDate);
};

#endif // REPORTMANAGER_H

