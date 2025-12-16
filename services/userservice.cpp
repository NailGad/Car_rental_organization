#include "userservice.h"
#include "../database/databasemanager.h"
#include "../services/rentalservice.h"
#include "../utils/dateutils.h"
#include <QDebug>

UserService::UserService()
    : m_dbManager(nullptr), m_rentalService(nullptr)
{
    m_dbManager = &DatabaseManager::getInstance();
    m_rentalService = new RentalService();
}

QList<User> UserService::getAllUsers()
{
    if (!m_dbManager) {
        return QList<User>();
    }
    return m_dbManager->getAllUsers();
}

User UserService::getUserById(int userId)
{
    if (!m_dbManager) {
        return User();
    }
    return m_dbManager->getUserById(userId);
}

User UserService::getUserByUsername(const QString& username)
{
    if (!m_dbManager) {
        return User();
    }
    return m_dbManager->getUserByUsername(username);
}

bool UserService::addUser(const User& user)
{
    if (!m_dbManager) {
        return false;
    }
    return m_dbManager->addUser(user);
}

bool UserService::updateUser(const User& user)
{
    if (!m_dbManager) {
        return false;
    }
    return m_dbManager->updateUser(user);
}

void UserService::completeAllUserRentals(int userId, const QDate& currentDate)
{
    if (!m_rentalService) {
        return;
    }
    
    QList<Rental> userRentals = m_rentalService->getUserRentals(userId);
    for (const Rental& rental : userRentals) {
        if (!rental.isCompleted()) {
            // Если есть просрочка - начисляем штраф на текущую дату
            if (currentDate > rental.getEndDate()) {
                Fine fine = m_rentalService->calculateFine(rental.getId(), currentDate, 1.5);
                if (fine.getAmount() > 0) {
                    m_rentalService->applyFine(fine);
                }
            }
            // Завершаем аренду на текущую дату
            m_rentalService->completeRental(rental.getId(), currentDate);
        }
    }
}

bool UserService::deleteUser(int userId)
{
    if (!m_dbManager) {
        return false;
    }
    
    // Завершаем все активные аренды пользователя с расчетом штрафов
    QDate currentDate = DateUtils::currentDate();
    completeAllUserRentals(userId, currentDate);
    
    // Удаляем пользователя
    return m_dbManager->deleteUser(userId);
}

bool UserService::authenticateUser(const QString& username, const QString& password)
{
    if (!m_dbManager) {
        return false;
    }
    return m_dbManager->authenticateUser(username, password);
}

