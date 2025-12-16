#include "adminmainwindow.h"
#include "cardialog.h"
#include "reportswindow.h"
#include "importexportdialog.h"
#include "../services/carservice.h"
#include "../services/userservice.h"
#include "../services/rentalsearchservice.h"
#include "../utils/dateutils.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QGridLayout>
#include <QTimer>
#include <QFormLayout>
#include <QGroupBox>
#include <QDateEdit>

AdminMainWindow::AdminMainWindow(const User& user, QWidget *parent)
    : QMainWindow(parent), m_user(user)
{
    m_dbManager = &DatabaseManager::getInstance();
    m_rentalService = new RentalService();
    m_carService = new CarService();
    m_userService = new UserService();
    m_rentalSearchService = new RentalSearchService();
    m_reportManager = new ReportManager();
    
    setupUI();
    
    // Проверяем просроченные аренды при открытии окна
    m_rentalService->checkAndApplyOverdueFines();
    
    loadCars();
    loadRentals();
    updateStatistics();
    loadUsers();
    
    setWindowTitle(QString("Агентство аренды - Администратор: %1").arg(m_user.getUsername()));
    resize(1000, 700);
}

AdminMainWindow::~AdminMainWindow()
{
    delete m_rentalService;
    delete m_carService;
    delete m_userService;
    delete m_rentalSearchService;
    delete m_reportManager;
}

void AdminMainWindow::setupUI()
{
    // Меню
    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->addMenu("Файл");
    QAction* reportsAction = fileMenu->addAction("Отчеты");
    QAction* usersAction = fileMenu->addAction("Пользователи");
    QAction* importExportAction = fileMenu->addAction("Импорт/Экспорт");
    QAction* logoutAction = fileMenu->addAction("Выход");
    connect(reportsAction, &QAction::triggered, this, &AdminMainWindow::onShowReports);
    connect(usersAction, &QAction::triggered, this, &AdminMainWindow::onShowUsers);
    connect(importExportAction, &QAction::triggered, this, &AdminMainWindow::onImportExport);
    connect(logoutAction, &QAction::triggered, this, &AdminMainWindow::onLogout);
    
    // Статус бар
    statusBar()->showMessage(QString("Администратор: %1").arg(m_user.getUsername()));
    
    // Добавляем отображение текущей даты в статус-бар
    m_dateLabel = new QLabel(this);
    m_dateLabel->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 3px 8px; border-radius: 3px; }");
    updateDateLabel();
    statusBar()->addPermanentWidget(m_dateLabel);
    
    // Обновляем дату каждую минуту
    QTimer* dateTimer = new QTimer(this);
    connect(dateTimer, &QTimer::timeout, this, &AdminMainWindow::updateDateLabel);
    dateTimer->start(60000); // Обновление каждую минуту
    
    // Центральный виджет
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);
    
    setupCarsTab();
    setupRentalsTab();
    setupStatsTab();
    setupUsersTab();

    // Автообновление списка пользователей при переходе на вкладку
    connect(m_tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (m_usersTab && m_tabWidget->widget(index) == m_usersTab) {
            loadUsers();
        }
    });
}

void AdminMainWindow::setupCarsTab()
{
    m_carsTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_carsTab);
    
    // Кнопки управления
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_addCarButton = new QPushButton("Добавить автомобиль");
    m_editCarButton = new QPushButton("Редактировать");
    m_deleteCarButton = new QPushButton("Удалить");
    
    connect(m_addCarButton, &QPushButton::clicked, this, &AdminMainWindow::onAddCar);
    connect(m_editCarButton, &QPushButton::clicked, this, &AdminMainWindow::onEditCar);
    connect(m_deleteCarButton, &QPushButton::clicked, this, &AdminMainWindow::onDeleteCar);
    
    buttonLayout->addWidget(m_addCarButton);
    buttonLayout->addWidget(m_editCarButton);
    buttonLayout->addWidget(m_deleteCarButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // Таблица автомобилей
    m_carsTable = new QTableWidget();
    m_carsTable->setColumnCount(5);
    m_carsTable->setHorizontalHeaderLabels(QStringList() << "ID" << "Марка" << "Модель" << "Статус" << "Цена/день");
    m_carsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_carsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_carsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_carsTable->horizontalHeader()->setStretchLastSection(true);
    
    layout->addWidget(m_carsTable);
    
    m_tabWidget->addTab(m_carsTab, "Автомобили");
}

