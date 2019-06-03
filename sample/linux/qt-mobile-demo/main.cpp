#include <QApplication>
#include <QPlastiqueStyle>
#include "widget.h"
#include "controller.hpp"
#include "mobile.hpp"

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
    std::cout << "vehicle.." << std::endl;

    //FlightController f_controller(vehicle);
    Controller controller(vehicle,&w);
    w.set_controller(&controller);

    Mobile mobile(&controller);
    mobile.initMSDKParsing(vehicle, &linuxEnvironment);

    w.show();
    return a.exec();
}
