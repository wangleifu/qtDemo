#include "controller.hpp"

using namespace DJI;
using namespace DJI::OSDK;

Controller::Controller(Vehicle* vehicle, Widget* widget)
{
	this->vehicle = vehicle;
	this->widget  = widget;
	this->print   = true;

	Controller::initPrintMission();
}
Controller::~Controller()
{
	this->print   = false;

	this->vehicle = NULL;
	this->widget  = NULL;
}