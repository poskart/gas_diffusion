#pragma once

#include "Particle.h"
#include <vector>
#include <algorithm>

class Gas
{
public:
	Gas() {}
	Gas(int partCount);
	~Gas() {}

	void initializeParticles(RECT * rect);
	void drawParticles(HDC * contextHandle);
	void updateParticles(RECT * rect, int timeDelta);
	bool particlesCollide(const Particle & p1, const Particle & p2);
	inline bool positionCollides(const double x, const double y, const int radius);
	void handleParticlesCollision(Particle & p1, Particle & p2);
	void handleCollisionWalls(Particle & p1, RECT * rect);

private:
	std::vector<Particle> particles;
};

