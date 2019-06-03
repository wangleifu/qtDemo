#ifndef MISSION_MANAGER_HPP
#define MISSION_MANAGER_HPP

class MissionManager
{
public:
	MissionManager(DJI::OSDK::Vehicle* vehicle);
	~MissionManager();

	// 单例模式创建任务线程

private:
	void print();
	void track();
	pthread_t createMission(int type);

public:
	void startMission();
	void pauseMission();
	void continueMission();
	void terminateMission();

private:
	DJI::OSDK::Vehicle* vehicle;
	pthread_t printThread, missionThread;
	MissionThread* mission;

private:
	pthread_mutex_t m_missionLock;
	pthread_cond_t  m_missionCond;
}

#endif