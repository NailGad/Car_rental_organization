#include "rental.h"
#include "../utils/dateutils.h"

Rental::Rental()
    : m_id(0), m_carId(0), m_userId(0), m_totalCost(0.0), m_isCompleted(false)
{
    m_startDate = DateUtils::currentDate();
    m_endDate = DateUtils::currentDate();
    m_actualReturnDate = QDate();
}

Rental::Rental(int id, int carId, int userId, const QDate& startDate, 
               const QDate& endDate, double totalCost, bool isCompleted)
    : m_id(id), m_carId(carId), m_userId(userId), m_startDate(startDate), 
      m_endDate(endDate), m_totalCost(totalCost), m_isCompleted(isCompleted)
{
    m_actualReturnDate = QDate();
}

int Rental::getDaysRented() const
{
    QDate end = m_actualReturnDate.isValid() ? m_actualReturnDate : DateUtils::currentDate();
    return m_startDate.daysTo(end) + 1;
}

int Rental::getDaysOverdue() const
{
    if (m_isCompleted) {
        return 0;
    }
    int overdue = DateUtils::currentDate().daysTo(m_endDate);
    return overdue < 0 ? -overdue : 0;
}

