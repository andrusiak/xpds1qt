#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);

    mThread = new WorkThread(this);

    connect(mThread, SIGNAL(DataChanged()), this, SLOT(on_saveButton_clicked()));

    // setup a timer that repeatedly calls MainWindow::onDataChanged:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(onDataChanged()));
    dataTimer.start(20); // Interval 0 means to refresh as fast as possible
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    mThread->argc = this->argc;
    mThread->argv = this->argv;

    mThread->stop=false;

    ui->progressBar->setVisible(true);

    mThread->start();
}

void MainWindow::onDataChanged()
{
//    mThread->mutex.lock();

    setupPotentialChart(ui->chart_2);
    setupConcentrationChart(ui->chart);

//    setupData(ui->chart);
//    setupNumberParticleChart(ui->chart_2);

    ui->progressBar->setVisible(false);

    ui->timeBox->setText(QString("Time: %1").arg(t/dt));
    ui->dustChargeBox->setText(QString("%1").arg(qdust/1.602e-19));
    ui->energyFluxBox->setText(QString("%1").arg(energy_flux[0],energy_flux[1]));

    //release mutex
//    mThread->mutex.unlock();
}

void MainWindow::setupPotentialChart(QCustomPlot *customPlot){
    customPlot->clearGraphs();

    // prepare data:
    int size = nc*3/4;
    QVector<double> r(size), potential(size);


    for (int i=0; i<r.size(); ++i)
    {
        r[i] = r_array[i];
        potential[i] = phi[i];
        if(DEBUG)
        {
        std::cout << r_array[i] <<"\t"<< potential[i]<<"\n";
        }
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

void MainWindow::setupConcentrationChart(QCustomPlot *customPlot){
    customPlot->clearGraphs();

    // prepare data:

    int size = nc*3/4;

    QVector<double> r(size), ne(size),ni(size);

    for (int i=0; i<r.size(); ++i)
    {
        r[i] = r_array[i];
        ne[i] = srho[0][i];
        ni[i] = srho[1][i];
        if(DEBUG)
        {
        std::cout << r_array[i] <<"\t"<< ne[i]<<"\t"<< ni[i]<<"\n";
        }
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

void MainWindow::setupNumberParticleChart(QCustomPlot *customPlot)
{
    customPlot->clearGraphs();

    // prepare data:

    std::cout <<"Number of Particle\n";

    QVector<double> x1(hist_hi), np_e(hist_hi),np_i(hist_hi);
    for (int i=0; i<x1.size(); ++i)
    {
        x1[i] = t_array[i];
        np_e[i] = np_hist[0][i];
        np_i[i] = np_hist[1][i];

        if(DEBUG)
        {
        std::cout << t_array[i]/dt <<"\t"<< np_hist[0][i]<<"\t"<< np_hist[1][i]<<"\n";
        }
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

void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  QMessageBox::critical(this, "", "You're using Qt < 4.7, the realtime data demo needs functions that are available with Qt 4.7 to work properly");
#endif
//  demoName = "Real Time Data Demo";

  // include this section to fully disable antialiasing for higher performance:
  /*
  customPlot->setNotAntialiasedElements(QCP::aeAll);
  QFont font;
  font.setStyleStrategy(QFont::NoAntialias);
  customPlot->xAxis->setTickLabelFont(font);
  customPlot->yAxis->setTickLabelFont(font);
  customPlot->legend->setFont(font);
  */
  customPlot->addGraph(); // blue line
  customPlot->graph(0)->setPen(QPen(Qt::blue));
  customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
  customPlot->graph(0)->setAntialiasedFill(false);
  customPlot->addGraph(); // red line
  customPlot->graph(1)->setPen(QPen(Qt::red));
  customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1));

  customPlot->addGraph(); // blue dot
  customPlot->graph(2)->setPen(QPen(Qt::blue));
  customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot->addGraph(); // red dot
  customPlot->graph(3)->setPen(QPen(Qt::red));
  customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);

  customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
  customPlot->xAxis->setAutoTickStep(false);
  customPlot->xAxis->setTickStep(2);
  customPlot->axisRect()->setupFullAxesBox();

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void MainWindow::setupData(QCustomPlot *customPlot){
    customPlot->clearGraphs();

    // prepare data:

    QVector<double> x1(hist_hi), jwall0(hist_hi),jwall1(hist_hi);
    for (int i=0; i<x1.size(); ++i)
    {
        x1[i] = t_array[i];
        jwall0[i] = jwall_hist[0][i];
        jwall1[i] = jwall_hist[1][i];

        if(DEBUG)
        {
            std::cout << t_array[i]/dt <<"\t"<< jwall0[i]<<"\t"<< jwall1[i]<<"\n";
        }
    }

    // add data and plot
    QCPGraph *graph1 = customPlot->addGraph();
    graph1->setData(x1, jwall0);
    QCPGraph *graph2 = customPlot->addGraph();
    graph2->setData(x1, jwall1);


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

void MainWindow::saveToFile(QVector<double> x,QVector<double> y,QString filename){
    QFile file(filename);
    if ( file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream stream( &file );

                for (int i=0; i<x.size(); ++i)
                {
                    stream << x[i] << "\t" << y[i] << endl;
                }
                file.close();
            }

}

void MainWindow::saveToFile2(QVector<double> x,QVector<double> y, QVector<double> y2,QString filename){
    QFile file(filename);
    if ( file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream stream( &file );

                for (int i=0; i<x.size(); ++i)
                {
                    stream << x[i] << "\t" << y[i]<< "\t" << y2[i] << endl;
                }
                file.close();
            }

}

void MainWindow::realtimeDataSlot()
{
    setupPotentialChart(ui->chart);
    setupConcentrationChart(ui->chart_2);
}

void MainWindow::setupStyledDemo(QCustomPlot *customPlot)
{
//  demoName = "Styled Demo";

  // prepare data:
  QVector<double> x1(20), y1(20);
  QVector<double> x2(100), y2(100);
  QVector<double> x3(20), y3(20);
  QVector<double> x4(20), y4(20);
  for (int i=0; i<x1.size(); ++i)
  {
    x1[i] = i/(double)x1.size()*10;
    y1[i] = qCos(x1[i]*0.8+qSin(x1[i]*0.16+1.0))*qSin(x1[i]*0.54)+1.4;
  }
  for (int i=0; i<x2.size(); ++i)
  {
    x2[i] = i/(double)x2.size()*10;
    y2[i] = qCos(x2[i]*0.85+qSin(x2[i]*0.165+1.1))*qSin(x2[i]*0.50)+1.7;
  }
  for (int i=0; i<x3.size(); ++i)
  {
    x3[i] = i/(double)x3.size()*10;
    y3[i] = 0.05+3*(0.5+qCos(x3[i]*x3[i]*0.2+2)*0.5)/(double)(x3[i]+0.7)+qrand()/(double)RAND_MAX*0.01;
  }
  for (int i=0; i<x4.size(); ++i)
  {
    x4[i] = x3[i];
    y4[i] = (0.5-y3[i])+((x4[i]-2)*(x4[i]-2)*0.02);
  }

  // create and configure plottables:
  QCPGraph *graph1 = customPlot->addGraph();
  graph1->setData(x1, y1);
  graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
  graph1->setPen(QPen(QColor(120, 120, 120), 2));

  QCPGraph *graph2 = customPlot->addGraph();
  graph2->setData(x2, y2);
  graph2->setPen(Qt::NoPen);
  graph2->setBrush(QColor(200, 200, 200, 20));
  graph2->setChannelFillGraph(graph1);

//  QCPBars *bars1 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
//  customPlot->addPlottable(bars1);
//  bars1->setWidth(9/(double)x3.size());
//  bars1->setData(x3, y3);
//  bars1->setPen(Qt::NoPen);
//  bars1->setBrush(QColor(10, 140, 70, 160));

//  QCPBars *bars2 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
//  customPlot->addPlottable(bars2);
//  bars2->setWidth(9/(double)x4.size());
//  bars2->setData(x4, y4);
//  bars2->setPen(Qt::NoPen);
//  bars2->setBrush(QColor(10, 100, 50, 70));
//  bars2->moveAbove(bars1);

  // move bars above graphs and grid below bars:
  customPlot->addLayer("abovemain", customPlot->layer("main"), QCustomPlot::limAbove);
  customPlot->addLayer("belowmain", customPlot->layer("main"), QCustomPlot::limBelow);
  graph1->setLayer("abovemain");
  customPlot->xAxis->grid()->setLayer("belowmain");
  customPlot->yAxis->grid()->setLayer("belowmain");

  // set some pens, brushes and backgrounds:
  customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
  customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
  customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
  customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
  customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
  customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
  customPlot->xAxis->setTickLabelColor(Qt::white);
  customPlot->yAxis->setTickLabelColor(Qt::white);
  customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
  customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
  customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
  customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
  customPlot->xAxis->grid()->setSubGridVisible(true);
  customPlot->yAxis->grid()->setSubGridVisible(true);
  customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
  customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
  customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
  customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
  QLinearGradient plotGradient;
  plotGradient.setStart(0, 0);
  plotGradient.setFinalStop(0, 350);
  plotGradient.setColorAt(0, QColor(80, 80, 80));
  plotGradient.setColorAt(1, QColor(50, 50, 50));
  customPlot->setBackground(plotGradient);
  QLinearGradient axisRectGradient;
  axisRectGradient.setStart(0, 0);
  axisRectGradient.setFinalStop(0, 350);
  axisRectGradient.setColorAt(0, QColor(80, 80, 80));
  axisRectGradient.setColorAt(1, QColor(30, 30, 30));
  customPlot->axisRect()->setBackground(axisRectGradient);

  customPlot->rescaleAxes();
  customPlot->yAxis->setRange(0, 2);
}


void MainWindow::on_saveButton_clicked()
{
    int size = nc*3/4;
    char* potentialFilename="out/potential-%1.txt";
    char* concFilename="out/ni_ne-%1.txt";

    //Save potential
    QVector<double> r(size), potential(size);
    for (int i=0; i<size; ++i)
    {
        r[i] = r_array[i];
        potential[i] = phi[i];
    }
    saveToFile(r,potential,QString(potentialFilename).arg((t/dt)));

    //Save concentration of the electrons and ions
    QVector<double> /*r(size),*/ ne(size),ni(size);
    for (int i=0; i<size; ++i)
    {
        r[i] = r_array[i];
        ne[i] = srho[0][i];
        ni[i] = srho[1][i];
    }
    saveToFile2(r,ne,ni, QString(concFilename).arg((t/dt)));

}

void MainWindow::on_stopButton_clicked()
{
    if(!mThread->stop) mThread->stop=true;
}
