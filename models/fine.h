#ifndef FINE_H
#define FINE_H

#include <QDate>
#include <QString>

class Fine
{
public:
    Fine();
    Fine(int id, int rentalId, double amount, const QDate& date, 
         const QString& reason);
    
    // Getters
    int getId() const { return m_id; }
    int getRentalId() const { return m_rentalId; }
    double getAmount() const { return m_amount; }
    QDate getDate() const { return m_date; }
    QString getReason() const { return m_reason; }
    
    // Setters
    void setId(int id) { m_id = id; }
    void setRentalId(int rentalId) { m_rentalId = rentalId; }
    void setAmount(double amount) { m_amount = amount; }
    void setDate(const QDate& date) { m_date = date; }
    void setReason(const QString& reason) { m_reason = reason; }

private:
    int m_id;
    int m_rentalId;
    double m_amount;
    QDate m_date;
    QString m_reason;
};

#endif // FINE_H

