#include "rentalsearchservice.h"
#include "../database/databasemanager.h"
#include <QSet>

RentalSearchService::RentalSearchService()
    : m_dbManager(nullptr)
{
    m_dbManager = &DatabaseManager::getInstance();
}

QList<Rental> RentalSearchService::intersectRentalLists(const QList<QList<Rental>>& lists)
{
    if (lists.isEmpty()) {
        return QList<Rental>();
    }
    
    if (lists.size() == 1) {
        return lists[0];
    }
    
    // Находим пересечение всех результатов
    // Начинаем с первого списка
    QSet<int> commonIds;
    for (const Rental& r : lists[0]) {
        commonIds.insert(r.getId());
    }
    
    // Пересекаем с остальными списками
    for (int i = 1; i < lists.size(); ++i) {
        QSet<int> currentIds;
        for (const Rental& r : lists[i]) {
            currentIds.insert(r.getId());
        }
        // Оставляем только те ID, которые есть в обоих множествах
        commonIds.intersect(currentIds);
    }
    
    // Собираем финальный список аренд по общим ID
    // Используем первый список как источник данных
    QList<Rental> result;
    for (const Rental& r : lists[0]) {
        if (commonIds.contains(r.getId())) {
            result.append(r);
        }
    }
    
    return result;
}

QList<Rental> RentalSearchService::searchRentals(const QString& clientName, 
                                                  const QDate& dateFrom,
                                                  const QDate& dateTo,
                                                  const QString& carBrand)
{
    if (!m_dbManager) {
        return QList<Rental>();
    }
    
    QList<QList<Rental>> resultsLists;
    
    // Собираем результаты по каждому заполненному критерию
    if (!clientName.isEmpty()) {
        resultsLists.append(m_dbManager->searchRentalsByClientName(clientName));
    }
    
    // Проверяем диапазон дат
    bool dateFromValid = dateFrom.isValid();
    bool dateToValid = dateTo.isValid();
    
    if (dateFromValid && dateToValid) {
        // Если обе даты валидны - используем диапазон
        if (dateFrom <= dateTo) {
            resultsLists.append(m_dbManager->searchRentalsByDateRange(dateFrom, dateTo));
        } else {
            // Если даты перепутаны - меняем местами
            resultsLists.append(m_dbManager->searchRentalsByDateRange(dateTo, dateFrom));
        }
    } else if (dateFromValid) {
        // Если только начальная дата - ищем от этой даты до будущего
        resultsLists.append(m_dbManager->searchRentalsByDateRange(dateFrom, QDate(2099, 12, 31)));
    } else if (dateToValid) {
        // Если только конечная дата - ищем от прошлого до этой даты
        resultsLists.append(m_dbManager->searchRentalsByDateRange(QDate(1900, 1, 1), dateTo));
    }
    
    if (!carBrand.isEmpty()) {
        resultsLists.append(m_dbManager->searchRentalsByCarBrand(carBrand));
    }
    
    // Если нет критериев - возвращаем все аренды
    if (resultsLists.isEmpty()) {
        return m_dbManager->getAllRentals();
    }
    
    // Находим пересечение результатов
    return intersectRentalLists(resultsLists);
}

