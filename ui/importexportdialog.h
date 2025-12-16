#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include "../utils/dataexporter.h"
#include "../utils/dataimporter.h"
#include "../database/databasemanager.h"

class ImportExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportExportDialog(DatabaseManager* dbManager, QWidget *parent = nullptr);
    ~ImportExportDialog();

private slots:
    void onExportClicked();
    void onImportClicked();
    void onExportReportClicked();

private:
    DatabaseManager* m_dbManager;
    DataExporter* m_exporter;
    DataImporter* m_importer;
    
    QButtonGroup* m_exportGroup;
    QButtonGroup* m_importGroup;
    
    QRadioButton* m_exportAllRadio;
    QRadioButton* m_exportCarsRadio;
    QRadioButton* m_exportRentalsRadio;
    QRadioButton* m_exportUsersRadio;
    QRadioButton* m_exportFinesRadio;
    
    QRadioButton* m_importAllRadio;
    QRadioButton* m_importCarsRadio;
    QRadioButton* m_importRentalsRadio;
    QRadioButton* m_importUsersRadio;
    QRadioButton* m_importFinesRadio;
    
    void setupUI();
    QString getExportFileName(const QString& defaultName);
    QString getImportFileName();
};

#endif // IMPORTEXPORTDIALOG_H

