
#include "settings.h"
#include "ui_settings.h"

Settings_H::Settings_H(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings_H)
{
    ui->setupUi(this);  // Load the UI file

    // Now you can access widgets from the .ui file like:
    // ui->myButton, ui->myLabel, etc.
}

Settings_H::~Settings_H()
{
    delete ui;
}

