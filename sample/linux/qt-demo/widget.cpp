#include "widget.h"
#include "ui_widget.h"
#include "qmessagebox.h"
#include <QDebug>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    ctr(NULL),
    m_currentRunLoaclThread(NULL)
{
    ui->setupUi(this);
    this->setWindowTitle("Debug window");
    qRegisterMetaType< HUST::FlightData >("HUST::FlightData");
    qRegisterMetaType< HUST::Positon >("HUST::Positon");
}
void
Widget::set_controller(Controller *ctr)
{
    this->ctr = ctr;

    p_thread = new CThread(0, NULL);
    p_thread->set_controller(ctr);
    connect(p_thread,SIGNAL(finished()), p_thread,SLOT(deleteLater()));
    connect(p_thread, SIGNAL(send_info(HUST::FlightData)), this, SLOT(show_info(HUST::FlightData)));
    p_thread->start();
}
Widget::~Widget()
{
    p_thread->terminate();
    p_thread->wait();
    delete ui;
}



// 设置当前点为原点
void Widget::on_pushButton_clicked()
{
    if(m_currentRunLoaclThread)
    {
        QMessageBox::information(this, tr("Info"), "There is another mission is running, please wait!");
    } else {
        ctr->set_homePoint();
    }
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
    if(m_currentRunLoaclThread && !m_currentRunLoaclThread->isPause)
    {
        m_currentRunLoaclThread->m_lock.lock();
        m_currentRunLoaclThread->isPause = true;
    }
}
// 继续
void Widget::on_pushButton_5_clicked()
{
    if(m_currentRunLoaclThread && m_currentRunLoaclThread->isPause)
    {
        m_currentRunLoaclThread->isPause = false;
        m_currentRunLoaclThread->m_lock.unlock();
    }
}
// 结束
void Widget::on_pushButton_9_clicked()
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
// 开始记录
void Widget::on_pushButton_6_clicked()
{
    startMission(1, "");
}
// 停止记录
void Widget::on_pushButton_7_clicked()
{
    if(m_currentRunLoaclThread && m_currentRunLoaclThread->get_missionType() == 1)
    {
        m_currentRunLoaclThread->stopImmediately();
        Dialog *dialog = new Dialog(this);
        connect(dialog,SIGNAL(getText(QString, QString)), this, SLOT(parseText(QString, QString)));
        dialog->show();
    }
    else
    {
        QMessageBox::information(this, tr("Info"), "Please start recording first!");
    }
}
// slots： 存储数据
void Widget::parseText(QString nick_name, QString describe)
{
    qDebug() << "get signal...";
    if (!nick_name.isEmpty() && !describe.isEmpty())
    {
        qDebug() << "save data to database";
        ctr->save(nick_name.toStdString(), describe.toStdString());
    }
    else if(!nick_name.isEmpty())
    {
        qDebug() << "save delta data, after dialog";
        ctr->save_delta(nick_name.toStdString());
    }

    ctr->clear();
}
// 导入路径
void Widget::on_pushButton_8_clicked()
{
    ui->comboBox->clear();
    QString s = QString::fromStdString(ctr->get_trace());
    ui->comboBox->addItems(s.split(','));
}

void
Widget::cal_delta(int id, HUST::FlightData fd)
{
    //ctr->cal_delta_all(fd, delta, max);
    return;
}
void
Widget::show_info(HUST::FlightData fd)
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
void Widget::show_error_info(int id, HUST::Positon delta)
{
    ui->label_39->setNum(id);
    ui->label_29->setNum(delta.x);
    ui->label_27->setNum(delta.y);
    ui->label_30->setNum(delta.z);
}
void Widget::show_max_error_info(int id, HUST::Positon delta,float max)
{
    ui->label_41->setNum(id);
    ui->label_36->setNum(delta.x);
    ui->label_37->setNum(delta.y);
    ui->label_35->setNum(delta.z);
    ui->label_38->setNum(max);
}


// 多线程
void Widget::onLocalThreadDestroy(QObject* obj)
{
    if(qobject_cast<QObject*>(m_currentRunLoaclThread) == obj)
    {
        qDebug() << "set NULL pointer.\n";
        m_currentRunLoaclThread = NULL;
        QMessageBox::information(this, tr("Info"), "Mission finish!");
        Dialog *dialog = new Dialog(this);
        connect(dialog,SIGNAL(getText(QString, QString)), this, SLOT(parseText(QString, QString)));
        dialog->show();
    }
}
void Widget::startMission(int n, string s)
{
    if(m_currentRunLoaclThread)
    {
        QMessageBox::information(this, tr("Info"), "There is another mission is running, please wait!");
        return;
    }

    int freq = ui->spinBox->value();
    double x = ui->doubleSpinBox->value();
    double dt = ui->doubleSpinBox_2->value();
    double kp = ui->doubleSpinBox_3->value();
    double ki = ui->doubleSpinBox_4->value();
    double kd = ui->doubleSpinBox_5->value();
    if(freq < 1)
    {
        QMessageBox::information(this, tr("Info"), "freq con not be 0!");
        return;
    }


    CThread* thread = new CThread(n, NULL);
    if (s != "")
    {
        thread->set_nickName(s);
    }
    ctr->set_X(x);
    ctr->set_PID(kp, ki, kd);
    thread->set_controller(ctr);
    thread->set_freq(freq);
    thread->set_dt(dt);

    connect(thread,SIGNAL(finished()), thread,SLOT(deleteLater()));//线程结束后调用deleteLater来销毁分配的内存
    connect(thread,SIGNAL(destroyed(QObject*)), this,SLOT(onLocalThreadDestroy(QObject*)));
    connect(thread, SIGNAL(send_fData(int,HUST::FlightData)), this, SLOT(cal_delta(int,HUST::FlightData)));
    connect(thread, SIGNAL(send_error_info(int,HUST::Positon)), this, SLOT(show_error_info(int,HUST::Positon)));
    connect(thread, SIGNAL(send_max_error_info(int,HUST::Positon,float)), this, SLOT(show_max_error_info(int,HUST::Positon,float)));

    thread->start();
    m_currentRunLoaclThread = thread;
}
