#pragma once
#include "Vehicle.hpp"
#include "Messages.hpp"


class MyVehicle : public Vehicle {
protected:
	VehicleModel vehiclemodel;
	double time;
public:
	MyVehicle();
	MyVehicle(VehicleModel vehiclemodel);
	double getTime();
	void setTime(double speed, double speed_);
	void fullshape();
	void draw();
	//void draw_remote();
};