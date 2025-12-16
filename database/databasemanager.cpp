#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QDate>
#include <QCoreApplication>

DatabaseManager& DatabaseManager::getInstance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    // Сохраняем базу данных в папке проекта Organization/database/
    QDir appDir(QCoreApplication::applicationDirPath());
    QString dbPath;
    
    // Ищем папку Organization/database относительно исполняемого файла
    // Если запускаем из build папки, поднимаемся на уровень выше
    if (appDir.dirName().contains("build") || appDir.dirName().contains("debug") || appDir.dirName().contains("release")) {
        appDir.cdUp();
    }
    // Проверяем, есть ли папка Organization
    if (appDir.exists("Organization")) {
        dbPath = appDir.absoluteFilePath("Organization/database");
    } else {
        // Если не нашли, используем текущую директорию
        QDir currentDir = QDir::current();
        if (currentDir.dirName().contains("build") || currentDir.dirName().contains("debug") || currentDir.dirName().contains("release")) {
            currentDir.cdUp();
        }
        if (currentDir.exists("Organization")) {
            dbPath = currentDir.absoluteFilePath("Organization/database");
        } else {
            // В крайнем случае используем папку приложения
            dbPath = appDir.absoluteFilePath("database");
        }
    }
    
    QDir().mkpath(dbPath);
    m_database.setDatabaseName(dbPath + "/car_rental.db");
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

bool DatabaseManager::initializeDatabase()
{
    if (!m_database.open()) {
        qDebug() << "Ошибка открытия базы данных:" << m_database.lastError().text();
        return false;
    }
    
    if (!createTables()) {
        qDebug() << "Ошибка создания таблиц";
        return false;
    }
    
    return true;
}

void DatabaseManager::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseManager::isOpen() const
{
    return m_database.isOpen();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);
    
    // Таблица пользователей
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT UNIQUE NOT NULL,"
               "password TEXT NOT NULL,"
               "full_name TEXT NOT NULL,"
               "role INTEGER NOT NULL)");
    
    // Таблица автомобилей
    query.exec("CREATE TABLE IF NOT EXISTS cars ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "brand TEXT NOT NULL,"
               "model TEXT NOT NULL,"
               "status INTEGER NOT NULL,"
               "daily_price REAL NOT NULL)");
    
    // Таблица аренды
    query.exec("CREATE TABLE IF NOT EXISTS rentals ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "car_id INTEGER NOT NULL,"
               "user_id INTEGER NOT NULL,"
               "start_date DATE NOT NULL,"
               "end_date DATE NOT NULL,"
               "actual_return_date DATE,"
               "total_cost REAL NOT NULL,"
               "is_completed INTEGER NOT NULL DEFAULT 0,"
               "FOREIGN KEY(car_id) REFERENCES cars(id),"
               "FOREIGN KEY(user_id) REFERENCES users(id))");
    
    // Таблица штрафов
    query.exec("CREATE TABLE IF NOT EXISTS fines ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "rental_id INTEGER NOT NULL,"
               "amount REAL NOT NULL,"
               "date DATE NOT NULL,"
               "reason TEXT,"
               "FOREIGN KEY(rental_id) REFERENCES rentals(id))");
    
    if (query.lastError().isValid()) {
        qDebug() << "Ошибка создания таблиц:" << query.lastError().text();
        return false;
    }
    
    return true;
}

// User operations
bool DatabaseManager::addUser(const User& user)
{
    QSqlQuery query(m_database);
    int newId = getNextAvailableUserId();
    query.prepare("INSERT INTO users (id, username, password, full_name, role) "
                 "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(newId);
    query.addBindValue(user.getUsername());
    query.addBindValue(user.getPassword());
    query.addBindValue(user.getFullName());
    query.addBindValue(static_cast<int>(user.getRole()));
    return query.exec();
}

bool DatabaseManager::updateUser(const User& user)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE users SET username=?, password=?, full_name=?, role=? WHERE id=?");
    query.addBindValue(user.getUsername());
    query.addBindValue(user.getPassword());
    query.addBindValue(user.getFullName());
    query.addBindValue(static_cast<int>(user.getRole()));
    query.addBindValue(user.getId());
    return query.exec();
}

