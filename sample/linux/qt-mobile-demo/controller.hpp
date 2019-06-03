#ifndef CONTROLLER_H
#define CONTROLLER_H

//class CThread;
//class FlightController;

#include <QDebug>
#include <QMessageBox>
#include "dji_vehicle.hpp"
#include "dialog.h"
#include "widget.h"
#include "cthread.h"
#include "flight_controller.hpp"
class Widget;
class FlightController;

class Controller
{
public:
    Controller(DJI::OSDK::Vehicle* vehicle);
    Controller(DJI::OSDK::Vehicle* vehicle, Widget* widget);
	~Controller();

signals:
    void send_info(DATA::FlightData fd);
    // 多线程
public:
    void onLocalThreadDestroy(QObject* obj);

private:
    void initPrintMission();


public:
    void takeOff();
    void land();
    void goHome();

	// 设置飞行原点
    void setHomePoint();
    void goHomePoint();

	// 记录飞行轨迹
	void beginTrackRecord();
	void endTrackRecord();
    void storageTrackToDB(std::string trace_name, std::string describe); //  存储轨迹到数据库
    void storageTraceToText(std::string trace_name);       // 存储实际飞行轨迹到txt，用于与预订轨迹做对比
    void clear();

    // 获取飞行数据
    std::string get_trace();

	// 任务管理
    void missionStart(int missionType, std::string trace_name, int freq, double kp, double ki, double kd);
    void missionPause();
    void missionContinue();
    void missionTerminate();

private:
	bool print;

private:
	Widget*              widget;
	FlightController*    f_controller;
    DJI::OSDK::Vehicle*  vehicle;
    //PrintThread*         printThread;
    //TrackThread*         trackThread;
    //MissionController*   m_controller;
	// MobileController* m_controller;
private:
    CThread* p_thread;
    CThread* m_currentRunLoaclThread;
};

#endif
