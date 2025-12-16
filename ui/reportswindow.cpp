#include "reportswindow.h"
#include "../utils/dateutils.h"
#include <QHeaderView>
#include <QMessageBox>

ReportsWindow::ReportsWindow(ReportManager* reportManager, QWidget *parent)
    : QMainWindow(parent), m_reportManager(reportManager)
{
    setupUI();
    setWindowTitle("Отчеты и статистика");
    resize(800, 600);
    
    // Генерируем отчет за текущий месяц по умолчанию
    QDate today = DateUtils::currentDate();
    m_startDateEdit->setDate(QDate(today.year(), today.month(), 1));
    m_endDateEdit->setDate(today);
    generateReport();
}

ReportsWindow::~ReportsWindow()
{
}

void ReportsWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Период отчета
    QGroupBox* periodGroup = new QGroupBox("Период отчета", this);
    QFormLayout* periodLayout = new QFormLayout(periodGroup);
    
    m_startDateEdit = new QDateEdit(DateUtils::currentDate(), this);
    m_startDateEdit->setCalendarPopup(true);
    periodLayout->addRow("Дата начала:", m_startDateEdit);
    
    m_endDateEdit = new QDateEdit(DateUtils::currentDate(), this);
    m_endDateEdit->setCalendarPopup(true);
    periodLayout->addRow("Дата окончания:", m_endDateEdit);
    
    QPushButton* generateButton = new QPushButton("Сформировать отчет", this);
    connect(generateButton, &QPushButton::clicked, this, &ReportsWindow::onGenerateReport);
    periodLayout->addRow("", generateButton);
    
    mainLayout->addWidget(periodGroup);
    
    // Общая статистика
    QGroupBox* statsGroup = new QGroupBox("Общая статистика", this);
    QFormLayout* statsLayout = new QFormLayout(statsGroup);
    
    m_totalRevenueLabel = new QLabel("Общий доход: 0 руб", this);
    m_totalFinesLabel = new QLabel("Доход от штрафов: 0 руб", this);
    m_totalRentalsLabel = new QLabel("Всего аренд: 0", this);
    m_activeRentalsLabel = new QLabel("Активных аренд: 0", this);
    m_avgDurationLabel = new QLabel("Средняя длительность: 0 дней", this);
    m_fleetUtilizationLabel = new QLabel("Загруженность парка: 0%", this);
    
    statsLayout->addRow(m_totalRevenueLabel);
    statsLayout->addRow(m_totalFinesLabel);
    statsLayout->addRow(m_totalRentalsLabel);
    statsLayout->addRow(m_activeRentalsLabel);
    statsLayout->addRow(m_avgDurationLabel);
    statsLayout->addRow(m_fleetUtilizationLabel);
    
    mainLayout->addWidget(statsGroup);
    
    // Популярные автомобили
    QGroupBox* popularGroup = new QGroupBox("Популярные автомобили", this);
    QVBoxLayout* popularLayout = new QVBoxLayout(popularGroup);
    
    m_popularCarsTable = new QTableWidget(this);
    m_popularCarsTable->setColumnCount(3);
    m_popularCarsTable->setHorizontalHeaderLabels(QStringList() << "Автомобиль" << "Количество аренд" << "Доход");
    m_popularCarsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_popularCarsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_popularCarsTable->horizontalHeader()->setStretchLastSection(true);
    
    popularLayout->addWidget(m_popularCarsTable);
    mainLayout->addWidget(popularGroup);
    
    mainLayout->addStretch();
}

void ReportsWindow::onGenerateReport()
{
    generateReport();
}

void ReportsWindow::onRefresh()
{
    generateReport();
}

void ReportsWindow::generateReport()
{
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    
    if (startDate > endDate) {
        QMessageBox::warning(this, "Ошибка", "Дата начала не может быть позже даты окончания!");
        return;
    }
    
    RevenueReport report = m_reportManager->generateRevenueReport(startDate, endDate);
    
    m_totalRevenueLabel->setText(QString("Общий доход: %1 руб").arg(report.totalRevenue, 0, 'f', 2));
    m_totalFinesLabel->setText(QString("Доход от штрафов: %1 руб").arg(report.totalFines, 0, 'f', 2));
    m_totalRentalsLabel->setText(QString("Всего аренд: %1").arg(report.totalRentals));
    m_activeRentalsLabel->setText(QString("Активных аренд: %1").arg(report.activeRentals));
    m_avgDurationLabel->setText(QString("Средняя длительность: %1 дней").arg(report.averageRentalDuration, 0, 'f', 1));
    m_fleetUtilizationLabel->setText(QString("Загруженность парка: %1%").arg(report.fleetUtilization, 0, 'f', 1));
    
    // Популярные автомобили
    QList<CarStatistics> popularCars = m_reportManager->getPopularCars(startDate, endDate, 10);
    m_popularCarsTable->setRowCount(popularCars.size());
    
    for (int i = 0; i < popularCars.size(); ++i) {
        const CarStatistics& stats = popularCars[i];
        m_popularCarsTable->setItem(i, 0, new QTableWidgetItem(stats.carName));
        m_popularCarsTable->setItem(i, 1, new QTableWidgetItem(QString::number(stats.rentalCount)));
        m_popularCarsTable->setItem(i, 2, new QTableWidgetItem(QString::number(stats.totalRevenue, 'f', 2) + " руб"));
    }
}

