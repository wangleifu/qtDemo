#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller : public QThread
{
public:
	Controller(Vehicle* vehicle, Widget* widget;);
	~Controller();

signals:
	void send_info(HUST::FlightData fd);

private:
	initPrintMission()


public:
	void takeOff();
	void land();
	void goHome();

	// 设置飞行原点
	// bool setHomePoint();
	// bool goHomePoint();

	// 记录飞行轨迹
	void beginTrackRecord();
	void endTrackRecord();
	void storageTrack();

	// 任务管理
	void startMission();
	void pauseMission();
	void continueMission();
	void terminateMission();

private:
	bool print;

private:
	Widget*              widget;
	FlightController*    f_controller;
	MissionController*   m_controller;
	// MobileController* m_controller;
}

#endif