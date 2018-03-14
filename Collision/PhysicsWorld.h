#ifndef _PHYSICS_WORLD_H_
#define _PHYSICS_WORLD_H_

#include <vector>

#include "DynamicBody.h"
#include "Application.h"

class HeightMap;

XMALIGN struct PhysicsStaticCollision
{
	DynamicBody* body;
	XMVECTOR collisionNormal;
	XMVECTOR collisionPosition;
	float penetrationDepth;

	PhysicsStaticCollision(DynamicBody* mBody)
	{
		body = mBody;
		collisionPosition = collisionNormal = XMVectorSet(0, 0, 0, 0);
		penetrationDepth = 0;
	}

	XMNEW;
	XMDELETE;
};

class PhysicsWorld
{
public:
	PhysicsWorld();
	PhysicsWorld(HeightMap* mHeightMap);

	~PhysicsWorld();

	void AddBody(DynamicBody* body);
	void UpdateWorld();

private:

	void HandleStaticCollision();


private:

	std::vector<DynamicBody*> m_dynamicBodyList;

	std::vector<PhysicsStaticCollision> m_staticCollisionList;

	HeightMap* m_pHeightMap;
};

#endif

