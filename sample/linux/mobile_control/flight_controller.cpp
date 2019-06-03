#include "flight_controller.hpp"

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;

Controller::Controller(Vehicle *v)
{
    std::cout << "begin...\n";
    X                     = 0;
	vehicle               = v;
    Controller::startGlobalPositionBroadcast();
    Controller::set_homePoint();
    this->kp = this->ki = this->kd = 0.0;
}
Controller::~Controller()
{
}
void Controller::obtainControl()
{
    this->vehicle->obtainCtrlAuthority(1);
}
void Controller::releaseControl()
{
    this->vehicle->releaseCtrlAuthority(1);
}

// 设置参数： 窗口： widget  误差系数： X
//          频率： freq       原点： homeGP
void
Controller::set_X(double n)
{
    X = n;
}
void
Controller::set_homePoint()
{
    std::cout << "set home point..." << std::endl;
    Telemetry::Quaternion quaternion            = vehicle->broadcast->getQuaternion();
    Telemetry::Vector3f   ans                   = toEulerAngle((static_cast<void*>(&quaternion)));

    this->homeYaw   = ans.z * RAD2DEG;
    this->homeGP    = vehicle->broadcast->getGlobalPosition();
}
void
Controller::goHome()
{
    int                        atHome_count = 0;
    int                        speedFactory = 2;

    float                      x, y, z, yaw;
    Telemetry::Vector3f        localOffset;
    Telemetry::GlobalPosition  currentBroadcastGP;

    std::cout << "go to home point..." << std::endl;
    while(atHome_count < 30)
    {

        currentBroadcastGP  = vehicle->broadcast->getGlobalPosition();
        localOffsetFromGpsOffset(localOffset, static_cast<void*>(&homeGP),
                                              static_cast<void*>(&currentBroadcastGP));
        if(std::abs(localOffset.x) < 0.08 && std::abs(localOffset.y) < 0.08 && std::abs(localOffset.z) < 0.16)
        {
            atHome_count += 1;
        } else {
            atHome_count = 0;
        }


        if(localOffset.x > 0)
        {
            x = (localOffset.x < speedFactory) ? localOffset.x : speedFactory;
        } else if (localOffset.x < 0) {
            x = (localOffset.x > -1 * speedFactory) ? localOffset.x : -1 * speedFactory;
        }
        if(localOffset.y > 0)
        {
            y = (localOffset.y < speedFactory) ? localOffset.y : speedFactory;
        } else if (localOffset.y < 0) {
            y = (localOffset.y > -1 * speedFactory) ? localOffset.y : -1 * speedFactory;
        }
        if(std::abs(x) == speedFactory)
        {
            y = std::abs(localOffset.y / localOffset.x) * y;
        } else if (std::abs(y) == speedFactory) {
            x = std::abs(localOffset.x / localOffset.y) * x;
        }

        z   = homeGP.altitude;
        yaw = homeYaw;
        vehicle->control->positionAndYawCtrl(x, y, z, yaw);
        usleep(20 * 1000);
    }
}
void
Controller::goLast(HUST::FlightData& fd)
{
    int                        atHome_count = 0;
    float                      speedFactory = 0.5;

    float                      x, y, z, yaw;
    Telemetry::Vector3f        localOffset;
    Telemetry::Vector3f        delta;
    Telemetry::GlobalPosition  currentBroadcastGP;

    std::cout << "go to last point..." << std::endl;
    while(atHome_count < 30)
    {
        currentBroadcastGP  = vehicle->broadcast->getGlobalPosition();
        localOffsetFromGpsOffset(localOffset, static_cast<void*>(&currentBroadcastGP),
                                              static_cast<void*>(&homeGP));
        delta.x = fd.pos.x - localOffset.x;
        delta.y = fd.pos.y - localOffset.y;
        delta.z = fd.pos.z - currentBroadcastGP.altitude;
        if(std::abs(delta.x) < 0.16 && std::abs(delta.y) < 0.16 && std::abs(delta.z) < 0.16)
        {
            atHome_count += 1;
        } else {
            atHome_count = 0;
        }

        if(delta.x > 0)
        {
            x = (delta.x < speedFactory) ? delta.x : speedFactory;
        } else if (delta.x < 0) {
            x = (delta.x > -1 * speedFactory) ? delta.x : -1 * speedFactory;
        }
        if(delta.y > 0)
        {
            y = (delta.y < speedFactory) ? delta.y : speedFactory;
        } else if (delta.y < 0) {
            y = (delta.y > -1 * speedFactory) ? delta.y : -1 * speedFactory;
        }
        if(std::abs(x) == speedFactory)
        {
            y = std::abs(delta.y / delta.x) * y;
        } else if (std::abs(y) == speedFactory) {
            x = std::abs(delta.x / delta.y) * x;
        }

        z   = fd.pos.z;
        yaw = fd.att.yaw;
        vehicle->control->positionAndYawCtrl(x, y, z, yaw);
        usleep(20 * 1000);
    }
}