void AdminMainWindow::setupRentalsTab()
{
    m_rentalsTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_rentalsTab);
    
    // Панель поиска
    QGroupBox* searchGroup = new QGroupBox("Поиск аренд", this);
    QFormLayout* searchLayout = new QFormLayout(searchGroup);
    
    m_searchClientEdit = new QLineEdit(this);
    m_searchClientEdit->setPlaceholderText("Логин клиента (необязательно)");
    searchLayout->addRow("По клиенту:", m_searchClientEdit);
    
    // Диапазон дат
    QHBoxLayout* dateRangeLayout = new QHBoxLayout();
    m_searchDateFromEdit = new QDateEdit(this);
    m_searchDateFromEdit->setCalendarPopup(true);
    m_searchDateFromEdit->setDate(QDate());
    m_searchDateFromEdit->setSpecialValueText("С даты");
    m_searchDateFromEdit->setDisplayFormat("dd.MM.yyyy");
    dateRangeLayout->addWidget(m_searchDateFromEdit);
    
    QLabel* dateSeparator = new QLabel("—", this);
    dateSeparator->setAlignment(Qt::AlignCenter);
    dateRangeLayout->addWidget(dateSeparator);
    
    m_searchDateToEdit = new QDateEdit(this);
    m_searchDateToEdit->setCalendarPopup(true);
    m_searchDateToEdit->setDate(QDate());
    m_searchDateToEdit->setSpecialValueText("По дату");
    m_searchDateToEdit->setDisplayFormat("dd.MM.yyyy");
    dateRangeLayout->addWidget(m_searchDateToEdit);
    dateRangeLayout->addStretch();
    
    searchLayout->addRow("По дате:", dateRangeLayout);
    
    m_searchBrandEdit = new QLineEdit(this);
    m_searchBrandEdit->setPlaceholderText("Марка автомобиля (необязательно)");
    searchLayout->addRow("По марке:", m_searchBrandEdit);
    
    QHBoxLayout* searchButtonLayout = new QHBoxLayout();
    m_searchRentalsButton = new QPushButton("Найти", this);
    m_clearSearchButton = new QPushButton("Очистить", this);
    searchButtonLayout->addWidget(m_searchRentalsButton);
    searchButtonLayout->addWidget(m_clearSearchButton);
    searchButtonLayout->addStretch();
    searchLayout->addRow("", searchButtonLayout);
    
    connect(m_searchRentalsButton, &QPushButton::clicked, this, &AdminMainWindow::onSearchRentals);
    connect(m_clearSearchButton, &QPushButton::clicked, this, [this]() {
        m_searchClientEdit->clear();
        m_searchDateFromEdit->setDate(QDate());
        m_searchDateToEdit->setDate(QDate());
        m_searchBrandEdit->clear();
        loadRentals();
    });
    
    layout->addWidget(searchGroup);
    
    // Кнопка завершения аренды
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_completeRentalButton = new QPushButton("Завершить аренду");
    
    connect(m_completeRentalButton, &QPushButton::clicked, this, &AdminMainWindow::onCompleteRental);
    
    buttonLayout->addWidget(m_completeRentalButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // Таблица аренд
    m_rentalsTable = new QTableWidget();
    m_rentalsTable->setColumnCount(7);
    m_rentalsTable->setHorizontalHeaderLabels(QStringList() << "ID" << "Клиент" << "Автомобиль" << "Начало" << "Окончание" << "Стоимость" << "Статус");
    m_rentalsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_rentalsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_rentalsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_rentalsTable->horizontalHeader()->setStretchLastSection(true);
    
    layout->addWidget(m_rentalsTable);
    
    m_tabWidget->addTab(m_rentalsTab, "Аренды");
}

