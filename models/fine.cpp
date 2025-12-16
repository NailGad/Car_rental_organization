#include "fine.h"
#include "../utils/dateutils.h"

Fine::Fine()
    : m_id(0), m_rentalId(0), m_amount(0.0), m_date(DateUtils::currentDate()), m_reason("")
{
}

Fine::Fine(int id, int rentalId, double amount, const QDate& date, 
           const QString& reason)
    : m_id(id), m_rentalId(rentalId), m_amount(amount), m_date(date), m_reason(reason)
{
}

