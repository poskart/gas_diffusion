#pragma once

#include "stdafx.h"

class Particle
{
public:
	Particle() : radius(25){}
	Particle(double xPos, double yPos, double xVel,
		double yVel, int rad = partDefaultRadius) : x(xPos), y(yPos), vx(xVel), vy(yVel), radius(rad) {}
	~Particle() {}
		
	double getXPos() const;
	double getYPos() const;
	double getXVel() const;
	double getYVel() const;
	int getRadius() const;
	void setXPos(double xPos);
	void setYPos(double yPos);
	void setXVel(double xVel);
	void setYVel(double yVel);
	void updatePos(int deltaT);

	static const int partDefaultRadius = 25;

private:
	double x;
	double y;
	double vx;
	double vy;

	int radius;
};

