#include "mainwindow.h"
#include <QApplication>

extern "C" {
    #include "pds1.h"
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    pds_main(argc, (char **)argv);
    printf("Number of cells is %d\n",nc);

    return a.exec();
}
