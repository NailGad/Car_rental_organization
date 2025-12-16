#include "car.h"

Car::Car()
    : m_id(0), m_brand(""), m_model(""), m_status(CarStatus::Available), m_dailyPrice(0.0)
{
}

Car::Car(int id, const QString& brand, const QString& model, 
         CarStatus status, double dailyPrice)
    : m_id(id), m_brand(brand), m_model(model), m_status(status), m_dailyPrice(dailyPrice)
{
}

QString Car::getStatusString() const
{
    switch (m_status) {
    case CarStatus::Available:
        return "Доступен";
    case CarStatus::Rented:
        return "В аренде";
    case CarStatus::Maintenance:
        return "На обслуживании";
    case CarStatus::Reserved:
        return "Зарезервирован";
    default:
        return "Неизвестно";
    }
}

CarStatus Car::statusFromString(const QString& status)
{
    if (status == "Доступен" || status == "Available") return CarStatus::Available;
    if (status == "В аренде" || status == "Rented") return CarStatus::Rented;
    if (status == "На обслуживании" || status == "Maintenance") return CarStatus::Maintenance;
    if (status == "Зарезервирован" || status == "Reserved") return CarStatus::Reserved;
    return CarStatus::Available;
}

