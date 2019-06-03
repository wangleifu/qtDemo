#ifndef WIDGET_H
#define WIDGET_H

class Controller;
#include <QWidget>
#include <string>
#include "dialog.h"
#include "hust.hpp"
#include "controller.hpp"


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
    void set_controller(Controller *controller);
    void startMission(int n, std::string s);
    template <typename Type> QString numToString(const Type& num);

public slots:
    void onLocalThreadDestroy(QObject* obj);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();

    void parseText(QString nick_name, QString describe);
    void show_info(DATA::FlightData fd);
    void show_error_info(int id, DATA::Positon delta);
    void show_max_error_info(int id, DATA::Positon delta, float max);

private:
    Controller *controller;

private:
    Ui::Widget *ui;
    bool flag;
};

#endif // WIDGET_H
