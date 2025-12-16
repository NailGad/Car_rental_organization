#include "dateutils.h"
#include <QDateTime>

QDate DateUtils::currentDate()
{
    // Получаем текущее время в UTC
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    
    // Получаем системную временную зону
    QTimeZone timeZone = QTimeZone::systemTimeZone();
    
    // Конвертируем UTC время в локальное время региона
    QDateTime localDateTime = utcDateTime.toTimeZone(timeZone);
    
    // Возвращаем только дату
    return localDateTime.date();
}

QDateTime DateUtils::currentDateTime()
{
    // Получаем текущее время в UTC
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    
    // Получаем системную временную зону
    QTimeZone timeZone = QTimeZone::systemTimeZone();
    
    // Конвертируем UTC время в локальное время региона
    return utcDateTime.toTimeZone(timeZone);
}

QTimeZone DateUtils::systemTimeZone()
{
    return QTimeZone::systemTimeZone();
}

