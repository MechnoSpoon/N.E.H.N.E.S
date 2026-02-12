/********************************************************************************
** Form generated from reading UI file 'antivirus.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ANTIVIRUS_H
#define UI_ANTIVIRUS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Antivirus
{
public:
    QPushButton *scanButton;
    QProgressBar *progressBar;
    QLabel *titleLabel;
    QTextEdit *scanResults;
    QLabel *statusLabel;
    QListWidget *infectedFilesList;
    QPushButton *deleteButton;

    void setupUi(QWidget *Antivirus)
    {
        if (Antivirus->objectName().isEmpty())
            Antivirus->setObjectName("Antivirus");
        Antivirus->resize(640, 534);
        scanButton = new QPushButton(Antivirus);
        scanButton->setObjectName("scanButton");
        scanButton->setGeometry(QRect(230, 260, 56, 18));
        progressBar = new QProgressBar(Antivirus);
        progressBar->setObjectName("progressBar");
        progressBar->setGeometry(QRect(310, 190, 118, 23));
        progressBar->setValue(24);
        titleLabel = new QLabel(Antivirus);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setGeometry(QRect(160, 40, 161, 91));
        scanResults = new QTextEdit(Antivirus);
        scanResults->setObjectName("scanResults");
        scanResults->setGeometry(QRect(230, 290, 261, 121));
        scanResults->setReadOnly(true);
        statusLabel = new QLabel(Antivirus);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setGeometry(QRect(160, 150, 191, 16));
        infectedFilesList = new QListWidget(Antivirus);
        infectedFilesList->setObjectName("infectedFilesList");
        infectedFilesList->setGeometry(QRect(420, 30, 201, 151));
        deleteButton = new QPushButton(Antivirus);
        deleteButton->setObjectName("deleteButton");
        deleteButton->setGeometry(QRect(470, 200, 121, 21));

        retranslateUi(Antivirus);

        QMetaObject::connectSlotsByName(Antivirus);
    } // setupUi

    void retranslateUi(QWidget *Antivirus)
    {
        Antivirus->setWindowTitle(QCoreApplication::translate("Antivirus", "Form", nullptr));
        scanButton->setText(QCoreApplication::translate("Antivirus", "Scan File", nullptr));
        titleLabel->setText(QCoreApplication::translate("Antivirus", "TextLabel", nullptr));
        statusLabel->setText(QCoreApplication::translate("Antivirus", "TextLabel", nullptr));
        deleteButton->setText(QCoreApplication::translate("Antivirus", "Delete", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Antivirus: public Ui_Antivirus {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ANTIVIRUS_H
