#include "antivirus.h"
#include "ui_antivirus.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QStandardPaths>

Antivirus::Antivirus(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Antivirus)
    , totalScanned(0)
    , scanner(nullptr)
    , clamEngine(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("NEHNES Antivirus");

    // Initialize ClamAV
    initializeClamAV();

    connect(ui->scanButton, &QPushButton::clicked, this, &Antivirus::onScanClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &Antivirus::onDeleteClicked);
    connect(ui->deleteAllButton, &QPushButton::clicked, this, &Antivirus::onDeleteAllClicked);

    ui->progressBar->setValue(0);
    ui->statusLabel->setText("Ready to scan");
    ui->deleteButton->setEnabled(false);
    ui->deleteAllButton->setEnabled(false);
    ui->infectedFilesList->clear();
}

Antivirus::~Antivirus()
{
    if (scanner && scanner->isRunning()) {
        scanner->requestInterruption();
        scanner->wait();
    }
    cleanupClamAV();
    delete ui;
}

void Antivirus::initializeClamAV()
{
    cl_error_t ret;
    unsigned int sigs = 0;

    // Initialize ClamAV library
    ret = cl_init(CL_INIT_DEFAULT);
    if (ret != CL_SUCCESS) {
        QMessageBox::critical(this, "ClamAV Error",
                              QString("Failed to initialize ClamAV: %1").arg(cl_strerror(ret)));
        return;
    }

    // Create new engine
    clamEngine = cl_engine_new();
    if (!clamEngine) {
        QMessageBox::critical(this, "ClamAV Error", "Failed to create ClamAV engine");
        return;
    }

    // Load virus database
    // Try common database locations
    QStringList dbPaths = {
        "C:/Program Files/ClamAV/database",
        "C:/ProgramData/ClamAV",
        "C:/Program Files/ClamAV",
        QDir::homePath() + "/.clamav" // User directory
    };

    QString dbPath;
    for (const QString& path : dbPaths) {
        if (QDir(path).exists()) {
            dbPath = path;
            break;
        }
    }

    if (dbPath.isEmpty()) {
        QMessageBox::warning(this, "ClamAV Warning",
                             "ClamAV database not found!\n\n"
                             "Please install ClamAV and update virus definitions:\n"
                             "Linux: sudo apt-get install clamav\n"
                             "       sudo freshclam\n"
                             "macOS: brew install clamav\n"
                             "       freshclam\n"
                             "Windows: Download from clamav.net\n\n"
                             "Falling back to basic signature detection.");
        loadSignatures();
        return;
    }

    // Load the virus database
    ret = cl_load(dbPath.toUtf8().constData(), clamEngine, &sigs, CL_DB_STDOPT);
    if (ret != CL_SUCCESS) {
        QMessageBox::warning(this, "ClamAV Warning",
                             QString("Failed to load virus database: %1\n\nFalling back to basic detection.")
                                 .arg(cl_strerror(ret)));
        cl_engine_free(clamEngine);
        clamEngine = nullptr;
        loadSignatures();
        return;
    }

    // Compile the engine
    ret = cl_engine_compile(clamEngine);
    if (ret != CL_SUCCESS) {
        QMessageBox::warning(this, "ClamAV Warning",
                             QString("Failed to compile engine: %1\n\nFalling back to basic detection.")
                                 .arg(cl_strerror(ret)));
        cl_engine_free(clamEngine);
        clamEngine = nullptr;
        loadSignatures();
        return;
    }

    ui->scanResults->append(QString("✓ ClamAV initialized successfully"));
    ui->scanResults->append(QString("✓ Loaded %1 virus signatures").arg(sigs));
    ui->scanResults->append("");
}

void Antivirus::cleanupClamAV()
{
    if (clamEngine) {
        cl_engine_free(clamEngine);
        clamEngine = nullptr;
    }
}

void Antivirus::loadSignatures()
{
    // Fallback signatures if ClamAV is not available
    virusSignatures["EICAR-Test"] = "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";
    virusSignatures["TestVirus-A"] = "TEST-VIRUS-SIGNATURE-ALPHA-2024";
    virusSignatures["TestVirus-B"] = "DEMO-MALWARE-PATTERN-BETA";

    ui->scanResults->append("⚠ Using basic signature detection (ClamAV not available)");
    ui->scanResults->append("");
}

QStringList Antivirus::getAllFilesRecursive(const QString &path)
{
    QStringList allFiles;
    QDir dir(path);
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo &entry : entries) {
        if (entry.isDir()) {
            allFiles.append(getAllFilesRecursive(entry.absoluteFilePath()));
        } else {
            allFiles.append(entry.absoluteFilePath());
        }
    }

    return allFiles;
}

void Antivirus::onScanClicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this,
                                                        "Select Directory to Scan",
                                                        QDir::homePath());

    if (dirPath.isEmpty()) {
        return;
    }

    ui->scanResults->clear();
    ui->scanResults->append("\n    NEHNES ANTIVIRUS SCAN STARTED");
    ui->scanResults->append("Scanning directory: " + dirPath);
    ui->scanResults->append("");

    ui->scanButton->setEnabled(false);
    ui->progressBar->setValue(0);
    ui->deleteButton->setEnabled(false);
    ui->deleteAllButton->setEnabled(false);

    // Get all files first
    QStringList allFiles = getAllFilesRecursive(dirPath);

    if (allFiles.isEmpty()) {
        ui->statusLabel->setText("No files found");
        ui->scanButton->setEnabled(true);
        return;
    }

    totalScanned = 0;
    infectedFiles.clear();
    ui->infectedFilesList->clear();

    // Create and configure scanner thread
    scanner = new AntivirusScanner(allFiles, clamEngine, this);

    connect(scanner, &AntivirusScanner::scanProgress, this, &Antivirus::onScanProgress);
    connect(scanner, &AntivirusScanner::threatFound, this, &Antivirus::onThreatFound);
    connect(scanner, &AntivirusScanner::scanComplete, this, &Antivirus::onScanComplete);
    connect(scanner, &QThread::finished, scanner, &QObject::deleteLater);

    scanner->start();
}

