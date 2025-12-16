#ifndef PRICINGSTRATEGY_H
#define PRICINGSTRATEGY_H

#include <QDate>
#include <QString>

// Базовый интерфейс стратегии расчета стоимости
class PricingStrategy
{
public:
    virtual ~PricingStrategy() = default;
    
    // Расчет стоимости аренды
    virtual double calculateCost(double basePrice, const QDate& startDate, const QDate& endDate) const = 0;
    
    // Получить название стратегии
    virtual QString getName() const = 0;
    
    // Получить описание стратегии
    virtual QString getDescription() const = 0;
};

// Стратегия расчета по дням (базовая цена)
class DailyPricingStrategy : public PricingStrategy
{
public:
    double calculateCost(double basePrice, const QDate& startDate, const QDate& endDate) const override;
    QString getName() const override { return "По дням"; }
    QString getDescription() const override { return "Базовая цена × количество дней"; }
};

// Стратегия расчета по неделям (скидка 10%)
class WeeklyPricingStrategy : public PricingStrategy
{
public:
    double calculateCost(double basePrice, const QDate& startDate, const QDate& endDate) const override;
    QString getName() const override { return "По неделям"; }
    QString getDescription() const override { return "Базовая цена × недели × 0.9 (скидка 10%)"; }
};

// Стратегия расчета по месяцам (скидка 20%)
class MonthlyPricingStrategy : public PricingStrategy
{
public:
    double calculateCost(double basePrice, const QDate& startDate, const QDate& endDate) const override;
    QString getName() const override { return "По месяцам"; }
    QString getDescription() const override { return "Базовая цена × месяцы × 0.8 (скидка 20%)"; }
};

// Стратегия расчета штрафа за просрочку (коэффициент 1.5)
class FinePricingStrategy : public PricingStrategy
{
public:
    FinePricingStrategy(double fineMultiplier = 1.5);
    double calculateCost(double basePrice, const QDate& startDate, const QDate& endDate) const override;
    QString getName() const override { return "Штраф за просрочку"; }
    QString getDescription() const override { return QString("Базовая цена × %1 × дни просрочки").arg(m_fineMultiplier); }
    
    void setFineMultiplier(double multiplier) { m_fineMultiplier = multiplier; }
    double getFineMultiplier() const { return m_fineMultiplier; }

private:
    double m_fineMultiplier;
};

#endif // PRICINGSTRATEGY_H

