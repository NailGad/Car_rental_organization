#ifndef RENTALDIALOG_H
#define RENTALDIALOG_H

#include <QDialog>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include "../models/car.h"
#include "../services/pricingcalculator.h"
#include "../services/rentalservice.h"

class RentalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RentalDialog(const Car& car, int userId, PricingCalculator* calculator, QWidget *parent = nullptr);
    ~RentalDialog();

private slots:
    void onDateChanged();
    void onAccept();

private:
    Car m_car;
    int m_userId;
    PricingCalculator* m_pricingCalculator;
    RentalService* m_rentalService;
    
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    QLabel* m_daysLabel;
    QLabel* m_costLabel;
    QLabel* m_calculationInfoLabel;
    
    void setupUI();
    void calculateCost();
    int getDays() const;
};

#endif // RENTALDIALOG_H

