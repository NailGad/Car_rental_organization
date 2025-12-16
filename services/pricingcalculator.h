#ifndef PRICINGCALCULATOR_H
#define PRICINGCALCULATOR_H

#include "../patterns/pricingstrategy.h"
#include "../models/car.h"
#include <QDate>

class PricingCalculator
{
public:
    PricingCalculator();
    ~PricingCalculator();
    
    // Установить стратегию расчета
    void setStrategy(PricingStrategy* strategy);
    
    // Рассчитать стоимость аренды
    double calculateRentalCost(const Car& car, const QDate& startDate, const QDate& endDate);
    
    // Автоматически выбрать оптимальную стратегию на основе периода аренды
    PricingStrategy* selectOptimalStrategy(const QDate& startDate, const QDate& endDate);
    
    // Получить информацию о расчете
    QString getCalculationInfo() const;
    
    // Получить текущую стратегию
    PricingStrategy* getCurrentStrategy() const { return m_strategy; }

private:
    PricingStrategy* m_strategy;
    DailyPricingStrategy* m_dailyStrategy;
    WeeklyPricingStrategy* m_weeklyStrategy;
    MonthlyPricingStrategy* m_monthlyStrategy;
    FinePricingStrategy* m_fineStrategy;
    
    QString m_lastCalculationInfo;
};

#endif // PRICINGCALCULATOR_H

