#include "widget.h"
#include "ui_widget.h"
#include "qmessagebox.h"
#include <QDebug>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("Debug window");
    qRegisterMetaType< DATA::FlightData >("DATA::FlightData");
    qRegisterMetaType< DATA::Positon >("DATA::Positon");
}
void
Widget::set_controller(Controller *ctr)
{
    this->controller = ctr;
}
Widget::~Widget()
{
    delete ui;
}



// 设置当前点为原点
void Widget::on_pushButton_clicked()
{
    //controller->setHomePoint();
    controller->takeOff();
}
// 返回原点
void Widget::on_pushButton_2_clicked()
{
    startMission(8,"");
}
// 开始
void Widget::on_pushButton_3_clicked()
{
    QString nick_name = ui->comboBox->currentText();
    if(nick_name.isEmpty())
    {
        QMessageBox::information(this, tr("Info"), "Please input trace name!");
        return;
    }
    if (ui->radioButton->isChecked())
    {
        if(ui->checkBox->isChecked())
        {
            startMission(5, nick_name.toStdString());
        } else
        {
            startMission(2,nick_name.toStdString());
        }
    }
    else if (ui->radioButton_2->isChecked())
    {
        if(ui->checkBox->isChecked())
        {
            startMission(6, nick_name.toStdString());
        }
        else
        {
            startMission(3,nick_name.toStdString());
        }

    }
    else if (ui->radioButton_3->isChecked())
    {
        if(ui->checkBox->isChecked())
        {
            startMission(7, nick_name.toStdString());
        }
        else
        {
            startMission(4,nick_name.toStdString());
        }
    } else
    {
        QMessageBox::information(this, tr("Info"), "Please chose control mode!");
    }
}
// 暂停
void Widget::on_pushButton_4_clicked()
{
    controller->missionPause();
}
// 继续
void Widget::on_pushButton_5_clicked()
{
    controller->missionContinue();
}
// 结束
void Widget::on_pushButton_9_clicked()
{
    controller->missionTerminate();
}
// 开始记录
void Widget::on_pushButton_6_clicked()
{
    startMission(1, "");
}
// 停止记录
void Widget::on_pushButton_7_clicked()
{
    controller->endTrackRecord();
}
// slots： 存储数据
void Widget::parseText(QString nick_name, QString describe)
{
    if (!nick_name.isEmpty() && !describe.isEmpty())
    {
        qDebug() << "save data to database";
        controller->storageTrackToDB(nick_name.toStdString(), describe.toStdString());
    }
    else if(!nick_name.isEmpty())
    {
        qDebug() << "save delta data, after dialog";
        controller->storageTraceToText(nick_name.toStdString());
    }

    controller->clear();
}
// 导入路径
void Widget::on_pushButton_8_clicked()
{
    ui->comboBox->clear();
    QString s = QString::fromStdString(controller->get_trace());
    ui->comboBox->addItems(s.split(','));
}
void Widget::show_info(DATA::FlightData fd)
{
    ui->label_5->setNum(fd.pos.x);
    ui->label_7->setNum(fd.pos.y);
    ui->label_9->setNum(fd.pos.z);

    ui->label_15->setNum(fd.vel.Vx);
    ui->label_13->setNum(fd.vel.Vy);
    ui->label_16->setNum(fd.vel.Vz);

    ui->label_22->setNum(fd.att.roll);
    ui->label_20->setNum(fd.att.pitch);
    ui->label_23->setNum(fd.att.yaw);
}
void Widget::show_error_info(int id, DATA::Positon delta)
{
    ui->label_39->setNum(id);
    ui->label_29->setNum(delta.x);
    ui->label_27->setNum(delta.y);
    ui->label_30->setNum(delta.z);
}
void Widget::show_max_error_info(int id, DATA::Positon delta,float max)
{
    ui->label_41->setNum(id);
    ui->label_36->setNum(delta.x);
    ui->label_37->setNum(delta.y);
    ui->label_35->setNum(delta.z);
    ui->label_38->setNum(max);
}
void Widget::startMission(int missionType, string trace_name)
{
    int freq = ui->spinBox->value();
    //double x = ui->doubleSpinBox->value();
    //double dt = ui->doubleSpinBox_2->value();
    double kp = ui->doubleSpinBox_3->value();
    double ki = ui->doubleSpinBox_4->value();
    double kd = ui->doubleSpinBox_5->value();

    controller->missionStart(missionType, trace_name, freq, kp, ki, kd);
}
void
Widget::onLocalThreadDestroy(QObject* obj)
{
    // 销毁线程 并 弹出提示框
    this->controller->onLocalThreadDestroy(obj);
}
