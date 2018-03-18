#include "PhysicsWorld.h"
#include "HeightMap.h"


PhysicsWorld::PhysicsWorld()
{
	m_pHeightMap = nullptr;

	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		m_AABBArray[i] = nullptr;
	}
}

PhysicsWorld::PhysicsWorld(HeightMap * mHeightMap)
{
	m_pHeightMap = mHeightMap;
}


PhysicsWorld::~PhysicsWorld()
{
	m_pHeightMap = nullptr;


	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		if (m_AABBArray[i] != nullptr)
		{
			//delete m_AABBArray[i];
		}
	}
}

void PhysicsWorld::AddBody(DynamicBody * body)
{
	m_dynamicBodyList.push_back(body);

	m_AABBArray[m_AABBArrayIndx] = new AABB(body->GetPosition(), body->GetRadius(), body);
	m_AABBArrayIndx++;

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

			if (XMVectorGetY(body->GetPosition()) < -10.0f)
			{
				body->SetActive(false);
			}
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
	SortAndSweepAABBArray();

	for (auto collision : m_dynamicCollisionList)
	{
		collision.bodyA->ResolveCollision(collision.collisionNormal);
		collision.bodyB->ResolveCollision(collision.collisionNormal);
	}

	for (auto collision : m_dynamicCollisionList)
	{
		PositionalCorrection(&collision);
	}
	//GeneratePairs();
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
		PositionalCorrection(&collision);
	}
}

void PhysicsWorld::PositionalCorrection(PhysicsDynamicCollision * collisionPair)
{
	const float percent = 0.003f;
	const float slop = 0.00002f;

	XMVECTOR correction = max(collisionPair->penetrationDepth - slop, 0.0f) * percent * collisionPair->collisionNormal;
	collisionPair->bodyA->SetPosition(collisionPair->bodyA->GetPosition() - correction);
	collisionPair->bodyB->SetPosition(collisionPair->bodyB->GetPosition() + correction);
}

bool PhysicsWorld::CircleVsCircle(PhysicsDynamicCollision * collisionPair)
{
	//static int functionCallCount = 0;
	//functionCallCount++;
	//dprintf("Function calls to CircleVSCircle : %i	\n", functionCallCount);

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

int PhysicsWorld::AABBvsAABB(AABB * a, AABB * b)
{
	if (a->maxPoint[0] < b->minPoint[0] || a->minPoint[0] > b->maxPoint[0]) return 0;
	if (a->maxPoint[1] < b->minPoint[1] || a->minPoint[1] > b->maxPoint[1]) return 0;
	if (a->maxPoint[2] < b->minPoint[2] || a->minPoint[2] > b->maxPoint[2]) return 0;
	return 1;
}

void PhysicsWorld::UpdateAABBs()
{
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		//if (i < m_AABBArrayIndx)
		{
			m_AABBArray[i]->UpdatePosition(m_AABBArray[i]->body->GetPosition(), (m_AABBArray[i]->body->GetRadius()));
		}
	}
}


void PhysicsWorld::SortAndSweepAABBArray()
{
	UpdateAABBs();

	dprintf("Sorting Axis: %i \n", m_sortingAxis);

	m_dynamicCollisionList.clear();

	std::sort(std::begin(m_AABBArray), std::end(m_AABBArray), [this](const void* a, const void* b) -> int
	{
		float minA = ((AABB *)a)->minPoint[m_sortingAxis];
		float minB = ((AABB *)b)->minPoint[m_sortingAxis];

		return minA > minB;
	});

	float s[3] = { 0.0f, 0.0f, 0.0f }, s2[3] = { 0.0f, 0.0f, 0.0f }, v[3];

	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		//Determine the centre point of the AABB
		Point p = { 0.5f * (m_AABBArray[i]->minPoint[0] + m_AABBArray[i]->maxPoint[0]), 0.5f * ( m_AABBArray[i]->minPoint[1] + m_AABBArray[i]->maxPoint[1]),  0.5f * (m_AABBArray[i]->minPoint[2] + m_AABBArray[i]->maxPoint[2]) };

		//Update sum and sum2 for computing variance
		for (int c = 0; c < 3; c++)
		{
			s[c] += p[c];
			s2[c] += p[c] * p[c];
		}

		//Test collisions against all possible overlapping AABBs following current one
		for (int j = 0; j < MAX_OBJECTS; j++)
		{
			if (m_AABBArray[j]->body == m_AABBArray[i]->body)
			{
				continue;
			}
			if (m_AABBArray[j]->minPoint[m_sortingAxis] > m_AABBArray[i]->maxPoint[m_sortingAxis])
			{
				continue;
			}
			//If AABBS overlap 
			if (AABBvsAABB(m_AABBArray[i], m_AABBArray[j]))
			{
				PhysicsDynamicCollision collisionPair(m_AABBArray[i]->body, m_AABBArray[j]->body);

				//Do collision test here
				if (CircleVsCircle(&collisionPair))
				{
					//Add to list to resolve
					m_dynamicCollisionList.push_back(collisionPair);
				}
			}
		}

		//Calculate variance
		for (int c = 0; c < 3; c++)
		{
			v[c] = s2[c] - s[c] * s[c] / MAX_OBJECTS;
		}

		//Update axis to text next
		m_sortingAxis = 0;
		if (v[1] > v[0])
		{
			m_sortingAxis = 1;
		}
		if (v[2] > v[m_sortingAxis])
		{
			m_sortingAxis = 2;
		}
	}

}
