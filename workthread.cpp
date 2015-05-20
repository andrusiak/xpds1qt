#include "workthread.h"
#include <QFile>
#include <QTextStream>
#include <QtCore>
#include <iostream>


WorkThread::WorkThread(QObject *parent) :
    QThread(parent)
{
}

void WorkThread::run()
{
    /*********************************************************/
    /* Read input file, and initialize params and vars. This */
    /* function calls SPECIES and LOAD or Restore            */
    start_(argc, argv);

   // init output file
    QString filename = "Qdust(t).txt";
    QFile file(filename);

    history();

    MDFIELDS? mdfields(): fields();

    int step=0;
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream.setRealNumberPrecision(20);

        while(True)
        {
          if(!stop){
//            mutex.lock();
              t += dt;

              move();
              adjust();
              mcc();
              MDFIELDS? mdfields(): fields();
              history();

              if(((int)(t/dt))%2==0) stream << t<<"\t"<< qdust << "\t"<< energy_flux[0] <<"\t"<<  energy_flux[1] << endl;

              updatePlasmaModel();

             if(step%200==0 && step>0) {
//                 stop=true;
                 std::cout<< "Step "<<step<<": a signal sent to save data\n"<<endl;
                 emit DataChanged();
             }

            } else{
                this->msleep(50);
            }
            step++;
        }
    }
}

void WorkThread::setPlasmaModel(PlasmaModel *pm)
{
    plasma = pm;
}

QVector<double> time, Q_d, dE_d;
PlasmaModel *WorkThread::updatePlasmaModel()
{
    int size = nc*4/4;
    //Copy values from array to QVector. Required by QCustomPlot
    QVector<double> r(size),phi_(size),e_(size),ne(size),ni(size),npe(size),npi(size);

    for(int i=0; i<size;i++){
        r[i] = r_array[i];
        phi_[i] = phi[i];
        e_[i] = e[i];
        ne[i] = srho[0][i];
        ni[i] = srho[1][i];
        npe[i] = np_hist[0][i];
        npi[i] = np_hist[1][i];
        time.push_back(t/dt);
        Q_d.push_back(qdust/e0);
        dE_d.push_back(energy_flux[0]);
    }

    //Set the vectors into model and so to the views
    plasma->setR(r);
    plasma->setTime(time);
    if(!MDFIELDS) plasma->setPhiDistribution(phi_);
    plasma->setFieldDistribution(e_);
    plasma->setElectronConcDistribution(ne);
    plasma->setIonConcDistribution(ni);
    plasma->setElectronsNumber(npe);
    plasma->setIonsNumber(npi);
    plasma->setDustCharge(Q_d);
    plasma->setEnergyFluxes(dE_d);

    return plasma;
}