void AdminMainWindow::setupStatsTab()
{
    m_statsTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_statsTab);
    
    QGroupBox* statsGroup = new QGroupBox("Статистика", this);
    QGridLayout* statsLayout = new QGridLayout(statsGroup);
    
    m_totalCarsLabel = new QLabel("Всего автомобилей: 0", this);
    m_availableCarsLabel = new QLabel("Доступно: 0", this);
    m_rentedCarsLabel = new QLabel("В аренде: 0", this);
    m_activeRentalsLabel = new QLabel("Активных аренд: 0", this);
    
    statsLayout->addWidget(m_totalCarsLabel, 0, 0);
    statsLayout->addWidget(m_availableCarsLabel, 0, 1);
    statsLayout->addWidget(m_rentedCarsLabel, 1, 0);
    statsLayout->addWidget(m_activeRentalsLabel, 1, 1);
    
    layout->addWidget(statsGroup);
    layout->addStretch();
    
    QPushButton* reportsButton = new QPushButton("Открыть отчеты", this);
    reportsButton->setMinimumHeight(50);
    connect(reportsButton, &QPushButton::clicked, this, &AdminMainWindow::onShowReports);
    layout->addWidget(reportsButton);
    
    m_tabWidget->addTab(m_statsTab, "Статистика");
}

void AdminMainWindow::setupUsersTab()
{
    m_usersTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_usersTab);
    
    // Кнопка удаления пользователя
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_deleteUserButton = new QPushButton("Удалить пользователя");
    buttonLayout->addWidget(m_deleteUserButton);
    buttonLayout->addStretch();
    connect(m_deleteUserButton, &QPushButton::clicked, this, &AdminMainWindow::onDeleteUser);
    layout->addLayout(buttonLayout);
    
    // Таблица пользователей
    m_usersTable = new QTableWidget();
    m_usersTable->setColumnCount(3);
    m_usersTable->setHorizontalHeaderLabels(QStringList() << "ID" << "Логин" << "Роль");
    m_usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_usersTable->horizontalHeader()->setStretchLastSection(true);
    
    layout->addWidget(m_usersTable);
    
    m_tabWidget->addTab(m_usersTab, "Пользователи");
}

void AdminMainWindow::loadUsers()
{
    if (!m_usersTable) return;
    m_usersTable->clearContents();
    QList<User> users = m_userService->getAllUsers();
    updateUsersTable(users);
}

void AdminMainWindow::updateUsersTable(const QList<User>& users)
{
    m_usersTable->setRowCount(users.size());
    
    for (int i = 0; i < users.size(); ++i) {
        const User& user = users[i];
        m_usersTable->setItem(i, 0, new QTableWidgetItem(QString::number(user.getId())));
        m_usersTable->setItem(i, 1, new QTableWidgetItem(user.getUsername()));
        m_usersTable->setItem(i, 2, new QTableWidgetItem(user.getRoleString()));
    }
}

void AdminMainWindow::loadCars()
{
    QList<Car> cars = m_carService->getAllCars();
    updateCarsTable(cars);
}

void AdminMainWindow::loadRentals()
{
    // Используем сервис поиска без параметров для получения всех аренд
    QList<Rental> rentals = m_rentalSearchService->searchRentals();
    updateRentalsTable(rentals);
}

int AdminMainWindow::getSelectedUserId()
{
    if (!m_usersTable) {
        return 0;
    }
    int row = m_usersTable->currentRow();
    if (row >= 0 && row < m_usersTable->rowCount()) {
        QTableWidgetItem* item = m_usersTable->item(row, 0);
        if (item) {
            return item->text().toInt();
        }
    }
    return 0;
}

