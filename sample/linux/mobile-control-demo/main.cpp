#include "mission_manager.hpp"
#include "mobile_callback.hpp"
#include <dji_linux_helpers.hpp>

using namespace DJI::OSDK;

void sendTraceName(DJI::OSDK::Vehicle* vehiclePtr);
bool setupMSDKParsing(Vehicle* vehicle, LinuxSetup* linuxEnvironment);
void parseFromMobileCallback(Vehicle* vehicle, RecvContainer recvFrame, UserData userData);

ThreadManager threadManager;

// 程序入口函数
int main(int argc, char *argv[])
{
    //DJI::OSDK::Log::instance().disableStatusLogging();
    DJI::OSDK::Log::instance().enableDebugLogging();

    // Setup OSDK.
    LinuxSetup linuxEnvironment(argc, argv);
    Vehicle*   vehicle = linuxEnvironment.getVehicle();
    if (vehicle == NULL)
    {
        std::cout << "Vehicle not initialized, exiting.\n";
        return -1;
    }

    threadManager.setVehicle(vehicle);
    setupMSDKParsing(vehicle, &linuxEnvironment);

    while(true);
    return 0;
}





// 设置mbile的监听器
bool setupMSDKParsing(Vehicle* vehicle, LinuxSetup* linuxEnvironment)
{
    DSTATUS("--------setup MSDK Callback function--------");
    vehicle->moc->setFromMSDKCallback(parseFromMobileCallback, linuxEnvironment);
    return true;
}

// mobile 通信内容解析 与 功能执行
void parseFromMobileCallback(Vehicle* vehicle, RecvContainer recvFrame, UserData userData)
{
    DSTATUS("--------parse mobile singal--------");
    uint16_t mobile_data_id;
    mobile_data_id = *(reinterpret_cast<uint16_t*>(&recvFrame.recvData.raw_ack_array));

    std::cout << "RecvContainer Info: " << std::endl;

    std::cout << "  DJI::OSDK::ACK::Entry     recvInfo: " << std::endl;
    std::cout << "     cmd_set: "
              << recvFrame.recvInfo.cmd_set << std::endl;
    std::cout << "     cmd_id: "
              << recvFrame.recvInfo.cmd_id << std::endl;
    std::cout << "     len: "
              << recvFrame.recvInfo.len << std::endl;
    std::cout << "     buf: "
              << recvFrame.recvInfo.buf << std::endl;
    std::cout << "     seqNumber: "
              << recvFrame.recvInfo.seqNumber << std::endl;
    std::cout << "     version: "
              << recvFrame.recvInfo.version << std::endl;

    std::cout << "  DJI::OSDK::ACK::TypeUnion recvData: " << std::endl;
    std::cout << "     raw_ack_array[MAX_INCOMING_DATA_SIZE]: "
              << recvFrame.recvData.raw_ack_array[0] << std::endl;
    std::cout << "     versionACK[MAX_ACK_SIZE]: "
              << recvFrame.recvData.versionACK << std::endl;
    std::cout << "     ack: "
              << recvFrame.recvData.ack << std::endl;
    std::cout << "     commandACK: "
              << recvFrame.recvData.commandACK << std::endl;
    std::cout << "     missionACK: "
              << recvFrame.recvData.missionACK << std::endl;
    std::cout << "     subscribeACK: "
              << recvFrame.recvData.subscribeACK << std::endl;
    std::cout << "     mfioACK: "
              << recvFrame.recvData.mfioACK << std::endl;


    std::cout << "  DJI::OSDK::DispatchInfo   dispatchInfo:" << std::endl;
    std::cout << "     isAck: "
              << recvFrame.dispatchInfo.isAck << std::endl;
    std::cout << "     isCallback: "
              << recvFrame.dispatchInfo.isCallback << std::endl;
    std::cout << "     callbackID: "
              << recvFrame.dispatchInfo.callbackID << std::endl;

    switch(mobile_data_id)
    {
    case ThreadManager::SET_HOME_POINT:
        DDEBUG("case 0: setHomePoint.");
        //threadManager.setHomePoint();
        break;
    case ThreadManager::OBTAIN_CTR_AUTHORITY:
        DDEBUG("case 1: obtainCtrAuthority.");
        vehicle->obtainCtrlAuthority(controlAuthorityMobileCallback);
        break;
    case ThreadManager::RELEASE_CTR_AUTHORITY:
        DDEBUG("case 2: releaseCtrAuthority.");
        vehicle->releaseCtrlAuthority(controlAuthorityMobileCallback);
        break;
    case ThreadManager::TAKE_OFF:
        DDEBUG("case 3: takeOff");
        vehicle->control->takeoff(actionMobileCallback);
        break;
    case ThreadManager::LAND:
        DDEBUG("case 4: land");
        vehicle->control->land(actionMobileCallback);
        break;
    case ThreadManager::TRACKING:
        DDEBUG("case 5: missionStart");
        threadManager.start(ThreadManager::TRACKING);
        sendAckToMobile(vehicle, mobile_data_id);
        break;
    case ThreadManager::PAUSE:
        DDEBUG("case 6: pause.");
        threadManager._pause();
        sendAckToMobile(vehicle, mobile_data_id);
        break;
    case ThreadManager::CONTINUE:
        DDEBUG("case 7: continue.");
        threadManager._continue();
        sendAckToMobile(vehicle, mobile_data_id);
        break;
    case ThreadManager::FINISH:
        DDEBUG("case 8: terminate.");
        threadManager._finish();
        sendAckToMobile(vehicle, mobile_data_id);
        break;
    case ThreadManager::GO_HOME:
        DDEBUG("case 9: goHome.");
        //threadManager.start(GO_HOME);
        vehicle->control->goHome(actionMobileCallback);
        break;
    case 10:
        sendTraceName(vehicle);
        break;
    default:
        break;
    }
    DSTATUS("----------------over----------------");
}

void
sendTraceName(Vehicle* vehiclePtr)
{
    // We will get the trace names from local DB
  std::string traces = threadManager.getTraces();
  if (traces.length() > 100)
  {
      DERROR("Data is too large for Onboard transfor to Mobile.");
  }
  char     tempRawString[100] = { 0 };
  memcpy(&tempRawString, traces.c_str(), traces.length());
  // Now pack this uo into a mobile packet
  TraceNamePacket tracesPack(&tempRawString[0]);

  // And send it
  vehiclePtr->moc->sendDataToMSDK(reinterpret_cast<uint8_t*>(&tracesPack),
                                  sizeof(tracesPack));
}
