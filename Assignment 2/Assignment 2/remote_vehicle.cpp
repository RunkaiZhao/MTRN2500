#include "remote_vehicle.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#elif defined(WIN32)
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "MyVehicle.h"
#include "Vehicle.hpp"
#include <math.h>
#include "Rectangular.h"
#include "Trapezium.h"
#include "Cylinder.h"
#include "Triangle.h"
#include <vector>
#include "Messages.hpp"


remote_vehicle::remote_vehicle()
{

}

void remote_vehicle::draw()
{
	glPushMatrix();
	positionInGL();
	for (std::vector<Shape*>::iterator it1 = remote_shape.begin(); it1 != remote_shape.end();it1++)
	{
		(*it1)->draw();
	}
	glPopMatrix();
}
