#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QString>
#include <QVector>

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
    void reloadTable(QVector<TimeWorkOnOff>& timeWorkOnOff, int& countTimeOnMSec, int& countTimeOffMSec);
    void reloadData();

public:
    QVector<TimeWorkOnOff> readFileIn(QString pathIn);
    void calcTime(QVector<TimeWorkOnOff>& dateTimeIn, int& countTimeOnMSec, int& countTimeOffMSec);
};
#endif // MAINWINDOW_H
