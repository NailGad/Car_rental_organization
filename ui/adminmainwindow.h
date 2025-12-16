#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QDateEdit>
#include "../models/user.h"
#include "../models/car.h"
#include "../models/rental.h"
#include "../database/databasemanager.h"
#include "../services/rentalservice.h"
#include "../services/carservice.h"
#include "../services/userservice.h"
#include "../services/rentalsearchservice.h"
#include "../managers/reportmanager.h"

class AdminMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminMainWindow(const User& user, QWidget *parent = nullptr);
    ~AdminMainWindow();

private slots:
    void onAddCar();
    void onEditCar();
    void onDeleteCar();
    void onCompleteRental();
    void onShowReports();
    void onShowUsers();
    void onDeleteUser();
    void onImportExport();
    void onLogout();
    void onSearchRentals();

private:
    User m_user;
    DatabaseManager* m_dbManager;
    RentalService* m_rentalService;
    CarService* m_carService;
    UserService* m_userService;
    RentalSearchService* m_rentalSearchService;
    ReportManager* m_reportManager;
    
    // UI элементы
    QTabWidget* m_tabWidget;
    
    // Вкладка "Автомобили"
    QWidget* m_carsTab;
    QPushButton* m_addCarButton;
    QPushButton* m_editCarButton;
    QPushButton* m_deleteCarButton;
    QTableWidget* m_carsTable;
    
    // Вкладка "Аренды"
    QWidget* m_rentalsTab;
    QPushButton* m_completeRentalButton;
    QTableWidget* m_rentalsTable;
    QLineEdit* m_searchClientEdit;
    QDateEdit* m_searchDateFromEdit;
    QDateEdit* m_searchDateToEdit;
    QLineEdit* m_searchBrandEdit;
    QPushButton* m_searchRentalsButton;
    QPushButton* m_clearSearchButton;
    
    // Вкладка "Статистика"
    QWidget* m_statsTab;
    QLabel* m_totalCarsLabel;
    QLabel* m_availableCarsLabel;
    QLabel* m_rentedCarsLabel;
    QLabel* m_activeRentalsLabel;
    
    // Вкладка "Пользователи"
    QWidget* m_usersTab;
    QTableWidget* m_usersTable;
    QPushButton* m_deleteUserButton;
    
    // Статус-бар
    QLabel* m_dateLabel;
    
    void setupUI();
    void updateDateLabel();
    void setupCarsTab();
    void setupRentalsTab();
    void setupStatsTab();
    void setupUsersTab();
    void loadCars();
    void loadRentals();
    void loadUsers();
    void updateCarsTable(const QList<Car>& cars);
    void updateRentalsTable(const QList<Rental>& rentals);
    void updateUsersTable(const QList<User>& users);
    void updateStatistics();
    int getSelectedCarId();
    int getSelectedRentalId();
    int getSelectedUserId();
};

#endif // ADMINMAINWINDOW_H

