#include "dataexporter.h"
#include "dateutils.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QTime>
#include "../managers/reportmanager.h"

DataExporter::DataExporter(DatabaseManager* dbManager)
    : m_dbManager(dbManager)
{
}

bool DataExporter::exportToJson(const QString& filePath)
{
    if (!m_dbManager) {
        return false;
    }
    
    QJsonObject root;
    
    // Метаданные
    root["metadata"] = createMetadata();
    root["export_date"] = DateUtils::currentDate().toString("yyyy-MM-dd");
    root["version"] = "1.0";
    
    // Автомобили
    QJsonArray carsArray;
    QList<Car> cars = m_dbManager->getAllCars();
    for (const Car& car : cars) {
        carsArray.append(carToJson(car));
    }
    root["cars"] = carsArray;
    
    // Пользователи
    QJsonArray usersArray;
    QList<User> users = m_dbManager->getAllUsers();
    for (const User& user : users) {
        usersArray.append(userToJson(user));
    }
    root["users"] = usersArray;
    
    // Аренды
    QJsonArray rentalsArray;
    QList<Rental> rentals = m_dbManager->getAllRentals();
    for (const Rental& rental : rentals) {
        rentalsArray.append(rentalToJson(rental));
    }
    root["rentals"] = rentalsArray;
    
    // Штрафы
    QJsonArray finesArray;
    QList<Fine> fines = m_dbManager->getAllFines();
    for (const Fine& fine : fines) {
        finesArray.append(fineToJson(fine));
    }
    root["fines"] = finesArray;
    
    // Сохранение в файл
    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Не удалось открыть файл для записи:" << filePath;
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

bool DataExporter::exportCarsToJson(const QString& filePath)
{
    if (!m_dbManager) {
        return false;
    }
    
    QJsonObject root;
    root["metadata"] = createMetadata();
    root["export_date"] = DateUtils::currentDate().toString("yyyy-MM-dd");
    root["type"] = "cars";
    
    QJsonArray carsArray;
    QList<Car> cars = m_dbManager->getAllCars();
    for (const Car& car : cars) {
        carsArray.append(carToJson(car));
    }
    root["cars"] = carsArray;
    
    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

bool DataExporter::exportRentalsToJson(const QString& filePath)
{
    if (!m_dbManager) {
        return false;
    }
    
    QJsonObject root;
    root["metadata"] = createMetadata();
    root["export_date"] = DateUtils::currentDate().toString("yyyy-MM-dd");
    root["type"] = "rentals";
    
    QJsonArray rentalsArray;
    QList<Rental> rentals = m_dbManager->getAllRentals();
    for (const Rental& rental : rentals) {
        rentalsArray.append(rentalToJson(rental));
    }
    root["rentals"] = rentalsArray;
    
    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

bool DataExporter::exportUsersToJson(const QString& filePath)
{
    if (!m_dbManager) {
        return false;
    }
    
    QJsonObject root;
    root["metadata"] = createMetadata();
    root["export_date"] = DateUtils::currentDate().toString("yyyy-MM-dd");
    root["type"] = "users";
    
    QJsonArray usersArray;
    QList<User> users = m_dbManager->getAllUsers();
    for (const User& user : users) {
        usersArray.append(userToJson(user));
    }
    root["users"] = usersArray;
    
    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

bool DataExporter::exportFinesToJson(const QString& filePath)
{
    if (!m_dbManager) {
        return false;
    }
    
    QJsonObject root;
    root["metadata"] = createMetadata();
    root["export_date"] = DateUtils::currentDate().toString("yyyy-MM-dd");
    root["type"] = "fines";
    
    QJsonArray finesArray;
    QList<Fine> fines = m_dbManager->getAllFines();
    for (const Fine& fine : fines) {
        finesArray.append(fineToJson(fine));
    }
    root["fines"] = finesArray;
    
    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

bool DataExporter::exportReportToJson(const QString& filePath, const QDate& startDate, const QDate& endDate)
{
    if (!m_dbManager) {
        return false;
    }
    
    ReportManager reportManager;
    RevenueReport report = reportManager.generateRevenueReport(startDate, endDate);
    QList<CarStatistics> popularCars = reportManager.getPopularCars(startDate, endDate, 10);
    
    QJsonObject root;
    root["metadata"] = createMetadata();
    root["export_date"] = DateUtils::currentDate().toString("yyyy-MM-dd");
    root["type"] = "report";
    root["period_start"] = startDate.toString("yyyy-MM-dd");
    root["period_end"] = endDate.toString("yyyy-MM-dd");
    
    // Статистика
    QJsonObject stats;
    stats["total_revenue"] = report.totalRevenue;
    stats["total_fines"] = report.totalFines;
    stats["total_rentals"] = report.totalRentals;
    stats["active_rentals"] = report.activeRentals;
    stats["average_duration"] = report.averageRentalDuration;
    stats["fleet_utilization"] = report.fleetUtilization;
    root["statistics"] = stats;
    
    // Популярные автомобили
    QJsonArray popularArray;
    for (const CarStatistics& carStats : popularCars) {
        QJsonObject carObj;
        carObj["car_name"] = carStats.carName;
        carObj["rental_count"] = carStats.rentalCount;
        carObj["total_revenue"] = carStats.totalRevenue;
        popularArray.append(carObj);
    }
    root["popular_cars"] = popularArray;
    
    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

QJsonObject DataExporter::carToJson(const Car& car)
{
    QJsonObject obj;
    obj["id"] = car.getId();
    obj["brand"] = car.getBrand();
    obj["model"] = car.getModel();
    obj["status"] = static_cast<int>(car.getStatus());
    obj["status_string"] = car.getStatusString();
    obj["daily_price"] = car.getDailyPrice();
    return obj;
}

QJsonObject DataExporter::rentalToJson(const Rental& rental)
{
    QJsonObject obj;
    obj["id"] = rental.getId();
    obj["car_id"] = rental.getCarId();
    obj["user_id"] = rental.getUserId();
    obj["start_date"] = rental.getStartDate().toString("yyyy-MM-dd");
    obj["end_date"] = rental.getEndDate().toString("yyyy-MM-dd");
    if (rental.getActualReturnDate().isValid()) {
        obj["actual_return_date"] = rental.getActualReturnDate().toString("yyyy-MM-dd");
    }
    obj["total_cost"] = rental.getTotalCost();
    obj["is_completed"] = rental.isCompleted();
    obj["days_rented"] = rental.getDaysRented();
    obj["days_overdue"] = rental.getDaysOverdue();
    return obj;
}

QJsonObject DataExporter::userToJson(const User& user)
{
    QJsonObject obj;
    obj["id"] = user.getId();
    obj["username"] = user.getUsername();
    // Для совместимости с прежним форматом дублируем логин в поле full_name
    obj["full_name"] = user.getUsername();
    obj["role"] = static_cast<int>(user.getRole());
    obj["role_string"] = user.getRoleString();
    // Пароль не экспортируем по соображениям безопасности
    return obj;
}

QJsonObject DataExporter::fineToJson(const Fine& fine)
{
    QJsonObject obj;
    obj["id"] = fine.getId();
    obj["rental_id"] = fine.getRentalId();
    obj["amount"] = fine.getAmount();
    obj["date"] = fine.getDate().toString("yyyy-MM-dd");
    obj["reason"] = fine.getReason();
    return obj;
}

QJsonObject DataExporter::createMetadata()
{
    QJsonObject metadata;
    metadata["application"] = "Car Rental Agency";
    metadata["version"] = "1.0";
    metadata["export_date"] = DateUtils::currentDate().toString("yyyy-MM-dd");
    metadata["export_time"] = QTime::currentTime().toString("hh:mm:ss");
    return metadata;
}