void Antivirus::onScanProgress(int current, int total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(current);
    ui->statusLabel->setText(QString("Scanning %1 of %2").arg(current).arg(total));
}

void Antivirus::onThreatFound(QString fileName, QString threatName)
{
    infectedFiles.append(fileName);
    totalScanned++;

    ui->scanResults->append(QString(" THREAT DETECTED!"));
    ui->scanResults->append(QString("   Threat: %1").arg(threatName));
    ui->scanResults->append(QString("   File: %1").arg(fileName));
    ui->scanResults->append("");
}

void Antivirus::onScanComplete()
{
    ui->scanButton->setEnabled(true);
    ui->infectedFilesList->clear();
    int infected = infectedFiles.count();

    ui->scanResults->append("         SCAN COMPLETE");
    ui->scanResults->append(QString("\n Total files scanned: %1").arg(totalScanned));
    ui->scanResults->append(QString("\n  Threats found: %1").arg(infected));

    if (infected > 0) {
        ui->deleteButton->setEnabled(true);
        ui->deleteAllButton->setEnabled(true);
        ui->statusLabel->setText(QString("️ Scan complete - %1 threat(s) detected!").arg(infected));
        ui->infectedFilesList->addItems(infectedFiles);
        QMessageBox::warning(this, "Threats Detected",
                             QString("️ Warning!\n\nFound %1 infected file(s)!\n\nSelect files in the list and click 'Delete Selected' to remove them.").arg(infected));
    } else {
        ui->statusLabel->setText("✓ Scan complete - No threats detected");
        QMessageBox::information(this, "Scan Complete",
                                 "✓ Good news!\n\nNo threats detected.\nYour system is clean!");
    }
}

void Antivirus::onDeleteClicked()
{
    QList<QListWidgetItem*> selectedItems = ui->infectedFilesList->selectedItems();

    if (selectedItems.isEmpty()) {
        QMessageBox::information(this, "No Selection",
                                 "Please select at least one file to delete from the list");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Deletion",
                                  QString("Are you sure you want to delete %1 selected file(s)?\n\nThis action cannot be undone!")
                                      .arg(selectedItems.count()),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        return;
    }

    int deletedCount = 0;
    int failedCount = 0;

    for (QListWidgetItem* item : selectedItems) {
        QString filePath = item->text();
        QFile file(filePath);

        if (file.remove()) {
            deletedCount++;
            ui->scanResults->append(QString("✓ Deleted: %1").arg(filePath));
            delete item;
            infectedFiles.removeOne(filePath);
        } else {
            failedCount++;
            ui->scanResults->append(QString("✗ Failed to delete: %1").arg(filePath));
        }
    }

    QString summary;
    if (failedCount == 0) {
        summary = QString("Successfully deleted %1 infected file(s)!").arg(deletedCount);
        QMessageBox::information(this, "Deletion Complete", summary);
    } else {
        summary = QString("Deleted: %1\nFailed: %2\n\nSome files may be in use or require administrator privileges.").arg(deletedCount).arg(failedCount);
        QMessageBox::warning(this, "Deletion Incomplete", summary);
    }

    ui->scanResults->append("");
    ui->scanResults->append(summary);

    if (ui->infectedFilesList->count() == 0) {
        ui->deleteButton->setEnabled(false);
        ui->deleteAllButton->setEnabled(false);
        ui->statusLabel->setText("✓ All threats removed!");
    }
}

void Antivirus::onDeleteAllClicked()
{
    if (infectedFiles.isEmpty()) {
        QMessageBox::information(this, "No Threats",
                                 "There are no infected files to delete.");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Delete All",
                                  QString(" WARNING!\n\nAre you sure you want to delete ALL %1 infected file(s)?\n\nThis action cannot be undone!")
                                      .arg(infectedFiles.count()),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        return;
    }

    int deletedCount = 0;
    int failedCount = 0;

    QStringList filesToDelete = infectedFiles;

    for (const QString& filePath : filesToDelete) {
        QFile file(filePath);

        if (file.remove()) {
            deletedCount++;
            ui->scanResults->append(QString("✓ Deleted: %1").arg(filePath));
        } else {
            failedCount++;
            ui->scanResults->append(QString("✗ Failed to delete: %1").arg(filePath));
        }
    }

    ui->infectedFilesList->clear();
    infectedFiles.clear();

    QString summary;
    if (failedCount == 0) {
        summary = QString("✓ Successfully deleted all %1 infected file(s)!").arg(deletedCount);
        QMessageBox::information(this, "Deletion Complete", summary);
        ui->statusLabel->setText("✓ All threats removed!");
    } else {
        summary = QString("Deleted: %1\nFailed: %2\n\nSome files may be in use or require administrator privileges.").arg(deletedCount).arg(failedCount);
        QMessageBox::warning(this, "Deletion Incomplete", summary);
        ui->statusLabel->setText(QString("%1 threat(s) could not be removed").arg(failedCount));
    }

    ui->scanResults->append("");
    ui->scanResults->append(summary);

    ui->deleteButton->setEnabled(false);
    ui->deleteAllButton->setEnabled(false);
}
