#include "MainWindow.h"
#include "TimeWorkOnOff.h"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QVector>
#include <QTextStream>
#include <QHeaderView>

enum
{
    DT     = 0,
    STATUS = 1
};

MainWindow::MainWindow(QWidget* parent) : QWidget(parent)
{
    setGeometry(40, 40, 500, 600);
    setWindowTitle("MainWindow");

    tWidget = new QTableWidget();
    tWidget->setColumnCount(4);
    tWidget->setHorizontalHeaderLabels({"Дата и время", "Состояние", "Время работы", "Номер переключения"});
    // Получили указатель на горизонтальный заголовок
    QHeaderView* headerView = tWidget->horizontalHeader();

//  Устанавливаем признак растяжения последней колонки
//  headerView->setStretchLastSection(true);

//  Устанавливаем режим растяжения чтоб колонки растягивались равномерно
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    pButton = new QPushButton();
    pButton->setText("Выбрать файл");
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(pButton);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(tWidget);
    vLayout->addLayout(hLayout);

    connect(pButton, &QPushButton::clicked, this, &MainWindow::reloadData);
}

void MainWindow::reloadTable( QVector<TimeWorkOnOff>& timeWorkOnOff, int& countTimeOnMSec, int& countTimeOffMSec)
{
    if(timeWorkOnOff.isEmpty())
    {
        return;
    }

    int row = 0;

    for(int i = 0; i < timeWorkOnOff.count(); i++)
    {
        QString val = timeWorkOnOff[i].status ? "On" : "Off"; // тернарный оператор (вместо if/else)

        QTableWidgetItem* dateTimeItem    = new QTableWidgetItem();
        QTableWidgetItem* switchItem      = new QTableWidgetItem();
        QTableWidgetItem* timeItem        = new QTableWidgetItem();
        QTableWidgetItem* countSwitcItem  = new QTableWidgetItem();

        dateTimeItem  ->setData(Qt::DisplayRole, timeWorkOnOff[i].dateTime.toString("dd.MM.yyyy hh:mm:ss"));
        switchItem    ->setData(Qt::DisplayRole, val);
        timeItem      ->setData(Qt::DisplayRole, QTime::fromMSecsSinceStartOfDay(timeWorkOnOff[i].timeWorkMSec).toString("hh:mm:ss"));
        countSwitcItem->setData(Qt::DisplayRole, timeWorkOnOff[i].countSwitch);

        tWidget->insertRow(row);

        tWidget->setItem(row, 0, dateTimeItem);
        tWidget->setItem(row, 1, switchItem);
        tWidget->setItem(row, 2, timeItem);
        tWidget->setItem(row, 3, countSwitcItem);
        row++;
    }
    {
        QTableWidgetItem* titleItem  = new QTableWidgetItem();
        QTableWidgetItem* switchItem = new QTableWidgetItem();
        QTableWidgetItem* timeItem   = new QTableWidgetItem();

        titleItem ->setData(Qt::DisplayRole, "Total");
        switchItem->setData(Qt::DisplayRole, "On");
        timeItem  ->setData(Qt::DisplayRole, QTime::fromMSecsSinceStartOfDay(countTimeOnMSec). toString("hh:mm:ss"));

        tWidget->insertRow(row);

        tWidget->setItem(row, 0, titleItem);
        tWidget->setItem(row, 1, switchItem);
        tWidget->setItem(row, 2, timeItem);
        row++;
    }
    {
        QTableWidgetItem* titleItem  = new QTableWidgetItem();
        QTableWidgetItem* switchItem = new QTableWidgetItem();
        QTableWidgetItem* timeItem   = new QTableWidgetItem();

        titleItem->  setData(Qt::DisplayRole, "Total");
        switchItem->  setData(Qt::DisplayRole, "Off");
        timeItem->setData(Qt::DisplayRole, QTime::fromMSecsSinceStartOfDay(countTimeOffMSec). toString("hh:mm:ss"));

        tWidget->insertRow(row);

        tWidget->setItem(row, 0, titleItem);
        tWidget->setItem(row, 1, switchItem);
        tWidget->setItem(row, 2, timeItem);
        row++;
    }

    int mediumOn;
    int mediumOff;
    int sizeVec   = timeWorkOnOff.count();
    int lastIndex = sizeVec - 1;

    for(int i = lastIndex - 1; i <= lastIndex; i++)
    {
        if(timeWorkOnOff[i].status)
        {
            mediumOn  = countTimeOnMSec  / timeWorkOnOff[i].countSwitch;
        }

        if(!timeWorkOnOff[i].status)
        {
            mediumOff = countTimeOffMSec / timeWorkOnOff[i].countSwitch;
        }
    }
    {
        QTableWidgetItem* titleItem  = new QTableWidgetItem();
        QTableWidgetItem* switchItem = new QTableWidgetItem();
        QTableWidgetItem* timeItem   = new QTableWidgetItem();

        titleItem-> setData(Qt::DisplayRole, "Medium");
        switchItem->setData(Qt::DisplayRole, "On");
        timeItem->  setData(Qt::DisplayRole, QTime::QTime::fromMSecsSinceStartOfDay(mediumOn). toString("hh:mm:ss"));

        tWidget->insertRow(row);

        tWidget->setItem(row, 0, titleItem);
        tWidget->setItem(row, 1, switchItem);
        tWidget->setItem(row, 2, timeItem);
        row++;
    }
    {
        QTableWidgetItem* titleItem  = new QTableWidgetItem();
        QTableWidgetItem* switchItem = new QTableWidgetItem();
        QTableWidgetItem* timeItem   = new QTableWidgetItem();

        titleItem ->setData(Qt::DisplayRole, "Medium");
        switchItem->setData(Qt::DisplayRole, "Off");
        timeItem  ->setData(Qt::DisplayRole, QTime::fromMSecsSinceStartOfDay(mediumOff).toString("hh:mm:ss"));

        tWidget->insertRow(row);

        tWidget->setItem(row, 0, titleItem);
        tWidget->setItem(row, 1, switchItem);
        tWidget->setItem(row, 2, timeItem);
    }
}
//------------------------------------------------------------------------------------

