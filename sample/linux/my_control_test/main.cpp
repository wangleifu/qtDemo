#include <dji_vehicle.hpp>
#include <dji_linux_helpers.hpp>

using namespace DJI::OSDK;
using namespace std;

void moveByPositionAndYaw(DJI::OSDK::Vehicle* vehicle, float x, float y, float z, float yaw);
void moveByVelocityAndYawRate(DJI::OSDK::Vehicle* vehicle, float Vx,float Vy,float Vz,float yawRate);
void moveByAttitudeAndVertPos(DJI::OSDK::Vehicle* vehicle, float roll, float pitch, float yaw, float z);
void moveByAngularRateAndVertPos(DJI::OSDK::Vehicle* vehicle, float rollRate, float pitchRate, float yawRate, float z);

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
			<< "| [e] exit.                                               |"
			<< std::endl;


		std::cin >> inputChar;
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
			case 'e':
				exit(0);
				break;
			default:
				cout << "Wrong command!" << endl;
				break;
		}

		system("read -p \"\nPress any key to continue.\" var");
		system("clear");
	}

	return 0;
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