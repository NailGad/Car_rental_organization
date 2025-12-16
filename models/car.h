#ifndef CAR_H
#define CAR_H

#include <QString>

enum class CarStatus {
    Available,      // Доступен
    Rented,         // В аренде
    Maintenance,    // На обслуживании
    Reserved        // Зарезервирован
};

class Car
{
public:
    Car();
    Car(int id, const QString& brand, const QString& model, 
        CarStatus status, double dailyPrice);
    
    // Getters
    int getId() const { return m_id; }
    QString getBrand() const { return m_brand; }
    QString getModel() const { return m_model; }
    CarStatus getStatus() const { return m_status; }
    double getDailyPrice() const { return m_dailyPrice; }
    
    // Setters
    void setId(int id) { m_id = id; }
    void setBrand(const QString& brand) { m_brand = brand; }
    void setModel(const QString& model) { m_model = model; }
    void setStatus(CarStatus status) { m_status = status; }
    void setDailyPrice(double price) { m_dailyPrice = price; }
    
    // Helpers
    QString getStatusString() const;
    QString getFullName() const { return m_brand + " " + m_model; }
    static CarStatus statusFromString(const QString& status);

private:
    int m_id;
    QString m_brand;
    QString m_model;
    CarStatus m_status;
    double m_dailyPrice;
};

#endif // CAR_H

