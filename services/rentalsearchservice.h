#ifndef RENTALSEARCHSERVICE_H
#define RENTALSEARCHSERVICE_H

#include "../models/rental.h"
#include <QList>
#include <QDate>
#include <QString>

class DatabaseManager;

class RentalSearchService
{
public:
    RentalSearchService();
    
    // Универсальный поиск аренд по любым комбинациям параметров
    // Если параметр пустой/невалидный, он не применяется
    QList<Rental> searchRentals(const QString& clientName = QString(), 
                                 const QDate& dateFrom = QDate(),
                                 const QDate& dateTo = QDate(),
                                 const QString& carBrand = QString());

private:
    DatabaseManager* m_dbManager;
    
    // Найти пересечение списков аренд
    QList<Rental> intersectRentalLists(const QList<QList<Rental>>& lists);
};

#endif // RENTALSEARCHSERVICE_H