void AdminMainWindow::updateCarsTable(const QList<Car>& cars)
{
    m_carsTable->setRowCount(cars.size());
    
    for (int i = 0; i < cars.size(); ++i) {
        const Car& car = cars[i];
        m_carsTable->setItem(i, 0, new QTableWidgetItem(QString::number(car.getId())));
        m_carsTable->setItem(i, 1, new QTableWidgetItem(car.getBrand()));
        m_carsTable->setItem(i, 2, new QTableWidgetItem(car.getModel()));
        m_carsTable->setItem(i, 3, new QTableWidgetItem(car.getStatusString()));
        m_carsTable->setItem(i, 4, new QTableWidgetItem(QString::number(car.getDailyPrice(), 'f', 2) + " руб"));
    }
}

void AdminMainWindow::updateRentalsTable(const QList<Rental>& rentals)
{
    m_rentalsTable->setRowCount(rentals.size());
    
    for (int i = 0; i < rentals.size(); ++i) {
        const Rental& rental = rentals[i];
        Car car = m_carService->getCarById(rental.getCarId());
        User user = m_userService->getUserById(rental.getUserId());
        
        m_rentalsTable->setItem(i, 0, new QTableWidgetItem(QString::number(rental.getId())));
        m_rentalsTable->setItem(i, 1, new QTableWidgetItem(user.getUsername()));
        m_rentalsTable->setItem(i, 2, new QTableWidgetItem(car.getFullName()));
        m_rentalsTable->setItem(i, 3, new QTableWidgetItem(rental.getStartDate().toString("dd.MM.yyyy")));
        m_rentalsTable->setItem(i, 4, new QTableWidgetItem(rental.getEndDate().toString("dd.MM.yyyy")));
        m_rentalsTable->setItem(i, 5, new QTableWidgetItem(QString::number(rental.getTotalCost(), 'f', 2) + " руб"));
        m_rentalsTable->setItem(i, 6, new QTableWidgetItem(rental.isCompleted() ? "Завершена" : "Активна"));
    }
}

void AdminMainWindow::updateStatistics()
{
    QList<Car> allCars = m_carService->getAllCars();
    int totalCars = allCars.size();
    int availableCars = 0;
    int rentedCars = 0;
    
    for (const Car& car : allCars) {
        if (car.getStatus() == CarStatus::Available) {
            availableCars++;
        } else if (car.getStatus() == CarStatus::Rented) {
            rentedCars++;
        }
    }
    
    QList<Rental> activeRentals = m_rentalService->getActiveRentals();
    
    m_totalCarsLabel->setText(QString("Всего автомобилей: %1").arg(totalCars));
    m_availableCarsLabel->setText(QString("Доступно: %1").arg(availableCars));
    m_rentedCarsLabel->setText(QString("В аренде: %1").arg(rentedCars));
    m_activeRentalsLabel->setText(QString("Активных аренд: %1").arg(activeRentals.size()));
}

void AdminMainWindow::onAddCar()
{
    CarDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        loadCars();
        updateStatistics();
    }
}

void AdminMainWindow::onEditCar()
{
    int carId = getSelectedCarId();
    if (carId == 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите автомобиль из списка!");
        return;
    }
    
    Car car = m_carService->getCarById(carId);
    if (car.getId() == 0) {
        QMessageBox::warning(this, "Ошибка", "Автомобиль не найден!");
        return;
    }
    
    CarDialog dialog(car, this);
    if (dialog.exec() == QDialog::Accepted) {
        loadCars();
        updateStatistics();
    }
}

void AdminMainWindow::onDeleteCar()
{
    int carId = getSelectedCarId();
    if (carId == 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите автомобиль из списка!");
        return;
    }
    
    int ret = QMessageBox::question(this, "Подтверждение", 
                                     "Вы уверены, что хотите удалить этот автомобиль?",
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        if (m_carService->deleteCar(carId)) {
            QMessageBox::information(this, "Успех", "Автомобиль удален!");
            loadCars();
            updateStatistics();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить автомобиль!");
        }
    }
}