bool DatabaseManager::deleteUser(int userId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM users WHERE id=?");
    query.addBindValue(userId);
    return query.exec();
}

User DatabaseManager::getUserById(int userId)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM users WHERE id=?");
    query.addBindValue(userId);
    query.exec();
    
    if (query.next()) {
        return User(query.value(0).toInt(),
                   query.value(1).toString(),
                   query.value(2).toString(),
                   query.value(3).toString(),
                   static_cast<UserRole>(query.value(4).toInt()));
    }
    return User();
}

User DatabaseManager::getUserByUsername(const QString& username)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM users WHERE username=?");
    query.addBindValue(username);
    query.exec();
    
    if (query.next()) {
        return User(query.value(0).toInt(),
                   query.value(1).toString(),
                   query.value(2).toString(),
                   query.value(3).toString(),
                   static_cast<UserRole>(query.value(4).toInt()));
    }
    return User();
}

QList<User> DatabaseManager::getAllUsers()
{
    QList<User> users;
    QSqlQuery query("SELECT * FROM users ORDER BY id ASC", m_database);
    
    while (query.next()) {
        users.append(User(query.value(0).toInt(),
                         query.value(1).toString(),
                         query.value(2).toString(),
                         query.value(3).toString(),
                         static_cast<UserRole>(query.value(4).toInt())));
    }
    return users;
}

int DatabaseManager::getNextAvailableUserId()
{
    QSqlQuery query(m_database);
    // Находим первую свободную "дыру" или следующий после максимального
    query.exec("SELECT COALESCE((SELECT MIN(t.id)+1 FROM users t "
               "WHERE NOT EXISTS (SELECT 1 FROM users t2 WHERE t2.id = t.id + 1)), 1)");
    if (query.next()) {
        int candidate = query.value(0).toInt();
        if (candidate <= 0) candidate = 1;
        return candidate;
    }
    return 1;
}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password)
{
    User user = getUserByUsername(username);
    return !user.getUsername().isEmpty() && user.getPassword() == password;
}

// Car operations
bool DatabaseManager::addCar(const Car& car)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO cars (brand, model, status, daily_price) "
                 "VALUES (?, ?, ?, ?)");
    query.addBindValue(car.getBrand());
    query.addBindValue(car.getModel());
    query.addBindValue(static_cast<int>(car.getStatus()));
    query.addBindValue(car.getDailyPrice());
    return query.exec();
}

bool DatabaseManager::updateCar(const Car& car)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE cars SET brand=?, model=?, status=?, daily_price=? WHERE id=?");
    query.addBindValue(car.getBrand());
    query.addBindValue(car.getModel());
    query.addBindValue(static_cast<int>(car.getStatus()));
    query.addBindValue(car.getDailyPrice());
    query.addBindValue(car.getId());
    return query.exec();
}

bool DatabaseManager::deleteCar(int carId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM cars WHERE id=?");
    query.addBindValue(carId);
    return query.exec();
}

Car DatabaseManager::getCarById(int carId)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM cars WHERE id=?");
    query.addBindValue(carId);
    query.exec();
    
    if (query.next()) {
        return Car(query.value(0).toInt(),
                  query.value(1).toString(),
                  query.value(2).toString(),
                  static_cast<CarStatus>(query.value(3).toInt()),
                  query.value(4).toDouble());
    }
    return Car();
}

QList<Car> DatabaseManager::getAllCars()
{
    QList<Car> cars;
    QSqlQuery query("SELECT * FROM cars", m_database);
    
    while (query.next()) {
        cars.append(Car(query.value(0).toInt(),
                       query.value(1).toString(),
                       query.value(2).toString(),
                       static_cast<CarStatus>(query.value(3).toInt()),
                       query.value(4).toDouble()));
    }
    return cars;
}

QList<Car> DatabaseManager::getCarsByBrand(const QString& brand)
{
    QList<Car> cars;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM cars WHERE brand=?");
    query.addBindValue(brand);
    query.exec();
    
    while (query.next()) {
        cars.append(Car(query.value(0).toInt(),
                       query.value(1).toString(),
                       query.value(2).toString(),
                       static_cast<CarStatus>(query.value(3).toInt()),
                       query.value(4).toDouble()));
    }
    return cars;
}

