#ifndef _PHYSICS_WORLD_H_
#define _PHYSICS_WORLD_H_

#include <vector>
#include <algorithm>

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

XMALIGN struct PhysicsDynamicCollision
{
	DynamicBody* bodyA;
	DynamicBody* bodyB;
	XMVECTOR collisionNormal;
	float penetrationDepth;

	PhysicsDynamicCollision(DynamicBody* mBodyA, DynamicBody* mBodyB)
	{
		bodyA = mBodyA;
		bodyB = mBodyB;
		collisionNormal = XMVectorSet(0, 0, 0, 0);
		penetrationDepth = 0;
	}

	XMNEW;
	XMDELETE;
};


//REFERENCE NOTE : FROM GAMEDEVTUTS.COM
struct BodyPair
{
	DynamicBody* bodyA;
	DynamicBody* bodyB;
};

class PhysicsWorld
{
public:
	PhysicsWorld();
	PhysicsWorld(HeightMap* mHeightMap);

	~PhysicsWorld();

	void AddBody(DynamicBody* body);
	void RemoveBody(DynamicBody* body);

	void UpdateWorld();

private:

	void HandleStaticCollision();
	void HandleDynamicCollision();
	void GeneratePairs();

	void PositionalCorrection(PhysicsDynamicCollision* collisionPair);

	bool CircleVsCircle(PhysicsDynamicCollision* collisionPair);

private:

	std::vector<DynamicBody*> m_dynamicBodyList;

	std::vector<PhysicsStaticCollision> m_staticCollisionList;
	std::vector<PhysicsDynamicCollision> m_dynamicCollisionList;

	std::vector<BodyPair> m_dynamicBodyPairs;

	HeightMap* m_pHeightMap;

	const float GRAVITY = -8.0f;
};

#endif

