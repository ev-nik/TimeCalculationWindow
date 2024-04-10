#ifndef TIMEWORKONOFF_H
#define TIMEWORKONOFF_H

#include <QDateTime>

class TimeWorkOnOff
{
public:
    TimeWorkOnOff();

public:
    QDateTime dateTime;
    bool      status;
    int       timeWorkMSec;
    int       countSwitch;
};
#endif // TIMEWORKONOFF_H
