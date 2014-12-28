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
    ui->chartFrame_2->setDataSource(plasma);


//    connect(mThread, SIGNAL(DataChanged()), this, SLOT(on_saveButton_clicked()));

//    setup a timer that repeatedly calls
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(onDataChanged()));
    connect(&dataTimer, SIGNAL(timeout()), ui->chartFrame, SLOT(refresh()));
    connect(&dataTimer, SIGNAL(timeout()), ui->chartFrame_2, SLOT(refresh()));

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
    ui->timeBox->setText(QString("Time: %1").arg(t/dt));
    ui->dustChargeBox->setText(QString("%1").arg(qdust/1.602e-19));
    ui->energyFluxBox->setText(QString("%1").arg(energy_flux[0],energy_flux[1]));
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
