#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "string.h"
#include "workthread.h"
#include "pds.h"

#define DEBUG false

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
    void saveToFile(QVector<double> x, QVector<double> y, QString filename);
    void saveToFile2(QVector<double> x, QVector<double> y, QVector<double> y2, QString filename);


private slots:
    void onDataChanged();
    void realtimeDataSlot();

    void on_startButton_clicked();
    void on_saveButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;
    QString demoName;

};

#endif // MAINWINDOW_H
