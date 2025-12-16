#include "cardialog.h"
#include "../services/carservice.h"
#include <QMessageBox>

CarDialog::CarDialog(QWidget *parent)
    : QDialog(parent), m_isEditMode(false)
{
    m_carService = new CarService();
    setupUI();
    setWindowTitle("Добавление автомобиля");
}

CarDialog::CarDialog(const Car& car, QWidget *parent)
    : QDialog(parent), m_car(car), m_isEditMode(true)
{
    m_carService = new CarService();
    setupUI();
    loadCarData();
    setWindowTitle("Редактирование автомобиля");
}

CarDialog::~CarDialog()
{
    delete m_carService;
}

void CarDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QFormLayout* formLayout = new QFormLayout();
    
    m_brandEdit = new QLineEdit(this);
    m_brandEdit->setPlaceholderText("Например: Toyota");
    formLayout->addRow("Марка:", m_brandEdit);
    
    m_modelEdit = new QLineEdit(this);
    m_modelEdit->setPlaceholderText("Например: Camry");
    formLayout->addRow("Модель:", m_modelEdit);
    
    m_statusCombo = new QComboBox(this);
    m_statusCombo->addItem("Доступен", static_cast<int>(CarStatus::Available));
    m_statusCombo->addItem("В аренде", static_cast<int>(CarStatus::Rented));
    m_statusCombo->addItem("На обслуживании", static_cast<int>(CarStatus::Maintenance));
    m_statusCombo->addItem("Зарезервирован", static_cast<int>(CarStatus::Reserved));
    formLayout->addRow("Статус:", m_statusCombo);
    
    m_priceSpin = new QDoubleSpinBox(this);
    m_priceSpin->setMinimum(0.0);
    m_priceSpin->setMaximum(1000000.0);
    m_priceSpin->setDecimals(2);
    m_priceSpin->setSuffix(" руб");
    formLayout->addRow("Цена за день:", m_priceSpin);
    
    mainLayout->addLayout(formLayout);
    
    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* cancelButton = new QPushButton("Отмена", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    QPushButton* acceptButton = new QPushButton(m_isEditMode ? "Сохранить" : "Добавить", this);
    acceptButton->setDefault(true);
    connect(acceptButton, &QPushButton::clicked, this, &CarDialog::onAccept);
    buttonLayout->addWidget(acceptButton);
    
    mainLayout->addLayout(buttonLayout);
    
    resize(400, 200);
}

void CarDialog::loadCarData()
{
    m_brandEdit->setText(m_car.getBrand());
    m_modelEdit->setText(m_car.getModel());
    m_priceSpin->setValue(m_car.getDailyPrice());
    
    // Устанавливаем текущий статус
    int index = m_statusCombo->findData(static_cast<int>(m_car.getStatus()));
    if (index >= 0) {
        m_statusCombo->setCurrentIndex(index);
    }
}

void CarDialog::onAccept()
{
    if (m_brandEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите марку автомобиля!");
        m_brandEdit->setFocus();
        return;
    }
    
    if (m_modelEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите модель автомобиля!");
        m_modelEdit->setFocus();
        return;
    }
    
    if (m_priceSpin->value() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Цена должна быть больше нуля!");
        m_priceSpin->setFocus();
        return;
    }
    
    m_car.setBrand(m_brandEdit->text().trimmed());
    m_car.setModel(m_modelEdit->text().trimmed());
    m_car.setStatus(static_cast<CarStatus>(m_statusCombo->currentData().toInt()));
    m_car.setDailyPrice(m_priceSpin->value());
    
    bool success = false;
    if (m_isEditMode) {
        success = m_carService->updateCar(m_car);
    } else {
        success = m_carService->addCar(m_car);
    }
    
    if (success) {
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка", 
                              m_isEditMode ? "Не удалось обновить автомобиль!" : "Не удалось добавить автомобиль!");
    }
}

