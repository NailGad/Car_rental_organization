#ifndef DATAIMPORTER_H
#define DATAIMPORTER_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "../database/databasemanager.h"

struct ImportResult {
    int carsImported;
    int usersImported;
    int rentalsImported;
    int finesImported;
    int errors;
    QStringList errorMessages;
    
    ImportResult() : carsImported(0), usersImported(0), rentalsImported(0), finesImported(0), errors(0) {}
};

class DataImporter
{
public:
    DataImporter(DatabaseManager* dbManager);
    
    // Импорт всех данных из JSON
    ImportResult importFromJson(const QString& filePath, bool skipExisting = true);
    
    // Импорт только автомобилей
    ImportResult importCarsFromJson(const QString& filePath, bool skipExisting = true);
    
    // Импорт только пользователей
    ImportResult importUsersFromJson(const QString& filePath, bool skipExisting = true);
    
    // Импорт только аренд
    ImportResult importRentalsFromJson(const QString& filePath, bool skipExisting = true);
    
    // Импорт только штрафов
    ImportResult importFinesFromJson(const QString& filePath, bool skipExisting = true);

private:
    DatabaseManager* m_dbManager;
    
    Car jsonToCar(const QJsonObject& obj);
    User jsonToUser(const QJsonObject& obj);
    Rental jsonToRental(const QJsonObject& obj);
    Fine jsonToFine(const QJsonObject& obj);
    bool validateJsonStructure(const QJsonObject& root);
};

#endif // DATAIMPORTER_H