QVector<TimeWorkOnOff> MainWindow::readFileIn(QString pathIn)
{
    QVector<TimeWorkOnOff> dateTimeIn;

    pathIn = QFileDialog::getOpenFileName();

    QFile fileIn(pathIn);

    if(!fileIn.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             QString("Не удалось открыть файл: %1").arg(pathIn),
                             QMessageBox::Close,
                             QMessageBox::Close);
        return dateTimeIn;
    }

    QTextStream readStream(&fileIn);

    while(!readStream.atEnd())
    {
        TimeWorkOnOff tWork;

        QString val = readStream.readLine();

        QStringList split_val = val.split(';');

        tWork.dateTime = QDateTime::fromString(split_val[DT], "dd.MM.yyyy hh:mm:ss");

        if(split_val[STATUS].contains("ON", Qt::CaseInsensitive))
        {
            tWork.status = true;
        }
        else if(split_val[STATUS].contains("OFF", Qt::CaseSensitive))
        {
            tWork.status = false;
        }

        dateTimeIn.append(tWork);
    }

    return dateTimeIn;
}
//------------------------------------------------------------------------------------

void MainWindow::calcTime(QVector<TimeWorkOnOff>& dateTimeIn, int& countTimeOnMSec, int& countTimeOffMSec)
{
    countTimeOnMSec  = 0;
    countTimeOffMSec = 0;
    int sizeVec      = dateTimeIn.count();

    if(dateTimeIn.isEmpty())
    {
        return;
    }

    if(dateTimeIn[0].status)
    {
        countTimeOffMSec = dateTimeIn[0].dateTime.time().msecsSinceStartOfDay();
    }
    else
    {
        countTimeOnMSec = dateTimeIn[0].dateTime.time().msecsSinceStartOfDay();
    }

    dateTimeIn[0].countSwitch++;

    for(int i = 1; i < sizeVec; i++)
    {
        TimeWorkOnOff& current = dateTimeIn[i];     //текущее состояние
        TimeWorkOnOff& prev    = dateTimeIn[i - 1]; // предыдущее состояние

        prev.timeWorkMSec = prev.dateTime.time().msecsTo(current.dateTime.time());

        current.countSwitch++;

        if(current.status)
        {
            countTimeOffMSec += prev.timeWorkMSec;
            if(i > 1)
            {
                current.countSwitch = dateTimeIn[i - 2].countSwitch + 1;
            }
        }
        else
        {
            countTimeOnMSec += prev.timeWorkMSec;
            if(i > 1)
            {
                current.countSwitch = dateTimeIn[i - 2].countSwitch + 1;
            }
        }
    }

    int lastIndex = sizeVec - 1;

    // время работы последнего срабатывания
    dateTimeIn[ lastIndex ].timeWorkMSec = dateTimeIn [ lastIndex ].dateTime.time().msecsTo(QTime(23, 59, 59));

    if(dateTimeIn[ lastIndex ].status)
    {
        countTimeOnMSec += dateTimeIn[ lastIndex ].timeWorkMSec;
    }
    else
    {
        countTimeOffMSec += dateTimeIn[ lastIndex ].timeWorkMSec;
    }
}
//------------------------------------------------------------------------------------

void MainWindow::reloadData()
{
    QVector<TimeWorkOnOff> timeWorkOnOff = readFileIn(pathIn);

    int countTimeOnMSec  = 0;
    int countTimeOffMSec = 0;

    calcTime   (timeWorkOnOff, countTimeOnMSec, countTimeOffMSec);
    reloadTable(timeWorkOnOff, countTimeOnMSec, countTimeOffMSec);
}
