#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <pds.h>
#include <QThread>
#include <QMutex>
#include <plasmamodel.h>

class WorkThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkThread(QObject *parent = 0);
    void setPlasmaModel(PlasmaModel*);
    PlasmaModel* updatePlasmaModel();

    void run();

    int argc;
    char **argv;
    QMutex mutex;
    bool stop;
    PlasmaModel* plasma;

signals:
    void DataChanged();

};

#endif // WORKTHREAD_H
