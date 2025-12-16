#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QList>
#include "../models/user.h"
#include "../models/car.h"
#include "../models/rental.h"
#include "../models/fine.h"

class DatabaseManager
{
public:
    static DatabaseManager& getInstance();
    
    bool initializeDatabase();
    void closeDatabase();
    bool isOpen() const;
    
    // User operations
    bool addUser(const User& user);
    bool updateUser(const User& user);
    bool deleteUser(int userId);
    User getUserById(int userId);
    User getUserByUsername(const QString& username);
    QList<User> getAllUsers();
    bool authenticateUser(const QString& username, const QString& password);
    
    // Car operations
    bool addCar(const Car& car);
    bool updateCar(const Car& car);
    bool deleteCar(int carId);
    Car getCarById(int carId);
    QList<Car> getAllCars();
    QList<Car> getCarsByBrand(const QString& brand);
    QList<Car> getAvailableCars();
    
    // Rental operations
    bool addRental(const Rental& rental);
    bool updateRental(const Rental& rental);
    bool deleteRental(int rentalId);
    Rental getRentalById(int rentalId);
    QList<Rental> getAllRentals();
    QList<Rental> getRentalsByUserId(int userId);
    QList<Rental> getRentalsByDateRange(const QDate& startDate, const QDate& endDate);
    QList<Rental> getActiveRentals();
    
    // Fine operations
    bool addFine(const Fine& fine);
    bool updateFine(const Fine& fine);
    bool deleteFine(int fineId);
    Fine getFineById(int fineId);
    QList<Fine> getAllFines();
    QList<Fine> getFinesByRentalId(int rentalId);
    
    // Расширенный поиск
    QList<Car> searchCars(const QString& brand, const QString& model, CarStatus status = CarStatus::Available);
    QList<Rental> searchRentalsByClientName(const QString& clientName);
    QList<Rental> searchRentalsByDate(const QDate& date);
    QList<Rental> searchRentalsByDateRange(const QDate& startDate, const QDate& endDate);
    QList<Rental> searchRentalsByCarBrand(const QString& brand);

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    QSqlDatabase m_database;
    bool createTables();
    int getNextAvailableUserId();
};

#endif // DATABASEMANAGER_H

