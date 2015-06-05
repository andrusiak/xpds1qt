#include "chartwidget.h"
#include "ui_chartwidget.h"

ChartWidget::ChartWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ChartWidget)
{
    ui->setupUi(this);
    setDefaultIndex(0);
    prepareChart();
}

ChartWidget::~ChartWidget()
{
    delete ui;
}

ChartWidget::setDefaultIndex(int index)
{
    mode = index;
//    this->refresh();
//    ui->comboBox->setCurrentIndex(mode);
}
/** Makes pre-configuration of the chart: sets the lines, grid style, legend etc
 * @brief ChartWidget::initChart
 */
void ChartWidget::prepareChart(){
    QCustomPlot *customPlot = ui->chart;
    customPlot->clearGraphs();
    graphs.clear();

    switch(mode){
    case 0:
        customPlot->xAxis->setLabel("r");
        customPlot->yAxis->setLabel("Concentration");

        graphs.append(customPlot->addGraph());
        graphs.append(customPlot->addGraph());

        graphs[0]->setPen(QPen(Qt::blue,1));
        graphs[1]->setPen(QPen(Qt::red,1));

        graphs[0]->setName("Electrons");
        graphs[1]->setName("Ions");

        customPlot->legend->setVisible(true);
        break;
    case 1:

        customPlot->xAxis->setLabel("r");
        customPlot->yAxis->setLabel("Number of particles");

        graphs.append(customPlot->addGraph());
        graphs.append(customPlot->addGraph());

        graphs[0]->setPen(QPen(Qt::blue,1));
        graphs[1]->setPen(QPen(Qt::red,1));

        graphs[0]->setName("Electrons");
        graphs[1]->setName("Ions");

        customPlot->legend->setVisible(true);
        break;
    case 2:
        graphs.append(customPlot->addGraph());

        graphs[0]->setPen(QPen(Qt::blue,1));
        customPlot->xAxis->setLabel("r");
        customPlot->yAxis->setLabel("Potential, eV");

        customPlot->legend->setVisible(false);
        break;
    case 3:
        graphs.append(customPlot->addGraph());
//        graphs[0]->setPen(QPen(Qt::green,1));
        customPlot->xAxis->setLabel("r");
        customPlot->yAxis->setLabel("Field, eV");

        customPlot->legend->setVisible(false);
        break;
     case 4: //Q_d
        graphs.append(customPlot->addGraph());
        customPlot->xAxis->setLabel("time");
        customPlot->yAxis->setLabel("Dust grain charge, in e");
        customPlot->legend->setVisible(false);
        break;
     case 5: //dE_d
       graphs.append(customPlot->addGraph());
       customPlot->xAxis->setLabel("Time");
       customPlot->yAxis->setLabel("Ion energy flux");
       customPlot->legend->setVisible(false);
       break;
    }

        // set some pens, brushes and backgrounds:
        customPlot->xAxis->setBasePen(QPen(Qt::black, 1));
        customPlot->yAxis->setBasePen(QPen(Qt::black, 1));
        customPlot->xAxis->setTickPen(QPen(Qt::black, 1));
        customPlot->yAxis->setTickPen(QPen(Qt::black, 1));
        customPlot->xAxis->setTickLabelColor(Qt::black);
        customPlot->yAxis->setTickLabelColor(Qt::black);

        customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
        customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
}

void ChartWidget::refresh(){

    switch(mode){
    case 0:
        graphs[0]->setData(plasma->getR(), plasma->getElectronConcDistribution());
        graphs[1]->setData(plasma->getR(), plasma->getIonConcDistribution());
        break;
    case 1:
        graphs[0]->setData(plasma->getR(), plasma->getElectronsNumber());
        graphs[1]->setData(plasma->getR(), plasma->getIonsNumber());
        break;
    case 2:
        graphs[0]->setData(plasma->getR(),plasma->getPhiDistribution());
        break;
    case 3:
        graphs[0]->setData(plasma->getR(),plasma->getFieldDistribution());
        break;
    case 4:
        graphs[0]->setData(plasma->getTime(),plasma->getDustCharge());
        break;
    case 5:
        graphs[0]->setData(plasma->getTime(),plasma->getEnergyFluxes());
//        graphs[1]->setData(plasma->getTime(),plasma->getEnergyFluxes());
        break;

    }
    ui->chart->rescaleAxes();
    ui->chart->replot();
}

void ChartWidget::setDataSource(PlasmaModel *plasmaModel)
{
    plasma = plasmaModel;
}

void ChartWidget::on_comboBox_currentIndexChanged(int index)
{
    mode=index;
    prepareChart();
    refresh();
}
