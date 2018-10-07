#ifndef WIDGET_H
#define WIDGET_H

class Controller;
class CThread;
#include <QWidget>
#include <string>
#include "dialog.h"
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
    void startMission(int n, string s);
    template <typename Type> QString numToString(const Type& num);
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();


    void on_pushButton_8_clicked();

    void parseText(QString nick_name, QString describe);
    void cal_delta(int id, HUST::FlightData fd);
    void show_info(HUST::FlightData fd);
    void show_error_info(int id, HUST::Positon delta);
    void show_max_error_info(int id, HUST::Positon delta, float max);

private:
    Controller *ctr;

private:
    Ui::Widget *ui;
    bool flag;

// 多线程
private slots:
    void onLocalThreadDestroy(QObject* obj);
    void on_pushButton_9_clicked();

private:
    CThread* p_thread;
    CThread* m_currentRunLoaclThread;
};

#endif // WIDGET_H
