#include "Particle.h"



double Particle::getXPos() const
{
	return x;
}

double Particle::getYPos() const
{
	return y;
}

double Particle::getXVel() const
{
	return vx;
}

double Particle::getYVel() const
{
	return vy;
}

int Particle::getRadius() const
{
	return radius;
}

void Particle::setXPos(double xPos)
{
	x = xPos;
}

void Particle::setYPos(double yPos)
{
	y = yPos;
}
void Particle::setXVel(double xVel)
{
	vx = xVel;
}

void Particle::setYVel(double yVel)
{
	vy = yVel;
}

void Particle::updatePos(int deltaT)
{
	x += vx * deltaT;
	y += vy * deltaT;
}
