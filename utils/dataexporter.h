#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "../models/car.h"
#include "../models/rental.h"
#include "../models/user.h"
#include "../models/fine.h"
#include "../database/databasemanager.h"

class DataExporter
{
public:
    DataExporter(DatabaseManager* dbManager);
    
    // Экспорт всех данных в JSON
    bool exportToJson(const QString& filePath);
    
    // Экспорт только автомобилей
    bool exportCarsToJson(const QString& filePath);
    
    // Экспорт только аренд
    bool exportRentalsToJson(const QString& filePath);
    
    // Экспорт только пользователей
    bool exportUsersToJson(const QString& filePath);
    
    // Экспорт только штрафов
    bool exportFinesToJson(const QString& filePath);
    
    // Экспорт отчета в JSON
    bool exportReportToJson(const QString& filePath, const QDate& startDate, const QDate& endDate);

private:
    DatabaseManager* m_dbManager;
    
    QJsonObject carToJson(const Car& car);
    QJsonObject rentalToJson(const Rental& rental);
    QJsonObject userToJson(const User& user);
    QJsonObject fineToJson(const Fine& fine);
    QJsonObject createMetadata();
};

#endif // DATAEXPORTER_H

