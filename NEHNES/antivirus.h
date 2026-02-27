#ifndef ANTIVIRUS_H
#define ANTIVIRUS_H

#include <QDialog>
#include <QMap>
#include <QStringList>
#include "antivirusscanner.h"

namespace Ui {
class Antivirus;
}

class Antivirus : public QDialog
{
    Q_OBJECT

public:
    explicit Antivirus(QWidget *parent = nullptr);
    ~Antivirus();

private slots:
    void onScanClicked();
    void onDeleteClicked();
    void onDeleteAllClicked();
    void onScanProgress(int current, int total);
    void onThreatFound(QString fileName, QString threatName);
    void onScanComplete();

private:
    Ui::Antivirus *ui;
    AntivirusScanner *scanner;

    // ClamAV engine
    struct cl_engine *clamEngine;

    QMap<QString, QString> virusSignatures;
    QStringList infectedFiles;
    int totalScanned;

    void loadSignatures();
    void initializeClamAV();
    void cleanupClamAV();
    QStringList getAllFilesRecursive(const QString &path);
};

#endif // ANTIVIRUS_H
