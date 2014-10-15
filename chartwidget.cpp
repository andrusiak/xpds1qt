#include "chartwidget.h"
#include "ui_chartwidget.h"

ChartWidget::ChartWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ChartWidget)
{
    ui->setupUi(this);
}

ChartWidget::~ChartWidget()
{
    delete ui;
}

void ChartWidget::refresh(){

    switch(ui->comboBox->currentIndex()){
    case 0:
        setupConcentrationChart();
        break;
    case 1:
        setupNumberParticleChart();
        break;
    case 2:
        setupPotentialChart();
        break;
    }
}

void ChartWidget::setupPotentialChart(){
    QCustomPlot *customPlot = ui->chart;
    customPlot->clearGraphs();

    // prepare data:

    QVector<double> r(nc), potential(nc);


    for (int i=0; i<r.size(); ++i)
    {
        r[i] = r_array[i];
        potential[i] = phi[i];
        std::cout << r_array[i] <<"\t"<< potential[i]<<"\n";
    }

    // add data and plot
    QCPGraph *graph1 = customPlot->addGraph();
    graph1->setData(r, potential);

     // graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
     graph1->setPen(QPen(QColor(120, 120, 120), 2));

     customPlot->xAxis->setLabel("r");
     customPlot->yAxis->setLabel("Potential, eV");

      // set some pens, brushes and backgrounds:
      customPlot->xAxis->setBasePen(QPen(Qt::black, 1));
      customPlot->yAxis->setBasePen(QPen(Qt::black, 1));
      customPlot->xAxis->setTickPen(QPen(Qt::black, 1));
      customPlot->yAxis->setTickPen(QPen(Qt::black, 1));
      customPlot->xAxis->setSubTickPen(QPen(Qt::blue, 1));
      customPlot->yAxis->setSubTickPen(QPen(Qt::blue, 1));
      customPlot->xAxis->setTickLabelColor(Qt::black);
      customPlot->yAxis->setTickLabelColor(Qt::black);
      customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
      customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
      customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
      customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
      customPlot->xAxis->grid()->setSubGridVisible(true);
      customPlot->yAxis->grid()->setSubGridVisible(true);
      customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
      customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

      customPlot->rescaleAxes();
      customPlot->replot();

}

void ChartWidget::setupConcentrationChart(){
    QCustomPlot *customPlot = ui->chart;
    customPlot->clearGraphs();

    // prepare data:

    QVector<double> r(ng), ne(ng),ni(ng);

    for (int i=0; i<r.size(); ++i)
    {
        r[i] = r_array[i];
        ne[i] = srho[0][i];
        ni[i] = srho[1][i];
        std::cout << r_array[i] <<"\t"<< ne[i]<<"\t"<< ni[i]<<"\n";
    }

    // add data and plot
    QCPGraph *graph1 = customPlot->addGraph();
    graph1->setData(r, ne);

    QCPGraph *graph2 = customPlot->addGraph();
    graph2->setData(r, ni);

     // graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
     graph1->setPen(QPen(QColor(120, 120, 120), 2));

     customPlot->xAxis->setLabel("r");
     customPlot->yAxis->setLabel("Particle Concentration");

      // set some pens, brushes and backgrounds:
      customPlot->xAxis->setBasePen(QPen(Qt::black, 1));
      customPlot->yAxis->setBasePen(QPen(Qt::black, 1));
      customPlot->xAxis->setTickPen(QPen(Qt::black, 1));
      customPlot->yAxis->setTickPen(QPen(Qt::black, 1));
      customPlot->xAxis->setSubTickPen(QPen(Qt::blue, 1));
      customPlot->yAxis->setSubTickPen(QPen(Qt::blue, 1));
      customPlot->xAxis->setTickLabelColor(Qt::black);
      customPlot->yAxis->setTickLabelColor(Qt::black);
      customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
      customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
      customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
      customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
      customPlot->xAxis->grid()->setSubGridVisible(true);
      customPlot->yAxis->grid()->setSubGridVisible(true);
      customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
      customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

      customPlot->rescaleAxes();
      customPlot->replot();

}

void ChartWidget::setupNumberParticleChart()
{
    QCustomPlot *customPlot = ui->chart;
    customPlot->clearGraphs();

    // prepare data:
    std::cout <<"Number of Particle\n";

    QVector<double> x1(hist_hi), np_e(hist_hi),np_i(hist_hi);
    for (int i=0; i<x1.size(); ++i)
    {
        x1[i] = t_array[i];
        np_e[i] = np_hist[0][i];
        np_i[i] = np_hist[1][i];

        std::cout << t_array[i]/dt <<"\t"<< np_hist[0][i]<<"\t"<< np_hist[1][i]<<"\n";
    }

    // add data and plot
    QCPGraph *graph1 = customPlot->addGraph();
    graph1->setData(x1, np_e);
    QCPGraph *graph2 = customPlot->addGraph();
    graph2->setData(x1, np_i);


     // graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
     graph1->setPen(QPen(QColor(120, 120, 120), 2));

     customPlot->xAxis->setLabel("Time, s");
     customPlot->yAxis->setLabel("Jwall, eV");

      // set some pens, brushes and backgrounds:
      customPlot->xAxis->setBasePen(QPen(Qt::black, 1));
      customPlot->yAxis->setBasePen(QPen(Qt::black, 1));
      customPlot->xAxis->setTickPen(QPen(Qt::black, 1));
      customPlot->yAxis->setTickPen(QPen(Qt::black, 1));
      customPlot->xAxis->setSubTickPen(QPen(Qt::blue, 1));
      customPlot->yAxis->setSubTickPen(QPen(Qt::blue, 1));
      customPlot->xAxis->setTickLabelColor(Qt::black);
      customPlot->yAxis->setTickLabelColor(Qt::black);
      customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
      customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
      customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
      customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
      customPlot->xAxis->grid()->setSubGridVisible(true);
      customPlot->yAxis->grid()->setSubGridVisible(true);
      customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
      customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

      customPlot->rescaleAxes();
      customPlot->replot();

}
