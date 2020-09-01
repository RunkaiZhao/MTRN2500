
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <map>

#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
	#include <unistd.h>
	#include <sys/time.h>
#elif defined(WIN32)
	#include <Windows.h>
	#include <tchar.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
	#include <unistd.h>
	#include <sys/time.h>
#endif


#include "Camera.hpp"
#include "Ground.hpp"
#include "KeyManager.hpp"

#include "Shape.hpp"
#include "Vehicle.hpp"

#include "RemoteDataManager.hpp"
#include "Messages.hpp"
#include "HUD.hpp"
#include "ObstacleManager.hpp"
#include "MyVehicle.h"
#include "Rectangular.h"
#include "Triangle.h"
#include "Trapezium.h"
#include "Cylinder.h"
#include "remote_vehicle.h"

void display();
void reshape(int width, int height);
void idle();

void keydown(unsigned char key, int x, int y);
void keyup(unsigned char key, int x, int y);
void special_keydown(int keycode, int x, int y);
void special_keyup(int keycode, int x, int y);

void mouse(int button, int state, int x, int y);
void dragged(int x, int y);
void motion(int x, int y);

using namespace std;
using namespace scos;

// Used to store the previous mouse location so we
//   can calculate relative mouse movement.
int prev_mouse_x = -1;
int prev_mouse_y = -1;

// vehicle control related variables
Vehicle * vehicle = NULL;
double speed = 0;
double steering = 0;

// default goal location
std::deque<GoalState> goals;

std::map<int, Vehicle *> otherVehicles;

int frameCounter = 0;

//int _tmain(int argc, _TCHAR* argv[]) {
int main(int argc, char ** argv) {

	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	glutInit(&argc, (char**)(argv));
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("MTRN3500 - GL");

	Camera::get()->setWindowDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);

	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	glutKeyboardFunc(keydown);
	glutKeyboardUpFunc(keyup);
	glutSpecialFunc(special_keydown);
	glutSpecialUpFunc(special_keyup);

	glutMouseFunc(mouse);
	glutMotionFunc(dragged);
	glutPassiveMotionFunc(motion);

	// -------------------------------------------------------------------------
	// Please uncomment the following line of code and replace 'MyVehicle'
	//   with the name of the class you want to show as the current 
	//   custom vehicle.
	// -------------------------------------------------------------------------

	vehicle = new MyVehicle();


	// add test obstacles
	ObstacleManager::get()->addObstacle(Obstacle(10,10, 1));
	ObstacleManager::get()->addObstacle(Obstacle(10,-10, 1));
	ObstacleManager::get()->addObstacle(Obstacle(-10,10, 1));
	ObstacleManager::get()->addObstacle(Obstacle(-10,-10, 1));

	// add test goal
	GoalState g;
	g.x = 25;
	g.z = 0;
	goals.push_back(g);


	glutMainLoop();

	if (vehicle != NULL) {
		delete vehicle;
	}

	return 0;
}

void drawGoals()
{
	for (int i = 0; i < goals.size(); ++i) {
		GoalState goal = goals[i];

		glPushMatrix();
		glTranslatef(goal.x, 0, goal.z);
		glRotated(90, -1, 0, 0);
		static GLUquadric * quad = gluNewQuadric();

		// make first goal purple
		if (i == 0)
			glColor3f(1, .3, 1);
		else
			glColor3f(1, 1, 1);

		gluCylinder(quad, .5, .5, 10, 5, 1);
		glPopMatrix();
	}
}

void display() {
	frameCounter++;
	// -------------------------------------------------------------------------
	//  This method is the main draw routine. 
	// -------------------------------------------------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(Camera::get()->isPursuitMode() && vehicle != NULL) {
		double x = vehicle->getX(), y = vehicle->getY(), z = vehicle->getZ();
		double dx = cos(vehicle->getRotation() * 3.141592765 / 180.0);
		double dy = sin(vehicle->getRotation() * 3.141592765 / 180.0);
		Camera::get()->setDestPos(x + (-20 * dx), y + 15, z + (-20 * dy));
		Camera::get()->setDestDir(dx, -0.25, dy);
	}
	Camera::get()->updateLocation();
	Camera::get()->setLookAt();

	Ground::draw();
	
	// draw other vehicles
	for(std::map<int, Vehicle *>::iterator iter = otherVehicles.begin(); iter != otherVehicles.end(); ++iter) 
		iter->second->draw();

	// draw my vehicle
	if (vehicle != NULL) {
		vehicle->draw();

	}

	// draw obstacles
	ObstacleManager::get()->drawAll();

	// draw goals
	drawGoals();

	// draw HUD
	HUD::Draw();

	glutSwapBuffers();
};

