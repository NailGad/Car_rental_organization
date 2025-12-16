#ifndef DATEUTILS_H
#define DATEUTILS_H

#include <QDate>
#include <QDateTime>
#include <QTimeZone>

/**
 * Утилита для работы с датами с учетом временной зоны региона
 * Использует системную временную зону для получения текущей даты
 */
class DateUtils
{
public:
    /**
     * Получает текущую дату в системной временной зоне региона
     * Вместо использования даты ПК, использует всемирное время (UTC)
     * с учетом временной зоны, где запущено приложение
     * @return Текущая дата в системной временной зоне
     */
    static QDate currentDate();
    
    /**
     * Получает текущую дату и время в системной временной зоне региона
     * @return Текущая дата и время в системной временной зоне
     */
    static QDateTime currentDateTime();
    
    /**
     * Получает системную временную зону
     * @return Системная временная зона
     */
    static QTimeZone systemTimeZone();

private:
    DateUtils() = default; // Утилитный класс, не создается
};

#endif // DATEUTILS_H

