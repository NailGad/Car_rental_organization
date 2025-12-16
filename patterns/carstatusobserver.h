#ifndef CARSTATUSOBSERVER_H
#define CARSTATUSOBSERVER_H

#include "../models/car.h"
#include <QList>

// Интерфейс наблюдателя для обновления статуса автомобиля
class ICarStatusObserver
{
public:
    virtual ~ICarStatusObserver() = default;
    virtual void onCarStatusChanged(int carId, CarStatus newStatus) = 0;
};

// Конкретный наблюдатель - обновляет статус автомобиля в базе данных
class CarStatusObserver : public ICarStatusObserver
{
public:
    CarStatusObserver();
    void onCarStatusChanged(int carId, CarStatus newStatus) override;

private:
    class DatabaseManager* m_dbManager;
};

// Субъект - класс, который уведомляет наблюдателей об изменении статуса
class CarStatusSubject
{
public:
    void attachObserver(ICarStatusObserver* observer);
    void detachObserver(ICarStatusObserver* observer);
    void notifyStatusChanged(int carId, CarStatus newStatus);

private:
    QList<ICarStatusObserver*> m_observers;
};

#endif // CARSTATUSOBSERVER_H

