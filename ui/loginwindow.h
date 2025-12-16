#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "../models/user.h"
#include "../services/userservice.h"
#include "registerdialog.h"

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    
    User getLoggedInUser() const { return m_loggedInUser; }
    bool isLoggedIn() const { return m_loggedIn; }

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    QLineEdit* m_usernameEdit;
    QLineEdit* m_passwordEdit;
    QPushButton* m_loginButton;
    QPushButton* m_registerButton;
    
    User m_loggedInUser;
    bool m_loggedIn;
    UserService* m_userService;
    
    void setupUI();
    bool validateInput();
};

#endif // LOGINWINDOW_H

