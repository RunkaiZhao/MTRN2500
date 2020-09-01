
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

#define PI 3.14159265358979323846

VehicleModel vehiclemodel;

//std::vector<Shape *> shapes;
MyVehicle::MyVehicle()
{	
			ShapeInit section;
			section.type = RECTANGULAR_PRISM;
			section.params.rect.xlen = 3.7;
			section.params.rect.ylen = 0.3;
			section.params.rect.zlen = 1.8;
			section.xyz[0] = 0;
			section.xyz[1] = 0.4;
			section.xyz[2] = 1;
			section.rotation = 0;
			section.rgb[0] = 0.35;
			section.rgb[1] = 0.35;
			section.rgb[2] = 0.35;
			vehiclemodel.shapes.push_back(section);
		
			section.type = RECTANGULAR_PRISM;
			section.params.rect.xlen = 2.4;
			section.params.rect.ylen = 1.4;
			section.params.rect.zlen = 1.8;
			section.xyz[0] = -0.65;
			section.xyz[1] = 0.7;
			section.xyz[2] = 1;
			section.rotation = 0;
			section.rgb[0] = 0.35;
			section.rgb[1] = 0.35;
			section.rgb[2] = 0.35;
			vehiclemodel.shapes.push_back(section);

			/*section.type = TRAPEZOIDAL_PRISM;
			section.params.trap.alen = 15;
			section.params.trap.blen = 5;
			section.params.trap.height = 2.5;
			section.params.trap.aoff = 2;
			section.params.trap.depth = 5;
			section.xyz[0] = 0;
			section.xyz[1] = 2.5;
			section.xyz[2] = 0;
			section.rotation = 0;
			section.rgb[0] = 0;
			section.rgb[1] = 0;
			section.rgb[2] = 1;
			vehiclemodel.shapes.push_back(section);*/
			
			//top cylinder
			/*section.type = CYLINDER;
			section.params.cyl.radius = 0.4;
			section.params.cyl.depth = 0.3;
			section.xyz[0] = 0.8;
			section.xyz[1] = 0.2;
			section.xyz[2] = 0;
			section.rotation = 90;
			section.rgb[0] = 0.2;
			section.rgb[1] = 0.2;
			section.rgb[2] = 0.2;
			section.params.cyl.isRolling = false;
			section.params.cyl.isSteering = false;
			vehiclemodel.shapes.push_back(section);*/


			//left back wheel LBackWheel(-5, 0, -2.5 - 0.25, 0, 2, 0.5, 0, 1, 0);
			section.type = CYLINDER;
			section.params.cyl.radius = 0.4;
			section.params.cyl.depth = 0.2;
			section.xyz[0] = -1.2;
			section.xyz[1] = 0.4;
			section.xyz[2] = 0;
			section.rotation = 0;
			section.rgb[0] = 0.2;
			section.rgb[1] = 0.2;
			section.rgb[2] = 0.2;
			section.params.cyl.isRolling = true;
			section.params.cyl.isSteering = false;
			vehiclemodel.shapes.push_back(section);
			
			//left front wheel LFrontWheel(5, 0, -2.5 - 0.25, 0, 2, 0.5, 0, 1, 0);
			section.type = CYLINDER;
			section.params.cyl.radius = 0.4;
			section.params.cyl.depth = 0.2;
			section.xyz[0] = 0.9;
			section.xyz[1] = 0.4;
			section.xyz[2] = 0;
			section.rotation = 0;
			section.rgb[0] = 0.2;
			section.rgb[1] = 0.2;
			section.rgb[2] = 0.2;
			section.params.cyl.isRolling = true;
			section.params.cyl.isSteering = true;
			vehiclemodel.shapes.push_back(section);
			
			//right back wheel RBackWheel(-5, 0, 2.5 + 0.25, 0, 2, 0.5, 0, 1, 0);
			section.type = CYLINDER;
			section.params.cyl.radius = 0.4;
			section.params.cyl.depth = 0.2;
			section.xyz[0] = -1.2;
			section.xyz[1] = 0.4;
			section.xyz[2] = 1.8;
			section.rotation = 0;
			section.rgb[0] = 0.2;
			section.rgb[1] = 0.2;
			section.rgb[2] = 0.2;
			section.params.cyl.isRolling = true;
			section.params.cyl.isSteering = false;
			vehiclemodel.shapes.push_back(section);


			//right front wheel RFrontWheel(5, 0, 2.5 + 0.25, 0, 2, 0.5, 0, 1, 0);
			section.type = CYLINDER;
			section.params.cyl.radius = 0.4;
			section.params.cyl.depth = 0.2;
			section.xyz[0] = 0.9;
			section.xyz[1] = 0.4;
			section.xyz[2] = 1.8;
			section.rotation = 0;
			section.rgb[0] = 0.2;
			section.rgb[1] = 0.2;
			section.rgb[2] = 0.2;
			section.params.cyl.isRolling = true;
			section.params.cyl.isSteering = true;
			vehiclemodel.shapes.push_back(section);

			/*//FBumper(8.25, 1, 0, 0, 1.5, 1.5, 90, 5, 1, 1, 1)
			section.type = TRIANGULAR_PRISM;
			section.params.tri.alen = 1.5;
			section.params.tri.blen = 1.5;
			section.params.tri.angle = 90;
			section.params.tri.depth = 5;
			section.xyz[0] = 8.25;
			section.xyz[1] = 1;
			section.xyz[2] = 0;
			section.rotation = 0;
			section.rgb[0] = 1;
			section.rgb[1] = 1;
			section.rgb[2] = 1;
			vehiclemodel.shapes.push_back(section);
			*/
			//here
			fullshape();
}

