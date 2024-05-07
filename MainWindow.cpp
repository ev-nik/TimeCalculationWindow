#include "MainWindow.h"
#include "TimeWorkOnOff.h"
//------------------------------------------------------------------------------------

#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QVector>
#include <QTextStream>
#include <QHeaderView>
#include <QDebug>
#include <QStringList>
#include <QDesktopServices>
//------------------------------------------------------------------------------------

enum
{
    DT     = 0,
    STATUS = 1
};
//------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget* parent) : QWidget(parent)
{
    setGeometry(40, 40, 1150, 600);
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

    tEdit = new QTextEdit();

    selectFileButton = new QPushButton();
    selectFileButton->setText("Выбрать файл");

    saveCSVButton = new QPushButton();
    saveCSVButton->setText("Сохранить в CSV");

    saveHTMLButton = new QPushButton();
    saveHTMLButton->setText("Сохранить в HTML");

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(selectFileButton);
    hLayout->addStretch();
    hLayout->addWidget(saveHTMLButton);
    hLayout->addWidget(saveCSVButton);

    QHBoxLayout* hLayout1 = new QHBoxLayout();
    hLayout1->addWidget(tWidget);
    hLayout1->addWidget(tEdit);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout);

    connect(selectFileButton,  &QPushButton::clicked, this, &MainWindow::reloadData);
    connect(saveCSVButton,     &QPushButton::clicked, this, &MainWindow::saveInfoCSV);
    connect(saveHTMLButton,    &QPushButton::clicked, this, &MainWindow::saveInfoHTML);
}
//------------------------------------------------------------------------------------

void MainWindow::reloadTable(const QVector<TimeWorkOnOff>& timeWorkOnOff, int countTimeOnMSec, int countTimeOffMSec)
{
    tWidget->setRowCount(0);

    if(timeWorkOnOff.isEmpty())
    {
        return;
    }

    int row = 0;

    for(int i = 0; i < timeWorkOnOff.count(); i++)
    {
        QString value = timeWorkOnOff[i].status ? "On" : "Off"; // тернарный оператор (вместо if/else)

        QTableWidgetItem* dateTimeItem    = new QTableWidgetItem();
        QTableWidgetItem* switchItem      = new QTableWidgetItem();
        QTableWidgetItem* timeItem        = new QTableWidgetItem();
        QTableWidgetItem* countSwitcItem  = new QTableWidgetItem();

        dateTimeItem  ->setData(Qt::DisplayRole, timeWorkOnOff[i].dateTime.toString("dd.MM.yyyy hh:mm:ss"));

        if(timeWorkOnOff[i].status)
        {
            switchItem->setData(Qt::ForegroundRole, QBrush(Qt::green));
        }
        else
        {
            switchItem->setData(Qt::ForegroundRole, QBrush(Qt::red));
        }

        switchItem    ->setData(Qt::DisplayRole, value);
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
        switchItem->setData(Qt::ForegroundRole, QBrush(Qt::green));
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

        titleItem ->setData(Qt::DisplayRole, "Total");
        switchItem->setData(Qt::ForegroundRole, QBrush(Qt::red));
        switchItem->setData(Qt::DisplayRole, "Off");
        timeItem  ->setData(Qt::DisplayRole, QTime::fromMSecsSinceStartOfDay(countTimeOffMSec). toString("hh:mm:ss"));

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

        titleItem ->setData(Qt::DisplayRole, "Medium");
        switchItem->setData(Qt::ForegroundRole, QBrush(Qt::green));
        switchItem->setData(Qt::DisplayRole, "On");
        timeItem  ->setData(Qt::DisplayRole, QTime::QTime::fromMSecsSinceStartOfDay(mediumOn). toString("hh:mm:ss"));

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
        switchItem->setData(Qt::ForegroundRole, QBrush(Qt::red));
        switchItem->setData(Qt::DisplayRole, "Off");
        timeItem  ->setData(Qt::DisplayRole, QTime::fromMSecsSinceStartOfDay(mediumOff).toString("hh:mm:ss"));

        tWidget->insertRow(row);

        tWidget->setItem(row, 0, titleItem);
        tWidget->setItem(row, 1, switchItem);
        tWidget->setItem(row, 2, timeItem);
    }
}
//------------------------------------------------------------------------------------

