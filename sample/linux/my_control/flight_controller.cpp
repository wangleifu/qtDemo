#include "flight_controller.hpp"

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;

Controller::Controller(Vehicle *v)
{
    X                     = 0;
	vehicle               = v;
//	Controller::startGlobalPositionBroadcast();
}
Controller::~Controller()
{
}

// 设置参数： 窗口： widget  误差系数： X
//          频率： freq       原点： homeGP
void
Controller::set_widget(Widget *w)
{
    widget = w;
}
void
Controller::set_X(double n)
{
    X = n;
}
void
Controller::set_freq(int freq)
{
    if(freq > 100 || freq < 0)
    {
        freq = 50;
    }
    this->freq = freq;
}
void
Controller::set_homePoint()
{
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

    while(atHome_count < 30)
    {

        currentBroadcastGP  = vehicle->broadcast->getGlobalPosition();
        localOffsetFromGpsOffset(localOffset, static_cast<void*>(&homeGP),
                                              static_cast<void*>(&currentBroadcastGP));
        if(std::abs(localOffset.x) < 0.08 && std::abs(localOffset.y) < 0.08)
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
Controller::goLast(HUST::FlightData fd)
{
    int                        atHome_count = 0;
    float                      speedFactory = 0.5;

    float                      x, y, z, yaw;
    Telemetry::Vector3f        localOffset;
    Telemetry::Vector3f        delta;
    Telemetry::GlobalPosition  currentBroadcastGP;

    while(atHome_count < 30)
    {
        currentBroadcastGP  = vehicle->broadcast->getGlobalPosition();
        localOffsetFromGpsOffset(localOffset, static_cast<void*>(&currentBroadcastGP),
                                              static_cast<void*>(&homeGP));
        delta.x = fd.pos.x - localOffset.x;
        delta.y = fd.pos.y - localOffset.y;
        delta.z = fd.pos.z - currentBroadcastGP.altitude;
        if(std::abs(delta.x) < 0.08 && std::abs(delta.y) < 0.08 && std::abs(delta.z) < 0.08)
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
// 计算 当前点 与 记录中对应点 偏差
void
Controller::cal_delta(HUST::FlightData fd, HUST::Positon& delta)
{
    Telemetry::Vector3f        localOffset;      // offset from global position
    Telemetry::GlobalPosition  currentBroadcastGP;

    currentBroadcastGP  = vehicle->broadcast->getGlobalPosition();
    localOffsetFromGpsOffset(localOffset, static_cast<void*>(&currentBroadcastGP),
                                          static_cast<void*>(&homeGP));

    delta.x   = (fd.pos.x - localOffset.x);
    delta.y   = (fd.pos.y - localOffset.y);
    delta.z   = -localOffset.z;
}
bool
Controller::isBigger(HUST::Positon delta, HUST::Positon max_delta)
{
    float f1 = std::pow(delta.x, 2) + std::pow(delta.y, 2) + std::pow(delta.z, 2);
    float f2 = std::pow(max_delta.x, 2) + std::pow(max_delta.y, 2) + std::pow(max_delta.z, 2);
    return f1 > f2;
}




// 飞控

//位置控制 + 返回结束点
bool
Controller::flightByPosAndYaw(std::string trace_nick_name)
{
	float                      x, y, z, yaw;
	vector<HUST::FlightData>   flight_data;

	trace.get(trace_nick_name, flight_data);
	std::cout << "Successfullt loda flight data.\n";
	std::cout << "data point numbers: " << flight_data.size() << std::endl;

	HUST::FlightData     fd;
    HUST::Positon        delta, max_delta;
    max_delta.x = max_delta.y = max_delta.z = 0.0;
	if(!flight_data.empty())
	{
		int d_size = flight_data.size();
        // 控制循环
        this->flag = true;
        for(int i = 0; i < d_size;)
		{
            // lock
            pthread_mutex_lock(&mut);
            // loop
			fd = flight_data[i];

            Controller::cal_delta(fd, delta);
            widget->show_error_info(i, delta);
            if(Controller::isBigger(delta, max_delta))
            {
                max_delta = delta;
                widget->show_max_error(i, max_delta);
            }


            x   = delta.x;
            y   = delta.y;
            z   = fd.pos.z;
            yaw = fd.att.yaw;
            Controller::moveByPositionAndYaw(x, y, z ,yaw);

            // unlock
            pthread_mutex_unlock(&mut);
            if(!flag)
            {
                return true;
            }
            i += 100 / freq;
		}
        // 返回结束点
        Controller::goLast(flight_data[d_size - 1]);
		return true;
	}
	return false;
}

//姿态控制 + 返回结束点
bool
Controller::flightByAttAndVertPos(std::string trace_nick_name)
{
    float                     roll, pitch, yaw, z;
	vector<HUST::FlightData>  flight_data;

	trace.get(trace_nick_name, flight_data);
	std::cout << "Successfullt loda flight data.\n";
	std::cout << "data point numbers: " << flight_data.size() << std::endl;

	HUST::FlightData     fd;
    HUST::Positon        delta, max_delta;
    max_delta.x = max_delta.y = max_delta.z = 0.0;
	if(!flight_data.empty())
	{
		int d_size = flight_data.size();
        // 控制循环
        this->flag = true;
        for(int i = 0; i < d_size;)
		{
            // lock
            pthread_mutex_lock(&mut);
            // loop
            fd = flight_data[i];

            Controller::cal_delta(fd, delta);
            widget->show_error_info(i, delta);
            if(Controller::isBigger(delta, max_delta))
            {
                max_delta = delta;
                widget->show_max_error(i, max_delta);
            }

            roll   = fd.att.roll;
            pitch  = fd.att.pitch;
            yaw    = fd.att.yaw;
            z      = fd.pos.z;
            Controller::moveByAttitudeAndVertPos(roll, pitch, yaw, z);

            // unlock
            pthread_mutex_unlock(&mut);
            if(!flag)
            {
                return true;
            }
            i += 100 / freq;
		}
        // 返回结束点
        Controller::goLast(flight_data[d_size-1]);
		return true;
	}
	return false;
}

//速度控制 + 返回结束点
bool
Controller::flightByVelAndYawRate(std::string trace_nick_name)
{
	float                     Vx, Vy, Vz, yawRate;	
	vector<HUST::FlightData>  flight_data;

	trace.get(trace_nick_name, flight_data);
	std::cout << "Successfullt loda flight data.\n";
	std::cout << "data point numbers: " << flight_data.size() << std::endl;

	HUST::FlightData     fd;
    HUST::Positon        delta, max_delta;
    max_delta.x = max_delta.y = max_delta.z = 0.0;
	if(!flight_data.empty())
	{
		int d_size = flight_data.size();
        // 控制循环
        this->flag = true;
        for(int i = 0; i < d_size;)
		{
            // lock
            pthread_mutex_lock(&mut);

            // loop
            fd = flight_data[i];

            Controller::cal_delta(fd, delta);
            widget->show_error_info(i, delta);
            if(Controller::isBigger(delta, max_delta))
            {
                max_delta = delta;
                widget->show_max_error(i, max_delta);
            }

            Vx      = fd.vel.Vx + delta.x*X;
            Vy      = fd.vel.Vy + delta.y*X;
            Vz      = fd.vel.Vz + delta.z*X;
			yawRate = fd.ang.yawRate;
			Controller::moveByVelocityAndYawRate(Vx, Vy, Vz, yawRate);

            // unlock
            pthread_mutex_unlock(&mut);
            if(!flag)
            {
                return true;
            }
            i += 100 / freq;
		}
        // 返回结束点
        Controller::goLast(flight_data[d_size-1]);
		return true;
	}
	return false;
}

//姿态平均值
bool
Controller::flightByAttAndVertPos_average(std::string trace_nick_name)
{
    std::cout << trace_nick_name << std::endl;
    int i = 0;
    this->flag = true;
    while(true)
    {
        pthread_mutex_lock(&mut);
        std::cout << i << ": attitude average..." << std::endl;
        pthread_mutex_unlock(&mut);
        if(!flag)
        {
            return true;
        }
        i += 1;
        usleep(20 * 1000);
    }
    return true;
}

//速度平均值
bool
Controller::flightByVelAndYawRate_average(std::string trace_nick_name)
{
}

//速度平均值
bool
Controller::flightByPosAndYaw_average(std::string trace_nick_name)
{

}

//Core-contrl function
void
Controller::moveByPositionAndYaw(float x, float y, float z, float yaw)
{
    int cycleTimeInMs   = 1000 / freq;
    vehicle->control->positionAndYawCtrl(x, y, z, yaw);

    usleep(cycleTimeInMs * 1000);
}
void
Controller::moveByVelocityAndYawRate(float Vx,float Vy,float Vz,float yawRate)
{
    int cycleTimeInMs   = 1000 / freq;
    vehicle->control->velocityAndYawRateCtrl(Vx, Vy, Vz, yawRate);

    usleep(cycleTimeInMs * 1000);
}
void
Controller::moveByAttitudeAndVertPos(float roll, float pitch, float yaw, float z)
{
    int cycleTimeInMs   = 1000 / freq;
    vehicle->control->attitudeAndVertPosCtrl(roll, pitch, yaw, z);
		
    usleep(cycleTimeInMs * 1000);
}




// 数据获取， 判断是否存储
bool
Controller::getFlightData()
{
    //getBroadcastData();

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

    return true;
}
void Controller::printData()
{
    Telemetry::GlobalPosition currentBroadcastGP;
    Telemetry::Vector3f       velocity;
    Telemetry::Quaternion     quaternion;
    Telemetry::Vector3f       angularRate;

    Telemetry::Vector3f       ans;
    Telemetry::Vector3f       localOffset;

    HUST::FlightData     fd;
    fd.pos.x             = 0.00001;
    fd.pos.y             = 0.00001;
    fd.pos.z             = 0.00001;
    fd.vel.Vx            = 0.00001;
    fd.vel.Vy            = 0.00001;
    fd.vel.Vz            = 0.00001;
    fd.att.roll          = 0.00001;
    fd.att.pitch         = 0.00001;
    fd.att.yaw           = 0.00001;
    int i = 0;
    HUST::Positon        max_delta;
    max_delta.x = max_delta.y = max_delta.z = 0.01;
    while(true)
    {
//        currentBroadcastGP   = vehicle->broadcast->getGlobalPosition();
//        velocity             = vehicle->broadcast->getVelocity();
//        quaternion           = vehicle->broadcast->getQuaternion();
//        angularRate          = vehicle->broadcast->getAngularRate();

//        ans = toEulerAngle((static_cast<void*>(&quaternion)));
//        localOffsetFromGpsOffset(localOffset, static_cast<void*>(&currentBroadcastGP),
//                                              static_cast<void*>(&homeGP));
//        fd.pos.x             = localOffset.x;
//        fd.pos.y             = localOffset.y;
//        fd.pos.z             = currentBroadcastGP.altitude;
//        fd.vel.Vx            = velocity.x;
//        fd.vel.Vy            = velocity.y;
//        fd.vel.Vz            = velocity.z;
//        fd.att.roll          = ans.y * RAD2DEG;
//        fd.att.pitch         = ans.x * RAD2DEG;
//        fd.att.yaw           = ans.z * RAD2DEG;
//        fd.ang.rollRate      = angularRate.x;
//        fd.ang.pitchRate     = angularRate.y;
//        fd.ang.yawRate       = angularRate.z;

        fd.pos.x             += 0.00001;
        fd.pos.y             += 0.00001;
        fd.pos.z             += 0.00001;
        fd.vel.Vx            += 0.00001;
        fd.vel.Vy            += 0.00001;
        fd.vel.Vz            += 0.00001;
        fd.att.roll          += 0.00001;
        fd.att.pitch         += 0.00001;
        fd.att.yaw           += 0.00001;

        widget->show_error_info(i, fd.pos);
        if(Controller::isBigger(fd.pos, max_delta))
        {
            max_delta = fd.pos;
            widget->show_max_error(i, max_delta);
        }
        i += 1;
        widget->showInfo(fd);
        usleep(10 * 1000);
    }
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
  freq[1]  = DataBroadcast::FREQ_50HZ;
  freq[2]  = DataBroadcast::FREQ_HOLD;
  freq[3]  = DataBroadcast::FREQ_HOLD;
  freq[4]  = DataBroadcast::FREQ_50HZ;
  freq[5]  = DataBroadcast::FREQ_50HZ; // This is the only one we want to change
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


void
Controller::susppend()
{
    if(!flag)
    {
        return;
    }
    pthread_mutex_lock(&mut);
}
void
Controller::resume()
{
    if(!flag)
    {
        return;
    }
    pthread_mutex_unlock(&mut);
}
void
Controller::stop()
{
    if(!flag)
    {
        return;
    }
    this->flag = false;
    pthread_mutex_unlock(&mut);
}






bool Controller::monitoredTakeoff()
{
    return false;
}

bool Controller::monitoredLanding()
{
    return false;
}
