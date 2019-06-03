#ifndef FLIGHT_CONTROLLER_HPP
#define FLIGHT_CONTROLLER_HPP

class FlightControl
{
public:
	FlightControl(Vehicle* vehicle);
	~FlightControl();

public:
	// 基础控制命令
	bool takeOff(int timeout = 1);
	bool land(int timeout = 1);
	bool goHome();

	// 记录飞行轨迹
	void beginTrackRecord();
	void endTrackRecord();
	void storageTrack();

private:
	Vehicle* vehicle;
};

#endif