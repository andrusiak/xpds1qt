#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "qcustomplot.h"
#include "chartwidget.h"

#include "workthread.h"
//#include "pds.h"s


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int argc;
    char **argv;

    WorkThread* mThread;

private slots:
    void on_startButton_clicked();
    void onDataChanged();
    void realtimeDataSlot();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;
    QString demoName;

};

#endif // MAINWINDOW_H
