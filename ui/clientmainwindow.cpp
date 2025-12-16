#include "clientmainwindow.h"
#include "rentaldialog.h"
#include "../models/fine.h"
#include "../services/carservice.h"
#include "../utils/dateutils.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QDate>
#include <QTimer>

ClientMainWindow::ClientMainWindow(const User& user, QWidget *parent)
    : QMainWindow(parent), m_user(user)
{
    m_dbManager = &DatabaseManager::getInstance();
    m_rentalService = new RentalService();
    m_pricingCalculator = new PricingCalculator();
    m_carService = new CarService();
    
    setupUI();
    
    // Проверяем просроченные аренды при открытии окна
    m_rentalService->checkAndApplyOverdueFines();
    
    loadAvailableCars();
    loadUserRentals();
    
    setWindowTitle(QString("Агентство аренды - Клиент: %1").arg(m_user.getUsername()));
    resize(900, 600);
}

ClientMainWindow::~ClientMainWindow()
{
    delete m_rentalService;
    delete m_pricingCalculator;
    delete m_carService;
}

void ClientMainWindow::setupUI()
{
    // Меню
    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->addMenu("Файл");
    QAction* logoutAction = fileMenu->addAction("Выход");
    connect(logoutAction, &QAction::triggered, this, &ClientMainWindow::onLogout);
    
    // Статус бар
    statusBar()->showMessage(QString("Пользователь: %1").arg(m_user.getUsername()));
    
    // Добавляем отображение текущей даты в статус-бар
    m_dateLabel = new QLabel(this);
    m_dateLabel->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 3px 8px; border-radius: 3px; }");
    updateDateLabel();
    statusBar()->addPermanentWidget(m_dateLabel);
    
    // Обновляем дату каждую минуту
    QTimer* dateTimer = new QTimer(this);
    connect(dateTimer, &QTimer::timeout, this, &ClientMainWindow::updateDateLabel);
    dateTimer->start(60000); // Обновление каждую минуту
    
    // Центральный виджет
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);
    
    setupCarsTab();
    setupRentalsTab();
}

void ClientMainWindow::setupCarsTab()
{
    m_carsTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_carsTab);
    
    // Панель поиска
    QHBoxLayout* searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Поиск по марке...");
    m_searchButton = new QPushButton("Найти");
    
    searchLayout->addWidget(new QLabel("Поиск:"));
    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_searchButton);
    searchLayout->addStretch();
    
    connect(m_searchButton, &QPushButton::clicked, this, &ClientMainWindow::onSearchCars);
    
    layout->addLayout(searchLayout);
    
    // Таблица автомобилей
    m_carsTable = new QTableWidget();
    m_carsTable->setColumnCount(5);
    m_carsTable->setHorizontalHeaderLabels(QStringList() << "ID" << "Марка" << "Модель" << "Статус" << "Цена/день");
    m_carsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_carsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_carsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_carsTable->horizontalHeader()->setStretchLastSection(true);
    
    layout->addWidget(m_carsTable);
    
    // Кнопка аренды
    QPushButton* rentButton = new QPushButton("Забронировать автомобиль");
    rentButton->setMinimumHeight(40);
    connect(rentButton, &QPushButton::clicked, this, &ClientMainWindow::onRentCar);
    layout->addWidget(rentButton);
    
    m_tabWidget->addTab(m_carsTab, "Доступные автомобили");
}

void ClientMainWindow::setupRentalsTab()
{
    m_rentalsTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_rentalsTab);
    
    // Кнопка завершения аренды
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_completeRentalButton = new QPushButton("Завершить аренду");
    buttonLayout->addWidget(m_completeRentalButton);
    buttonLayout->addStretch();
    connect(m_completeRentalButton, &QPushButton::clicked, this, &ClientMainWindow::onCompleteRental);
    
    layout->addLayout(buttonLayout);
    
    // Таблица аренд
    m_rentalsTable = new QTableWidget();
    m_rentalsTable->setColumnCount(7);
    m_rentalsTable->setHorizontalHeaderLabels(QStringList() << "ID" << "Автомобиль" << "Начало" << "Окончание" << "Стоимость" << "Статус" << "Просрочка");
    m_rentalsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_rentalsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_rentalsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_rentalsTable->horizontalHeader()->setStretchLastSection(true);
    
    layout->addWidget(m_rentalsTable);
    
    m_tabWidget->addTab(m_rentalsTab, "Мои аренды");
}

void ClientMainWindow::loadAvailableCars()
{
    QList<Car> cars = m_carService->getAvailableCars();
    updateCarsTable(cars);
}

void ClientMainWindow::loadUserRentals()
{
    QList<Rental> rentals = m_rentalService->getUserRentals(m_user.getId());
    updateRentalsTable(rentals);
}

void ClientMainWindow::updateCarsTable(const QList<Car>& cars)
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

