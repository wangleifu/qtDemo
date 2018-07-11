#ifndef WIDGET_H
#define WIDGET_H

class Controller;
class CThread;
#include <QWidget>
#include "dialog.h"
#include "flight_controller.hpp"
#include "cthread.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public:
    void set_controller(Controller *ctr);
    void showInfo(HUST::FlightData fd);
    void show_error_info(int id, HUST::Positon delta);
    void show_max_error(int id, HUST::Positon delta);
    void startMission(int n, string s);
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();


    void on_pushButton_8_clicked();

    void parseText(QString nick_name);

private:
    Controller *ctr;

private:
    Ui::Widget *ui;
    bool flag;

// 多线程
//signals:
//    void onQThreadFinished();
private slots:
    void onLocalThreadDestroy(QObject* obj);
    void on_pushButton_9_clicked();

private:
    CThread* p_thread;
    CThread* m_currentRunLoaclThread;
};

#endif // WIDGET_H
