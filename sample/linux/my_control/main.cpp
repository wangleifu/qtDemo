#include <QApplication>
#include "widget.h"
#include "flight_controller.hpp"

using namespace DJI::OSDK;
int main(int argc, char *argv[])
{
//    // Initialize variables
//    int functionTimeout = 1;

//    // Setup OSDK.
//    LinuxSetup linuxEnvironment(argc, argv);
//    Vehicle*   vehicle = linuxEnvironment.getVehicle();
//    if (vehicle == NULL)
//    {
//        std::cout << "Vehicle not initialized, exiting.\n";
//        return -1;
//    }

//    // Obtain Control Authority
//    vehicle->obtainCtrlAuthority(functionTimeout);

    QApplication a(argc, argv);

    Widget w;
    Controller ctr(0);

    ctr.set_widget(&w);
    w.set_controller(&ctr);

    w.show();
    
    return a.exec();
}