//拿到所有路径名
std::string
Controller::get_trace()
{
    return trace.get_trace();
}
void
Controller::getData(std::string trace_nick_name, vector<HUST::FlightData>& flight_data)
{
    trace.get(trace_nick_name, flight_data);
    std::cout << "Successfullt loda flight data.\n";
    std::cout << "data point numbers: " << flight_data.size() << std::endl;
}
void Controller::clear()
{
    trace.clear();
}
void
Controller::save(string nick_name, string describe)
{
    trace.save(nick_name, describe);
}
// 计算 当前点 与 记录中对应点 偏差
void
Controller::cal_delta(HUST::FlightData& fd, HUST::Positon& delta)
{
    Telemetry::GlobalPosition currentBroadcastGP    = vehicle->broadcast->getGlobalPosition();
    Telemetry::Vector3f       velocity              = vehicle->broadcast->getVelocity();
    Telemetry::Quaternion     quaternion            = vehicle->broadcast->getQuaternion();
    Telemetry::Vector3f       angularRate           = vehicle->broadcast->getAngularRate();

    Telemetry::Vector3f       ans                   = toEulerAngle((static_cast<void*>(&quaternion)));
    Telemetry::Vector3f       localOffset;
    localOffsetFromGpsOffset(localOffset, static_cast<void*>(&currentBroadcastGP),
                                          static_cast<void*>(&homeGP));

    HUST::FlightData     fd1;
    fd1.pos.x             = localOffset.x;
    fd1.pos.y             = localOffset.y;
    fd1.pos.z             = currentBroadcastGP.altitude;
    fd1.vel.Vx            = velocity.x;
    fd1.vel.Vy            = velocity.y;
    fd1.vel.Vz            = velocity.z;
    fd1.att.roll          = ans.y * RAD2DEG;
    fd1.att.pitch         = ans.x * RAD2DEG;
    fd1.att.yaw           = ans.z * RAD2DEG;
    fd1.ang.rollRate      = angularRate.x;
    fd1.ang.pitchRate     = angularRate.y;
    fd1.ang.yawRate       = angularRate.z;
//    Telemetry::Vector3f        localOffset;      // offset from global position
//    Telemetry::GlobalPosition  currentBroadcastGP;

//    currentBroadcastGP  = vehicle->broadcast->getGlobalPosition();
//    localOffsetFromGpsOffset(localOffset, static_cast<void*>(&currentBroadcastGP),
//                                          static_cast<void*>(&homeGP));

    trace.push(fd1);

    delta.x   = (fd.pos.x - localOffset.x);
    delta.y   = (fd.pos.y - localOffset.y);
    delta.z   = fd.pos.z - currentBroadcastGP.altitude;
}
void Controller::save_delta(std::string nick_name)
{
    trace.save_delta(nick_name);

}
void Controller::set_PID(double kp, double ki, double kd)
{
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
    std::cout << "---set PID---\nkp: " << this->kp << " ki: " << this->ki << " kd: " << this->kd << std::endl;
}
void
Controller::cal_ut(HUST::Positon& delta, HUST::Positon& delta_sum, HUST::Positon& delta_last, HUST::Positon& ut, HUST::Positon& ut_last, HUST::Positon& ut_delta)
{
    ut.x = kp * delta.x + ki * delta_sum.x + kd * (delta.x - delta_last.x);
    ut.y = kp * delta.y + ki * delta_sum.y + kd * (delta.y - delta_last.y);
    ut.z = kp * delta.z + ki * delta_sum.z + kd * (delta.z - delta_last.z);

    ut_delta.x = ut.x - ut_last.x;
    ut_delta.y = ut.y - ut_last.y;
    ut_delta.z = ut.z - ut_last.z;

    ut_last.x = ut.x;
    ut_last.y = ut.y;
    ut_last.z = ut.z;

    delta_last.x = delta.x;
    delta_last.y = delta.y;
    delta_last.z = delta.z;

    delta_sum.x += delta.x;
    delta_sum.y += delta.y;
    delta_sum.z += delta.z;
}
bool
Controller::isBigger(HUST::Positon& delta, HUST::Positon& max_delta)
{
    float f1 = std::pow(delta.x, 2) + std::pow(delta.y, 2) + std::pow(delta.z, 2);
    float f2 = std::pow(max_delta.x, 2) + std::pow(max_delta.y, 2) + std::pow(max_delta.z, 2);
    return f1 > f2;
}




