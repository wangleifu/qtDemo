#ifndef PRINT_THREAD_HPP
#define PRINT_THREAD_HPP

#include <QThread>

class PrintThread : public QThread
{
	Q_OBJECT

public:
	PrintThread(FlightController* f_controller, QObject* par);
	~PrintThread();
	void run();

signals:
	void send_info(HUST::FlightData fd);

private:
	void printing();

private:
	FlightController*  f_controller;

}

#endif