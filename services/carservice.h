#ifndef CARSERVICE_H
#define CARSERVICE_H

#include "../models/car.h"
#include <QList>

class DatabaseManager;

class CarService
{
public:
    CarService();
    
    // Получить все автомобили
    QList<Car> getAllCars();
    
    // Получить автомобиль по ID
    Car getCarById(int carId);
    
    // Получить доступные автомобили
    QList<Car> getAvailableCars();
    
    // Поиск автомобилей по марке
    QList<Car> getCarsByBrand(const QString& brand);
    
    // Добавить автомобиль
    bool addCar(const Car& car);
    
    // Обновить автомобиль
    bool updateCar(const Car& car);
    
    // Удалить автомобиль
    bool deleteCar(int carId);

private:
    DatabaseManager* m_dbManager;
};

#endif // CARSERVICE_H

