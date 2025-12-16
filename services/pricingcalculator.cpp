#include "pricingcalculator.h"
#include <QDebug>

PricingCalculator::PricingCalculator()
    : m_strategy(nullptr)
{
    // Создаем все стратегии
    m_dailyStrategy = new DailyPricingStrategy();
    m_weeklyStrategy = new WeeklyPricingStrategy();
    m_monthlyStrategy = new MonthlyPricingStrategy();
    m_fineStrategy = new FinePricingStrategy(1.5);
    
    // По умолчанию используем дневную стратегию
    m_strategy = m_dailyStrategy;
}

PricingCalculator::~PricingCalculator()
{
    // Не удаляем стратегии, так как они используются повторно
    // В реальном приложении можно использовать умные указатели
}

void PricingCalculator::setStrategy(PricingStrategy* strategy)
{
    if (strategy) {
        m_strategy = strategy;
    }
}

double PricingCalculator::calculateRentalCost(const Car& car, const QDate& startDate, const QDate& endDate)
{
    if (!m_strategy) {
        qDebug() << "Стратегия не установлена!";
        return 0.0;
    }
    
    int days = startDate.daysTo(endDate) + 1;
    double cost = m_strategy->calculateCost(car.getDailyPrice(), startDate, endDate);
    
    // Формируем информацию о расчете
    m_lastCalculationInfo = QString("Расчет по стратегии: %1\n"
                                   "Автомобиль: %2 %3\n"
                                   "Цена за день: %4 руб\n"
                                   "Период: %5 - %6 (%7 дней)\n"
                                   "Итого: %8 руб")
                                   .arg(m_strategy->getName())
                                   .arg(car.getBrand())
                                   .arg(car.getModel())
                                   .arg(car.getDailyPrice())
                                   .arg(startDate.toString("dd.MM.yyyy"))
                                   .arg(endDate.toString("dd.MM.yyyy"))
                                   .arg(days)
                                   .arg(cost);
    
    return cost;
}

PricingStrategy* PricingCalculator::selectOptimalStrategy(const QDate& startDate, const QDate& endDate)
{
    int days = startDate.daysTo(endDate) + 1;
    
    if (days >= 30) {
        // Аренда на месяц и более - используем месячную стратегию
        m_strategy = m_monthlyStrategy;
        return m_monthlyStrategy;
    } else if (days >= 7) {
        // Аренда на неделю и более - используем недельную стратегию
        m_strategy = m_weeklyStrategy;
        return m_weeklyStrategy;
    } else {
        // Аренда менее недели - используем дневную стратегию
        m_strategy = m_dailyStrategy;
        return m_dailyStrategy;
    }
}

QString PricingCalculator::getCalculationInfo() const
{
    return m_lastCalculationInfo;
}

