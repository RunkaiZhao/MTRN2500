#pragma once
#include "Shape.hpp"

class Cylinder : public Shape {
protected:
	float Radius;
	float Depth;
	bool isRolling;
	bool isSteering;

public:
	Cylinder();
	Cylinder(float x1, float y1, float z1, float rotation, float radius, float depth, float r, float g,
			float b1, bool isrolling, bool issteering);
	float GetRadius();
	float GetDepth();
	bool Getrolling();
	bool Getsteering();

	void SetRadius(float radius);
	void SetDepth(float depth);
	void Setrolling(float isrolling);
	void Setsteering(float issteering);
	void draw();

};