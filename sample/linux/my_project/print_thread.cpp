#include "print_thread.hpp"

PrintThread::PrintThread(FlightController* f_controller, QObject* par) : QThread(par)
{
	this->f_controller = f_controller;
}

PrintThread::~PrintThread()
{
	this->f_controller = NULL;
}

void PrintThread::run()
{
	PrintThread::printing();
}

void PrintThread::printing()
{
	//
	HUST::FlightData fd;
	f_controller->set_homePoint();
	while(true)
	{
		f_controller->printData(fd);
		emit send_info(fd);

		usleep(100 * 1000);
	}
}