#include "mission_manager.hpp"

using namespace DJI;
using namespace DJI::OSDK;

MissionManagerMissionManager(DJI::OSDK::Vehicle* vehicle)
{
	this->vehicle        = vehicle;
	this->printThread    = Mission::print();
	this->missionThread  = NULL;

}
MissionManager~MissionManager()
{
	this-vehicle         = NULL;
	this->printThread    = NULL;
	this->missionThread  = NULL;
}

void MissionManager::print()
{
}

void MissionManager::track()
{
}


pthread_t MissionManager::createMission(int type)
{
	int         ret = -1;
	std::string infoStr;

	pthread_t      threadID;
	pthread_attr_t attr;

	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (type == 0) 
	{
		ret = pthread_create(&threadID, NULL, print, (void*)vehicle);
	}
	else if (type = 1)
	{
		ret = pthread_create(&threadID, NULL, track, (void*)vehicle);
	}
	infoStr = "mobilePoll";

	if (0 != ret)
	{
		DERROR("fail to create thread for %s!\n", infoStr.c_str());
	}

	ret = pthread_setname_np(threadID, infoStr.c_str());
	if (0 != ret)
	{
		DERROR("fail to set thread name for %s!\n", infoStr.c_str());
	}
	return threadID;
}