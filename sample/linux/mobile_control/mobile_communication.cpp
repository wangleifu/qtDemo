#include "mobile_communication.hpp"

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;

MobileController::MobileController(Vehicle* vehicle, LinuxSetup* linuxEnvironment)
{
  // First, register the callback for parsing mobile data
  vehicle->moc->setFromMSDKCallback(parseFromMobileCallback, linuxEnvironment);

  // // Then, setup a thread to poll the incoming data, for large functions
  // pthread_t threadID = setupSamplePollingThread(vehicle);

  return true;
}

void MobileController::parseFromMobileCallback(DJI::OSDK::Vehicle*      vehicle,
                             DJI::OSDK::RecvContainer recvFrame,
                             DJI::OSDK::UserData      userData)
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
      sendDroneVersionFromCache(vehicle);
      break;
    case 2:
      vehicle->obtainCtrlAuthority(controlAuthorityMobileCallback);
      break;
    case 3:
      vehicle->releaseCtrlAuthority(controlAuthorityMobileCallback);
      break;
    case 4:
      vehicle->activate(linuxEnvironment->getActivateData(),
                        activateMobileCallback);
      break;
    case 5:
      vehicle->control->armMotors(actionMobileCallback);
      break;
    case 6:
      vehicle->control->disArmMotors(actionMobileCallback);
      break;
    case 7:
      vehicle->control->takeoff(actionMobileCallback);
      break;
    case 8:
      vehicle->control->land(actionMobileCallback);
      break;
    case 9:
      vehicle->control->goHome(actionMobileCallback);
      break;
    case 10:
      vehicle->camera->shootPhoto();
      sendAckToMobile(vehicle, mobile_data_id);
      break;
    case 11:
      vehicle->camera->videoStart();
      sendAckToMobile(vehicle, mobile_data_id);
      break;
    case 12:
      vehicle->camera->videoStop();
      sendAckToMobile(vehicle, mobile_data_id);
      break;
    case 0x3E:
      //! Since we have a long function call involving blocking API calls here,
      //! we'll be setting a global and servicing it in a different thread.
      mobileDataID_glob = mobile_data_id;
      break;
    case 0x40:
      //! Since we have a long function call involving blocking API calls here,
      //! we'll be setting a global and servicing it in a different thread.
      mobileDataID_glob = mobile_data_id;
      break;
    case 0x41:
      //! Since we have a long function call involving blocking API calls here,
      //! we'll be setting a global and servicing it in a different thread.
      mobileDataID_glob = mobile_data_id;
      break;
    case 0x42:
      //! Since we have a long function call involving blocking API calls here,
      //! we'll be setting a global and servicing it in a different thread.
      mobileDataID_glob = mobile_data_id;
      break;
    default:
      break;
  }
}