#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include "../models/user.h"
#include "../services/userservice.h"

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void onRegisterClicked();

private:
    QLineEdit* m_usernameEdit;
    QLineEdit* m_passwordEdit;
    QComboBox* m_roleCombo;
    QPushButton* m_registerButton;
    
    UserService* m_userService;
    
    void setupUI();
    bool validateInput();
};

#endif // REGISTERDIALOG_H

