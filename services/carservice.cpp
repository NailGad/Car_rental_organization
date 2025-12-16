#include "carservice.h"
#include "../database/databasemanager.h"

CarService::CarService()
    : m_dbManager(nullptr)
{
    m_dbManager = &DatabaseManager::getInstance();
}

QList<Car> CarService::getAllCars()
{
    if (!m_dbManager) {
        return QList<Car>();
    }
    return m_dbManager->getAllCars();
}

Car CarService::getCarById(int carId)
{
    if (!m_dbManager) {
        return Car();
    }
    return m_dbManager->getCarById(carId);
}

QList<Car> CarService::getAvailableCars()
{
    if (!m_dbManager) {
        return QList<Car>();
    }
    return m_dbManager->getAvailableCars();
}

QList<Car> CarService::getCarsByBrand(const QString& brand)
{
    if (!m_dbManager) {
        return QList<Car>();
    }
    return m_dbManager->getCarsByBrand(brand);
}

bool CarService::addCar(const Car& car)
{
    if (!m_dbManager) {
        return false;
    }
    return m_dbManager->addCar(car);
}

bool CarService::updateCar(const Car& car)
{
    if (!m_dbManager) {
        return false;
    }
    return m_dbManager->updateCar(car);
}

bool CarService::deleteCar(int carId)
{
    if (!m_dbManager) {
        return false;
    }
    return m_dbManager->deleteCar(carId);
}

