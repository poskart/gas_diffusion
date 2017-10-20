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

/*
	Initialize particles positions and velocity
	to random numbers.
*/
void Gas::initializeParticles(RECT * rect)
{
	int randX, randY;
	srand(time(NULL));

	for(auto & p : particles)
	{
		// Choose position which does not collide with the other part.
		do
		{
			randX = double(rand() % (rect->right - 2 * Particle::partDefaultRadius));
			randY = double(rand() % (rect->bottom - 2 * Particle::partDefaultRadius));
		} while (positionCollides(randX, randY, p.getRadius()));
		
		p.setXPos(randX);
		p.setYPos(randY);
		p.setXVel(.08);
		p.setYVel(.05);
	}
}

/*
	Draw particles in the given window context
*/
void Gas::drawParticles(HDC * contextHandle)
{
	for(const auto & p : particles)
	{
		Ellipse(*contextHandle, int(p.getXPos()), int(p.getYPos()), 
			int(p.getXPos()) + 2 * p.getRadius(), int(p.getYPos()) + 2 * p.getRadius());
	}
}

/*
	Update particles movement after timeDelta time period.
*/
void Gas::updateParticles(RECT * rect, int timeDelta)
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
		handleCollisionWalls(*p1, rect);
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
	for each (const Particle & p in particles)
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
	double dotProd = (p2.getXVel() - p1.getXVel())*(p2.getXPos() - p1.getXPos()) + (p2.getYVel() - p1.getYVel())*(p2.getYPos() - p1.getYPos());
	double norm = p1.getRadius() + p2.getRadius();
	p1.setXVel(p1.getXVel() - dotProd/norm/norm*(p1.getXPos() - p2.getXPos()));
	p1.setYVel(p1.getYVel() - dotProd/norm/norm*(p1.getYPos() - p2.getYPos()));

	p2.setXVel(p2.getXVel() - dotProd / norm / norm*(p2.getXPos() - p1.getXPos()));
	p2.setYVel(p2.getYVel() - dotProd / norm / norm*(p2.getYPos() - p1.getYPos()));
}

void Gas::handleCollisionWalls(Particle & p1, RECT * rect)
{
	// Right wall and left wall
	if (p1.getXPos() + 2 * p1.getRadius() >= rect->right ||
		p1.getXPos() <= rect->left)
	{
		p1.setXVel(-p1.getXVel());
	}
	// Bottom wall and top wall
	if (p1.getYPos() + 2 * p1.getRadius() >= rect->bottom ||
		p1.getYPos() <= rect->top)
	{
		p1.setYVel(-p1.getYVel());
	}
}