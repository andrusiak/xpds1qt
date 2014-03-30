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
    int step=0;
    while(True)
    {
      t += dt;
      move();
      adjust();
      mcc();

      history();

//      printf("#%d\t%f\t%d\n",step,t, hist_hi);
      if((step+1)%10000==0){
          emit DataChanged();
          this->terminate();
      } else step++;
    }
}
