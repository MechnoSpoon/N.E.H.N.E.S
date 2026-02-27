#include "antivirusscanner.h"
#include <QFile>
#include <QThread>

AntivirusScanner::AntivirusScanner(const QStringList& filesToScan,
                                   struct cl_engine *engine,
                                   QObject *parent)
    : QThread(parent)
    , files(filesToScan)
    , clamEngine(engine)
{
}

void AntivirusScanner::run()
{
    int total = files.size();

    for (int i = 0; i < total; ++i) {
        if (isInterruptionRequested()) {
            break;
        }

        emit scanProgress(i + 1, total);

        QString detectedThreat;
        if (scanFileWithClamAV(files[i], detectedThreat)) {
            emit threatFound(files[i], detectedThreat);
        }

        // Small delay to prevent overwhelming the UI
        QThread::msleep(1);
    }

    emit scanComplete();
}

bool AntivirusScanner::scanFileWithClamAV(const QString& filePath, QString& detectedThreat)
{
    if (!clamEngine) {
        // Fallback to basic EICAR detection if ClamAV not available
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }

        QByteArray fileData = file.readAll();
        file.close();

        QString eicarSignature = "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";
        if (fileData.contains(eicarSignature.toUtf8())) {
            detectedThreat = "EICAR-Test";
            return true;
        }
        return false;
    }

    // Use ClamAV to scan the file
    const char *virname = nullptr;
    unsigned long int scanned = 0;

    struct cl_scan_options options = {};
    options.general = CL_SCAN_GENERAL_ALLMATCHES;
    options.parse = ~0u;
    cl_error_t ret = cl_scanfile(filePath.toUtf8().constData(), &virname, &scanned, clamEngine, &options);

    if (ret == CL_VIRUS) {
        detectedThreat = QString::fromUtf8(virname);
        return true;
    }

    return false;
}
