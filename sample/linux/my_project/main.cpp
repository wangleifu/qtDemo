#include <QApplication>
#include <QPlastiqueStyle>
#include "widget.h"
#include "flight_controller.hpp"
#include "mobile_communication.hpp"

using namespace DJI::OSDK;
int main(int argc, char* argv)
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

    Controller controller(vehicle);
    MobileService mobileService(vehicle, linuxEnvironment, &controller);
    
    w.setController(&controller);

    w.show();
    return a.exec();
}