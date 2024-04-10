#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QString>

#include <TimeWorkOnOff.h>

class MainWindow : public QWidget
{
public:
    MainWindow(QWidget* parent = nullptr);

private:
    QPushButton*  pButton;
    QTableWidget* tWidget;
    QString       pathIn;

public slots:
    void readWriteInfo();
    void printTWidget(QVector<TimeWorkOnOff>& dateTimeIn, int& countTimeOnMSec, int& countTimeOffMSec);
};
#endif // MAINWINDOW_H
