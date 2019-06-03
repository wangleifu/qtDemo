#include "flight_control.hpp"

using namespace DJI;
using namespace DJI::OSDK;


FlightControl::FlightControl(Vehicle* vehicle)
{
	this->vehicle = vehicle;
}
FlightControl::~FlightControl()
{
	this->vehicle = NULL;
}


// 基础控制命令
void FlightControl::takeOff(int timeout)
{
	  //@todo: remove this once the getErrorCode function signature changes
	char func[50];
	int  pkgIndex;

	if (!vehicle->isM100() && !vehicle->isLegacyM600())
	{
	// Telemetry: Verify the subscription
	ACK::ErrorCode subscribeStatus;
	subscribeStatus = vehicle->subscribe->verify(timeout);
	if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
	{
		ACK::getErrorCodeMessage(subscribeStatus, func);
		return false;
	}

	// Telemetry: Subscribe to flight status and mode at freq 10 Hz
	pkgIndex                  = 0;
	int       freq            = 10;
	TopicName topicList10Hz[] = { TOPIC_STATUS_FLIGHT,
	                              TOPIC_STATUS_DISPLAYMODE };
	int  numTopic        = sizeof(topicList10Hz) / sizeof(topicList10Hz[0]);
	bool enableTimestamp = false;

	bool pkgStatus = vehicle->subscribe->initPackageFromTopicList(
	  pkgIndex, numTopic, topicList10Hz, enableTimestamp, freq);
	if (!(pkgStatus))
	{
		return pkgStatus;
	}
	subscribeStatus = vehicle->subscribe->startPackage(pkgIndex, timeout);
	if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
	{
		ACK::getErrorCodeMessage(subscribeStatus, func);
		// Cleanup before return
		vehicle->subscribe->removePackage(pkgIndex, timeout);
		return false;
	}
	}

	// Start takeoff
	ACK::ErrorCode takeoffStatus = vehicle->control->takeoff(timeout);
	if (ACK::getError(takeoffStatus) != ACK::SUCCESS)
	{
		ACK::getErrorCodeMessage(takeoffStatus, func);
		return false;
	}

	// First check: Motors started
	int motorsNotStarted = 0;
	int timeoutCycles    = 20;

	if (!vehicle->isM100() && !vehicle->isLegacyM600())
	{
	while (vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>() !=
		   VehicleStatus::FlightStatus::ON_GROUND &&
		   vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
		   VehicleStatus::DisplayMode::MODE_ENGINE_START &&
		   motorsNotStarted < timeoutCycles)
	{
		motorsNotStarted++;
		usleep(100000);
	}

	if (motorsNotStarted == timeoutCycles)
	{
		std::cout << "Takeoff failed. Motors are not spinning." << std::endl;
		// Cleanup
		if (!vehicle->isM100() && !vehicle->isLegacyM600())
		{
			vehicle->subscribe->removePackage(0, timeout);
		}
		return false;
	}
	else
	{
	  std::cout << "Motors spinning...\n";
	}
	}
	else if (vehicle->isLegacyM600())
	{
	while ((vehicle->broadcast->getStatus().flight <
			DJI::OSDK::VehicleStatus::FlightStatus::ON_GROUND) &&
			motorsNotStarted < timeoutCycles)
	{
		motorsNotStarted++;
		usleep(100000);
	}

	if (motorsNotStarted < timeoutCycles)
	{
		std::cout << "Successful TakeOff!" << std::endl;
	}
	}
	else // M100
	{
	while ((vehicle->broadcast->getStatus().flight <
			DJI::OSDK::VehicleStatus::M100FlightStatus::TAKEOFF) &&
			motorsNotStarted < timeoutCycles)
	{
		motorsNotStarted++;
		usleep(100000);
	}

	if (motorsNotStarted < timeoutCycles)
	{
		std::cout << "Successful TakeOff!" << std::endl;
	}
	}

	// Second check: In air
	int stillOnGround = 0;
	timeoutCycles     = 110;

	if (!vehicle->isM100() && !vehicle->isLegacyM600())
	{
	while (vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>() !=
		VehicleStatus::FlightStatus::IN_AIR &&
		(vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
		VehicleStatus::DisplayMode::MODE_ASSISTED_TAKEOFF ||
		vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
		VehicleStatus::DisplayMode::MODE_AUTO_TAKEOFF) &&
		stillOnGround < timeoutCycles)
	{
		stillOnGround++;
		usleep(100000);
	}

	if (stillOnGround == timeoutCycles)
	{
		std::cout << "Takeoff failed. Aircraft is still on the ground, but the "
					 "motors are spinning."
		<< std::endl;
	  // Cleanup
		if (!vehicle->isM100() && !vehicle->isLegacyM600())
		{
			vehicle->subscribe->removePackage(0, timeout);
		}
	  return false;
	}
	else
	{
		std::cout << "Ascending...\n";
	}
	}
	else if (vehicle->isLegacyM600())
	{
	while ((vehicle->broadcast->getStatus().flight <
			DJI::OSDK::VehicleStatus::FlightStatus::IN_AIR) &&
			stillOnGround < timeoutCycles)
	{
		stillOnGround++;
		usleep(100000);
	}

	if (stillOnGround < timeoutCycles)
	{
		std::cout << "Aircraft in air!" << std::endl;
	}
	}
	else // M100
	{
	while ((vehicle->broadcast->getStatus().flight !=
			DJI::OSDK::VehicleStatus::M100FlightStatus::IN_AIR_STANDBY) &&
			stillOnGround < timeoutCycles)
	{
		stillOnGround++;
		usleep(100000);
	}

	if (stillOnGround < timeoutCycles)
	{
		std::cout << "Aircraft in air!" << std::endl;
	}
	}

	// Final check: Finished takeoff
	if (!vehicle->isM100() && !vehicle->isLegacyM600())
	{
	while (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() ==
		VehicleStatus::DisplayMode::MODE_ASSISTED_TAKEOFF ||
		vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() ==
		VehicleStatus::DisplayMode::MODE_AUTO_TAKEOFF)
	{
	  sleep(1);
	}

	if (!vehicle->isM100() && !vehicle->isLegacyM600())
	{
		if (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
			VehicleStatus::DisplayMode::MODE_P_GPS ||
			vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
			VehicleStatus::DisplayMode::MODE_ATTITUDE)
		{
		std::cout << "Successful takeoff!\n";
		}
		else
		{
			std::cout
			<< "Takeoff finished, but the aircraft is in an unexpected mode. "
			   "Please connect DJI GO.\n";
			vehicle->subscribe->removePackage(0, timeout);
			return false;
		}
	}
	}
	else
	{
		float32_t                 delta;
		Telemetry::GlobalPosition currentHeight;
		Telemetry::GlobalPosition deltaHeight =
		vehicle->broadcast->getGlobalPosition();

		do
		{
			sleep(4);
			currentHeight = vehicle->broadcast->getGlobalPosition();
			delta         = fabs(currentHeight.altitude - deltaHeight.altitude);
			deltaHeight.altitude = currentHeight.altitude;
		} while (delta >= 0.009);

		std::cout << "Aircraft hovering at " << currentHeight.altitude << "m!\n";
	}

	// Cleanup
	if (!vehicle->isM100() && !vehicle->isLegacyM600())
	{
		ACK::ErrorCode ack = vehicle->subscribe->removePackage(pkgIndex, timeout);
		if (ACK::getError(ack))
		{
			std::cout
			<< "Error unsubscribing; please restart the drone/FC to get back "
			   "to a clean state.\n";
		}
	}

	return true;
}
void FlightControl::land(int timeout)
{
  //@todo: remove this once the getErrorCode function signature changes
  char func[50];
  int  pkgIndex;

  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    // Telemetry: Verify the subscription
    ACK::ErrorCode subscribeStatus;
    subscribeStatus = vehicle->subscribe->verify(timeout);
    if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
    {
      ACK::getErrorCodeMessage(subscribeStatus, func);
      return false;
    }

    // Telemetry: Subscribe to flight status and mode at freq 10 Hz
    pkgIndex                  = 0;
    int       freq            = 10;
    TopicName topicList10Hz[] = { TOPIC_STATUS_FLIGHT,
                                  TOPIC_STATUS_DISPLAYMODE };
    int  numTopic        = sizeof(topicList10Hz) / sizeof(topicList10Hz[0]);
    bool enableTimestamp = false;

    bool pkgStatus = vehicle->subscribe->initPackageFromTopicList(
      pkgIndex, numTopic, topicList10Hz, enableTimestamp, freq);
    if (!(pkgStatus))
    {
      return pkgStatus;
    }
    subscribeStatus = vehicle->subscribe->startPackage(pkgIndex, timeout);
    if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
    {
      ACK::getErrorCodeMessage(subscribeStatus, func);
      // Cleanup before return
      vehicle->subscribe->removePackage(pkgIndex, timeout);
      return false;
    }
  }

  // Start landing
  ACK::ErrorCode landingStatus = vehicle->control->land(timeout);
  if (ACK::getError(landingStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(landingStatus, func);
    return false;
  }

  // First check: Landing started
  int landingNotStarted = 0;
  int timeoutCycles     = 20;

  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    while (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
             VehicleStatus::DisplayMode::MODE_AUTO_LANDING &&
           landingNotStarted < timeoutCycles)
    {
      landingNotStarted++;
      usleep(100000);
    }
  }
  else if (vehicle->isM100())
  {
    while (vehicle->broadcast->getStatus().flight !=
             DJI::OSDK::VehicleStatus::M100FlightStatus::LANDING &&
           landingNotStarted < timeoutCycles)
    {
      landingNotStarted++;
      usleep(100000);
    }
  }

  if (landingNotStarted == timeoutCycles)
  {
    std::cout << "Landing failed. Aircraft is still in the air." << std::endl;
    // Cleanup before return
    ACK::ErrorCode ack = vehicle->subscribe->removePackage(pkgIndex, timeout);
    if (ACK::getError(ack))
    {
      std::cout << "Error unsubscribing; please restart the drone/FC to get "
                   "back to a clean state.\n";
    }
    return false;
  }
  else
  {
    std::cout << "Landing...\n";
  }

  // Second check: Finished landing
  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    while (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() ==
             VehicleStatus::DisplayMode::MODE_AUTO_LANDING &&
           vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>() ==
             VehicleStatus::FlightStatus::IN_AIR)
    {
      sleep(1);
    }

    if (vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
          VehicleStatus::DisplayMode::MODE_P_GPS ||
        vehicle->subscribe->getValue<TOPIC_STATUS_DISPLAYMODE>() !=
          VehicleStatus::DisplayMode::MODE_ATTITUDE)
    {
      std::cout << "Successful landing!\n";
    }
    else
    {
      std::cout
        << "Landing finished, but the aircraft is in an unexpected mode. "
           "Please connect DJI GO.\n";
      ACK::ErrorCode ack = vehicle->subscribe->removePackage(pkgIndex, timeout);
      if (ACK::getError(ack))
      {
        std::cout << "Error unsubscribing; please restart the drone/FC to get "
                     "back to a clean state.\n";
      }
      return false;
    }
  }
  else if (vehicle->isLegacyM600())
  {
    while (vehicle->broadcast->getStatus().flight >
           DJI::OSDK::VehicleStatus::FlightStatus::STOPED)
    {
      sleep(1);
    }

    Telemetry::GlobalPosition gp;
    do
    {
      sleep(2);
      gp = vehicle->broadcast->getGlobalPosition();
    } while (gp.altitude != 0);

    if (gp.altitude != 0)
    {
      std::cout
        << "Landing finished, but the aircraft is in an unexpected mode. "
           "Please connect DJI GO.\n";
      return false;
    }
    else
    {
      std::cout << "Successful landing!\n";
    }
  }
  else // M100
  {
    while (vehicle->broadcast->getStatus().flight ==
           DJI::OSDK::VehicleStatus::M100FlightStatus::FINISHING_LANDING)
    {
      sleep(1);
    }

    Telemetry::GlobalPosition gp;
    do
    {
      sleep(2);
      gp = vehicle->broadcast->getGlobalPosition();
    } while (gp.altitude != 0);

    if (gp.altitude != 0)
    {
      std::cout
        << "Landing finished, but the aircraft is in an unexpected mode. "
           "Please connect DJI GO.\n";
      return false;
    }
    else
    {
      std::cout << "Successful landing!\n";
    }
  }

  // Cleanup
  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    ACK::ErrorCode ack = vehicle->subscribe->removePackage(pkgIndex, timeout);
    if (ACK::getError(ack))
    {
      std::cout
        << "Error unsubscribing; please restart the drone/FC to get back "
           "to a clean state.\n";
    }
  }

  return true;
}
void FlightControl::goHome()
{

}

// 记录飞行轨迹
void FlightControl::beginTrackRecord();
void FlightControl::endTrackRecord();
void FlightControl::storageTrack();