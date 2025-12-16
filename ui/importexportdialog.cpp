#include "importexportdialog.h"
#include "../utils/dateutils.h"
#include <QDateEdit>
#include <QFormLayout>

ImportExportDialog::ImportExportDialog(DatabaseManager* dbManager, QWidget *parent)
    : QDialog(parent), m_dbManager(dbManager)
{
    m_exporter = new DataExporter(dbManager);
    m_importer = new DataImporter(dbManager);
    setupUI();
    setWindowTitle("Импорт/Экспорт данных");
    setModal(true);
    resize(500, 400);
}

ImportExportDialog::~ImportExportDialog()
{
    delete m_exporter;
    delete m_importer;
}

void ImportExportDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Экспорт
    QGroupBox* exportGroup = new QGroupBox("Экспорт данных", this);
    QVBoxLayout* exportLayout = new QVBoxLayout(exportGroup);
    
    m_exportGroup = new QButtonGroup(this);
    m_exportAllRadio = new QRadioButton("Все данные", this);
    m_exportCarsRadio = new QRadioButton("Только автомобили", this);
    m_exportRentalsRadio = new QRadioButton("Только аренды", this);
    m_exportUsersRadio = new QRadioButton("Только пользователи", this);
    m_exportFinesRadio = new QRadioButton("Только штрафы", this);
    
    m_exportAllRadio->setChecked(true);
    
    m_exportGroup->addButton(m_exportAllRadio, 0);
    m_exportGroup->addButton(m_exportCarsRadio, 1);
    m_exportGroup->addButton(m_exportRentalsRadio, 2);
    m_exportGroup->addButton(m_exportUsersRadio, 3);
    m_exportGroup->addButton(m_exportFinesRadio, 4);
    
    exportLayout->addWidget(m_exportAllRadio);
    exportLayout->addWidget(m_exportCarsRadio);
    exportLayout->addWidget(m_exportRentalsRadio);
    exportLayout->addWidget(m_exportUsersRadio);
    exportLayout->addWidget(m_exportFinesRadio);
    
    QPushButton* exportButton = new QPushButton("Экспорт в JSON", this);
    connect(exportButton, &QPushButton::clicked, this, &ImportExportDialog::onExportClicked);
    exportLayout->addWidget(exportButton);
    
    QPushButton* exportReportButton = new QPushButton("Экспорт отчета", this);
    connect(exportReportButton, &QPushButton::clicked, this, &ImportExportDialog::onExportReportClicked);
    exportLayout->addWidget(exportReportButton);
    
    mainLayout->addWidget(exportGroup);
    
    // Импорт
    QGroupBox* importGroup = new QGroupBox("Импорт данных", this);
    QVBoxLayout* importLayout = new QVBoxLayout(importGroup);
    
    m_importGroup = new QButtonGroup(this);
    m_importAllRadio = new QRadioButton("Все данные", this);
    m_importCarsRadio = new QRadioButton("Только автомобили", this);
    m_importRentalsRadio = new QRadioButton("Только аренды", this);
    m_importUsersRadio = new QRadioButton("Только пользователи", this);
    m_importFinesRadio = new QRadioButton("Только штрафы", this);
    
    m_importAllRadio->setChecked(true);
    
    m_importGroup->addButton(m_importAllRadio, 0);
    m_importGroup->addButton(m_importCarsRadio, 1);
    m_importGroup->addButton(m_importRentalsRadio, 2);
    m_importGroup->addButton(m_importUsersRadio, 3);
    m_importGroup->addButton(m_importFinesRadio, 4);
    
    importLayout->addWidget(m_importAllRadio);
    importLayout->addWidget(m_importCarsRadio);
    importLayout->addWidget(m_importRentalsRadio);
    importLayout->addWidget(m_importUsersRadio);
    importLayout->addWidget(m_importFinesRadio);
    
    QPushButton* importButton = new QPushButton("Импорт из JSON", this);
    connect(importButton, &QPushButton::clicked, this, &ImportExportDialog::onImportClicked);
    importLayout->addWidget(importButton);
    
    mainLayout->addWidget(importGroup);
    
    // Кнопка закрытия
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton* closeButton = new QPushButton("Закрыть", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);
}

QString ImportExportDialog::getExportFileName(const QString& defaultName)
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить файл",
                                                    defaultName,
                                                    "JSON Files (*.json);;All Files (*)");
    return fileName;
}

QString ImportExportDialog::getImportFileName()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                     "Выберите файл для импорта",
                                                     "",
                                                     "JSON Files (*.json);;All Files (*)");
    return fileName;
}

