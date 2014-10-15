#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QFrame>
//#include "pds.h"
#include <iostream>

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

private:
    Ui::ChartWidget *ui;

    void setupPotentialChart();
    void setupConcentrationChart();
    void setupNumberParticleChart();
};

#endif // CHARTWIDGET_H
