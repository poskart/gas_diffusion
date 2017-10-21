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
	Set number of particles in gas
*/
void Gas::setParticlesCount(int pCount)
{
	particles.resize(pCount,
		Particle(0.0, 0.0, 0.0, 0.0, Particle::partDefaultRadius));
}

void Gas::removeParticle(int index)
{
	auto it = particles.begin();
	it += index;
	particles.erase(it);
}

void Gas::insertParticle(Particle * newParticle)
{
	particles.push_back(*newParticle);
}

Particle * Gas::getParticle(int index)
{
	return &particles.data()[index];
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
std::vector<int> Gas::updateParticles(int width, int height, int appId, bool isCoupled, int timeDelta)
{
	std::vector<int> lostParticles;
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
		if (handleCollisionWalls(*p1, width, height, appId, isCoupled))
		{
			// Particle is out of the window
			lostParticles.push_back(i);
		}
		p1->updatePos(timeDelta);
	}
	return lostParticles;
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

/*
	Change two particles (p1 and p2) velocity according to
	elastic collision
*/
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

/*
	Bounce off the walls of the window and include
	particles in window if window has been resized.
*/
bool Gas::handleCollisionWalls(Particle & p1, int width, int height, int appId, bool isCoupled)
{
	bool particleLost = false;
	// Right wall
	if (p1.getXPos() + 2 * p1.getRadius() >= width)
	{
		// If two apps coupled then check if particle is lost
		if (appId == 1 && isCoupled)
		{
			if (p1.getXPos() + p1.getRadius() >= width)
				particleLost = true;
		}
		else  // bounce of the wall
		{
			// if right wall moved and covered particle (at least half of it)
			if (p1.getXPos() + p1.getRadius() >= width)
				p1.setXPos(width - 2 * p1.getRadius() - 1);
			else if(!isCoupled || appId == 2)   //normal bounce off the wall
				p1.setXVel(-p1.getXVel());
		}
	}
	// Left wall
	else if (p1.getXPos() <= 0)
	{
		// If two apps coupled then check if particle is lost
		if (appId == 2 && isCoupled)
		{
			if (p1.getXPos() <= -p1.getRadius())
				particleLost = true;
		}
		else
		{
			// if left wall moved and covered particle (at least half of it)
			if (p1.getXPos() <= -p1.getRadius())
				p1.setXPos(1);
			else if(!isCoupled || appId == 1)   //normal bounce off the wall
				p1.setXVel(-p1.getXVel());
		}
	}
	// Bottom wall
	if (p1.getYPos() <= -height)
	{
		// if bottom wall moved and covered particle (at least half of it)
		if (p1.getYPos() <= -height - p1.getRadius())
			p1.setYPos(-height + 1);
		else    //normal bounce off the wall
			p1.setYVel(-p1.getYVel());
	}
	// Top wall
	else if (p1.getYPos() >= -2*p1.getRadius())
	{
		// if top wall moved and covered particle (at least half of it)
		if (p1.getYPos() + p1.getRadius() >= 0)
			p1.setYPos(-1.0 - 2*p1.getRadius());
		else	//normal bounce off the wall
			p1.setYVel(-p1.getYVel());
	}
	return particleLost;
}