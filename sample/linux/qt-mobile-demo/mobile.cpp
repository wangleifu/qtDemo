#include "mobile.hpp"

using namespace DJI;
using namespace DJI::OSDK;

Mobile::Mobile(Controller *controller)
{
    this->controller = controller;
    //this->controller->takeOff();
}
Mobile::~Mobile()
{
}



bool
Mobile::initMSDKParsing(Vehicle* vehicle, LinuxSetup* linuxEnvironment)
{
    vehicle->moc->setFromMSDKCallback((VehicleCallBack)(&Mobile::parseFromMobileCallback), linuxEnvironment);
    return true;
}

void
Mobile::parseFromMobileCallback(Vehicle* vehicle, RecvContainer recvFrame, UserData userData)
{
    std::cout << "parsing mobile message...." << std::endl;
    std::cout << "vehicle:   " << vehicle << std::endl;
    std::cout << "recvFrame: " << recvFrame << std::endl;
    std::cout << "userData:  " << userData << std::endl;
    uint16_t   mobile_data_id;
    mobile_data_id = *(reinterpret_cast<uint16_t*>(&recvFrame.recvData.raw_ack_array));
    switch(mobile_data_id)
    {
    case 1:
        // do something
        std::cout << "get id: 1" << std::endl;
        break;
    case 2:
        // do something
        std::cout << "get id: 2" << std::endl;
        break;
    case 3:
        // do something
        std::cout << "get id: 3" << std::endl;
        break;
    case 4:
        // do something
        std::cout << "get id: 4" << std::endl;
        break;
    case 5:
        // do something
        std::cout << "get id: 5" << std::endl;
        break;
    default:
        break;
    }
    std::cout << "mobile message...." << std::endl;

//    std::cout << "send back..." << std::endl;
//    Mobile::sendAckToMobile(vehicle, 1);
}
/*
void
Mobile::parseFromMobileCallback(Vehicle* vehicle, RecvContainer recvFrame, UserData userData)
{
    // First, lets cast the userData to LinuxSetup*
      LinuxSetup* linuxEnvironment = (LinuxSetup*)userData;
      uint16_t    mobile_data_id;
      mobile_data_id =
        *(reinterpret_cast<uint16_t*>(&recvFrame.recvData.raw_ack_array));

      // Now, let's set up some variables that don't cross case initializations
      bool    coreMissionStatus;
      uint8_t wayptPolygonSides;
      int     hotptInitRadius;
      int     responseTimeout = 1;

      switch (mobile_data_id)
      {
        case 1:
          //sendDroneVersionFromCache(vehicle);
//          this->controller->takeOff();
          break;
        case 2:
          //vehicle->obtainCtrlAuthority(controlAuthorityMobileCallback);
          break;
        case 3:
          //vehicle->releaseCtrlAuthority(controlAuthorityMobileCallback);
          break;
        case 4:
          //vehicle->activate(linuxEnvironment->getActivateData(),
                            //activateMobileCallback);
          break;
        case 5:
          //vehicle->control->armMotors(actionMobileCallback);
          break;
        case 6:
          //vehicle->control->disArmMotors(actionMobileCallback);
          break;
        case 7:
          //vehicle->control->takeoff(actionMobileCallback);
          break;
        case 8:
          //vehicle->control->land(actionMobileCallback);
          break;
        case 9:
          //vehicle->control->goHome(actionMobileCallback);
          break;
        case 10:
          //vehicle->camera->shootPhoto();
          Mobile::sendAckToMobile(vehicle, mobile_data_id);
          break;
        case 11:
          //vehicle->camera->videoStart();
          Mobile::sendAckToMobile(vehicle, mobile_data_id);
          break;
        case 12:
          //vehicle->camera->videoStop();
          Mobile::sendAckToMobile(vehicle, mobile_data_id);
          break;
        case 0x3E:
          //! Since we have a long function call involving blocking API calls here,
          //! we'll be setting a global and servicing it in a different thread.
          //mobileDataID_glob = mobile_data_id;
          break;
        case 0x40:
          //! Since we have a long function call involving blocking API calls here,
          //! we'll be setting a global and servicing it in a different thread.
          //mobileDataID_glob = mobile_data_id;
          break;
        case 0x41:
          //! Since we have a long function call involving blocking API calls here,
          //! we'll be setting a global and servicing it in a different thread.
          //mobileDataID_glob = mobile_data_id;
          break;
        case 0x42:
          //! Since we have a long function call involving blocking API calls here,
          //! we'll be setting a global and servicing it in a different thread.
          //mobileDataID_glob = mobile_data_id;
          break;
        default:
          break;
      }
}
*/
void
Mobile::sendAckToMobile(Vehicle* vehiclePtr, uint16_t cmdID, uint16_t ack)
{
  // Generate a local ACK to send the ACK back to mobile
  AckReturnToMobile mobileAck;
  mobileAck.cmdID = cmdID;
  mobileAck.ack   = ack;
  vehiclePtr->moc->sendDataToMSDK(reinterpret_cast<uint8_t*>(&mobileAck),
                                  sizeof(mobileAck));
}
