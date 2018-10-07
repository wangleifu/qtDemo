#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include "dji_status.hpp"
#include <dji_vehicle.hpp>
#include <dji_linux_helpers.hpp>

#define C_EARTH (double)6378137.0
#define DEG2RAD 0.01745329252
#define RAD2DEG 57.2957795131

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;
using namespace std;

void moveByPositionAndYaw(DJI::OSDK::Vehicle* vehicle, float x, float y, float z, float yaw);
void moveByVelocityAndYawRate(DJI::OSDK::Vehicle* vehicle, float Vx,float Vy,float Vz,float yawRate);
void moveByAttitudeAndVertPos(DJI::OSDK::Vehicle* vehicle, float roll, float pitch, float yaw, float z);
void moveByAngularRateAndVertPos(DJI::OSDK::Vehicle* vehicle, float rollRate, float pitchRate, float yawRate, float z);

void go_point(DJI::OSDK::Vehicle* vehicle, float x, float y, float z, float yaw, float kp, float ki, float kd);
void localOffsetFromGpsOffset(DJI::OSDK::Vehicle* vehicle, Telemetry::Vector3f& deltaNed, void* target, void* origin);
template <typename Type> string numToString(const Type& num);
int main(int argc, char** argv)
{
	// Initialize variables
	int functionTimeout = 1;

	// Setup OSDK.
	LinuxSetup linuxEnvironment(argc, argv);
	Vehicle*   vehicle = linuxEnvironment.getVehicle();
	if (vehicle == NULL)
	{
		std::cout << "Vehicle not initialized, exiting.\n";
		return -1;
	}

	// Obtain Control Authority
	vehicle->obtainCtrlAuthority(functionTimeout);

	int                       x, y, z, w;
	char                      inputChar;
	string                    nick_name;
	
	system("clear");
	while(true)
	{
		std::cout
			<< "| Available commands:                                     |"
			<< std::endl;
		std::cout
			<< "| [a] testing: velocity and yaw rate                      |"
			<< std::endl;
		std::cout
			<< "| [b] testing: attitude and vertial position              |"
			<< std::endl;
		std::cout
			<< "| [c] testing: angular rate and vertical position         |"
			<< std::endl;
		std::cout
			<< "| [d] testing: position and yaw rate                      |"
			<< std::endl;
        std::cout
            << "| [m] testing: position PID testing.                      |"
            << std::endl;
		std::cout
			<< "| [e] exit.                                               |"
			<< std::endl;


		std::cin >> inputChar;
        vehicle->obtainCtrlAuthority(1);
		switch (inputChar)
		{
			case 'a':
				std::cout << "Please input control data:" << std::endl;
				std::cout << "Vx: ";
				std::cin >> x;
				std::cout << "Vy: ";
				std::cin >> y;
				std::cout << "Vz: ";
				std::cin >> z;
				std::cout << "yawRate: ";
				std::cin >> w;
				moveByVelocityAndYawRate(vehicle, x, y, z, w);
				break;
			case 'b':
				std::cout << "Please input control data:" << std::endl;
				std::cout << "roll: ";
				std::cin >> x;
				std::cout << "pitch: ";
				std::cin >> y;
				std::cout << "yaw: ";
				std::cin >> z;
				std::cout << "verticla position: ";
				std::cin >> w;
				moveByAttitudeAndVertPos(vehicle, x, y, z, w);
				break;
			case 'c':
				std::cout << "Please input control data:" << std::endl;
				std::cout << "rollRate: ";
				std::cin >> x;
				std::cout << "pitchRate: ";
				std::cin >> y;
				std::cout << "yawRate: ";
				std::cin >> z;
				std::cout << "verticla position: ";
				std::cin >> w;
				moveByAngularRateAndVertPos(vehicle, x, y, z, w);
				break;
			case 'd':
				std::cout << "Please input control data:" << std::endl;
				std::cout << "x position: ";
				std::cin >> x;
				std::cout << "y positon: ";
				std::cin >> y;
				std::cout << "verticla position:";
				std::cin >> z;
				std::cout << "yaw: ";
				std::cin >> w;
				moveByPositionAndYaw(vehicle, x, y, z, w);
				break;
            case 'm':
//                for(float i = 60; i < 100; i+=1)
//                {
//                    go_point(vehicle, 1, 0 ,2, 0, i, 4, 0);
//                    sleep(10);
//                }
//                for(float i = 0.2; i < 20; i+=0.2)
//                {
//                    go_point(vehicle, 0, 1 ,2, 0, i, 2, 0);
//                    sleep(10);
//                }
                for(float i = 50; i < 120; i+=10)
                {
                    go_point(vehicle, 0, 0 ,3, 0, i, 10, 0);
                    sleep(5);
                }
                break;
			case 'e':
				exit(0);
				break;
			default:
				cout << "Wrong command!" << endl;
				break;
		}
        vehicle->releaseCtrlAuthority(1);

		system("read -p \"\nPress any key to continue.\" var");
		system("clear");
	}

	return 0;
}
void go_point(DJI::OSDK::Vehicle* vehicle, float x, float y, float z, float yaw, float kp, float ki, float kd)
{
    std::cout << "kp: " << kp << " ki: " << ki << " kd: " << kd << endl;
    float error, error_last1, error_last2;
    vector<float> xData;
    Telemetry::GlobalPosition  homeGP = vehicle->broadcast->getGlobalPosition();   // 原点GPS
    Telemetry::Vector3f        localOffset;
    Telemetry::GlobalPosition  currentBroadcastGP;

    for (int i = 0; i < 1000; i++)
    {

        currentBroadcastGP  = vehicle->broadcast->getGlobalPosition();
        localOffsetFromGpsOffset(vehicle, localOffset, static_cast<void*>(&currentBroadcastGP),
                                              static_cast<void*>(&homeGP));

        //std::cout << "localOffset: "<< localOffset.x <<" "<< localOffset.y <<" "<< localOffset.z << std::endl;

        if(y == 0.0 && z == 2.0)
        {
            xData.push_back(localOffset.x);
            error = 1 - localOffset.x;
            x = kp * (error - error_last1) + ki * error + kd * (error - 2 * error_last1 + error_last2);
        }
        else if (x == 0.0 && z == 2.0)
        {
            xData.push_back(localOffset.y);
            error = 1 - localOffset.y;
            y = kp * (error - error_last1) + ki * error + kd * (error - 2 * error_last1 + error_last2);
        }
        else if (x == 0.0 && y == 0.0)
        {
            xData.push_back(localOffset.z);
            error = 1 - localOffset.z;
            z = currentBroadcastGP.altitude + kp * (error - error_last1) + ki * error + kd * (error - 2 * error_last1 + error_last2);
        }
        error_last1 = error;
        error_last2 = error_last1;

        //std::cout << "x: " << x << " y: " << y << " z: " << z << std::endl;

        vehicle->control->positionAndYawCtrl(x, y, z, yaw);
        usleep(20 * 1000);
    }

    ofstream file;
    string path;
    if(y == 0.0 && z == 2.0)
    {
        path = "../data/x-" + numToString(kp) +"-" + numToString(ki) + "-" + numToString(kd) + ".txt";
    }
    else if (x == 0.0 && z == 2.0)
    {
        path = "../data/y-" + numToString(kp) +"-" + numToString(ki) + "-" + numToString(kd) + ".txt";
    }
    else if (x == 0.0 && y == 0.0)
    {
        path = "../data/z-" + numToString(kp) +"-" + numToString(ki) + "-" + numToString(kd) + ".txt";
    }
    file.open(path);
    for(int i = 0; i < xData.size(); i++)
    {
        file << xData[i] << std::endl;
    }
    file.close();
    std::cout << "save to path: " << path << std::endl;
}
template <typename Type> string numToString(const Type& num)
{
    ostringstream oss;   //创建一个格式化输出流
    oss<<num;             //把值传递如流中
    return oss.str();
}
void
localOffsetFromGpsOffset(DJI::OSDK::Vehicle* vehicle, Telemetry::Vector3f& deltaNed,
                                     void* target, void* origin)
{
  Telemetry::GPSFused*       subscriptionTarget;
  Telemetry::GPSFused*       subscriptionOrigin;
  Telemetry::GlobalPosition* broadcastTarget;
  Telemetry::GlobalPosition* broadcastOrigin;
  double                     deltaLon;
  double                     deltaLat;

  if (!vehicle->isM100() && !vehicle->isLegacyM600())
  {
    subscriptionTarget = (Telemetry::GPSFused*)target;
    subscriptionOrigin = (Telemetry::GPSFused*)origin;
    deltaLon   = subscriptionTarget->longitude - subscriptionOrigin->longitude;
    deltaLat   = subscriptionTarget->latitude - subscriptionOrigin->latitude;
    deltaNed.x = deltaLat * C_EARTH;
    deltaNed.y = deltaLon * C_EARTH * cos(subscriptionTarget->latitude);
    deltaNed.z = subscriptionTarget->altitude - subscriptionOrigin->altitude;
  }
  else
  {
    broadcastTarget = (Telemetry::GlobalPosition*)target;
    broadcastOrigin = (Telemetry::GlobalPosition*)origin;
    deltaLon        = broadcastTarget->longitude - broadcastOrigin->longitude;
    deltaLat        = broadcastTarget->latitude - broadcastOrigin->latitude;
    deltaNed.x      = deltaLat * C_EARTH;
    deltaNed.y      = deltaLon * C_EARTH * cos(broadcastTarget->latitude);
    deltaNed.z      = broadcastTarget->altitude - broadcastOrigin->altitude;
  }
}
void
moveByVelocityAndYawRate(DJI::OSDK::Vehicle* vehicle, float Vx,float Vy,float Vz,float yawRate)
{
    int elapsedTimeInMs = 0;
    int cycleTimeInMs   = 10;       // 间隔10ms
    int totalTimeInMs   = 2 * 1000; // 运行2s
    while(elapsedTimeInMs < totalTimeInMs)
    {

        vehicle->control->velocityAndYawRateCtrl(Vx, Vy, Vz, yawRate);

        usleep(cycleTimeInMs * 1000);
        elapsedTimeInMs += cycleTimeInMs;
    }
}