void ClientMainWindow::updateRentalsTable(const QList<Rental>& rentals)
{
    m_rentalsTable->setRowCount(rentals.size());
    
    for (int i = 0; i < rentals.size(); ++i) {
        const Rental& rental = rentals[i];
        Car car = m_carService->getCarById(rental.getCarId());
        
        m_rentalsTable->setItem(i, 0, new QTableWidgetItem(QString::number(rental.getId())));
        m_rentalsTable->setItem(i, 1, new QTableWidgetItem(car.getFullName()));
        m_rentalsTable->setItem(i, 2, new QTableWidgetItem(rental.getStartDate().toString("dd.MM.yyyy")));
        m_rentalsTable->setItem(i, 3, new QTableWidgetItem(rental.getEndDate().toString("dd.MM.yyyy")));
        m_rentalsTable->setItem(i, 4, new QTableWidgetItem(QString::number(rental.getTotalCost(), 'f', 2) + " руб"));
        
        QString status = rental.isCompleted() ? "Завершена" : "Активна";
        m_rentalsTable->setItem(i, 5, new QTableWidgetItem(status));
        
        // Информация о просрочке
        QString overdueInfo = "";
        if (!rental.isCompleted()) {
            int overdueDays = rental.getDaysOverdue();
            if (overdueDays > 0) {
                overdueInfo = QString("Просрочка: %1 дн.").arg(overdueDays);
                // Получаем сумму штрафа
                QList<Fine> fines = m_rentalService->getFinesByRentalId(rental.getId());
                double totalFine = 0.0;
                for (const Fine& fine : fines) {
                    totalFine += fine.getAmount();
                }
                if (totalFine > 0) {
                    overdueInfo += QString(" (штраф: %1 руб)").arg(totalFine, 0, 'f', 2);
                }
            } else {
                overdueInfo = "В срок";
            }
        } else {
            overdueInfo = "-";
        }
        m_rentalsTable->setItem(i, 6, new QTableWidgetItem(overdueInfo));
    }
}

void ClientMainWindow::onSearchCars()
{
    QString searchText = m_searchEdit->text().trimmed();
    
    if (searchText.isEmpty()) {
        loadAvailableCars();
        return;
    }
    
    QList<Car> cars = m_carService->getCarsByBrand(searchText);
    // Фильтруем только доступные
    QList<Car> availableCars;
    for (const Car& car : cars) {
        if (car.getStatus() == CarStatus::Available) {
            availableCars.append(car);
        }
    }
    updateCarsTable(availableCars);
}

void ClientMainWindow::onCompleteRental()
{
    int rentalId = getSelectedRentalId();
    if (rentalId == 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите аренду из списка!");
        return;
    }
    
    // Проверяем, что это аренда текущего пользователя
    QList<Rental> userRentals = m_rentalService->getUserRentals(m_user.getId());
    Rental rental;
    bool isUserRental = false;
    for (const Rental& r : userRentals) {
        if (r.getId() == rentalId) {
            rental = r;
            isUserRental = true;
            break;
        }
    }
    
    if (!isUserRental || rental.getId() == 0) {
        QMessageBox::warning(this, "Ошибка", "Аренда не найдена или это не ваша аренда!");
        return;
    }
    
    if (rental.isCompleted()) {
        QMessageBox::information(this, "Информация", "Эта аренда уже завершена!");
        return;
    }
    
    QDate currentDate = DateUtils::currentDate();
    QDate plannedReturnDate = rental.getEndDate();
    
    // Проверяем просрочку заранее для показа сообщения
    bool isOverdue = currentDate > plannedReturnDate;
    
    if (isOverdue) {
        int overdueDays = plannedReturnDate.daysTo(currentDate);
        Fine fine = m_rentalService->calculateFine(rentalId, currentDate, 1.5);
        double fineAmount = fine.getAmount();
        
        // Показываем информацию о просрочке
        QString message = QString("Аренда просрочена на %1 день(ей)!\n\n")
                         .arg(overdueDays);
        message += QString("Планируемая дата возврата: %1\n")
                  .arg(plannedReturnDate.toString("dd.MM.yyyy"));
        message += QString("Фактическая дата возврата: %1\n\n")
                  .arg(currentDate.toString("dd.MM.yyyy"));
        message += QString("Будет начислен штраф: %1 руб\n\n")
                  .arg(fineAmount, 0, 'f', 2);
        message += "Завершить аренду и начислить штраф?";
        
        int ret = QMessageBox::question(this, "Просрочка возврата", message,
                                       QMessageBox::Yes | QMessageBox::No);
        
        if (ret != QMessageBox::Yes) {
            return;
        }
    } else {
        // Нет просрочки - просто подтверждаем завершение
        int ret = QMessageBox::question(this, "Подтверждение", 
                                       "Завершить аренду?",
                                       QMessageBox::Yes | QMessageBox::No);
        
        if (ret != QMessageBox::Yes) {
            return;
        }
    }
    
    // Завершаем аренду с автоматическим начислением штрафа
    RentalService::CompleteRentalResult result = m_rentalService->completeRentalWithFine(rentalId, currentDate, 1.5);
    
    if (result.success) {
        QString successMessage = "Аренда успешно завершена!";
        if (result.wasOverdue) {
            successMessage += QString("\n\nНачислен штраф: %1 руб").arg(result.fineAmount, 0, 'f', 2);
        }
        QMessageBox::information(this, "Успех", successMessage);
        loadUserRentals();
        loadAvailableCars(); // Обновляем список доступных автомобилей
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось завершить аренду!");
    }
}

void ClientMainWindow::onRentCar()
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
    
    RentalDialog dialog(car, m_user.getId(), m_pricingCalculator, this);
    if (dialog.exec() == QDialog::Accepted) {
        loadAvailableCars();
        loadUserRentals();
        QMessageBox::information(this, "Успех", "Аренда успешно оформлена!");
    }
}

void ClientMainWindow::onLogout()
{
    close();
}

int ClientMainWindow::getSelectedCarId()
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

int ClientMainWindow::getSelectedRentalId()
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

void ClientMainWindow::updateDateLabel()
{
    QDate currentDate = DateUtils::currentDate();
    QString dateString = QString("Дата: %1").arg(currentDate.toString("dd.MM.yyyy"));
    m_dateLabel->setText(dateString);
}

