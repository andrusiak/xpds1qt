#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "qcustomplot.h"

#include "workthread.h"
#include "pds.h"


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

    WorkThread * mThread;

    void setupPotentialChart(QCustomPlot *customPlot);
    void setupConcentrationChart(QCustomPlot *customPlot);
    void setupNumberParticleChart(QCustomPlot *customPlot);

    void setupRealtimeDataDemo(QCustomPlot *customPlot);

    void setupData(QCustomPlot *customPlot);
    void setupStyledDemo(QCustomPlot *customPlot);

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
