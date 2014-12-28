#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QFrame>
//#include "pds.h"
#include <iostream>
#include "plasmamodel.h"
#include "qcustomplot.h"

namespace Ui {
class ChartWidget;
}

class ChartWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = 0);
    ~ChartWidget();

public slots:
    void refresh();
    void setDataSource(PlasmaModel*);

private slots:
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::ChartWidget* ui;
    QVector<QCPGraph*> graphs;
    PlasmaModel* plasma;
    int mode;

    void prepareChart();
    void setupPotentialChart();
    void setupConcentrationChart();
    void setupNumberParticleChart();
};

#endif // CHARTWIDGET_H
