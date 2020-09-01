
//#include "Shape.hpp"
#include <math.h>
#define PI 3.14159265358979323846
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
#include "Cylinder.h"

Cylinder::Cylinder()
{

}
Cylinder::Cylinder(float x1, float y1, float z1, float rotation, float radius, float depth, float r,
					float g,float b1, bool isrolling, bool issteering):Shape(x1,y1,z1, rotation)
{
	Radius = radius;
	Depth = depth;
	green = g;
	blue = b1;
	isRolling = isrolling;
	isSteering = issteering;
}
float Cylinder::GetRadius()
{
	return Radius;
}
float Cylinder::GetDepth()
{
	return Depth;
}
bool Cylinder::Getrolling()
{
	return isRolling;
}
bool Cylinder::Getsteering()
{
	return isSteering;
}
void Cylinder::SetRadius(float radius)
{
	Radius = radius;
}
void Cylinder::SetDepth(float depth)
{
	Depth = depth;
}
void Cylinder::Setrolling(float isrolling)
{
	glRotated(isrolling, 0, 0, 0);
}
void Cylinder::Setsteering(float issteering)
{
	rotation = issteering;
}
void Cylinder::draw()
{
	glPushMatrix();
	//glTranslated(0, Radius, -Depth/2);
	setColorInGL();
	positionInGL();
	
	static GLUquadric * disk = gluNewQuadric();
	double BaseRadius = Radius;
	double TopRadius = Radius;
	double height = Depth;
	int slices = 20;
	int stack = 100;
	gluCylinder(disk, BaseRadius, TopRadius, height, slices, stack);

	//Lid

	//glColor3d(1, 1, 1);
	glTranslated(0, 0, height);
	double InnerRadius = 0;
	double OuterRadius = Radius;
	int loops = 1;
	gluDisk(disk, InnerRadius, OuterRadius, slices, loops);
	
	//
	/*glBegin(GL_QUADS);
	//front
	glVertex3f(-Radius / 3, (11 / 5) * Radius, -Depth);
	glVertex3f(Radius / 3, (11 / 5) * Radius, -Depth);
	glVertex3f(Radius / 3, (11 / 6) * Radius, -Depth);
	glVertex3f(-Radius / 3, (11 / 6) * Radius, -Depth);
	glEnd();*/


	glPopMatrix();
}