void reshape(int width, int height) {

	Camera::get()->setWindowDimensions(width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
};

void remoteDriver(Vehicle * vehicle, double x, double z, double r, double speed_, double steering_)
{
	// filter 
	const double kSmoothingFactor = 0.5;
	vehicle->setX(vehicle->getX() * kSmoothingFactor + x * (1 - kSmoothingFactor));
	vehicle->setZ(vehicle->getZ() * kSmoothingFactor + z * (1 - kSmoothingFactor));

	vehicle->setRotation(r);
	vehicle->steering = steering_;
	vehicle->speed = speed_;
}

double getTime()
{
#if defined(WIN32)
	LARGE_INTEGER freqli;
	LARGE_INTEGER li;
	if(QueryPerformanceCounter(&li) && QueryPerformanceFrequency(&freqli)) {
		return double(li.QuadPart) / double(freqli.QuadPart);
	}
	else {
		static ULONGLONG start = GetTickCount64();
		return (GetTickCount64() - start) / 1000.0;
	}
#else
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + (t.tv_usec / 1000000.0);
#endif
}

void idle() {

	if (KeyManager::get()->isAsciiKeyPressed('a')) {
		Camera::get()->strafeLeft();
	}

	if (KeyManager::get()->isAsciiKeyPressed('c')) {
		Camera::get()->strafeDown();
	}

	if (KeyManager::get()->isAsciiKeyPressed('d')) {
		Camera::get()->strafeRight();
	}

	if (KeyManager::get()->isAsciiKeyPressed('s')) {
		Camera::get()->moveBackward();
	}

	if (KeyManager::get()->isAsciiKeyPressed('w')) {
		Camera::get()->moveForward();
	}

	if (KeyManager::get()->isAsciiKeyPressed(' ')) {
		Camera::get()->strafeUp();
	}

	speed = 0;
	steering = 0;

	if (KeyManager::get()->isSpecialKeyPressed(GLUT_KEY_LEFT)) {
		steering = Vehicle::MAX_LEFT_STEERING_DEGS * -1;   
	}

	if (KeyManager::get()->isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
		steering = Vehicle::MAX_RIGHT_STEERING_DEGS * -1;
	}

	if (KeyManager::get()->isSpecialKeyPressed(GLUT_KEY_UP)) {
		speed = Vehicle::MAX_FORWARD_SPEED_MPS;
	}

	if (KeyManager::get()->isSpecialKeyPressed(GLUT_KEY_DOWN)) {
		speed = Vehicle::MAX_BACKWARD_SPEED_MPS;
	}

	// attempt to do data communications every 4 frames if we've created a local vehicle
	if(frameCounter % 4 == 0 && vehicle != NULL) {

		// if not connected, attempt to connect every 2 seconds
		if(!RemoteDataManager::IsConnected()) {
			if(frameCounter % 120 == 0) {
		
				// erase other vehicles
				for(std::map<int, Vehicle*>::iterator iter = otherVehicles.begin(); iter  != otherVehicles.end(); ++iter) {
					delete iter->second;
				}
				otherVehicles.clear();

				// uncomment this line to connect to the robotics server.
				RemoteDataManager::Connect("www.robotics.unsw.edu.au","18081");

				// on connect, let's tell the server what we look like
				if (RemoteDataManager::IsConnected()) {
					ObstacleManager::get()->removeAll();

					VehicleModel vm;
					vm.remoteID = 0;

					//
					// student code goes here
					//
					/*ShapeInit section;
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
					vm.shapes.push_back(section);

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
					vm.shapes.push_back(section);

					section.type = TRAPEZOIDAL_PRISM;
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
					vehiclemodel.shapes.push_back(section);
					

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
					vm.shapes.push_back(section);

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
					vm.shapes.push_back(section);

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
					vm.shapes.push_back(section);


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
					vm.shapes.push_back(section);*/

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
					vm.shapes.push_back(section);*/
					
					RemoteDataManager::Write(GetVehicleModelStr(vm));
				}
			}
		}

		// if we're connected, send our vehicle state to the server
		if (RemoteDataManager::IsConnected()) {
			VehicleState vs;
			vs.remoteID = 0;
			vs.x = vehicle->getX();
			vs.z = vehicle->getZ();
			vs.rotation = vehicle->getRotation();
			vs.speed = vehicle->getSpeed();
			vs.steering = vehicle->getSteering();
			RemoteDataManager::Write(GetVehicleStateStr(vs));
		}

		// if we're still connected, receive and handle response messages from the server
		if (RemoteDataManager::IsConnected()) {
			std::vector<RemoteMessage> msgs = RemoteDataManager::Read();
			for(unsigned int i = 0; i < msgs.size(); i++ ) {

				RemoteMessage msg = msgs[i];
				//cout << msg.payload << endl;

				switch(msg.type) {
					// new models
					case 'M':
						{
							std::vector<VehicleModel> models = GetVehicleModels(msg.payload);
							for(unsigned int i = 0; i < models.size(); i++) {
								VehicleModel vm = models[i];
								
								// uncomment the line below to create remote vehicles
								otherVehicles[vm.remoteID] = new remote_vehicle();

								//
								// more student code goes here
								//
								//left back wheel LBackWheel(-5, 0, -2.5 - 0.25, 0, 2, 0.5, 0, 1, 0);
								
								/*for (int j = 0; j < vm.shapes.size(); j++)
								{
									if (vm.shapes[j].type == RECTANGULAR_PRISM)
									{
										float x = vm.shapes[j].xyz[0];
										float y = vm.shapes[j].xyz[1];
										float z = vm.shapes[j].xyz[2];
										float rotation = vm.shapes[j].rotation;
										float red = vm.shapes[j].rgb[0];
										float green = vm.shapes[j].rgb[1];
										float blue = vm.shapes[j].rgb[2];
										float xLength = vm.shapes[j].params.rect.xlen;
										float yLength = vm.shapes[j].params.rect.ylen;
										float zLength = vm.shapes[j].params.rect.zlen;
										Rectangular* ServerRect = new Rectangular(x, y, z, rotation, xLength, yLength, zLength, red, green, blue);
										otherVehicles[vm.remoteID]->add_remote_shape(ServerRect);
									}
									else if (vm.shapes[j].type == TRAPEZOIDAL_PRISM)
									{
										float x = vm.shapes[j].xyz[0];
										float y = vm.shapes[j].xyz[1];
										float z = vm.shapes[j].xyz[2];
										float rotation = vm.shapes[j].rotation;
										float red = vm.shapes[j].rgb[0];
										float green = vm.shapes[j].rgb[1];
										float blue = vm.shapes[j].rgb[2];
										float TrapA = vm.shapes[j].params.trap.alen;
										float TrapB = vm.shapes[j].params.trap.blen;
										float TrapAoff = vm.shapes[j].params.trap.aoff;
										float TrapHeight = vm.shapes[j].params.trap.height;
										float TrapDepth = vm.shapes[j].params.trap.depth;
										Trapezium * ServerTrap = new Trapezium(x, y, z, rotation, TrapA, TrapB, TrapAoff, TrapHeight, TrapDepth, red, green, blue);
										otherVehicles[vm.remoteID]->add_remote_shape(ServerTrap);
									}
									else if (vm.shapes[j].type == CYLINDER)
									{
										float x = vm.shapes[j].xyz[0];
										float y = vm.shapes[j].xyz[1];
										float z = vm.shapes[j].xyz[2];
										float rotation = vm.shapes[j].rotation;
										float red = vm.shapes[j].rgb[0];
										float green = vm.shapes[j].rgb[1];
										float blue = vm.shapes[j].rgb[2];
										float CylRadius = vm.shapes[j].params.cyl.radius;
										float CylDepth = vm.shapes[j].params.cyl.depth;
										bool isRolling = vm.shapes[j].params.cyl.isRolling;
										bool isSteering = vm.shapes[j].params.cyl.isSteering;
										Cylinder* ServerCyl = new Cylinder(x, y, z, rotation, CylRadius, CylDepth, red, green, blue, isRolling, isSteering);
										otherVehicles[vm.remoteID]->add_remote_shape(ServerCyl);
									}
									else if (vm.shapes[j].type == TRIANGULAR_PRISM)
									{
										float x = vm.shapes[j].xyz[0];
										float y = vm.shapes[j].xyz[1];
										float z = vm.shapes[j].xyz[2];
										float rotation = vm.shapes[j].rotation;
										float red = vm.shapes[j].rgb[0];
										float green = vm.shapes[j].rgb[1];
										float blue = vm.shapes[j].rgb[2];
										float TriA = vm.shapes[j].params.tri.alen;
										float TriB = vm.shapes[j].params.tri.blen;
										float TriAngle = vm.shapes[j].params.tri.angle;
										float TriDepth = vm.shapes[j].params.tri.depth;
										Triangle * ServerTri = new Triangle(x, y, z, rotation, TriA, TriB, TriAngle, TriDepth, red, green, blue);
										otherVehicles[vm.remoteID]->add_remote_shape(ServerTri);
									}
								}*/

								/*for (int z = 0; z < vm.shapes.size(); z++) {
									if (vm.shapes[z].type = RECTANGULAR_PRISM) {
										float x = vm.shapes[z].xyz[0];
										float y = vm.shapes[z].xyz[1];
										float z = vm.shapes[z].xyz[2];
										float rotation = vm.shapes[z].rotation;
										float red = vm.shapes[z].rgb[0];
										float green = vm.shapes[z].rgb[1];
										float blue = vm.shapes[z].rgb[2];
										float xlen = vm.shapes[z].params.rect.xlen;
										float ylen = vm.shapes[z].params.rect.ylen;
										float zlen = vm.shapes[z].params.rect.zlen;
										Rectangular * rect = new Rectangular(x, y, z, rotation, xlen, ylen, zlen, red, green, blue);
										vm.shapes.push_back(rect);
									}
								}*/


							}
							break;
						}

					// vehicle states
					case 'S': 
						{
							std::vector<VehicleState> states = GetVehicleStates(msg.payload);
							for(unsigned int i = 0; i < states.size(); i++) {
								VehicleState vs = states[i];

								std::map<int, Vehicle*>::iterator iter = otherVehicles.find(vs.remoteID);
								if(iter != otherVehicles.end()) {
									Vehicle * veh = iter->second;
									remoteDriver(veh, vs.x, vs.z, vs.rotation, vs.speed, vs.steering);
								}
							}
							break;
						}

					// goal state
					case 'G':
						{
							goals = GetGoals(msg.payload);
							break;
						}

					// obstacle state
					case 'O':
						{
							std::vector<ObstacleState> obs = GetObstacles(msg.payload);
							for(unsigned int i = 0; i < obs.size(); i++) {
								Obstacle o(obs[i].x, obs[i].z, obs[i].radius);
								ObstacleManager::get()->addObstacle(o);
							}
							break;
						}

					// disconnect list
					case 'D':
						{
							std::vector<int> disconnectedIDs = GetVehicleDisconnects(msg.payload);
							for(unsigned int i = 0; i < disconnectedIDs.size(); i++) {
								int id = disconnectedIDs[i];
								std::map<int, Vehicle*>::iterator iter = otherVehicles.find(id);
								if(iter != otherVehicles.end()) {
									delete iter->second;
									otherVehicles.erase(iter);
								}
							}
							break;
						}

					// error message
					case 'E':
						{
							cerr << "Server error: " << msg.payload << endl;
							break;
						}

				}
			} 
		}
	}


	const float sleep_time_between_frames_in_seconds = 0.025;

	static double previousTime = getTime();
	const double currTime = getTime();
	const double elapsedTime = currTime - previousTime;
	previousTime = currTime;

	// do a simulation step
	if (vehicle != NULL) {
		vehicle->update(speed, steering, elapsedTime);
	}
	for(std::map<int, Vehicle*>::iterator iter = otherVehicles.begin(); iter  != otherVehicles.end(); ++iter) {
		iter->second->update(elapsedTime);
	}

	display();

#ifdef _WIN32 
	Sleep(sleep_time_between_frames_in_seconds * 1000);
#else
	usleep(sleep_time_between_frames_in_seconds * 1e6);
#endif
};

void keydown(unsigned char key, int x, int y) {

	// keys that will be held down for extended periods of time will be handled
	//   in the idle function
	KeyManager::get()->asciiKeyPressed(key);

	// keys that react ocne when pressed rather than need to be held down
	//   can be handles normally, like this...
	switch (key) {
	case 27: // ESC key
		exit(0);
		break;      
	case '0':
		Camera::get()->jumpToOrigin();
		break;
	case 'p':
		Camera::get()->togglePursuitMode();
		break;
	}

};

void keyup(unsigned char key, int x, int y) {
	KeyManager::get()->asciiKeyReleased(key);
};

void special_keydown(int keycode, int x, int y) {

	KeyManager::get()->specialKeyPressed(keycode);

};

void special_keyup(int keycode, int x, int y) {  
	KeyManager::get()->specialKeyReleased(keycode);  
};

void mouse(int button, int state, int x, int y) {

};

void dragged(int x, int y) {

	if (prev_mouse_x >= 0) {

		int dx = x - prev_mouse_x;
		int dy = y - prev_mouse_y;

		Camera::get()->mouseRotateCamera(dx, dy);
	}

	prev_mouse_x = x;
	prev_mouse_y = y;
};

void motion(int x, int y) {

	prev_mouse_x = x;
	prev_mouse_y = y;
};


