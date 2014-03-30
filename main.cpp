#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.argc = argc;
    w.argv = (char **)argv;

    return a.exec();
}
