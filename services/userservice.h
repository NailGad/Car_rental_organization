#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "../models/user.h"
#include "../models/rental.h"
#include "../models/fine.h"
#include <QList>
#include <QDate>

class DatabaseManager;
class RentalService;

class UserService
{
public:
    UserService();
    
    // Получить всех пользователей
    QList<User> getAllUsers();
    
    // Получить пользователя по ID
    User getUserById(int userId);
    
    // Получить пользователя по логину
    User getUserByUsername(const QString& username);
    
    // Добавить пользователя
    bool addUser(const User& user);
    
    // Обновить пользователя
    bool updateUser(const User& user);
    
    // Удалить пользователя (с завершением всех активных аренд и начислением штрафов)
    bool deleteUser(int userId);
    
    // Аутентификация пользователя
    bool authenticateUser(const QString& username, const QString& password);

private:
    DatabaseManager* m_dbManager;
    RentalService* m_rentalService;
    
    // Завершить все активные аренды пользователя
    void completeAllUserRentals(int userId, const QDate& currentDate);
};

#endif // USERSERVICE_H

