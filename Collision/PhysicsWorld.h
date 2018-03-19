#ifndef _PHYSICS_WORLD_H_
#define _PHYSICS_WORLD_H_

#include <vector>
#include <algorithm>

#include "DynamicBody.h"
#include "Application.h"

class HeightMap;


//**********************************************************************************
// Struct : PhysicsStaticCollision
// Description : Holds data to do with a static collision (i.e between dyanmic body
// and the heightmap. Holds collision normal, collision position (not used but could
// be useful for the future), and penetration depth
//**********************************************************************************
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

//**********************************************************************************
// Struct : PhysicsDynamicCollision
// Description : Holds data to do with a dynamic collision (i.e between two dyanmic
// bodies. Holds collision normal, collision position, penetration depth, and pointers
// two both bodies involved in the collision
//**********************************************************************************
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

//Typedef a 3 element float array for AABB and Sort + Sweep
typedef float Point[3];

//**********************************************************************************
// Struct : AABB
// Description : Produces a min and max point from a position and radius. Used in 
// the Sort and Sweep broadphase for dynamic collisions.
//**********************************************************************************
struct AABB
{
	//Minimum bounds point
	Point minPoint;

	//Maximum bounds point
	Point maxPoint;

	//Body associated with this AABB boundary
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

	//Updates the position of a bounding box
	//Params : Position of centre of body, radius of body
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

//**********************************************************************************
// Class : PhysicsWorld
// Description : Controls and updates the physics of all bodies within the scene. Also handles
// the broadphase for dyanmic collisions.
//**********************************************************************************
class PhysicsWorld
{
public:

	PhysicsWorld();
	PhysicsWorld(HeightMap* mHeightMap);
	~PhysicsWorld();

	//Sets the pointer of the heightmap to test static collisions against
	//Params : Pointer of the heightmap to be tested against
	void SetHeightMapPtr(HeightMap* pHeightMap);

	//Adds a body to the list of bodies within the physics world
	//Params : Pointer to the body to add
	void AddBody(DynamicBody* body);

	//Removes a body from the physics world
	//Params : Pointer to the body to remove
	void RemoveBody(DynamicBody* body);

	//Controls the update of all bodies within the scene
	//Main function to be called
	void UpdateWorld();

private:

	//Controls the collision between the dynamic bodies
	//and the static heightmap
	void HandleStaticCollision();

	//Controls the collision between all dynamic bodies
	void HandleDynamicCollision();

	//Old function used to generate collision pairs for dynamic collisions
	//Bruteforce method and generally slow
	//void GeneratePairs();

	//Corrects the position of overlapping bodies during a dynamic vs dynamic
	//body collision
	//Params : Collision pair to be tested 
	void PositionalCorrection(PhysicsDynamicCollision* collisionPair);

	//Simple circle vs circle check (Taken from Real Time Collision Detection book)
	//Params : Collision pair to be tested
	//Returns : True if the two bodies are overlapping (colliding)
	bool CircleVsCircle(PhysicsDynamicCollision* collisionPair);

	//Simple AABB vs AABB check (Taken from Real Time Collision Detection book)
	//Params : Pointers to each AABB to check
	//Returns : 1 if two bounding boxes are overlapping, 0 if not
	int AABBvsAABB(AABB* a, AABB* b);

	//Updates all AABBs surrounding each dynamic body
	void UpdateAABBs();

	//Broadphase method used for dynamic collisions (Taken from Real Time Collision Detection book)
	//Sorts bodies on a specified axis and only checks those that are close together
	//Also calculates the variance of each axis and then decides which axis (X, Y, or Z) is best to sort against next
	void SortAndSweepAABBArray();

private:

	//Vector of all bodies within the scene
	std::vector<DynamicBody*> m_dynamicBodyList;

	//Vector of all static collisions to be resolved every frame
	std::vector<PhysicsStaticCollision> m_staticCollisionList;

	//Vector of all dynamic collisions to be resolved every frame
	std::vector<PhysicsDynamicCollision> m_dynamicCollisionList;

	//Pointer to the current heightmap to test against
	HeightMap* m_pHeightMap;

	//Sorting axis used durign the SortAndSweep broadphase method
	int m_sortingAxis = 0;

	//Index used when inserting into the AABB array
	int m_AABBArrayIndx = 0;

	//Array of AABB boundaries for each body
	AABB *m_AABBArray[MAX_OBJECTS];
};

#endif