void
moveByAttitudeAndVertPos(DJI::OSDK::Vehicle* vehicle, float roll, float pitch, float yaw, float z)
{
    int elapsedTimeInMs = 0;
    int cycleTimeInMs   = 10;       // 间隔10ms
    int totalTimeInMs   = 2 * 1000; // 运行2s
    while(elapsedTimeInMs < totalTimeInMs)
    {
        vehicle->control->attitudeAndVertPosCtrl(roll, pitch, yaw, z);

        usleep(cycleTimeInMs * 1000);
        elapsedTimeInMs += cycleTimeInMs;
    }
}

void
moveByAngularRateAndVertPos(DJI::OSDK::Vehicle* vehicle, float rollRate, float pitchRate, float yawRate, float z)
{
    int elapsedTimeInMs = 0;
    int cycleTimeInMs   = 10;       // 间隔10ms
    int totalTimeInMs   = 2 * 1000; // 运行2s
    while(elapsedTimeInMs < totalTimeInMs)
    {
        vehicle->control->angularRateAndVertPosCtrl(rollRate, pitchRate, yawRate, z);

        usleep(cycleTimeInMs * 1000);
        elapsedTimeInMs += cycleTimeInMs;
    }
}

void
moveByPositionAndYaw(DJI::OSDK::Vehicle* vehicle, float x, float y, float z, float yaw)
{
    int elapsedTimeInMs = 0;
    int cycleTimeInMs   = 10;       // 间隔10ms
    int totalTimeInMs   = 2 * 1000; // 运行2s
    while(elapsedTimeInMs < totalTimeInMs)
    {
        vehicle->control->positionAndYawCtrl(x, y, z, yaw);

        usleep(cycleTimeInMs * 1000);
        elapsedTimeInMs += cycleTimeInMs;
    }
}
