#include "controller.hpp"

using namespace DJI;
using namespace DJI::OSDK;

Controller::Controller(Vehicle* vehicle)
{
    this->vehicle = vehicle;
    this->f_controller = new FlightController(this->vehicle);
}
Controller::Controller(Vehicle* vehicle, Widget* widget)
{
	this->vehicle = vehicle;
	this->widget  = widget;
	this->print   = true;

    this->f_controller = new FlightController(this->vehicle);

    this->p_thread = NULL;
    this->m_currentRunLoaclThread = NULL;
    Controller::initPrintMission();
}
Controller::~Controller()
{
    p_thread->terminate();
    p_thread->wait();

    delete f_controller;
    delete widget;
    delete vehicle;
}



void Controller::takeOff()
{
    bool isSuccess = this->f_controller->monitoredTakeoff();
    std::cout << "take off status: " << isSuccess << std::endl;
}
void Controller::land()
{
    this->f_controller->monitoredLanding();
}
void Controller::goHome()
{
    this->f_controller->goHome();
}

void
Controller::setHomePoint()
{
    if(m_currentRunLoaclThread)
    {
        QMessageBox::information(widget, "Info", "There is another mission is running, please wait!");
    } else {
        f_controller->set_homePoint();
    }
}

std::string Controller::get_trace()
{
    return f_controller->get_trace();
}

void Controller::storageTrackToDB(std::string trace_name, std::string describe)
{
    f_controller->save(trace_name, describe);
}
void Controller::storageTraceToText(std::string trace_name)       // 存储实际飞行轨迹到txt，用于与预订轨迹做对比
{
    f_controller->save_delta(trace_name);
}
void Controller::clear()
{
    f_controller->clear();
}



void
Controller::initPrintMission()
{
    //  初始化 并 启动实时数据打印线程
    p_thread = new CThread(0, NULL);
    p_thread->set_controller(f_controller);
    QObject::connect(p_thread,SIGNAL(finished()), p_thread,SLOT(deleteLater()));
    QObject::connect(p_thread, SIGNAL(send_info(DATA::FlightData)), widget, SLOT(show_info(DATA::FlightData)));
    p_thread->start();
}

void
Controller::missionStart(int missionType, std::string trace_name, int freq, double kp, double ki, double kd)
{
    if(m_currentRunLoaclThread)
    {
        std::cout << m_currentRunLoaclThread << std::endl;
        QMessageBox::information(widget, "Info", "There is another mission is running, please wait!");
        return;
    }
    if(freq < 1)
    {
        QMessageBox::information(widget, "Info", "freq con not be 0!");
        return;
    }


    CThread* thread = new CThread(missionType, NULL);
    if (trace_name != "")
    {
        thread->set_nickName(trace_name);
    }
    //ctr->set_X(x);
    f_controller->set_PID(kp, ki, kd);
    thread->set_controller(f_controller);
    thread->set_freq(freq);
    //thread->set_dt(dt);

    QObject::connect(thread,SIGNAL(finished()), thread,SLOT(deleteLater()));//线程结束后调用deleteLater来销毁分配的内存
    QObject::connect(thread,SIGNAL(destroyed(QObject*)), widget,SLOT(onLocalThreadDestroy(QObject*)));
    //QObject::connect(thread, SIGNAL(send_fData(int,DATA::FlightData)), widget, SLOT(cal_delta(int,DATA::FlightData)));
    QObject::connect(thread, SIGNAL(send_error_info(int,DATA::Positon)), widget, SLOT(show_error_info(int,DATA::Positon)));
    QObject::connect(thread, SIGNAL(send_max_error_info(int,DATA::Positon,float)), widget, SLOT(show_max_error_info(int,DATA::Positon,float)));

    thread->start();
    m_currentRunLoaclThread = thread;
}

void
Controller::missionPause()
{
    if(m_currentRunLoaclThread && !m_currentRunLoaclThread->isPause)
    {
        m_currentRunLoaclThread->m_lock.lock();
        m_currentRunLoaclThread->isPause = true;
        f_controller->vehicle->control->emergencyBrake();
    }
}

void Controller::missionContinue()
{
    if(m_currentRunLoaclThread && m_currentRunLoaclThread->isPause)
    {
        f_controller->goLast(m_currentRunLoaclThread->m_currentPosition);
        m_currentRunLoaclThread->isPause = false;
        m_currentRunLoaclThread->m_lock.unlock();
    }
}
void Controller::missionTerminate()
{
    if(m_currentRunLoaclThread && m_currentRunLoaclThread->get_missionType() != 1)
    {
        if(m_currentRunLoaclThread->isPause)
        {
            m_currentRunLoaclThread->m_lock.unlock();
        }
        m_currentRunLoaclThread->terminate();
        m_currentRunLoaclThread->wait();
    }
}

void Controller::endTrackRecord()
{
    if(m_currentRunLoaclThread && m_currentRunLoaclThread->get_missionType() == 1)
    {
        m_currentRunLoaclThread->stopImmediately();
        Dialog *dialog = new Dialog(widget);
        QObject::connect(dialog,SIGNAL(getText(QString, QString)), widget, SLOT(parseText(QString, QString)));
        dialog->show();
    }
    else
    {
        QMessageBox::information(widget, "Info", "Please start recording first!");
    }
}

void
Controller::onLocalThreadDestroy(QObject* obj)
{
    // 销毁线程 并 弹出提示框
    if(qobject_cast<QObject*>(m_currentRunLoaclThread) == obj)
    {
        qDebug() << "set NULL pointer.\n";
        m_currentRunLoaclThread = NULL;
        QMessageBox::information(widget, "Info", "Mission finish!");
        Dialog *dialog = new Dialog(widget);
        QObject::connect(dialog,SIGNAL(getText(QString, QString)), widget, SLOT(parseText(QString, QString)));
        dialog->show();
    }
}