void ImportExportDialog::onExportClicked()
{
    QString fileName;
    bool success = false;
    
    if (m_exportAllRadio->isChecked()) {
        fileName = getExportFileName("export_all.json");
        if (!fileName.isEmpty()) {
            success = m_exporter->exportToJson(fileName);
        }
    } else if (m_exportCarsRadio->isChecked()) {
        fileName = getExportFileName("export_cars.json");
        if (!fileName.isEmpty()) {
            success = m_exporter->exportCarsToJson(fileName);
        }
    } else if (m_exportRentalsRadio->isChecked()) {
        fileName = getExportFileName("export_rentals.json");
        if (!fileName.isEmpty()) {
            success = m_exporter->exportRentalsToJson(fileName);
        }
    } else if (m_exportUsersRadio->isChecked()) {
        fileName = getExportFileName("export_users.json");
        if (!fileName.isEmpty()) {
            success = m_exporter->exportUsersToJson(fileName);
        }
    } else if (m_exportFinesRadio->isChecked()) {
        fileName = getExportFileName("export_fines.json");
        if (!fileName.isEmpty()) {
            success = m_exporter->exportFinesToJson(fileName);
        }
    }
    
    if (!fileName.isEmpty()) {
        if (success) {
            QMessageBox::information(this, "Успех", QString("Данные успешно экспортированы в:\n%1").arg(fileName));
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось экспортировать данные!");
        }
    }
}

void ImportExportDialog::onImportClicked()
{
    QString fileName = getImportFileName();
    if (fileName.isEmpty()) {
        return;
    }
    
    ImportResult result;
    
    if (m_importAllRadio->isChecked()) {
        result = m_importer->importFromJson(fileName, true);
    } else if (m_importCarsRadio->isChecked()) {
        result = m_importer->importCarsFromJson(fileName, true);
    } else if (m_importRentalsRadio->isChecked()) {
        result = m_importer->importRentalsFromJson(fileName, true);
    } else if (m_importUsersRadio->isChecked()) {
        result = m_importer->importUsersFromJson(fileName, true);
    } else if (m_importFinesRadio->isChecked()) {
        result = m_importer->importFinesFromJson(fileName, true);
    }
    
    QString message = QString("Импорт завершен:\n"
                            "Автомобилей: %1\n"
                            "Пользователей: %2\n"
                            "Аренд: %3\n"
                            "Штрафов: %4\n"
                            "Ошибок: %5")
                            .arg(result.carsImported)
                            .arg(result.usersImported)
                            .arg(result.rentalsImported)
                            .arg(result.finesImported)
                            .arg(result.errors);
    
    if (!result.errorMessages.isEmpty()) {
        message += "\n\nОшибки:\n" + result.errorMessages.join("\n");
    }
    
    QMessageBox::information(this, "Результат импорта", message);
}

void ImportExportDialog::onExportReportClicked()
{
    // Создаем диалог для выбора периода
    QDialog periodDialog(this);
    periodDialog.setWindowTitle("Выберите период для отчета");
    QVBoxLayout* layout = new QVBoxLayout(&periodDialog);
    
    QFormLayout* formLayout = new QFormLayout();
    QDateEdit* startDateEdit = new QDateEdit(DateUtils::currentDate().addMonths(-1), &periodDialog);
    startDateEdit->setCalendarPopup(true);
    QDateEdit* endDateEdit = new QDateEdit(DateUtils::currentDate(), &periodDialog);
    endDateEdit->setCalendarPopup(true);
    
    formLayout->addRow("Дата начала:", startDateEdit);
    formLayout->addRow("Дата окончания:", endDateEdit);
    layout->addLayout(formLayout);
    
    QPushButton* okButton = new QPushButton("Экспортировать", &periodDialog);
    connect(okButton, &QPushButton::clicked, &periodDialog, &QDialog::accept);
    layout->addWidget(okButton);
    
    if (periodDialog.exec() == QDialog::Accepted) {
        QString fileName = getExportFileName("report.json");
        if (!fileName.isEmpty()) {
            bool success = m_exporter->exportReportToJson(fileName, 
                                                         startDateEdit->date(), 
                                                         endDateEdit->date());
            if (success) {
                QMessageBox::information(this, "Успех", 
                                        QString("Отчет успешно экспортирован в:\n%1").arg(fileName));
            } else {
                QMessageBox::critical(this, "Ошибка", "Не удалось экспортировать отчет!");
            }
        }
    }
}

