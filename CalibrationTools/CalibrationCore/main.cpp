#include <QApplication>
#include "mainwindow.h"
#include <QMetaType>
#include <Eigen/Dense>
#include <vector>

int main (int argc, char *argv[])
{
    qRegisterMetaType<std::vector<Eigen::Vector3f>>("std::vector<Eigen::Vector3f>");

    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
