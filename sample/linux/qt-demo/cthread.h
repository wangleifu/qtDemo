#ifndef CTHREAD_H
#define CTHREAD_H
#include <QThread>
#include <QMutex>
#include <QMetaType>
class Controller;
#include "flight_controller.hpp"

class CThread : public QThread
{
    Q_OBJECT
public:
     CThread(int n, QObject* par);
     ~CThread();
     void run();

     void set_controller(Controller *ctr);
     void set_nickName(string nick_name);
     void set_freq(int freq);
     void set_dt(double dt);
     int  get_missionType();
signals:
     void send_fData(int id, HUST::FlightData fd);
     void send_info(HUST::FlightData fd);
     void send_error_info(int id, HUST::Positon fd);
     void send_max_error_info(int id, HUST::Positon fd, float max);
public slots:
    void stopImmediately();
public:
    bool       isPause;
    QMutex     m_lock;
private:
     int        n;
     int        dt;
     int        freq;
     int        cycleTimeInMs;
     string     nick_name;
     bool       flag;
     Controller *ctr;
};

#endif // CTHREAD_H