void AdminMainWindow::onSearchRentals()
{
    QString clientName = m_searchClientEdit->text().trimmed();
    QDate dateFrom = m_searchDateFromEdit->date();
    QDate dateTo = m_searchDateToEdit->date();
    QString brand = m_searchBrandEdit->text().trimmed();
    
    // Используем сервис поиска с диапазоном дат
    QList<Rental> rentals = m_rentalSearchService->searchRentals(clientName, dateFrom, dateTo, brand);
    updateRentalsTable(rentals);
}

void AdminMainWindow::onCompleteRental()
{
    int rentalId = getSelectedRentalId();
    if (rentalId == 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите аренду из списка!");
        return;
    }
    
    Rental rental = m_rentalService->getRentalById(rentalId);
    if (rental.isCompleted()) {
        QMessageBox::information(this, "Информация", "Эта аренда уже завершена!");
        return;
    }
    
    // Используем метод с автоматическим начислением штрафа
    RentalService::CompleteRentalResult result = m_rentalService->completeRentalWithFine(rentalId, DateUtils::currentDate(), 1.5);
    
    if (result.success) {
        QString message = "Аренда завершена!";
        if (result.wasOverdue) {
            message += QString("\n\nНачислен штраф: %1 руб").arg(result.fineAmount, 0, 'f', 2);
        }
        QMessageBox::information(this, "Успех", message);
        loadRentals();
        loadCars();
        updateStatistics();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось завершить аренду!");
    }
}

void AdminMainWindow::onDeleteUser()
{
    int userId = getSelectedUserId();
    if (userId == 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя для удаления!");
        return;
    }

    int ret = QMessageBox::question(this, "Подтверждение",
                                    "Удалить пользователя и завершить все его активные аренды?",
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return;
    }

    // Используем сервис для удаления пользователя с автоматическим завершением аренд
    if (m_userService->deleteUser(userId)) {
        QMessageBox::information(this, "Успех", "Пользователь и его активные аренды успешно обработаны и удалены.");

        // Если администратор удалил сам себя, закрываем его окно
        if (userId == m_user.getId()) {
            close();
            return;
        }

        // Иначе просто обновляем данные
        loadUsers();
        loadRentals();
        updateStatistics();
    }
    else {
        QMessageBox::critical(this, "Ошибка", "Не удалось удалить пользователя!");
    }
}

void AdminMainWindow::onShowReports()
{
    ReportsWindow* reportsWindow = new ReportsWindow(m_reportManager, this);
    reportsWindow->setAttribute(Qt::WA_DeleteOnClose);
    reportsWindow->show();
}

void AdminMainWindow::onShowUsers()
{
    if (m_usersTab) {
        m_tabWidget->setCurrentWidget(m_usersTab);
        loadUsers();
    }
}

void AdminMainWindow::onImportExport()
{
    ImportExportDialog dialog(m_dbManager, this);
    dialog.exec();
    // Обновляем данные после импорта
    loadCars();
    loadRentals();
    updateStatistics();
}

void AdminMainWindow::onLogout()
{
    close();
}

int AdminMainWindow::getSelectedCarId()
{
    int row = m_carsTable->currentRow();
    if (row >= 0 && row < m_carsTable->rowCount()) {
        QTableWidgetItem* item = m_carsTable->item(row, 0);
        if (item) {
            return item->text().toInt();
        }
    }
    return 0;
}

int AdminMainWindow::getSelectedRentalId()
{
    int row = m_rentalsTable->currentRow();
    if (row >= 0 && row < m_rentalsTable->rowCount()) {
        QTableWidgetItem* item = m_rentalsTable->item(row, 0);
        if (item) {
            return item->text().toInt();
        }
    }
    return 0;
}

void AdminMainWindow::updateDateLabel()
{
    QDate currentDate = DateUtils::currentDate();
    QString dateString = QString("Дата: %1").arg(currentDate.toString("dd.MM.yyyy"));
    m_dateLabel->setText(dateString);
}

