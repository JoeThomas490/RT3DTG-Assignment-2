#include "PhysicsWorld.h"
#include "HeightMap.h"




PhysicsWorld::PhysicsWorld()
{
	m_pHeightMap = nullptr;
}

PhysicsWorld::PhysicsWorld(HeightMap * mHeightMap)
{
	m_pHeightMap = mHeightMap;
}


PhysicsWorld::~PhysicsWorld()
{
	m_pHeightMap = nullptr;
}

void PhysicsWorld::AddBody(DynamicBody * body)
{
	m_dynamicBodyList.push_back(body);
}

void PhysicsWorld::UpdateWorld()
{
	HandleStaticCollision();

	m_staticCollisionList.clear();
}

void PhysicsWorld::HandleStaticCollision()
{
	if (m_pHeightMap != nullptr)
	{
		m_pHeightMap->ResetVertexColours();

		//Loop through all bodies and check collision with heightmap
		for (auto body : m_dynamicBodyList)
		{
			std::vector<PhysicsStaticCollision>	bodyCollisionList = m_pHeightMap->SphereHeightmap(body);
			m_staticCollisionList.insert(m_staticCollisionList.end(), bodyCollisionList.begin(), bodyCollisionList.end());
		}

		for (auto collision : m_staticCollisionList)
		{
			collision.body->ResolveCollision(collision.collisionPosition, collision.collisionNormal);
		}

		for(auto collision : m_staticCollisionList)
		{
			collision.body->PositionalCorrection(collision.penetrationDepth, collision.collisionNormal);
		}

		for (auto body : m_dynamicBodyList)
		{
			//Apply gravity
			body->ApplyForce(XMVectorSet(0, GRAVITY, 0, 0));
			body->IntegratePosition();
		}

		m_staticCollisionList.clear();

		m_pHeightMap->RebuildVertexData();
	}
}