QList<Car> DatabaseManager::getAvailableCars()
{
    QList<Car> cars;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM cars WHERE status=?");
    query.addBindValue(static_cast<int>(CarStatus::Available));
    query.exec();
    
    while (query.next()) {
        cars.append(Car(query.value(0).toInt(),
                       query.value(1).toString(),
                       query.value(2).toString(),
                       static_cast<CarStatus>(query.value(3).toInt()),
                       query.value(4).toDouble()));
    }
    return cars;
}

// Rental operations
bool DatabaseManager::addRental(const Rental& rental)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO rentals (car_id, user_id, start_date, end_date, "
                 "actual_return_date, total_cost, is_completed) "
                 "VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(rental.getCarId());
    query.addBindValue(rental.getUserId());
    query.addBindValue(rental.getStartDate().toString("yyyy-MM-dd"));
    query.addBindValue(rental.getEndDate().toString("yyyy-MM-dd"));
    if (rental.getActualReturnDate().isValid()) {
        query.addBindValue(rental.getActualReturnDate().toString("yyyy-MM-dd"));
    } else {
        query.addBindValue(QVariant());
    }
    query.addBindValue(rental.getTotalCost());
    query.addBindValue(rental.isCompleted() ? 1 : 0);
    return query.exec();
}

bool DatabaseManager::updateRental(const Rental& rental)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE rentals SET car_id=?, user_id=?, start_date=?, end_date=?, "
                 "actual_return_date=?, total_cost=?, is_completed=? WHERE id=?");
    query.addBindValue(rental.getCarId());
    query.addBindValue(rental.getUserId());
    query.addBindValue(rental.getStartDate().toString("yyyy-MM-dd"));
    query.addBindValue(rental.getEndDate().toString("yyyy-MM-dd"));
    if (rental.getActualReturnDate().isValid()) {
        query.addBindValue(rental.getActualReturnDate().toString("yyyy-MM-dd"));
    } else {
        query.addBindValue(QVariant());
    }
    query.addBindValue(rental.getTotalCost());
    query.addBindValue(rental.isCompleted() ? 1 : 0);
    query.addBindValue(rental.getId());
    return query.exec();
}

bool DatabaseManager::deleteRental(int rentalId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM rentals WHERE id=?");
    query.addBindValue(rentalId);
    return query.exec();
}

Rental DatabaseManager::getRentalById(int rentalId)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM rentals WHERE id=?");
    query.addBindValue(rentalId);
    query.exec();
    
    if (query.next()) {
        Rental rental(query.value(0).toInt(),
                     query.value(1).toInt(),
                     query.value(2).toInt(),
                     QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                     QDate::fromString(query.value(4).toString(), "yyyy-MM-dd"),
                     query.value(6).toDouble(),
                     query.value(7).toInt() == 1);
        if (!query.value(5).isNull()) {
            rental.setActualReturnDate(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
        }
        return rental;
    }
    return Rental();
}

