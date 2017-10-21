#include "Gas.h"

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

/*
	Initialize Gas with the given number of particles
*/
Gas::Gas(int partCount)
{
	particles.resize(partCount, 
		Particle(0.0, 0.0, 0.0, 0.0, Particle::partDefaultRadius));
}

void Gas::setParticlesCount(int pCount)
{
	particles.resize(pCount,
		Particle(0.0, 0.0, 0.0, 0.0, Particle::partDefaultRadius));
}


/*
	Initialize particles positions and velocity
	to random numbers.
*/
void Gas::initializeParticles(HWND * windowHandle)
{
	int randX, randY;
	srand(time(NULL));

	HDC hdc = GetDC(*windowHandle);
	SetMapMode(hdc, MM_LOMETRIC);

	RECT lpRect;
	GetClientRect(*windowHandle, &lpRect);
	DPtoLP(hdc, (LPPOINT)& lpRect, 2);

	for(auto & p : particles)
	{
		// Choose position which does not collide with the other part.
		do
		{
			randX = double(rand() % 
				(lpRect.right - 2 * Particle::partDefaultRadius));
			randY = -double(rand() % 
				(lpRect.bottom - 2 * Particle::partDefaultRadius)) 
				- 2 * Particle::partDefaultRadius;
		} while (positionCollides(randX, randY, p.getRadius()));
		
		p.setXPos(randX);
		p.setYPos(randY);
		p.setXVel(double((rand() % 100)/200.0 - 0.25));
		p.setYVel(double((rand() % 100) / 200.0 - 0.25));
	}
}

/*
	Draw particles in the given window context
*/
void Gas::drawParticles(HDC hdc)
{
	for(const auto & p : particles)
	{
		Ellipse(hdc, int(p.getXPos()), int(p.getYPos()), 
			int(p.getXPos()) + 2 * p.getRadius(), int(p.getYPos()) + 2 * p.getRadius());
	}
}

/*
	Update particles movement after timeDelta time period.
*/
void Gas::updateParticles(int width, int height, int timeDelta)
{
	Particle * p1 = nullptr, *p2 = nullptr;
	for (int i = 0; i < particles.size(); ++i)
	{
		p1 = &particles.data()[i];
		// Check collisions
		for (int j = i + 1; j < particles.size(); ++j)
		{
			p2 = &particles.data()[j];
			// Handle collision
			if (particlesCollide(*p1, *p2))
			{
				handleParticlesCollision(*p1, *p2);
				break;
			}
		}
		handleCollisionWalls(*p1, width, height);
		p1->updatePos(timeDelta);
	}
}

/*
	Check if 2 particles collide
*/
inline bool Gas::particlesCollide(const Particle & p1, const Particle & p2)
{
	return (sqrt(pow(p2.getXPos() - p1.getXPos(), 2.0)
		+ pow(p2.getYPos() - p1.getYPos(), 2.0))
		<= 1.01*(p1.getRadius() + p2.getRadius()));
}

/*
	Check if specific (x, y) position collides with existing particles
*/
inline bool Gas::positionCollides(const double x, const double y, 
	const int radius)
{
	for(const Particle & p : particles)
	{
		if ((sqrt(pow(x - p.getXPos(), 2.0)
			+ pow(y - p.getYPos(), 2.0))
			<= radius + p.getRadius()))
			return true;
	}
	return false;
}

void Gas::handleParticlesCollision(Particle & p1, Particle & p2)
{
	/*
		Collision handling for elastic collision according to
		https://en.wikipedia.org/wiki/Elastic_collision
		(last equations in the article)
	*/
	double dotProd = (p2.getXVel() - p1.getXVel())*(p2.getXPos() - p1.getXPos()) 
		+ (p2.getYVel() - p1.getYVel())*(p2.getYPos() - p1.getYPos());
	double norm = p1.getRadius() + p2.getRadius();
	p1.setXVel(p1.getXVel() - dotProd/norm/norm*(p1.getXPos() - p2.getXPos()));
	p1.setYVel(p1.getYVel() - dotProd/norm/norm*(p1.getYPos() - p2.getYPos()));

	p2.setXVel(p2.getXVel() - dotProd / norm / norm*(p2.getXPos() - p1.getXPos()));
	p2.setYVel(p2.getYVel() - dotProd / norm / norm*(p2.getYPos() - p1.getYPos()));
}

void Gas::handleCollisionWalls(Particle & p1, int width, int height)
{
	// Right wall and left wall
	if (p1.getXPos() + 2 * p1.getRadius() >= width)
	{
		// if right wall moved and covered particle (at least half of it)
		if (p1.getXPos() + p1.getRadius() >= width)
			p1.setXPos(width - 2*p1.getRadius() - 1);
		else    //normal bounce off the wall
			p1.setXVel(-p1.getXVel());
	}
	else if (p1.getXPos() <= 0)
	{
		// if left wall moved and covered particle (at least half of it)
		if (p1.getXPos() <= -p1.getRadius())
			p1.setXPos(1);
		else    //normal bounce off the wall
			p1.setXVel(-p1.getXVel());
	}
	// Bottom wall and top wall
	if (p1.getYPos() <= -height)
	{
		// if bottom wall moved and covered particle (at least half of it)
		if (p1.getYPos() <= -height - p1.getRadius())
			p1.setYPos(-height + 1);
		else    //normal bounce off the wall
			p1.setYVel(-p1.getYVel());
	}
	else if (p1.getYPos() >= -2*p1.getRadius())
	{
		// if top wall moved and covered particle (at least half of it)
		if (p1.getYPos() + p1.getRadius() >= 0)
			p1.setYPos(-1.0 - 2*p1.getRadius());
		else	//normal bounce off the wall
			p1.setYVel(-p1.getYVel());
	}
}