#ifndef REPORTSWINDOW_H
#define REPORTSWINDOW_H

#include <QMainWindow>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include "../managers/reportmanager.h"

class ReportsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReportsWindow(ReportManager* reportManager, QWidget *parent = nullptr);
    ~ReportsWindow();

private slots:
    void onGenerateReport();
    void onRefresh();

private:
    ReportManager* m_reportManager;
    
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    QLabel* m_totalRevenueLabel;
    QLabel* m_totalFinesLabel;
    QLabel* m_totalRentalsLabel;
    QLabel* m_activeRentalsLabel;
    QLabel* m_avgDurationLabel;
    QLabel* m_fleetUtilizationLabel;
    QTableWidget* m_popularCarsTable;
    
    void setupUI();
    void generateReport();
};

#endif // REPORTSWINDOW_H

