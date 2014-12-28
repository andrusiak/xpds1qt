#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    plasma = new PlasmaModel();
    mThread = new WorkThread(this);

    //pass data model into working thread and to the view
    mThread->setPlasmaModel(plasma);
    ui->chartFrame->setDataSource(plasma);

//    connect(mThread, SIGNAL(DataChanged()), this, SLOT(on_saveButton_clicked()));

//    setup a timer that repeatedly calls MainWindow::onDataChanged:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(onDataChanged()));
    connect(&dataTimer, SIGNAL(timeout()), ui->chartFrame, SLOT(refresh()));

    dataTimer.start(50); // Interval 0 means to refresh as fast as possible
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

    mThread->start();
}

void MainWindow::onDataChanged()
{
//    mThread->mutex.lock();

    setupConcentrationChart(ui->chart);
    setupPotentialChart(ui->chart_2);


//    setupData(ui->chart);
//    setupNumberParticleChart(ui->chart_2);

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

     graph1->setName("Potential");
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
    graph1->setName("Electrons");

    QCPGraph *graph2 = customPlot->addGraph();
    graph2->setData(r, ni);
    graph2->setName("Ions");


    //Configure legend
    customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);


     // graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
     graph1->setPen(QPen(QColor(51, 51, 255), 1));
     graph2->setPen(QPen(QColor(204, 0, 0), 1));


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
//      customPlot->xAxis->grid()->setSubGridVisible(true);
//      customPlot->yAxis->grid()->setSubGridVisible(true);
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
//      customPlot->replot();

}

void MainWindow::setupJwallChart(QCustomPlot *customPlot){
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
//      customPlot->replot();

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

void MainWindow::on_saveButton_clicked()
{
    int size = nc*3/4;
    char potentialFilename[]="out/potential-%1.txt";
    char concFilename[]="out/ni_ne-%1.txt";

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
