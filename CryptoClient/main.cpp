#include "mainwindow.h"
#include "Controller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("CryptoClient");
    QApplication::setApplicationVersion("0.5b");
    MainWindow w;
    Controller controller(&w);
    w.show();
    return QCoreApplication::exec();
}
