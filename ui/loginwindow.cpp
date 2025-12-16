#include "loginwindow.h"
#include "registerdialog.h"
#include <QFormLayout>
#include <QGroupBox>

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent), m_loggedIn(false)
{
    m_userService = new UserService();
    setupUI();
    setWindowTitle("Вход в систему - Агентство аренды автомобилей");
    setModal(true);
    resize(350, 200);
}

LoginWindow::~LoginWindow()
{
    delete m_userService;
}

void LoginWindow::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Заголовок
    QLabel* titleLabel = new QLabel("Агентство аренды автомобилей", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    // Форма входа
    QGroupBox* formGroup = new QGroupBox("Вход в систему", this);
    QFormLayout* formLayout = new QFormLayout(formGroup);
    
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("Введите логин");
    formLayout->addRow("Логин:", m_usernameEdit);
    
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("Введите пароль");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Пароль:", m_passwordEdit);
    
    mainLayout->addWidget(formGroup);
    
    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_loginButton = new QPushButton("Войти", this);
    m_loginButton->setDefault(true);
    connect(m_loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    buttonLayout->addWidget(m_loginButton);
    
    m_registerButton = new QPushButton("Зарегистрироваться", this);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    buttonLayout->addWidget(m_registerButton);
    
    mainLayout->addLayout(buttonLayout);
    
}

bool LoginWindow::validateInput()
{
    if (m_usernameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин!");
        m_usernameEdit->setFocus();
        return false;
    }
    
    if (m_passwordEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите пароль!");
        m_passwordEdit->setFocus();
        return false;
    }
    
    return true;
}

void LoginWindow::onLoginClicked()
{
    if (!validateInput()) {
        return;
    }
    
    QString username = m_usernameEdit->text();
    QString password = m_passwordEdit->text();
    
    if (m_userService->authenticateUser(username, password)) {
        m_loggedInUser = m_userService->getUserByUsername(username);
        m_loggedIn = true;
        accept();
    } else {
        QMessageBox::warning(this, "Ошибка входа", "Неверный логин или пароль!");
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
    }
}

void LoginWindow::onRegisterClicked()
{
    RegisterDialog registerDialog(this);
    if (registerDialog.exec() == QDialog::Accepted) {
        // После успешной регистрации можно обновить что-то или просто закрыть диалог
    }
}

