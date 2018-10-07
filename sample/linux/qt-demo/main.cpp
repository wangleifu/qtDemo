#include <QApplication>
#include <QPlastiqueStyle>
#include "widget.h"
#include "flight_controller.hpp"

using namespace DJI::OSDK;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(new QPlastiqueStyle);

    Widget w;

    // Setup OSDK.
    LinuxSetup linuxEnvironment(argc, argv);
    Vehicle*   vehicle = linuxEnvironment.getVehicle();
    if (vehicle == NULL)
    {
        std::cout << "Vehicle not initialized, exiting.\n";
        return -1;
    }

    Controller ctr(vehicle);
    w.set_controller(&ctr);

    w.show();
    return a.exec();
}
