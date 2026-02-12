#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(ui->antivirusButton, &QPushButton::clicked, this, &MainWindow::onAntivirusClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSettingsClicked()
{
    Settings_H settingsDialog(this);
    settingsDialog.exec();
}

void MainWindow::onAntivirusClicked()
{
    Antivirus antivirusDialog(this);
    antivirusDialog.exec();
}
