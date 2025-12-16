#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "../models/user.h"
#include "../models/car.h"
#include "../models/rental.h"
#include "../services/rentalservice.h"
#include "../services/pricingcalculator.h"
#include "../services/carservice.h"
#include "../database/databasemanager.h"

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientMainWindow(const User& user, QWidget *parent = nullptr);
    ~ClientMainWindow();

private slots:
    void onSearchCars();
    void onRentCar();
    void onCompleteRental();
    void onLogout();

private:
    User m_user;
    DatabaseManager* m_dbManager;
    RentalService* m_rentalService;
    PricingCalculator* m_pricingCalculator;
    CarService* m_carService;
    
    // UI элементы
    QTabWidget* m_tabWidget;
    
    // Вкладка "Доступные автомобили"
    QWidget* m_carsTab;
    QLineEdit* m_searchEdit;
    QPushButton* m_searchButton;
    QTableWidget* m_carsTable;
    
    // Вкладка "Мои аренды"
    QWidget* m_rentalsTab;
    QPushButton* m_completeRentalButton;
    QTableWidget* m_rentalsTable;
    
    // Статус-бар
    QLabel* m_dateLabel;
    
    void setupUI();
    void updateDateLabel();
    void setupCarsTab();
    void setupRentalsTab();
    void loadAvailableCars();
    void loadUserRentals();
    void updateCarsTable(const QList<Car>& cars);
    void updateRentalsTable(const QList<Rental>& rentals);
    int getSelectedCarId();
    int getSelectedRentalId();
};

#endif // CLIENTMAINWINDOW_H