QList<Rental> DatabaseManager::getAllRentals()
{
    QList<Rental> rentals;
    QSqlQuery query("SELECT * FROM rentals", m_database);
    
    while (query.next()) {
        Rental rental(query.value(0).toInt(),
                     query.value(1).toInt(),
                     query.value(2).toInt(),
                     QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                     QDate::fromString(query.value(4).toString(), "yyyy-MM-dd"),
                     query.value(6).toDouble(),
                     query.value(7).toInt() == 1);
        if (!query.value(5).isNull()) {
            rental.setActualReturnDate(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
        }
        rentals.append(rental);
    }
    return rentals;
}

QList<Rental> DatabaseManager::getRentalsByUserId(int userId)
{
    QList<Rental> rentals;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM rentals WHERE user_id=?");
    query.addBindValue(userId);
    query.exec();
    
    while (query.next()) {
        Rental rental(query.value(0).toInt(),
                     query.value(1).toInt(),
                     query.value(2).toInt(),
                     QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                     QDate::fromString(query.value(4).toString(), "yyyy-MM-dd"),
                     query.value(6).toDouble(),
                     query.value(7).toInt() == 1);
        if (!query.value(5).isNull()) {
            rental.setActualReturnDate(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
        }
        rentals.append(rental);
    }
    return rentals;
}

QList<Rental> DatabaseManager::getRentalsByDateRange(const QDate& startDate, const QDate& endDate)
{
    QList<Rental> rentals;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM rentals WHERE start_date >= ? AND end_date <= ?");
    query.addBindValue(startDate.toString("yyyy-MM-dd"));
    query.addBindValue(endDate.toString("yyyy-MM-dd"));
    query.exec();
    
    while (query.next()) {
        Rental rental(query.value(0).toInt(),
                     query.value(1).toInt(),
                     query.value(2).toInt(),
                     QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                     QDate::fromString(query.value(4).toString(), "yyyy-MM-dd"),
                     query.value(6).toDouble(),
                     query.value(7).toInt() == 1);
        if (!query.value(5).isNull()) {
            rental.setActualReturnDate(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
        }
        rentals.append(rental);
    }
    return rentals;
}

QList<Rental> DatabaseManager::getActiveRentals()
{
    QList<Rental> rentals;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM rentals WHERE is_completed=0");
    query.exec();
    
    while (query.next()) {
        Rental rental(query.value(0).toInt(),
                     query.value(1).toInt(),
                     query.value(2).toInt(),
                     QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                     QDate::fromString(query.value(4).toString(), "yyyy-MM-dd"),
                     query.value(6).toDouble(),
                     query.value(7).toInt() == 1);
        if (!query.value(5).isNull()) {
            rental.setActualReturnDate(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
        }
        rentals.append(rental);
    }
    return rentals;
}

// Fine operations
bool DatabaseManager::addFine(const Fine& fine)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO fines (rental_id, amount, date, reason) "
                 "VALUES (?, ?, ?, ?)");
    query.addBindValue(fine.getRentalId());
    query.addBindValue(fine.getAmount());
    query.addBindValue(fine.getDate().toString("yyyy-MM-dd"));
    query.addBindValue(fine.getReason());
    return query.exec();
}

bool DatabaseManager::updateFine(const Fine& fine)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE fines SET rental_id=?, amount=?, date=?, reason=? WHERE id=?");
    query.addBindValue(fine.getRentalId());
    query.addBindValue(fine.getAmount());
    query.addBindValue(fine.getDate().toString("yyyy-MM-dd"));
    query.addBindValue(fine.getReason());
    query.addBindValue(fine.getId());
    return query.exec();
}

bool DatabaseManager::deleteFine(int fineId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM fines WHERE id=?");
    query.addBindValue(fineId);
    return query.exec();
}

Fine DatabaseManager::getFineById(int fineId)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM fines WHERE id=?");
    query.addBindValue(fineId);
    query.exec();
    
    if (query.next()) {
        return Fine(query.value(0).toInt(),
                   query.value(1).toInt(),
                   query.value(2).toDouble(),
                   QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                   query.value(4).toString());
    }
    return Fine();
}

QList<Fine> DatabaseManager::getAllFines()
{
    QList<Fine> fines;
    QSqlQuery query("SELECT * FROM fines", m_database);
    
    while (query.next()) {
        fines.append(Fine(query.value(0).toInt(),
                         query.value(1).toInt(),
                         query.value(2).toDouble(),
                         QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                         query.value(4).toString()));
    }
    return fines;
}

QList<Fine> DatabaseManager::getFinesByRentalId(int rentalId)
{
    QList<Fine> fines;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM fines WHERE rental_id=?");
    query.addBindValue(rentalId);
    query.exec();
    
    while (query.next()) {
        fines.append(Fine(query.value(0).toInt(),
                         query.value(1).toInt(),
                         query.value(2).toDouble(),
                         QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                         query.value(4).toString()));
    }
    return fines;
}

// Расширенный поиск
QList<Car> DatabaseManager::searchCars(const QString& brand, const QString& model, CarStatus status)
{
    QList<Car> cars;
    QSqlQuery query(m_database);
    
    QString sql = "SELECT * FROM cars WHERE 1=1";
    if (!brand.isEmpty()) {
        sql += " AND brand LIKE '%" + brand + "%'";
    }
    if (!model.isEmpty()) {
        sql += " AND model LIKE '%" + model + "%'";
    }
    if (status != CarStatus::Available || !brand.isEmpty() || !model.isEmpty()) {
        sql += " AND status = " + QString::number(static_cast<int>(status));
    }
    
    query.exec(sql);
    
    while (query.next()) {
        cars.append(Car(query.value(0).toInt(),
                       query.value(1).toString(),
                       query.value(2).toString(),
                       static_cast<CarStatus>(query.value(3).toInt()),
                       query.value(4).toDouble()));
    }
    return cars;
}

