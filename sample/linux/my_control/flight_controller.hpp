#ifndef CONTROLLER_H
#define CONTROLLER_H

class Widget;
#include "hust.hpp"
#include "trace.hpp"
#include "widget.h"

#include <cmath>
#include <sys/time.h>
// DJI OSDK includes
#include "dji_status.hpp"
#include <dji_vehicle.hpp>
#include <dji_linux_helpers.hpp>


#define C_EARTH (double)6378137.0
#define DEG2RAD 0.01745329252
#define RAD2DEG 57.2957795131

class Controller
{
public:
	Controller(DJI::OSDK::Vehicle *v);
	~Controller();

// 获取数据
public:
    std::string get_trace();
    bool startGlobalPositionBroadcast();
    bool getFlightData();

    void clear();
    void save(std::string nick_name, std::string describe);

    void printData();

// 设置参数 + 计算误差
public:
    void set_X(double n);
    void set_freq(int freq);
    void set_widget(Widget *w);

    void set_homePoint();
	void goHome();
    void goLast(HUST::FlightData fd);

    void cal_delta(HUST::FlightData fd, HUST::Positon& delta);
    bool isBigger(HUST::Positon delta, HUST::Positon max_delta);

// 飞行控制
public:
	bool monitoredTakeoff();
	bool monitoredLanding();

	bool flightByPosAndYaw(std::string trace_nick_name);
	bool flightByVelAndYawRate(std::string trace_nick_name);
	bool flightByAttAndVertPos(std::string trace_nick_name);

    bool flightByPosAndYaw_average(std::string trace_nick_name);
    bool flightByVelAndYawRate_average(std::string trace_nick_name);
    bool flightByAttAndVertPos_average(std::string trace_nick_name);


private:
	void moveByPositionAndYaw(float x, float y, float z, float yaw);
	void moveByVelocityAndYawRate(float Vx,float Vy,float Vz,float yawRate);
    void moveByAttitudeAndVertPos(float roll, float pitch, float yaw, float z);

	void localOffsetFromGpsOffset(DJI::OSDK::Telemetry::Vector3f& deltaNed, void* target, void* origin);
	DJI::OSDK::Telemetry::Vector3f toEulerAngle(void* quaternionData);

public:
    bool flag;
    void susppend();
    void resume();
    void stop();
private:
    double                                X;        // 偏差系数
    int                                   freq;     // 控制频率
    DJI::OSDK::Telemetry::GlobalPosition  homeGP;   // 原点GPS
    float                                 homeYaw;  // 原点角度

    Widget*                        widget;
	Trace                          trace;
	DJI::OSDK::Vehicle*            vehicle;

    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
};
#endif
