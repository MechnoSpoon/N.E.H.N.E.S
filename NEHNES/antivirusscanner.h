#ifndef ANTIVIRUSSCANNER_H
#define ANTIVIRUSSCANNER_H

#include <QThread>
#include <QStringList>
#include <QMap>
#include <clamav.h>

class AntivirusScanner : public QThread
{
    Q_OBJECT

public:
    AntivirusScanner(const QStringList& filesToScan,
                     struct cl_engine *engine,
                     QObject *parent = nullptr);

    void run() override;

signals:
    void scanProgress(int current, int total);
    void threatFound(QString filePath, QString threatName);
    void scanComplete();

private:
    QStringList files;
    struct cl_engine *clamEngine;

    bool scanFileWithClamAV(const QString& filePath, QString& detectedThreat);
};

#endif // ANTIVIRUSSCANNER_H
