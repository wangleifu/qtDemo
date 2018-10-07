#include "cthread.h"
#include <QDebug>
#include <QMessageBox>
#include <QMetaType>
CThread::CThread(int n, QObject* par):
    QThread(par)
{
    this->n = n;
    this->flag = false;
    this->isPause = false;

    //moveToThread(this);
}
int CThread::get_missionType()
{
    return this->n;
}
CThread::~CThread()
{
    qDebug() << "thread destroyed.\n";
}

void CThread::set_controller(Controller *ctr)
{
    this->ctr = ctr;
}
void CThread::set_dt(double dt)
{
    this->dt = dt * 100;
    qDebug() << "set dt: " << this->dt;
}
void CThread::set_nickName(string nick_name)
{
   this->nick_name = nick_name;
}
void CThread::set_freq(int freq)
{
    this->freq = freq;
    this->cycleTimeInMs = 1000 / freq;
    qDebug() << "set freq: " << freq << "Hz, cycle time : " << cycleTimeInMs << "ms";
}
void CThread::stopImmediately()
{
    QMutexLocker locker(&m_lock);
    this->flag = false;
}

void CThread::run()
{
    flag = true;
    if (n == 0)
    {
        HUST::FlightData fd;
        ctr->set_homePoint();
        while(true)
        {
            ctr->printData(fd);
            emit send_info(fd);

            usleep(100 * 1000);
        }
    } else if (n ==8 )
    {
        ctr->obtainControl();
        ctr->goHome();
        ctr->releaseControl();
    }
    else if (n == 1)
    {
        ctr->set_homePoint();
        while(flag)
        {
            ctr->recordFlightData();
            usleep(10 * 1000);     // 获取数据信息: 100Hz
        }
    } else
    {
        ctr->obtainControl();
        vector<HUST::FlightData>  flight_data;

        ctr->getData(nick_name, flight_data);
        if(flight_data.empty())
        {
            qDebug() << "flight data is empty.\n";
            return;
        }

        int d_size           = flight_data.size();
        HUST::Positon        ut         = {0.0,0.0,0.0};
        HUST::Positon        ut_last    = {0.0,0.0,0.0};
        HUST::Positon        ut_delta    = {0.0,0.0,0.0};
        HUST::Positon        delta      = {0.0,0.0,0.0};
        HUST::Positon        delta_last = {0.0,0.0,0.0};
        HUST::Positon        delta_sum  = {0.0,0.0,0.0};
        HUST::Positon        max_delta  = {0.0,0.0,0.0};
        float max;
        if (n == 2)
        {
            ctr->flightByAttAndVertPos(flight_data[0]);
            usleep(cycleTimeInMs * 1000);

            for(int i = 1; i < d_size;i+=100/freq)
            {
                m_lock.lock();
                ctr->cal_delta(flight_data[i-1], delta);
                emit send_error_info(i-1, delta);
                if(ctr->isBigger(delta, max_delta))
                {
                    max_delta = delta;
                    max = std::sqrt(std::pow(max_delta.x, 2) + std::pow(max_delta.y, 2) + std::pow(max_delta.z, 2));
                    emit send_max_error_info(i-1, max_delta, max);
                }
                ctr->flightByAttAndVertPos(flight_data[i]);

                m_lock.unlock();
                usleep(cycleTimeInMs * 1000 - dt);
            }
        } else if (n == 3)
        {
            qDebug() << "flight by velocity and yawRate.";
            ctr->flightByVelAndYawRate(delta, flight_data[0]);
            usleep(cycleTimeInMs * 1000);

            for(int i = 1; i < d_size;i+=100/freq)
            {
                m_lock.lock();
                ctr->cal_delta(flight_data[i-1], delta);
                emit send_error_info(i-1, delta);
                if(ctr->isBigger(delta, max_delta))
                {
                    max_delta = delta;
                    max = std::sqrt(std::pow(max_delta.x, 2) + std::pow(max_delta.y, 2) + std::pow(max_delta.z, 2));
                    emit send_max_error_info(i-1, max_delta, max);
                }
                ctr->cal_ut(delta, delta_sum, delta_last, ut, ut_last, ut_delta);
                ctr->flightByVelAndYawRate(ut_delta, flight_data[i]);

                m_lock.unlock();
                usleep(cycleTimeInMs * 1000 - dt);
            }
        } else if (n == 4)
        {
            ctr->flightByPosAndYaw(delta, flight_data[0]);
            usleep(cycleTimeInMs * 1000);

            for(int i = 1; i < d_size;i+=100/freq)
            {
                m_lock.lock();
                ctr->cal_delta(flight_data[i-1], delta);
                emit send_error_info(i-1, delta);
                if(ctr->isBigger(delta, max_delta))
                {
                    max_delta = delta;
                    max = std::sqrt(std::pow(max_delta.x, 2) + std::pow(max_delta.y, 2) + std::pow(max_delta.z, 2));
                    emit send_max_error_info(i-1, max_delta, max);
                }
                ctr->cal_delta(flight_data[i], delta);
                ctr->cal_ut(delta, delta_sum, delta_last, ut, ut_last, ut_delta);
                ctr->flightByPosAndYaw(ut_delta, flight_data[i]);

                m_lock.unlock();
                usleep(cycleTimeInMs * 1000);
            }
        }
        else if (n == 5)
        {
            ctr->flightByAttAndVertPos_average(flight_data[0], flight_data[1]);
            usleep(cycleTimeInMs * 1000);

            for(int i = 1; i < d_size-1;i+=100/freq)
            {
                m_lock.lock();
                ctr->cal_delta(flight_data[i-1], delta);
                emit send_error_info(i-1, delta);
                if(ctr->isBigger(delta, max_delta))
                {
                    max_delta = delta;
                    max = std::sqrt(std::pow(max_delta.x, 2) + std::pow(max_delta.y, 2) + std::pow(max_delta.z, 2));
                    emit send_max_error_info(i-1, max_delta, max);
                }
                ctr->flightByAttAndVertPos_average(flight_data[i], flight_data[i+1]);
                m_lock.unlock();
                usleep(cycleTimeInMs * 1000);
            }
        } else if (n == 6)
        {
            ctr->flightByVelAndYawRate_average(flight_data[0], flight_data[1]);
            usleep(cycleTimeInMs * 1000);

            for(int i = 1; i < d_size-1;i+=100/freq)
            {
                m_lock.lock();
                ctr->cal_delta(flight_data[i], delta);
                emit send_error_info(i, delta);
                if(ctr->isBigger(delta, max_delta))
                {
                    max_delta = delta;
                    max = std::sqrt(std::pow(max_delta.x, 2) + std::pow(max_delta.y, 2) + std::pow(max_delta.z, 2));
                    emit send_max_error_info(i, max_delta, max);
                }
                //ctr->flightByVelAndYawRate_average(flight_data[i], flight_data[i+1]);
                ctr->flightByVelAndYawRate(ut, flight_data[i]);

                m_lock.unlock();
                usleep(cycleTimeInMs * 1000);
            }
        } else if (n == 7)
        {
            for(int i = 1; i < d_size-1;i++)
            {
                m_lock.lock();
                ctr->cal_delta(flight_data[i-1], delta);
                emit send_error_info(i-1, delta);
                if(ctr->isBigger(delta, max_delta))
                {
                    max_delta = delta;
                    max = std::pow(max_delta.x, 2) + std::pow(max_delta.y, 2) + std::pow(max_delta.z, 2);
                    emit send_max_error_info(i-1, max_delta, max);
                }
                while(std::abs(delta.x) > 0.8 || std::abs(delta.y) > 0.8 || std::abs(delta.z) > 0.8)
                {
                    ctr->flightByPosAndYaw_average(delta, flight_data[i]);
                    usleep(cycleTimeInMs * 1000);

                    ctr->cal_delta(flight_data[i], delta);
                    emit send_error_info(i, delta);
                    if(ctr->isBigger(delta, max_delta))
                    {
                        max_delta = delta;
                        max = std::sqrt(std::pow(max_delta.x, 2) + std::pow(max_delta.y, 2) + std::pow(max_delta.z, 2));
                        emit send_max_error_info(i, max_delta, max);
                    }
                }
                m_lock.unlock();
            }
        }
        ctr->goLast(flight_data[d_size-1]);
        ctr->goHome();
        ctr->releaseControl();
    }
}
