#ifndef TRACK_THREAD_HPP
#define TRACK_THREAD_HPP

#include <QThread>

class TrackThread : public QThread
{
	Q_OBJECT

public:
	TrackThread(FlightController* f_controller, QObject* par);
	~TrackThread();
	void run();

signals:
	void send_error_info(int id, HUST::Positon fd);
	void send_max_error_info(int id, HUST::Positon fd, float max);

private:
	void tracking();

private:
	bool               isPause;
	QMutex             m_lock;
	FlightController* f_controller;
}

#endif