// 飞控

//位置控制 + 返回结束点
void
Controller::flightByPosAndYaw(HUST::Positon& delta, HUST::FlightData& fd)
{
    float  x, y, z, yaw;
    x      = delta.x;
    y      = delta.y;
    z      = fd.pos.z;
    yaw    = fd.att.yaw;
    vehicle->control->positionAndYawCtrl(x, y, z, yaw);
}
//姿态控制 + 返回结束点
void
Controller::flightByAttAndVertPos(HUST::FlightData& fd)
{
    float  roll, pitch, yaw, z;
    roll   = fd.att.roll;
    pitch  = fd.att.pitch;
    yaw    = fd.att.yaw;
    z      = fd.pos.z;
    vehicle->control->attitudeAndVertPosCtrl(roll, pitch, yaw, z);
}
//速度控制 + 返回结束点
void
Controller::flightByVelAndYawRate(HUST::Positon& delta, HUST::FlightData& fd)
{
    //std::cout << "ut_delta.x: " << delta.x << " ut_delta.y: " << delta.y << " ut_delta.z: " << delta.z << std::endl;
    float   Vx, Vy, Vz, yawRate;
    Vx      = fd.vel.Vx + delta.x*X;
    Vy      = fd.vel.Vy + delta.y*X;
    Vz      = fd.vel.Vz + delta.z*X;
    yawRate = fd.ang.yawRate;
    vehicle->control->velocityAndYawRateCtrl(Vx, Vy, Vz, yawRate);
}

//位置平均值
void
Controller::flightByPosAndYaw_average(HUST::Positon& delta, HUST::FlightData& fd)
{
    float  x, y, z, yaw;
    x      = delta.x;
    y      = delta.z;
    z      = fd.pos.z;
    yaw    = fd.att.yaw;
    vehicle->control->positionAndYawCtrl(x, y, z, yaw);
}
//姿态平均值
void
Controller::flightByAttAndVertPos_average(HUST::FlightData& fd1, HUST::FlightData& fd2)
{
    float  roll, pitch, yaw, z;
    roll   = (fd1.att.roll + fd2.att.roll) / 2.0;
    pitch  = (fd1.att.pitch + fd2.att.pitch) / 2.0;
    yaw    = (fd1.att.yaw + fd2.att.yaw) / 2.0;
    z      = (fd1.pos.z + fd2.pos.z) / 2.0;
    vehicle->control->attitudeAndVertPosCtrl(roll, pitch, yaw, z);
}

//速度平均值
void
Controller::flightByVelAndYawRate_average(HUST::FlightData& fd1, HUST::FlightData& fd2)
{
    float   Vx, Vy, Vz, yawRate;
    Vx      = (fd1.vel.Vx + fd2.vel.Vx) / 2.0;
    Vy      = (fd1.vel.Vy + fd2.vel.Vy) / 2.0;
    Vz      = (fd1.vel.Vz + fd2.vel.Vz) / 2.0;
    yawRate = (fd1.ang.yawRate + fd2.ang.yawRate) / 2.0;
    vehicle->control->velocityAndYawRateCtrl(Vx, Vy, Vz, yawRate);
}



