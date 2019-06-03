#ifndef CTHREAD_H
#define CTHREAD_H
#include <QThread>
#include <QMutex>
#include <QMetaType>
//class FlightController;
#include "hust.hpp"
#include "flight_controller.hpp"
class FlightController;

class CThread : public QThread
{
    Q_OBJECT
public:
     CThread(int n, QObject* par);
     ~CThread();
     void run();

     void set_controller(FlightController* ctr);
     void set_nickName(std::string nick_name);
     void set_freq(int freq);
     void set_dt(double dt);
     int  get_missionType();
signals:
     void send_fData(int id, DATA::FlightData fd);
     void send_info(DATA::FlightData fd);
     void send_error_info(int id, DATA::Positon fd);
     void send_max_error_info(int id, DATA::Positon fd, float max);
public slots:
    void stopImmediately();
public:
    bool       isPause;
    QMutex     m_lock;
private:
     int              n;
     int              dt;
     int              freq;
     int              cycleTimeInMs;
    std::string       nick_name;
     bool             flag;
     FlightController* ctr;
     DATA::Positon     m_currentPosition;
};

#endif // CTHREAD_H
