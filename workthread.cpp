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


   // InitWindows(argc, argv);

    history();
    fields();

    int step=0;
    float *pot = phi;
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
              fields();
              history();

              if(((int)(t/dt))%2==0) stream << t<<"\t"<< qdust << "\t"<< energy_flux[0] <<"\t"<<  energy_flux[1] << endl;

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
