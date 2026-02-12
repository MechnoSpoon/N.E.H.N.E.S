#ifndef ANTIVIRUS_H
#define ANTIVIRUS_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QMap>

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
    void onScanProgress(int current, int total);
    void onThreatFound(const QString &fileName, const QString &threatName);
    void onScanComplete();

private:
    Ui::Antivirus *ui;

    // Built-in scanner functionality
    QMap<QString, QString> virusSignatures;
    QStringList infectedFiles;
    int totalScanned;

    // Scanner methods
    void loadSignatures();
    bool scanFile(const QString &filePath);
    void scanDirectory(const QString &dirPath);
    void scanFileRecursive(const QString &path, QStringList &allFiles);

signals:
    void scanProgress(int current, int total);
    void threatFound(const QString &fileName, const QString &threatName);
    void scanComplete();
};

#endif
