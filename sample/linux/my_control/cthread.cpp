#include "cthread.h"
#include <QDebug>
#include <QMessageBox>
CThread::CThread(int n, QObject* par):
    QThread(par)
{
    this->n = n;
}
int CThread::get_missionType()
{
    return this->n;
}
CThread::~CThread()
{
    qDebug() << "thread destroyed.\n";
}

void CThread::run()
{
    flag = true;
    if (n == 0)
    {
        ctr->printData();
    }
    if (n == 1)
    {
        while(true)
        {
            qDebug() << "do something.\n";
            //ctr->getFlightData(true);
            {
                QMutexLocker locker(&m_lock);
                if (!this->flag)
                {
                    return;
                }
            }
            usleep(10 * 1000); // 获取数据信息: 100Hz
        }
    } else if (n == 2)
    {
        //ctr->flightByAttAndVertPos(nick_name);
        qDebug() << "ctr->flightByAttAndVertPos(nick_name)";
    } else if (n == 3)
    {
        ctr->flightByVelAndYawRate(nick_name);
    } else if (n == 4)
    {
        ctr->flightByPosAndYaw(nick_name);
    }
    else if (n == 5)
    {
        ctr->flightByAttAndVertPos_average(nick_name);
    } else if (n == 6)
    {
        ctr->flightByVelAndYawRate_average(nick_name);
    } else if (n == 7)
    {
        ctr->flightByPosAndYaw_average(nick_name);
    }
}

void CThread::set_controller(Controller *ctr)
{
    this->ctr = ctr;
}
void CThread::set_nickName(string nick_name)
{
   this->nick_name = nick_name;
}
void CThread::stopImmediately()
{
    qDebug() << "lock & stop\n";
    QMutexLocker locker(&m_lock);
    this->flag = false;
    qDebug() << "set stoped.\n";
}
