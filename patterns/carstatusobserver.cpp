#include "carstatusobserver.h"
#include "../database/databasemanager.h"
#include <QDebug>

CarStatusObserver::CarStatusObserver()
    : m_dbManager(nullptr)
{
    m_dbManager = &DatabaseManager::getInstance();
}

void CarStatusObserver::onCarStatusChanged(int carId, CarStatus newStatus)
{
    if (!m_dbManager) {
        qDebug() << "DatabaseManager не инициализирован!";
        return;
    }
    
    Car car = m_dbManager->getCarById(carId);
    if (car.getId() == 0) {
        qDebug() << "Автомобиль с ID" << carId << "не найден!";
        return;
    }
    
    car.setStatus(newStatus);
    if (m_dbManager->updateCar(car)) {
        qDebug() << "Статус автомобиля" << carId << "обновлен на" << car.getStatusString();
    } else {
        qDebug() << "Ошибка обновления статуса автомобиля" << carId;
    }
}

void CarStatusSubject::attachObserver(ICarStatusObserver* observer)
{
    if (observer && !m_observers.contains(observer)) {
        m_observers.append(observer);
    }
}

void CarStatusSubject::detachObserver(ICarStatusObserver* observer)
{
    m_observers.removeAll(observer);
}

void CarStatusSubject::notifyStatusChanged(int carId, CarStatus newStatus)
{
    for (ICarStatusObserver* observer : m_observers) {
        if (observer) {
            observer->onCarStatusChanged(carId, newStatus);
        }
    }
}

