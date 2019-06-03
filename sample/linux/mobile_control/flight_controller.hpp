#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

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

class FlightController
{
public:
	Controller(DJI::OSDK::Vehicle *v);
	~Controller();

// 获取数据
public:
    bool startGlobalPositionBroadcast();

    std::string get_trace();
    void getData(std::string trace_nick_name, vector<HUST::FlightData>& flight_data);
    void recordFlightData();
    void clear();
    void save(std::string nick_name, std::string describe);
    void save_delta(std::string nick_name);

    void printData(HUST::FlightData& fd);

// 设置参数 + 计算误差
public:
    void set_X(double n);
    void set_homePoint();
	void goHome();
    void goLast(HUST::FlightData& fd);

    void set_PID(double kp, double ki, double kd);
    void cal_ut(HUST::Positon& delta, HUST::Positon& delta_sum, HUST::Positon& delta_last, HUST::Positon& ut, HUST::Positon& ut_last, HUST::Positon& ut_delt);
    void cal_delta(HUST::FlightData& fd, HUST::Positon& delta);
    bool isBigger(HUST::Positon& delta, HUST::Positon& max_delta);

// 飞行控制
public:
    void obtainControl();
    void releaseControl();
	bool monitoredTakeoff();
	bool monitoredLanding();

    void flightByPosAndYaw(HUST::Positon& delta, HUST::FlightData& fd);
    void flightByVelAndYawRate(HUST::Positon& delta, HUST::FlightData& fd);
    void flightByAttAndVertPos(HUST::FlightData& fd);

    void flightByPosAndYaw_average(HUST::Positon& delta, HUST::FlightData& fd);
    void flightByVelAndYawRate_average(HUST::FlightData& fd1, HUST::FlightData& fd2);
    void flightByAttAndVertPos_average(HUST::FlightData& fd1, HUST::FlightData& fd2);


private:
	void localOffsetFromGpsOffset(DJI::OSDK::Telemetry::Vector3f& deltaNed, void* target, void* origin);
	DJI::OSDK::Telemetry::Vector3f toEulerAngle(void* quaternionData);

private:
    double                                X;        // 偏差系数
    DJI::OSDK::Telemetry::GlobalPosition  homeGP;   // 原点GPS
    float                                 homeYaw;  // 原点角度
    double                                kp,ki,kd;

	Trace                          trace;
	DJI::OSDK::Vehicle*            vehicle;
};
#endif
