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

	void initializeParticles(HWND * windowHandle);
	void drawParticles(HDC hdc);
	void updateParticles(int rightWall, int bottomWall, int timeDelta);
	bool particlesCollide(const Particle & p1, const Particle & p2);
	inline bool positionCollides(const double x, const double y, const int radius);
	void handleParticlesCollision(Particle & p1, Particle & p2);
	void handleCollisionWalls(Particle & p1, int width, int height);

private:
	std::vector<Particle> particles;
};

