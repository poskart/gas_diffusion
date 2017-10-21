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

	void setParticlesCount(int pCount);
	void removeParticle(int index);
	void insertParticle(Particle * newParticle);
	Particle * getParticle(int index);
	void initializeParticles(HWND * windowHandle);
	void drawParticles(HDC hdc);
	std::vector<int> updateParticles(int rightWall, int bottomWall, int appId, bool isCoupled, int timeDelta);
	bool particlesCollide(const Particle & p1, const Particle & p2);
	inline bool positionCollides(const double x, const double y, const int radius);
	void handleParticlesCollision(Particle & p1, Particle & p2);
	bool handleCollisionWalls(Particle & p1, int width, int height, int appId, bool isCoupled);

private:
	std::vector<Particle> particles;
};

