#include "ui/loginwindow.h"
#include "ui/clientmainwindow.h"
#include "ui/adminmainwindow.h"
#include "database/databasemanager.h"
#include "services/rentalservice.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Инициализация базы данных
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.initializeDatabase()) {
        qDebug() << "Не удалось инициализировать базу данных!";
        return -1;
    }
    
    // Автоматическая проверка просроченных аренд и начисление штрафов
    RentalService rentalService;
    rentalService.checkAndApplyOverdueFines();
    
    // Показываем окно входа
    LoginWindow loginWindow;
    
    while (true) {
        if (loginWindow.exec() == QDialog::Accepted && loginWindow.isLoggedIn()) {
            User user = loginWindow.getLoggedInUser();
            
            if (user.isAdministrator()) {
                // Открываем окно администратора
                AdminMainWindow adminWindow(user);
                adminWindow.show();
                a.exec();
            } else {
                // Открываем окно клиента
                ClientMainWindow clientWindow(user);
                clientWindow.show();
                a.exec();
            }
            
            // После закрытия окна показываем окно входа снова
            loginWindow.show();
        } else {
            break; // Выход из приложения
        }
    }

    return 0;
}