void MainWindow::reloadHTML(const QVector<TimeWorkOnOff>& timeWorkOnOff, int countTimeOnMSec, int countTimeOffMSec)
{
    tEdit->clear();

    if(timeWorkOnOff.isEmpty())
    {
        return;
    }

    QString val = "<table border=1 width=100% height=100% cellspacing=-1 cellpadding=4>";
    val += "<tr>";
    val += "<th>Дата и время</th>";
    val += "<th>Состояние</th>";
    val += "<th>Время работы</th>";
    val += "<th>Номер переключения</th>";
    val += "</tr>";

    for(int i = 0; i < timeWorkOnOff.count(); i++)
    {
        QString value = timeWorkOnOff[i].status ? "On" : "Off"; // тернарный оператор (вместо if/else)

        val += "<tr>";
        //              сдвиг влево
        val += QString("<td align=center>%1</td>") .arg(timeWorkOnOff[i].dateTime.toString("dd.MM.yyyy hh:mm:ss"));
        if(value == "On")
        {
            val += QString("<td style=\"color:green;\" align=center>%1</td>").arg(value);
        }
        else
        {
            val += QString("<td style=\"color:red;\" align=center>%1</td>").arg(value);
        }

        val += QString("<td align=center>%1</td>") .arg(QTime::fromMSecsSinceStartOfDay(timeWorkOnOff[i].timeWorkMSec).toString("hh:mm:ss"));
        val += QString("<td width=10%>%1</td>")   .arg(timeWorkOnOff[i].countSwitch);
        val += "</tr>";
    }

    val += "</table>";

            // Перенос строки
    val += "<br/>";

    val += "<table border=1 width=100% height=100% cellspacing=-1 cellpadding=4>";
    val += "<tr>";
    val += "<th>Продолжительность работы</th>";
    val += "<th>Состояние</th>";
    val += "<th>Время работы</th>";
    val += "</tr>";

    {
        val += "<tr>";
        val += "<td>Всего</td>";
        val += "<td style=\"color:green;\">On</td>";
                            // объединить ячейки по колонкам
        val += QString("<td>%1</td>").arg(QTime::fromMSecsSinceStartOfDay(countTimeOnMSec).toString("hh:mm:ss"));
        val += "</tr>";
    }
    {
        val += "<tr>";
        val += "<td>Всего</td>";
        val += "<td style=\"color:red;\">Off</td>";
        val += QString("<td>%1</td>").arg(QTime::fromMSecsSinceStartOfDay(countTimeOffMSec).toString("hh:mm:ss"));
        val += "</tr>";
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
        val += "<tr>";
        val += "<td>Среднее</td>";
        val += "<td style=\"color:green;\">On</td>";
        val += QString("<td>%1</td>").arg(QTime::fromMSecsSinceStartOfDay(mediumOn).toString("hh:mm:ss"));
        val += "</tr>";
    }
    {
        val += "<tr>";
        val += "<td>Среднее</td>";
        val += "<td style=\"color:red;\">Off</td>";
        val += QString("<td>%1</td>").arg(QTime::fromMSecsSinceStartOfDay(mediumOff).toString("hh:mm:ss"));
        val += "</tr>";
    }

    val += "</table>";
    tEdit->setText(val);
}
//------------------------------------------------------------------------------------

QVector<TimeWorkOnOff> MainWindow::readFileIn()
{
    QVector<TimeWorkOnOff> dateTimeIn;

    pathIn = QFileDialog::getOpenFileName(this, "Открыть файл", "E:/");

    if(pathIn.isEmpty())
    {
        return dateTimeIn;
    }

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
    QVector<TimeWorkOnOff> timeWorkOnOff;
    timeWorkOnOff = readFileIn();

    int countTimeOnMSec  = 0;
    int countTimeOffMSec = 0;

    calcTime   (timeWorkOnOff, countTimeOnMSec, countTimeOffMSec);
    reloadTable(timeWorkOnOff, countTimeOnMSec, countTimeOffMSec);
    reloadHTML(timeWorkOnOff, countTimeOnMSec, countTimeOffMSec);
}
//------------------------------------------------------------------------------------

void MainWindow::saveInfoCSV()
{
    // @TODO: добавить путь на рабочий стол
    // @TODO: добавить имя базового файла в путь сохранения

    QString pathOutCSV = QFileDialog::getSaveFileName(this, "Сохранение", "Temper-out.csv");

    if(pathOutCSV.isEmpty())
    {
        return;
    }

    QFile fileOut(pathOutCSV);
    QFile::remove(pathOutCSV);

    if(!fileOut.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             QString("Не удалось сохранить файл: %1").arg(pathOutCSV),
                             QMessageBox::Close,
                             QMessageBox::Close);
        return;
    }

    QTextStream writeStream(&fileOut);
    QString strTable;

    for(int row = 0; row < tWidget->rowCount(); row++)
    {
        for(int col = 0; col < tWidget->columnCount(); col++)
        {
            QTableWidgetItem* item = tWidget->item(row, col);

            if(item == nullptr)
            {
                strTable += ";";
                continue;
            }

            QString str = item->text();

            strTable += str + ";";
        }

        strTable += "\n";
    }

    writeStream << strTable;
    fileOut.close();

    // @TODO: Добавить QMessageBox на открытие файла
}
//------------------------------------------------------------------------------------

void MainWindow::saveInfoHTML()
{
    // @TODO: добавить имя базового файла в путь сохранения

    QString defaultPath = QString("%1/Temper-out.html").arg(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));

    QString pathOutHTML = QFileDialog::getSaveFileName(this, "Сохранение", defaultPath);

    if(pathOutHTML.isEmpty())
    {
        return;
    }

    QFile fileOut(pathOutHTML);
    QFile::remove(pathOutHTML);

    if(!fileOut.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             QString("Не удалось сохранить файл: %1").arg(pathOutHTML),
                             QMessageBox::Close,
                             QMessageBox::Close);
        return;
    }

    QTextStream writeStream(&fileOut);
    writeStream << tEdit->toHtml();
    fileOut.close();

    QMessageBox::StandardButton clickedButton = QMessageBox::information(this,
                                                                         "Информация",
                                                                         QString("Файл успешно сохранен.\nОткрыть его?"),
                                                                         QMessageBox::Open,
                                                                         QMessageBox::Close);

    if(clickedButton == QMessageBox::Open)
    {
        QDesktopServices::openUrl( QUrl::fromLocalFile(pathOutHTML) );
    }
}
//------------------------------------------------------------------------------------












































