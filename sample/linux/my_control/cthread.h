#ifndef CTHREAD_H
#define CTHREAD_H
#include <QThread>
#include <QMutex>
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
     int get_missionType();

public slots:
    void stopImmediately();
private:
     int        n;
     string     nick_name;
     QMutex     m_lock;
     bool       flag;
     Controller *ctr;
};

#endif // CTHREAD_H
