#include "rentaldialog.h"
#include "../utils/dateutils.h"
#include <QMessageBox>

RentalDialog::RentalDialog(const Car& car, int userId, PricingCalculator* calculator, QWidget *parent)
    : QDialog(parent), m_car(car), m_userId(userId), m_pricingCalculator(calculator)
{
    m_rentalService = new RentalService();
    setupUI();
    setWindowTitle("Оформление аренды");
    setModal(true);
    resize(450, 350);
}

RentalDialog::~RentalDialog()
{
    delete m_rentalService;
}

void RentalDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Информация об автомобиле
    QGroupBox* carGroup = new QGroupBox("Автомобиль", this);
    QFormLayout* carLayout = new QFormLayout(carGroup);
    carLayout->addRow("Марка:", new QLabel(m_car.getBrand(), this));
    carLayout->addRow("Модель:", new QLabel(m_car.getModel(), this));
    carLayout->addRow("Цена за день:", new QLabel(QString::number(m_car.getDailyPrice(), 'f', 2) + " руб", this));
    mainLayout->addWidget(carGroup);
    
    // Даты аренды
    QGroupBox* datesGroup = new QGroupBox("Период аренды", this);
    QFormLayout* datesLayout = new QFormLayout(datesGroup);
    
    m_startDateEdit = new QDateEdit(DateUtils::currentDate(), this);
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setMinimumDate(DateUtils::currentDate());
    datesLayout->addRow("Дата начала:", m_startDateEdit);
    
    m_endDateEdit = new QDateEdit(DateUtils::currentDate().addDays(1), this);
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setMinimumDate(DateUtils::currentDate().addDays(1));
    datesLayout->addRow("Дата окончания:", m_endDateEdit);
    
    connect(m_startDateEdit, &QDateEdit::dateChanged, this, &RentalDialog::onDateChanged);
    connect(m_endDateEdit, &QDateEdit::dateChanged, this, &RentalDialog::onDateChanged);
    
    mainLayout->addWidget(datesGroup);
    
    // Расчет стоимости
    QGroupBox* costGroup = new QGroupBox("Расчет стоимости", this);
    QVBoxLayout* costLayout = new QVBoxLayout(costGroup);
    
    m_daysLabel = new QLabel("Количество дней: 1", this);
    costLayout->addWidget(m_daysLabel);
    
    m_calculationInfoLabel = new QLabel("", this);
    m_calculationInfoLabel->setWordWrap(true);
    costLayout->addWidget(m_calculationInfoLabel);
    
    m_costLabel = new QLabel("ИТОГО: 0 руб", this);
    QFont costFont = m_costLabel->font();
    costFont.setPointSize(12);
    costFont.setBold(true);
    m_costLabel->setFont(costFont);
    costLayout->addWidget(m_costLabel);
    
    mainLayout->addWidget(costGroup);
    
    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* cancelButton = new QPushButton("Отмена", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    QPushButton* acceptButton = new QPushButton("Оформить", this);
    acceptButton->setDefault(true);
    connect(acceptButton, &QPushButton::clicked, this, &RentalDialog::onAccept);
    buttonLayout->addWidget(acceptButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Первоначальный расчет
    onDateChanged();
}

void RentalDialog::onDateChanged()
{
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    if (endDate < startDate) {
        m_endDateEdit->setDate(startDate.addDays(1));
        endDate = m_endDateEdit->date();
    }
    
    m_endDateEdit->setMinimumDate(startDate.addDays(1));
    
    calculateCost();
}

void RentalDialog::calculateCost()
{
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    int days = getDays();
    m_daysLabel->setText(QString("Количество дней: %1").arg(days));
    
    // Автоматически выбираем оптимальную стратегию
    m_pricingCalculator->selectOptimalStrategy(startDate, endDate);
    
    double cost = m_pricingCalculator->calculateRentalCost(m_car, startDate, endDate);
    m_costLabel->setText(QString("ИТОГО: %1 руб").arg(cost, 0, 'f', 2));
    
    QString info = m_pricingCalculator->getCalculationInfo();
    m_calculationInfoLabel->setText(info);
}

int RentalDialog::getDays() const
{
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    return startDate.daysTo(endDate) + 1;
}

void RentalDialog::onAccept()
{
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    if (endDate <= startDate) {
        QMessageBox::warning(this, "Ошибка", "Дата окончания должна быть позже даты начала!");
        return;
    }
    
    // Проверяем доступность
    if (!m_rentalService->isCarAvailable(m_car.getId(), startDate, endDate)) {
        QMessageBox::warning(this, "Ошибка", "Автомобиль недоступен в указанный период!");
        return;
    }
    
    // Рассчитываем стоимость
    m_pricingCalculator->selectOptimalStrategy(startDate, endDate);
    double cost = m_pricingCalculator->calculateRentalCost(m_car, startDate, endDate);
    
    // Создаем аренду
    if (m_rentalService->createRental(m_car.getId(), m_userId, startDate, endDate, cost)) {
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось оформить аренду!");
    }
}

