/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Settings_H
{
public:

    void setupUi(QWidget *Settings_H)
    {
        if (Settings_H->objectName().isEmpty())
            Settings_H->setObjectName("Settings_H");
        Settings_H->resize(400, 300);

        retranslateUi(Settings_H);

        QMetaObject::connectSlotsByName(Settings_H);
    } // setupUi

    void retranslateUi(QWidget *Settings_H)
    {
        Settings_H->setWindowTitle(QCoreApplication::translate("Settings_H", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Settings_H: public Ui_Settings_H {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
