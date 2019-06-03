#include "track_thread.hpp"

TrackThread::TrackThread(FlightController* f_controller, QObject* par) : QThread(par)
{
	this->f_controller = f_controller;
}

TrackThread::~TrackThread()
{
	this->f_controller = NULL;
}

void TrackThread::run()
{
	TrackThread::tracking();
}




void TrackThread::tracking()
{
	//
	f_controller->obtainControl();
	vector<HUST::FlightData>  flight_data;

	f_controller->getData(nick_name, flight_data);
	if(flight_data.empty())
	{
	    qDebug() << "flight data is empty.\n";
	    return;
	}

	int d_size           = flight_data.size();
	HUST::Positon        ut         = {0.0,0.0,0.0};
	HUST::Positon        ut_last    = {0.0,0.0,0.0};
	HUST::Positon        ut_delta   = {0.0,0.0,0.0};
	HUST::Positon        delta      = {0.0,0.0,0.0};
	HUST::Positon        delta_last = {0.0,0.0,0.0};
	HUST::Positon        delta_sum  = {0.0,0.0,0.0};
	HUST::Positon        max_delta  = {0.0,0.0,0.0};
	float max;

	f_controller->flightByPosAndYaw(delta, flight_data[0]);
	usleep(cycleTimeInMs * 1000);

	for(int i = 1; i < d_size;i+=100/freq)
	{
		m_lock.lock();
		f_controller->cal_delta(flight_data[i-1], delta);
		emit send_error_info(i-1, delta);
		if(f_controller->isBigger(delta, max_delta))
		{
			max_delta = delta;
			max = std::sqrt(std::pow(max_delta.x, 2) + std::pow(max_delta.y, 2) + std::pow(max_delta.z, 2));
			emit send_max_error_info(i-1, max_delta, max);
		}
		f_controller->cal_delta(flight_data[i], delta);
		f_controller->cal_ut(delta, delta_sum, delta_last, ut, ut_last, ut_delta);
		f_controller->flightByPosAndYaw(ut_delta, flight_data[i]);

		m_lock.unlock();
		usleep(cycleTimeInMs * 1000);
	}
}