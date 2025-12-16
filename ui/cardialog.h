#ifndef CARDIALOG_H
#define CARDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "../models/car.h"
#include "../services/carservice.h"

class CarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CarDialog(QWidget *parent = nullptr);
    explicit CarDialog(const Car& car, QWidget *parent = nullptr);
    ~CarDialog();

private slots:
    void onAccept();

private:
    Car m_car;
    bool m_isEditMode;
    CarService* m_carService;
    
    QLineEdit* m_brandEdit;
    QLineEdit* m_modelEdit;
    QComboBox* m_statusCombo;
    QDoubleSpinBox* m_priceSpin;
    
    void setupUI();
    void loadCarData();
};

#endif // CARDIALOG_H

