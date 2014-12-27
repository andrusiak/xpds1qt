#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <pds.h>
#include <QThread>
#include <QMutex>

class WorkThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkThread(QObject *parent = 0);
    void run();

    int argc;
    char **argv;
    QMutex mutex;
    bool stop;

signals:
    void DataChanged();

};

#endif // WORKTHREAD_H