QList<Rental> DatabaseManager::searchRentalsByClientName(const QString& clientName)
{
    QList<Rental> rentals;
    QSqlQuery query(m_database);
    
    query.prepare("SELECT r.* FROM rentals r "
                 "JOIN users u ON r.user_id = u.id "
                 "WHERE u.username LIKE ?");
    query.addBindValue("%" + clientName + "%");
    query.exec();
    
    while (query.next()) {
        Rental rental(query.value(0).toInt(),
                     query.value(1).toInt(),
                     query.value(2).toInt(),
                     QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                     QDate::fromString(query.value(4).toString(), "yyyy-MM-dd"),
                     query.value(6).toDouble(),
                     query.value(7).toInt() == 1);
        if (!query.value(5).isNull()) {
            rental.setActualReturnDate(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
        }
        rentals.append(rental);
    }
    return rentals;
}

QList<Rental> DatabaseManager::searchRentalsByDate(const QDate& date)
{
    QList<Rental> rentals;
    QSqlQuery query(m_database);
    
    query.prepare("SELECT * FROM rentals WHERE start_date <= ? AND end_date >= ?");
    query.addBindValue(date.toString("yyyy-MM-dd"));
    query.addBindValue(date.toString("yyyy-MM-dd"));
    query.exec();
    
    while (query.next()) {
        Rental rental(query.value(0).toInt(),
                     query.value(1).toInt(),
                     query.value(2).toInt(),
                     QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                     QDate::fromString(query.value(4).toString(), "yyyy-MM-dd"),
                     query.value(6).toDouble(),
                     query.value(7).toInt() == 1);
        if (!query.value(5).isNull()) {
            rental.setActualReturnDate(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
        }
        rentals.append(rental);
    }
    return rentals;
}

QList<Rental> DatabaseManager::searchRentalsByDateRange(const QDate& startDate, const QDate& endDate)
{
    QList<Rental> rentals;
    QSqlQuery query(m_database);
    
    // Ищем аренды, которые пересекаются с указанным диапазоном
    // Аренда пересекается, если: start_date <= endDate AND end_date >= startDate
    query.prepare("SELECT * FROM rentals WHERE start_date <= ? AND end_date >= ?");
    query.addBindValue(endDate.toString("yyyy-MM-dd"));
    query.addBindValue(startDate.toString("yyyy-MM-dd"));
    query.exec();
    
    while (query.next()) {
        Rental rental(query.value(0).toInt(),
                     query.value(1).toInt(),
                     query.value(2).toInt(),
                     QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                     QDate::fromString(query.value(4).toString(), "yyyy-MM-dd"),
                     query.value(6).toDouble(),
                     query.value(7).toInt() == 1);
        if (!query.value(5).isNull()) {
            rental.setActualReturnDate(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
        }
        rentals.append(rental);
    }
    return rentals;
}

QList<Rental> DatabaseManager::searchRentalsByCarBrand(const QString& brand)
{
    QList<Rental> rentals;
    QSqlQuery query(m_database);
    
    query.prepare("SELECT r.* FROM rentals r "
                 "JOIN cars c ON r.car_id = c.id "
                 "WHERE c.brand LIKE ?");
    query.addBindValue("%" + brand + "%");
    query.exec();
    
    while (query.next()) {
        Rental rental(query.value(0).toInt(),
                     query.value(1).toInt(),
                     query.value(2).toInt(),
                     QDate::fromString(query.value(3).toString(), "yyyy-MM-dd"),
                     QDate::fromString(query.value(4).toString(), "yyyy-MM-dd"),
                     query.value(6).toDouble(),
                     query.value(7).toInt() == 1);
        if (!query.value(5).isNull()) {
            rental.setActualReturnDate(QDate::fromString(query.value(5).toString(), "yyyy-MM-dd"));
        }
        rentals.append(rental);
    }
    return rentals;
}
