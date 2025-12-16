#ifndef RENTAL_H
#define RENTAL_H

#include <QDate>

class Rental
{
public:
    Rental();
    Rental(int id, int carId, int userId, const QDate& startDate, 
           const QDate& endDate, double totalCost, bool isCompleted);
    
    // Getters
    int getId() const { return m_id; }
    int getCarId() const { return m_carId; }
    int getUserId() const { return m_userId; }
    QDate getStartDate() const { return m_startDate; }
    QDate getEndDate() const { return m_endDate; }
    QDate getActualReturnDate() const { return m_actualReturnDate; }
    double getTotalCost() const { return m_totalCost; }
    bool isCompleted() const { return m_isCompleted; }
    
    // Setters
    void setId(int id) { m_id = id; }
    void setCarId(int carId) { m_carId = carId; }
    void setUserId(int userId) { m_userId = userId; }
    void setStartDate(const QDate& date) { m_startDate = date; }
    void setEndDate(const QDate& date) { m_endDate = date; }
    void setActualReturnDate(const QDate& date) { m_actualReturnDate = date; }
    void setTotalCost(double cost) { m_totalCost = cost; }
    void setCompleted(bool completed) { m_isCompleted = completed; }
    
    // Helpers
    int getDaysRented() const;
    int getDaysOverdue() const;
    bool isActive() const { return !m_isCompleted; }

private:
    int m_id;
    int m_carId;
    int m_userId;
    QDate m_startDate;
    QDate m_endDate;
    QDate m_actualReturnDate;
    double m_totalCost;
    bool m_isCompleted;
};

#endif // RENTAL_H

