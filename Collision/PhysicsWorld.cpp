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

void PhysicsWorld::RemoveBody(DynamicBody* mBody)
{
	for (std::vector<DynamicBody*>::iterator it = m_dynamicBodyList.begin(); it < m_dynamicBodyList.end(); it++)
	{
		if ((*it) == mBody)
		{
			m_dynamicBodyList.erase(it);
			break;
		}
	}
}

void PhysicsWorld::UpdateWorld()
{
	HandleStaticCollision();
	HandleDynamicCollision();

	for (auto body : m_dynamicBodyList)
	{
		if (body->GetActive())
		{
			//Apply gravity
			body->ApplyForce(XMVectorSet(0, GRAVITY, 0, 0));
			body->IntegratePosition();
		}
	}

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
			if (body->GetActive())
			{

				std::vector<PhysicsStaticCollision>	bodyCollisionList = m_pHeightMap->SphereHeightmap(body);
				m_staticCollisionList.insert(m_staticCollisionList.end(), bodyCollisionList.begin(), bodyCollisionList.end());
			}
		}

		for (auto collision : m_staticCollisionList)
		{
			collision.body->ResolveCollision(collision.collisionNormal);
		}

		for (auto collision : m_staticCollisionList)
		{
			collision.body->PositionalCorrectionHeightmap(collision.penetrationDepth, collision.collisionNormal);
		}

		m_staticCollisionList.clear();

		m_pHeightMap->RebuildVertexData();
	}
}

void PhysicsWorld::HandleDynamicCollision()
{
	GeneratePairs();
}

void PhysicsWorld::GeneratePairs()
{

	m_dynamicCollisionList.clear();

	for (auto bodyA : m_dynamicBodyList)
	{
		for (auto bodyB : m_dynamicBodyList)
		{
			if (bodyA == bodyB)
			{
				continue;
			}


			PhysicsDynamicCollision collisionPair(bodyA, bodyB);

			if (CircleVsCircle(&collisionPair))
			{
				m_dynamicCollisionList.push_back(collisionPair);
			}
		}
	}


	for (auto collision : m_dynamicCollisionList)
	{
		collision.bodyA->ResolveCollision(collision.collisionNormal);
		collision.bodyB->ResolveCollision(collision.collisionNormal);
	}

	for (auto collision : m_dynamicCollisionList)
	{
		//std::sort(m_dynamicCollisionList.begin(), m_dynamicCollisionList.end());
		//m_dynamicCollisionList.erase(std::unique(m_dynamicCollisionList.begin(), m_dynamicCollisionList.end()), m_dynamicCollisionList.end());
		PositionalCorrection(&collision);
		/*collision.bodyA->PositionalCorrectionHeightmap(collision.penetrationDepth, collision.collisionNormal);
		collision.bodyB->PositionalCorrectionHeightmap(collision.penetrationDepth, collision.collisionNormal);*/
	}
}

void PhysicsWorld::PositionalCorrection(PhysicsDynamicCollision * collisionPair)
{
	const float percent = 0.01f;
	const float slop = 0.001f;

	XMVECTOR correction = max(collisionPair->penetrationDepth - slop, 0.0f) * percent * collisionPair->collisionNormal;
	collisionPair->bodyA->SetPosition(collisionPair->bodyA->GetPosition() - correction);
	collisionPair->bodyB->SetPosition(collisionPair->bodyB->GetPosition() + correction);
}

bool PhysicsWorld::CircleVsCircle(PhysicsDynamicCollision * collisionPair)
{
	DynamicBody* bodyA = collisionPair->bodyA;
	DynamicBody* bodyB = collisionPair->bodyB;

	XMVECTOR dist = bodyB->GetPosition() - bodyA->GetPosition();

	float r = bodyA->GetRadius() + bodyB->GetRadius();
	r *= r;

	if (XMVectorGetX(XMVector3LengthSq(dist)) > r)
	{
		return false;
	}

	//Circles have collided
	float d = XMVectorGetX(XMVector3Length(dist));

	if (d != 0)
	{
		collisionPair->penetrationDepth = r - d;
		collisionPair->collisionNormal = dist / d;

		return true;
	}
	//Circles are on the same position
	else
	{
		collisionPair->penetrationDepth = bodyA->GetRadius();
		collisionPair->collisionNormal = XMVectorSet(1, 0, 0, 0);
		return true;
	}
}
