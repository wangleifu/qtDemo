#ifndef MISSION_CONTROLLER_HPP
#define MISSION_CONTROLLER_HPP

class MissionControl
{
public:
	MissionControl(Vehicle* vehicle);
	~MissionControl();

private:
	createMission();

public:
	void startMission();
	void pauseMission();
	void continueMission();
	void terminateMission();

private:
	Vehicle* vehicle
	
	MissionThread* p_thread;
	MissionThread* m_currentRunLoaclThread;
};

#endif