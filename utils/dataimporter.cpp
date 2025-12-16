#include "dataimporter.h"
#include "../models/car.h"
#include "../models/user.h"
#include "../models/rental.h"
#include "../models/fine.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDate>

DataImporter::DataImporter(DatabaseManager* dbManager)
    : m_dbManager(dbManager)
{
}

ImportResult DataImporter::importFromJson(const QString& filePath, bool skipExisting)
{
    ImportResult result;
    
    if (!m_dbManager) {
        result.errors++;
        result.errorMessages.append("DatabaseManager не инициализирован");
        return result;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        result.errors++;
        result.errorMessages.append("Не удалось открыть файл: " + filePath);
        return result;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        result.errors++;
        result.errorMessages.append("Ошибка парсинга JSON: " + error.errorString());
        return result;
    }
    
    QJsonObject root = doc.object();
    
    if (!validateJsonStructure(root)) {
        result.errors++;
        result.errorMessages.append("Неверная структура JSON файла");
        return result;
    }
    
    // Импорт автомобилей
    if (root.contains("cars") && root["cars"].isArray()) {
        QJsonArray carsArray = root["cars"].toArray();
        for (const QJsonValue& value : carsArray) {
            if (value.isObject()) {
                Car car = jsonToCar(value.toObject());
                if (car.getId() > 0) {
                    // Проверяем существование
                    Car existing = m_dbManager->getCarById(car.getId());
                    if (existing.getId() == 0 || !skipExisting) {
                        if (existing.getId() > 0) {
                            m_dbManager->updateCar(car);
                        } else {
                            car.setId(0); // Сбрасываем ID для нового автомобиля
                            if (m_dbManager->addCar(car)) {
                                result.carsImported++;
                            } else {
                                result.errors++;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Импорт пользователей
    if (root.contains("users") && root["users"].isArray()) {
        QJsonArray usersArray = root["users"].toArray();
        for (const QJsonValue& value : usersArray) {
            if (value.isObject()) {
                User user = jsonToUser(value.toObject());
                if (!user.getUsername().isEmpty()) {
                    User existing = m_dbManager->getUserByUsername(user.getUsername());
                    if (existing.getId() == 0 || !skipExisting) {
                        if (existing.getId() > 0) {
                            m_dbManager->updateUser(user);
                        } else {
                            user.setId(0);
                            if (m_dbManager->addUser(user)) {
                                result.usersImported++;
                            } else {
                                result.errors++;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Импорт аренд
    if (root.contains("rentals") && root["rentals"].isArray()) {
        QJsonArray rentalsArray = root["rentals"].toArray();
        for (const QJsonValue& value : rentalsArray) {
            if (value.isObject()) {
                Rental rental = jsonToRental(value.toObject());
                if (rental.getId() > 0) {
                    Rental existing = m_dbManager->getRentalById(rental.getId());
                    if (existing.getId() == 0 || !skipExisting) {
                        if (existing.getId() > 0) {
                            m_dbManager->updateRental(rental);
                        } else {
                            rental.setId(0);
                            if (m_dbManager->addRental(rental)) {
                                result.rentalsImported++;
                            } else {
                                result.errors++;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Импорт штрафов
    if (root.contains("fines") && root["fines"].isArray()) {
        QJsonArray finesArray = root["fines"].toArray();
        for (const QJsonValue& value : finesArray) {
            if (value.isObject()) {
                Fine fine = jsonToFine(value.toObject());
                if (fine.getId() > 0) {
                    Fine existing = m_dbManager->getFineById(fine.getId());
                    if (existing.getId() == 0 || !skipExisting) {
                        if (existing.getId() > 0) {
                            m_dbManager->updateFine(fine);
                        } else {
                            fine.setId(0);
                            if (m_dbManager->addFine(fine)) {
                                result.finesImported++;
                            } else {
                                result.errors++;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

ImportResult DataImporter::importCarsFromJson(const QString& filePath, bool skipExisting)
{
    ImportResult result;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        result.errors++;
        result.errorMessages.append("Не удалось открыть файл");
        return result;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    QJsonObject root = doc.object();
    if (root.contains("cars") && root["cars"].isArray()) {
        QJsonArray carsArray = root["cars"].toArray();
        for (const QJsonValue& value : carsArray) {
            if (value.isObject()) {
                Car car = jsonToCar(value.toObject());
                if (car.getId() > 0) {
                    Car existing = m_dbManager->getCarById(car.getId());
                    if (existing.getId() == 0 || !skipExisting) {
                        car.setId(0);
                        if (m_dbManager->addCar(car)) {
                            result.carsImported++;
                        } else {
                            result.errors++;
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

ImportResult DataImporter::importUsersFromJson(const QString& filePath, bool skipExisting)
{
    ImportResult result;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        result.errors++;
        return result;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    QJsonObject root = doc.object();
    if (root.contains("users") && root["users"].isArray()) {
        QJsonArray usersArray = root["users"].toArray();
        for (const QJsonValue& value : usersArray) {
            if (value.isObject()) {
                User user = jsonToUser(value.toObject());
                if (!user.getUsername().isEmpty()) {
                    User existing = m_dbManager->getUserByUsername(user.getUsername());
                    if (existing.getId() == 0 || !skipExisting) {
                        user.setId(0);
                        if (m_dbManager->addUser(user)) {
                            result.usersImported++;
                        } else {
                            result.errors++;
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

ImportResult DataImporter::importRentalsFromJson(const QString& filePath, bool skipExisting)
{
    ImportResult result;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        result.errors++;
        return result;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    QJsonObject root = doc.object();
    if (root.contains("rentals") && root["rentals"].isArray()) {
        QJsonArray rentalsArray = root["rentals"].toArray();
        for (const QJsonValue& value : rentalsArray) {
            if (value.isObject()) {
                Rental rental = jsonToRental(value.toObject());
                if (rental.getId() > 0) {
                    rental.setId(0);
                    if (m_dbManager->addRental(rental)) {
                        result.rentalsImported++;
                    } else {
                        result.errors++;
                    }
                }
            }
        }
    }
    
    return result;
}

ImportResult DataImporter::importFinesFromJson(const QString& filePath, bool skipExisting)
{
    ImportResult result;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        result.errors++;
        return result;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    QJsonObject root = doc.object();
    if (root.contains("fines") && root["fines"].isArray()) {
        QJsonArray finesArray = root["fines"].toArray();
        for (const QJsonValue& value : finesArray) {
            if (value.isObject()) {
                Fine fine = jsonToFine(value.toObject());
                if (fine.getId() > 0) {
                    fine.setId(0);
                    if (m_dbManager->addFine(fine)) {
                        result.finesImported++;
                    } else {
                        result.errors++;
                    }
                }
            }
        }
    }
    
    return result;
}

Car DataImporter::jsonToCar(const QJsonObject& obj)
{
    Car car;
    if (obj.contains("id")) car.setId(obj["id"].toInt());
    if (obj.contains("brand")) car.setBrand(obj["brand"].toString());
    if (obj.contains("model")) car.setModel(obj["model"].toString());
    if (obj.contains("status")) car.setStatus(static_cast<CarStatus>(obj["status"].toInt()));
    if (obj.contains("daily_price")) car.setDailyPrice(obj["daily_price"].toDouble());
    return car;
}

User DataImporter::jsonToUser(const QJsonObject& obj)
{
    User user;
    if (obj.contains("id")) user.setId(obj["id"].toInt());
    if (obj.contains("username")) user.setUsername(obj["username"].toString());
    if (obj.contains("full_name")) user.setFullName(obj["full_name"].toString());
    else user.setFullName(user.getUsername());
    if (obj.contains("role")) user.setRole(static_cast<UserRole>(obj["role"].toInt()));
    // Пароль не импортируем - пользователь должен будет установить новый
    if (user.getPassword().isEmpty()) {
        user.setPassword("default123"); // Временный пароль
    }
    return user;
}

Rental DataImporter::jsonToRental(const QJsonObject& obj)
{
    Rental rental;
    if (obj.contains("id")) rental.setId(obj["id"].toInt());
    if (obj.contains("car_id")) rental.setCarId(obj["car_id"].toInt());
    if (obj.contains("user_id")) rental.setUserId(obj["user_id"].toInt());
    if (obj.contains("start_date")) {
        rental.setStartDate(QDate::fromString(obj["start_date"].toString(), "yyyy-MM-dd"));
    }
    if (obj.contains("end_date")) {
        rental.setEndDate(QDate::fromString(obj["end_date"].toString(), "yyyy-MM-dd"));
    }
    if (obj.contains("actual_return_date") && !obj["actual_return_date"].isNull()) {
        rental.setActualReturnDate(QDate::fromString(obj["actual_return_date"].toString(), "yyyy-MM-dd"));
    }
    if (obj.contains("total_cost")) rental.setTotalCost(obj["total_cost"].toDouble());
    if (obj.contains("is_completed")) rental.setCompleted(obj["is_completed"].toBool());
    return rental;
}

Fine DataImporter::jsonToFine(const QJsonObject& obj)
{
    Fine fine;
    if (obj.contains("id")) fine.setId(obj["id"].toInt());
    if (obj.contains("rental_id")) fine.setRentalId(obj["rental_id"].toInt());
    if (obj.contains("amount")) fine.setAmount(obj["amount"].toDouble());
    if (obj.contains("date")) {
        fine.setDate(QDate::fromString(obj["date"].toString(), "yyyy-MM-dd"));
    }
    if (obj.contains("reason")) fine.setReason(obj["reason"].toString());
    return fine;
}

bool DataImporter::validateJsonStructure(const QJsonObject& root)
{
    // Проверяем наличие хотя бы одного массива данных
    return root.contains("cars") || root.contains("users") || 
           root.contains("rentals") || root.contains("fines");
}

