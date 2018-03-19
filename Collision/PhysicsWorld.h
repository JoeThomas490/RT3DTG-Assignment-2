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

typedef float Point[3];

struct AABB
{
	Point minPoint;
	Point maxPoint;

	DynamicBody* body;

	AABB(XMFLOAT3 mMin, XMFLOAT3 mMax, DynamicBody* mBody)
	{
		minPoint[0] = mMin.x;
		minPoint[1] = mMin.y;
		minPoint[2] = mMin.z;

		maxPoint[0] = mMax.x;
		maxPoint[1] = mMax.y;
		maxPoint[2] = mMax.z;

		body = mBody;
	}

	AABB(XMVECTOR centrePos, float radius, DynamicBody* mBody)
	{
		minPoint[0] = XMVectorGetX(centrePos) - radius;
		minPoint[1] = XMVectorGetY(centrePos) - radius;
		minPoint[2] = XMVectorGetZ(centrePos) - radius;

		maxPoint[0] = XMVectorGetX(centrePos) + radius;
		maxPoint[1] = XMVectorGetY(centrePos) + radius;
		maxPoint[2] = XMVectorGetZ(centrePos) + radius;

		body = mBody;
	}

	void UpdatePosition(XMVECTOR centrePos, float radius)
	{
		minPoint[0] = XMVectorGetX(centrePos) - radius;
		minPoint[1] = XMVectorGetY(centrePos) - radius;
		minPoint[2] = XMVectorGetZ(centrePos) - radius;

		maxPoint[0] = XMVectorGetX(centrePos) + radius;
		maxPoint[1] = XMVectorGetY(centrePos) + radius;
		maxPoint[2] = XMVectorGetZ(centrePos) + radius;
	}
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

	void SetHeightMapPtr(HeightMap* pHeightMap);

	void AddBody(DynamicBody* body);
	void RemoveBody(DynamicBody* body);

	void UpdateWorld();

private:

	void HandleStaticCollision();
	void HandleDynamicCollision();
	void GeneratePairs();

	void PositionalCorrection(PhysicsDynamicCollision* collisionPair);

	bool CircleVsCircle(PhysicsDynamicCollision* collisionPair);

	int AABBvsAABB(AABB* a, AABB* b);

	void UpdateAABBs();
	void SortAndSweepAABBArray();

private:

	std::vector<DynamicBody*> m_dynamicBodyList;

	std::vector<PhysicsStaticCollision> m_staticCollisionList;
	std::vector<PhysicsDynamicCollision> m_dynamicCollisionList;

	std::vector<BodyPair> m_dynamicBodyPairs;

	HeightMap* m_pHeightMap;

	int m_sortingAxis = 2;

	int m_AABBArrayIndx = 0;

	DynamicBody *m_dynamicBodyArray[MAX_OBJECTS];
	AABB *m_AABBArray[MAX_OBJECTS];
};

#endif