// 数据获取， 判断是否存储
void
Controller::recordFlightData()
{
    Telemetry::GlobalPosition currentBroadcastGP    = vehicle->broadcast->getGlobalPosition();
    Telemetry::Vector3f       velocity              = vehicle->broadcast->getVelocity();
    Telemetry::Quaternion     quaternion            = vehicle->broadcast->getQuaternion();
    Telemetry::Vector3f       angularRate           = vehicle->broadcast->getAngularRate();

    Telemetry::Vector3f       ans                   = toEulerAngle((static_cast<void*>(&quaternion)));
    Telemetry::Vector3f       localOffset;
    localOffsetFromGpsOffset(localOffset, static_cast<void*>(&currentBroadcastGP),
                                          static_cast<void*>(&homeGP));

    HUST::FlightData     fd;
    fd.pos.x             = localOffset.x;
    fd.pos.y             = localOffset.y;
    fd.pos.z             = currentBroadcastGP.altitude;
    fd.vel.Vx            = velocity.x;
    fd.vel.Vy            = velocity.y;
    fd.vel.Vz            = velocity.z;
    fd.att.roll          = ans.y * RAD2DEG;
    fd.att.pitch         = ans.x * RAD2DEG;
    fd.att.yaw           = ans.z * RAD2DEG;
    fd.ang.rollRate      = angularRate.x;
    fd.ang.pitchRate     = angularRate.y;
    fd.ang.yawRate       = angularRate.z;

    trace.push(fd);
}
void Controller::printData(HUST::FlightData& fd)
{
    Telemetry::GlobalPosition currentBroadcastGP;
    Telemetry::Vector3f       velocity;
    Telemetry::Quaternion     quaternion;
    Telemetry::Vector3f       angularRate;

    Telemetry::Vector3f       ans;
    Telemetry::Vector3f       localOffset;

    currentBroadcastGP   = vehicle->broadcast->getGlobalPosition();
    velocity             = vehicle->broadcast->getVelocity();
    quaternion           = vehicle->broadcast->getQuaternion();
    angularRate          = vehicle->broadcast->getAngularRate();

    ans = toEulerAngle((static_cast<void*>(&quaternion)));
    localOffsetFromGpsOffset(localOffset, static_cast<void*>(&currentBroadcastGP),
                                          static_cast<void*>(&homeGP));
    fd.pos.x             = localOffset.x;
    fd.pos.y             = localOffset.y;
    fd.pos.z             = currentBroadcastGP.altitude;
    fd.vel.Vx            = velocity.x;
    fd.vel.Vy            = velocity.y;
    fd.vel.Vz            = velocity.z;
    fd.att.roll          = ans.y * RAD2DEG;
    fd.att.pitch         = ans.x * RAD2DEG;
    fd.att.yaw           = ans.z * RAD2DEG;
    fd.ang.rollRate      = angularRate.x;
    fd.ang.pitchRate     = angularRate.y;
    fd.ang.yawRate       = angularRate.z;

}


// 辅助计算函数
void
Controller::localOffsetFromGpsOffset(Telemetry::Vector3f& deltaNed,
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

Telemetry::Vector3f
Controller::toEulerAngle(void* quaternionData)
{
	Telemetry::Vector3f    ans;
	Telemetry::Quaternion* quaternion = (Telemetry::Quaternion*)quaternionData;

	double q2sqr = quaternion->q2 * quaternion->q2;
	double t0    = -2.0 * (q2sqr + quaternion->q3 * quaternion->q3) + 1.0;
	double t1 =
		+2.0 * (quaternion->q1 * quaternion->q2 + quaternion->q0 * quaternion->q3);
	double t2 =
		-2.0 * (quaternion->q1 * quaternion->q3 - quaternion->q0 * quaternion->q2);
	double t3 = 
		+2.0 * (quaternion->q2 * quaternion->q3 + quaternion->q0 * quaternion->q1);
	double t4 = -2.0 * (quaternion->q1 * quaternion->q1 + q2sqr) + 1.0;

	t2 = (t2 > 1.0) ? 1.0 : t2;
	t2 = (t2 < -1.0) ? -1.0 : t2;

	ans.x = asin(t2);       // pitch: rad
	ans.y = atan2(t3, t4);  // roll:  rad
	ans.z = atan2(t1, t0);  // yaw:   rad

  return ans;
}

bool
Controller::startGlobalPositionBroadcast()
{
  uint8_t freq[16];

  /* Channels definition for M100
   * 0 - Timestamp
   * 1 - Attitude Quaterniouns
   * 2 - Acceleration
   * 3 - Velocity (Ground Frame)
   * 4 - Angular Velocity (Body Frame)
   * 5 - Position
   * 6 - Magnetometer
   * 7 - RC Channels Data
   * 8 - Gimbal Data
   * 9 - Flight Status
   * 10 - Battery Level
   * 11 - Control Information
   */
  freq[0]  = DataBroadcast::FREQ_HOLD;
  freq[1]  = DataBroadcast::FREQ_100HZ;
  freq[2]  = DataBroadcast::FREQ_HOLD;
  freq[3]  = DataBroadcast::FREQ_HOLD;
  freq[4]  = DataBroadcast::FREQ_100HZ;
  freq[5]  = DataBroadcast::FREQ_100HZ; // This is the only one we want to change
  freq[6]  = DataBroadcast::FREQ_HOLD;
  freq[7]  = DataBroadcast::FREQ_HOLD;
  freq[8]  = DataBroadcast::FREQ_HOLD;
  freq[9]  = DataBroadcast::FREQ_HOLD;
  freq[10] = DataBroadcast::FREQ_HOLD;
  freq[11] = DataBroadcast::FREQ_HOLD;

  ACK::ErrorCode ack = vehicle->broadcast->setBroadcastFreq(freq, 1);
  if (ACK::getError(ack))
  {
    ACK::getErrorCodeMessage(ack, __func__);
    return false;
  }
  else
  {
    return true;
  }
}




bool Controller::monitoredTakeoff()
{
    return false;
}

bool Controller::monitoredLanding()
{
    return false;
}
