#include "pricingstrategy.h"
#include <QDate>
#include <cmath>

double DailyPricingStrategy::calculateCost(double basePrice, const QDate& startDate, const QDate& endDate) const
{
    int days = startDate.daysTo(endDate) + 1;
    if (days < 1) days = 1;
    return basePrice * days;
}

double WeeklyPricingStrategy::calculateCost(double basePrice, const QDate& startDate, const QDate& endDate) const
{
    int days = startDate.daysTo(endDate) + 1;
    if (days < 1) days = 1;
    
    // Если аренда больше или равна 7 дням, применяем скидку 10%
    if (days >= 7) {
        int weeks = std::ceil(days / 7.0);
        return basePrice * weeks * 7 * 0.9; // Скидка 10%
    }
    
    // Если меньше недели, считаем по дням без скидки
    return basePrice * days;
}

double MonthlyPricingStrategy::calculateCost(double basePrice, const QDate& startDate, const QDate& endDate) const
{
    int days = startDate.daysTo(endDate) + 1;
    if (days < 1) days = 1;
    
    // Если аренда больше или равна 30 дням, применяем скидку 20%
    if (days >= 30) {
        int months = std::ceil(days / 30.0);
        return basePrice * months * 30 * 0.8; // Скидка 20%
    }
    
    // Если меньше месяца, но больше недели - применяем недельную скидку
    if (days >= 7) {
        int weeks = std::ceil(days / 7.0);
        return basePrice * weeks * 7 * 0.9; // Скидка 10%
    }
    
    // Если меньше недели, считаем по дням без скидки
    return basePrice * days;
}

FinePricingStrategy::FinePricingStrategy(double fineMultiplier)
    : m_fineMultiplier(fineMultiplier)
{
}

double FinePricingStrategy::calculateCost(double basePrice, const QDate& startDate, const QDate& endDate) const
{
    // startDate - планируемая дата возврата
    // endDate - фактическая дата возврата
    int overdueDays = startDate.daysTo(endDate);
    
    if (overdueDays <= 0) {
        return 0.0; // Нет просрочки
    }
    
    return basePrice * m_fineMultiplier * overdueDays;
}

