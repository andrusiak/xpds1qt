#include "workthread.h"
#include <QtCore>

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

   // InitWindows(argc, argv);

    history();
    fields();

    int step=0;
    float *pot = phi;
    while(True)
    {
      t += dt;

      move();
      adjust();
      mcc();
      fields();
      history();

      pot = phi;
 //      pot = phi[nc/2];

//       printf("%f\t", phi[nc/2]);

//    printf("#%d\t%f\t%d\n",step,t, hist_hi);
      if((step+1)%100000==0){
//          mutex->unlock();
          emit DataChanged();
          this->terminate();
      } else {
          step++;
//          mutex->lock();
        }
    }
}
