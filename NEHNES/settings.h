#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>


namespace Ui {
class Settings_H;  // Forward declaration
}

class Settings_H : public QDialog
{
    Q_OBJECT

public:
    explicit Settings_H(QWidget *parent = nullptr);
    ~Settings_H();

private:
    Ui::Settings_H *ui;
};

#endif

