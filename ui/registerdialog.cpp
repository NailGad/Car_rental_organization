#include "registerdialog.h"
#include <QGroupBox>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
{
    m_userService = new UserService();
    setupUI();
    setWindowTitle("Регистрация нового пользователя");
    setModal(true);
    resize(400, 250);
}

RegisterDialog::~RegisterDialog()
{
    delete m_userService;
}

void RegisterDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel("Регистрация", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    QGroupBox* formGroup = new QGroupBox("Данные пользователя", this);
    QFormLayout* formLayout = new QFormLayout(formGroup);
    
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("Введите логин");
    formLayout->addRow("Логин:", m_usernameEdit);
    
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("Введите пароль");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Пароль:", m_passwordEdit);
    
    m_roleCombo = new QComboBox(this);
    m_roleCombo->addItem("Клиент", static_cast<int>(UserRole::Client));
    m_roleCombo->addItem("Администратор", static_cast<int>(UserRole::Administrator));
    formLayout->addRow("Роль:", m_roleCombo);
    
    mainLayout->addWidget(formGroup);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* cancelButton = new QPushButton("Отмена", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    m_registerButton = new QPushButton("Зарегистрировать", this);
    m_registerButton->setDefault(true);
    connect(m_registerButton, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);
    buttonLayout->addWidget(m_registerButton);
    
    mainLayout->addLayout(buttonLayout);
}

bool RegisterDialog::validateInput()
{
    if (m_usernameEdit->text().trimmed().isEmpty()) {
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

void RegisterDialog::onRegisterClicked()
{
    if (!validateInput()) {
        return;
    }
    
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    UserRole role = static_cast<UserRole>(m_roleCombo->currentData().toInt());
    
    // Проверяем, существует ли пользователь
    User existingUser = m_userService->getUserByUsername(username);
    if (existingUser.getId() != 0) {
        QMessageBox::warning(this, "Ошибка регистрации", "Пользователь с таким логином уже существует!");
        return;
    }
    
    // Создаем нового пользователя (полное имя = логин)
    User newUser(0, username, password, username, role);
    if (m_userService->addUser(newUser)) {
        QMessageBox::information(this, "Успех", "Пользователь успешно зарегистрирован!");
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка регистрации", "Не удалось зарегистрировать пользователя!");
    }
}

