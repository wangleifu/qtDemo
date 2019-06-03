#ifndef MOBILE_HPP
#define MOBILE_HPP

// DJI OSDK includes
#include "dji_vehicle.hpp"
// Helpers
#include "dji_linux_helpers.hpp"

#include "controller.hpp"

typedef struct AckReturnToMobile
{
  uint16_t cmdID;
  uint16_t ack;
} AckReturnToMobile;

class Mobile
{
public:
    Mobile(Controller* controller);
    ~Mobile();

public:
    bool initMSDKParsing(DJI::OSDK::Vehicle* vehicle, LinuxSetup* linuxEnvironment);
private:
    void parseFromMobileCallback(DJI::OSDK::Vehicle* vehicle, DJI::OSDK::RecvContainer recvFrame,
                            DJI::OSDK::UserData userData);
    void sendAckToMobile(DJI::OSDK::Vehicle* vehiclePtr, uint16_t cmdID, uint16_t ack = 0);

private:
    Controller* controller;
};

#endif
