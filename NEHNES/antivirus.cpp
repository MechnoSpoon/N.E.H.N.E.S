#include "antivirus.h"
#include "ui_antivirus.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>

Antivirus::Antivirus(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Antivirus)
    , totalScanned(0)
{
    ui->setupUi(this);

    setWindowTitle("NEHNES Antivirus Scanner");

    loadSignatures();

    connect(ui->scanButton, &QPushButton::clicked, this, &Antivirus::onScanClicked);
    connect(this, &Antivirus::scanProgress, this, &Antivirus::onScanProgress);
    connect(this, &Antivirus::threatFound, this, &Antivirus::onThreatFound);
    connect(this, &Antivirus::scanComplete, this, &Antivirus::onScanComplete);

    ui->progressBar->setValue(0);
    ui->statusLabel->setText("Ready to scan");
}

Antivirus::~Antivirus()
{
    delete ui;
}

void Antivirus::loadSignatures()
{
    // EICAR test virus (safe test file)
    virusSignatures["EICAR-Test"] = "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";

    // Add more signatures here as needed
}

bool Antivirus::scanFile(const QString &filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray fileData = file.readAll();
    file.close();

    totalScanned++;

    for (auto it = virusSignatures.begin(); it != virusSignatures.end(); ++it) {
        QString threatName = it.key();
        QString signature = it.value();

        if (fileData.contains(signature.toUtf8())) {
            infectedFiles.append(filePath);
            emit threatFound(filePath, threatName);
            return true;
        }
    }

    return false;
}

void Antivirus::scanFileRecursive(const QString &path, QStringList &allFiles)
{
    QDir dir(path);
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo &entry : entries) {
        if (entry.isDir()) {
            scanFileRecursive(entry.absoluteFilePath(), allFiles);
        } else {
            allFiles.append(entry.absoluteFilePath());
        }
    }
}

void Antivirus::scanDirectory(const QString &dirPath)
{
    QStringList allFiles;
    scanFileRecursive(dirPath, allFiles);

    int total = allFiles.count();
    totalScanned = 0;
    infectedFiles.clear();

    for (int i = 0; i < allFiles.count(); i++) {
        scanFile(allFiles[i]);
        emit scanProgress(i + 1, total);
    }

    emit scanComplete();
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
    ui->scanResults->append("═══════════════════════════════════════");
    ui->scanResults->append("    NEHNES ANTIVIRUS SCAN STARTED");
    ui->scanResults->append("═══════════════════════════════════════");
    ui->scanResults->append("Scanning directory: " + dirPath);
    ui->scanResults->append("");

    ui->scanButton->setEnabled(false);
    ui->progressBar->setValue(0);

    scanDirectory(dirPath);
}

void Antivirus::onScanProgress(int current, int total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(current);
    ui->statusLabel->setText(QString("⏳ Scanning... %1 of %2 files").arg(current).arg(total));
}

void Antivirus::onThreatFound(const QString &fileName, const QString &threatName)
{
    ui->scanResults->append(QString(" THREAT DETECTED!"));
    ui->scanResults->append(QString("   Threat: %1").arg(threatName));
    ui->scanResults->append(QString("   File: %1").arg(fileName));
    ui->scanResults->append("");
}

void Antivirus::onScanComplete()
{
    ui->scanButton->setEnabled(true);

    int infected = infectedFiles.count();

    ui->scanResults->append("═══════════════════════════════════════");
    ui->scanResults->append("         SCAN COMPLETE");
    ui->scanResults->append("═══════════════════════════════════════");
    ui->scanResults->append(QString(" Total files scanned: %1").arg(totalScanned));
    ui->scanResults->append(QString("  Threats found: %1").arg(infected));
    ui->scanResults->append("═══════════════════════════════════════");

    if (infected > 0) {
        ui->statusLabel->setText(QString("️ Scan complete - %1 threat(s) detected!").arg(infected));
        QMessageBox::warning(this, "Threats Detected",
                             QString("️ Warning!\n\nFound %1 infected file(s)!").arg(infected));
    } else {
        ui->statusLabel->setText(" Scan complete - No threats detected");
        QMessageBox::information(this, "Scan Complete",
                                 " Good news!\n\nNo threats detected.\nYour system is clean!");
    }
}
