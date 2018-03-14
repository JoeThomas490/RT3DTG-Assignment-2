#ifndef _PHYSICS_WORLD_H_
#define _PHYSICS_WORLD_H_

#include <vector>

#include "DynamicBody.h"
#include "Application.h"

class PhysicsWorld
{
public:
	PhysicsWorld();
	~PhysicsWorld();

	std::vector<DynamicBody> m_dynamicBodyList;

	HeightMap* m_pHeightmap;
};

#endif