MyVehicle::MyVehicle(VehicleModel vehiclemodel)
{
	this->vehiclemodel = vehiclemodel;

	//here
	fullshape();
}

double MyVehicle::getTime()
{
	return time;
}

void MyVehicle::setTime(double speed, double speed_)
{
	time = speed / (speed_ * 4);
}

void MyVehicle::fullshape()
{
		for (int j = 0; j < vehiclemodel.shapes.size(); j++)
		{
			//glPushMatrix();
			//positionInGL();
			
			if (vehiclemodel.shapes[j].type == RECTANGULAR_PRISM)
			{
				float x = vehiclemodel.shapes[j].xyz[0];
				float y = vehiclemodel.shapes[j].xyz[1];
				float z = vehiclemodel.shapes[j].xyz[2];
				float rotation = vehiclemodel.shapes[j].rotation;
				float red = vehiclemodel.shapes[j].rgb[0];
				float green = vehiclemodel.shapes[j].rgb[1];
				float blue = vehiclemodel.shapes[j].rgb[2];
				float xLength = vehiclemodel.shapes[j].params.rect.xlen;
				float yLength = vehiclemodel.shapes[j].params.rect.ylen;
				float zLength = vehiclemodel.shapes[j].params.rect.zlen;
				Rectangular* ServerRect = new Rectangular(x, y, z, rotation, xLength, yLength, zLength, red, green, blue);
				//vehiclemodel.shapes.push_back(ServerRect);
				addShape(ServerRect);
			}
			else if (vehiclemodel.shapes[j].type == TRAPEZOIDAL_PRISM)
			{
				float x = vehiclemodel.shapes[j].xyz[0];
				float y = vehiclemodel.shapes[j].xyz[1];
				float z = vehiclemodel.shapes[j].xyz[2];
				float rotation = vehiclemodel.shapes[j].rotation;
				float red = vehiclemodel.shapes[j].rgb[0];
				float green = vehiclemodel.shapes[j].rgb[1];
				float blue = vehiclemodel.shapes[j].rgb[2];
				float TrapA = vehiclemodel.shapes[j].params.trap.alen;
				float TrapB = vehiclemodel.shapes[j].params.trap.blen;
				float TrapAoff = vehiclemodel.shapes[j].params.trap.aoff;
				float TrapHeight = vehiclemodel.shapes[j].params.trap.height;
				float TrapDepth = vehiclemodel.shapes[j].params.trap.depth;
				Trapezium * ServerTrap = new Trapezium(x, y, z, rotation, TrapA, TrapB, TrapAoff, TrapHeight, TrapDepth, red, green, blue);
				addShape(ServerTrap);
			}
			else if (vehiclemodel.shapes[j].type == CYLINDER)
			{
				float x = vehiclemodel.shapes[j].xyz[0];
				float y = vehiclemodel.shapes[j].xyz[1];
				float z = vehiclemodel.shapes[j].xyz[2];
				float rotation = vehiclemodel.shapes[j].rotation;
				float red = vehiclemodel.shapes[j].rgb[0];
				float green = vehiclemodel.shapes[j].rgb[1];
				float blue = vehiclemodel.shapes[j].rgb[2];
				float CylRadius = vehiclemodel.shapes[j].params.cyl.radius;
				float CylDepth = vehiclemodel.shapes[j].params.cyl.depth;
				bool isRolling = vehiclemodel.shapes[j].params.cyl.isRolling;
				bool isSteering = vehiclemodel.shapes[j].params.cyl.isSteering;
				Cylinder* ServerCyl = new Cylinder(x, y, z, rotation, CylRadius, CylDepth, red, green, blue, isRolling, isSteering);
				addShape(ServerCyl);
			}

			else if (vehiclemodel.shapes[j].type == TRIANGULAR_PRISM)
			{
				float x = vehiclemodel.shapes[j].xyz[0];
				float y = vehiclemodel.shapes[j].xyz[1];
				float z = vehiclemodel.shapes[j].xyz[2];
				float rotation = vehiclemodel.shapes[j].rotation;
				float red = vehiclemodel.shapes[j].rgb[0];
				float green = vehiclemodel.shapes[j].rgb[1];
				float blue = vehiclemodel.shapes[j].rgb[2];
				float TriA = vehiclemodel.shapes[j].params.tri.alen;
				float TriB = vehiclemodel.shapes[j].params.tri.blen;
				float TriAngle = vehiclemodel.shapes[j].params.tri.angle;
				float TriDepth = vehiclemodel.shapes[j].params.tri.depth;
				Triangle * ServerTri = new Triangle(x, y, z, rotation, TriA, TriB, TriAngle, TriDepth, red, green, blue);
				addShape(ServerTri);
			}
		}

}

void MyVehicle::draw()
{	
	
	for (std::vector<Shape*>::iterator it = shapes.begin(); it != shapes.end();it++)
	{	
		glPushMatrix();
		positionInGL();
		Cylinder * wheel = dynamic_cast<Cylinder*>(*it);

		if (wheel != nullptr) {
			double radius = wheel->GetRadius();
			double angle = wheel->getRotation();
			angle += (speed*time) / (radius) * 180 / PI;
			if (fabs(angle) > 360) {
				angle = 0;
			}
			wheel->Setrolling(angle);

			if (wheel->Getsteering() == 1) {
				wheel->Setsteering(steering);
				wheel->draw();
			}
			else {
				wheel->draw();
			}
		}
		else {
			(*it)->draw();
		}
		glPopMatrix();
	}
	

}

/*void MyVehicle::draw_remote()
{
	glPushMatrix();
	positionInGL();
	for (std::vector<Shape*>::iterator it1 = remote_shape.begin(); it1 != remote_shape.end();it1++)
	{
		(*it1)->draw();
	}
	glPopMatrix();
}*/

