#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include <QString>
#include <QVector>

#include "TimeWorkOnOff.h"

class MainWindow : public QWidget
{
public:
    MainWindow(QWidget* parent = nullptr);

private:
    QTableWidget* tWidget;
    QTextEdit*    tEdit;
    QPushButton*  selectFileButton;
    QPushButton*  saveCSVButton;
    QPushButton*  saveHTMLButton;

    QString       pathIn;

public slots:
    void reloadData();
    void saveInfoCSV();
    void saveInfoHTML();

public:
    QVector<TimeWorkOnOff> readFileIn();
    void calcTime(QVector<TimeWorkOnOff>         & dateTimeIn,    int& countTimeOnMSec, int& countTimeOffMSec);
    void reloadTable(const QVector<TimeWorkOnOff>& timeWorkOnOff, int  countTimeOnMSec, int  countTimeOffMSec);
    void reloadHTML(const QVector<TimeWorkOnOff> & timeWorkOnOff, int  countTimeOnMSec, int  countTimeOffMSec);
};
#endif // MAINWINDOW_H
