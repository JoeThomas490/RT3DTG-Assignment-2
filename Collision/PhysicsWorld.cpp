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
}

//Sets the pointer of the heightmap to test static collisions against
//Params : Pointer of the heightmap to be tested against
void PhysicsWorld::SetHeightMapPtr(HeightMap * pHeightMap)
{
	m_pHeightMap = pHeightMap;
}

//Adds a body to the list of bodies within the physics world
//Params : Pointer to the body to add
void PhysicsWorld::AddBody(DynamicBody * body)
{
	//Push back the dynamic body pointer to the vector
	m_dynamicBodyList.push_back(body);

	//Also add a new body into the AABB array
	m_AABBArray[m_AABBArrayIndx] = new AABB(body->GetPosition(), body->GetRadius(), body);
	//Increment the AABB index for next insertion
	m_AABBArrayIndx++;

}

//Removes a body from the physics world
//Params : Pointer to the body to remove
void PhysicsWorld::RemoveBody(DynamicBody* mBody)
{
	//Iterate through the body vector
	for (std::vector<DynamicBody*>::iterator it = m_dynamicBodyList.begin(); it < m_dynamicBodyList.end(); it++)
	{
		//If the pointers match (i.e it's the same body)
		if ((*it) == mBody)
		{
			//Erase it from the vector
			m_dynamicBodyList.erase(it);
			//Break out the loop and exit function
			break;
		}
	}
}

//Controls the update of all bodies within the scene
//Main function to be called
void PhysicsWorld::UpdateWorld()
{
	HandleStaticCollision();
	HandleDynamicCollision();


	//Loop through all static collisions
	for (auto collision : m_staticCollisionList)
	{
		//Resolve each collision on each body
		collision.body->ResolveCollision(collision.collisionNormal);
	}

	//Loop through all dynamic collisions
	for (auto collision : m_dynamicCollisionList)
	{
		//Resolve each collison on each body
		collision.bodyA->ResolveCollision(collision.collisionNormal);
		collision.bodyB->ResolveCollision(collision.collisionNormal);
	}

	//Loop through all static collisions again
	for (auto collision : m_staticCollisionList)
	{
		//Correct the positions of each collision
		collision.body->PositionalCorrectionHeightmap(collision.penetrationDepth, collision.collisionNormal);
	}

	//Loop through all dynamic collisions again
	for (auto collision : m_dynamicCollisionList)
	{
		//Correct the positions of each collision
		PositionalCorrection(&collision);
	}

	//Loop through each body in the physics world
	for (auto body : m_dynamicBodyList)
	{
		//If the body is active (being rendered and active in the physics world)
		if (body->GetActive())
		{
			//Apply gravity
			body->ApplyForce(XMVectorSet(0, GRAVITY, 0, 0));
			
			//Finally update the position of the body after all collisions have been resolved
			body->IntegratePosition();

			//If the Y position of the body is below a certain value (-10)
			if (XMVectorGetY(body->GetPosition()) < -10.0f)
			{
				//Deactivate this body
				body->SetActive(false);
			}
		}
	}

	//Clear each collision vector for next frame
	m_staticCollisionList.clear();
	m_dynamicCollisionList.clear();
}

//Controls the collision between the dynamic bodies
//and the static heightmap
void PhysicsWorld::HandleStaticCollision()
{
	//Make sure the heightmap pointer has been set
	if (m_pHeightMap != nullptr)
	{
		//Reset the colours of the heightmap (Get rid of the red collision colour)
		m_pHeightMap->ResetVertexColours();

		//Loop through all bodies and check collision with heightmap
		for (auto body : m_dynamicBodyList)
		{
			//Only check the body against the heightmap if it's active
			if (body->GetActive())
			{
				//Create a new static collision vector for this body because the body could be colliding with more than one 
				//face on the heightmap
				std::vector<PhysicsStaticCollision>	bodyCollisionList = m_pHeightMap->SphereHeightmap(body);

				//Concatenate this vector with the static collision list for resolution at a future point
				m_staticCollisionList.insert(m_staticCollisionList.end(), bodyCollisionList.begin(), bodyCollisionList.end());
			}
		}

		//Rebuild the vertex data of the heightmap to get a red colour when colliding
		m_pHeightMap->RebuildVertexData();
	}
}

//Controls the collision between all dynamic bodies
void PhysicsWorld::HandleDynamicCollision()
{
	SortAndSweepAABBArray();
	//GeneratePairs();
}

//void PhysicsWorld::GeneratePairs()
//{
//	for (auto bodyA : m_dynamicBodyList)
//	{
//		for (auto bodyB : m_dynamicBodyList)
//		{
//			if (bodyA == bodyB)
//			{
//				continue;
//			}
//
//
//			PhysicsDynamicCollision collisionPair(bodyA, bodyB);
//
//			if (CircleVsCircle(&collisionPair))
//			{
//				m_dynamicCollisionList.push_back(collisionPair);
//			}
//		}
//	}
//
//
//	for (auto collision : m_dynamicCollisionList)
//	{
//		collision.bodyA->ResolveCollision(collision.collisionNormal);
//		collision.bodyB->ResolveCollision(collision.collisionNormal);
//	}
//
//	for (auto collision : m_dynamicCollisionList)
//	{
//		PositionalCorrection(&collision);
//	}
//}

//Corrects the position of overlapping bodies during a dynamic vs dynamic
//body collision
//Params : Collision pair to be tested 
void PhysicsWorld::PositionalCorrection(PhysicsDynamicCollision * collisionPair)
{
	//Percent to move position (high percent causes more jitter but stops overlap)
	const float percent = 0.0015f;

	//If penetration is less than slop value then don't correct
	const float slop = 0.001f;

	//Calulcate positional correction vector
	XMVECTOR correction = max(collisionPair->penetrationDepth - slop, 0.0f) * percent * collisionPair->collisionNormal;

	//Modify the positions of each body by this calculated correction amount (moving them away from each other)
	collisionPair->bodyA->SetPosition(collisionPair->bodyA->GetPosition() - correction);
	collisionPair->bodyB->SetPosition(collisionPair->bodyB->GetPosition() + correction);
}

//Simple circle vs circle check (Taken from Real Time Collision Detection book)
//Params : Collision pair to be tested
//Returns : True if the two bodies are overlapping (colliding)
bool PhysicsWorld::CircleVsCircle(PhysicsDynamicCollision * collisionPair)
{
	//Frame count test for CircleVsCircle
	//-------------------------------------
	//static int functionCallCount = 0;
	//functionCallCount++;
	//if (functionCallCount > 50000)
	//{
	//	int frameCount = Application::s_pApp->m_frameCount;
	//}
	//-------------------------------------
	//dprintf("Function calls to CircleVSCircle : %i	\n", functionCallCount);

	//Get pointers to each body
	DynamicBody* bodyA = collisionPair->bodyA;
	DynamicBody* bodyB = collisionPair->bodyB;

	//Get the distance between each body
	XMVECTOR dist = bodyB->GetPosition() - bodyA->GetPosition();

	//Get the sum of radii of each body
	float r = bodyA->GetRadius() + bodyB->GetRadius();

	//Square the radius
	r *= r;

	//Check the distance ^ 2 against the sum of radii ^ 2 
	//to stop an expensive square root operation.
	//If the distance is greater than the radius then there is 
	//no collision
	if (XMVectorGetX(XMVector3LengthSq(dist)) > r)
	{
		return false;
	}

	//Circles have collided

	//Get square rooted length of vector between bodies
	float d = XMVectorGetX(XMVector3Length(dist));

	if (d != 0)
	{
		//Calculate penetration depth and collision normal
		collisionPair->penetrationDepth = r - d;
		collisionPair->collisionNormal = dist / d;

		return true;
	}

	//Circles are on the same position
	else
	{
		//Set generic values to penetration depth and normal if circles
		//are on the exact same position
		collisionPair->penetrationDepth = bodyA->GetRadius();
		collisionPair->collisionNormal = XMVectorSet(1, 0, 0, 0);
		return true;
	}
}

//Simple AABB vs AABB check (Taken from Real Time Collision Detection book)
//Params : Pointers to each AABB to check
//Returns : 1 if two bounding boxes are overlapping, 0 if not
int PhysicsWorld::AABBvsAABB(AABB * a, AABB * b)
{
	if (a->maxPoint[0] < b->minPoint[0] || a->minPoint[0] > b->maxPoint[0]) return 0;
	if (a->maxPoint[1] < b->minPoint[1] || a->minPoint[1] > b->maxPoint[1]) return 0;
	if (a->maxPoint[2] < b->minPoint[2] || a->minPoint[2] > b->maxPoint[2]) return 0;
	return 1;
}

//Updates all AABBs surrounding each dynamic body
void PhysicsWorld::UpdateAABBs()
{
	//Loop through all objects
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		//If the body is active
		if (m_AABBArray[i]->body->GetActive())
		{
			//Then update it's bounds
			m_AABBArray[i]->UpdatePosition(m_AABBArray[i]->body->GetPosition(), (m_AABBArray[i]->body->GetRadius()));
		}
	}
}

//Broadphase method used for dynamic collisions (Taken from Real Time Collision Detection book)
//Sorts bodies on a specified axis and only checks those that are close together
//Also calculates the variance of each axis and then decides which axis (X, Y, or Z) is best to sort against next
void PhysicsWorld::SortAndSweepAABBArray()
{
	//Update all bounds
	UpdateAABBs();

	//Clear the old dynamic collision lit
	m_dynamicCollisionList.clear();

	//Sort the array based on their min point position
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
			//If it's the same body then skip over
			if (m_AABBArray[j]->body == m_AABBArray[i]->body)
			{
				continue;
			}

			//If either body is inactive then skip over
			if (!m_AABBArray[j]->body->GetActive() || !m_AABBArray[i]->body->GetActive())
			{
				continue;
			}

			//If the minimum point of body B is greater than maximum point of body A then skip over as
			//it's not close enough to bother checking collision
			if (m_AABBArray[j]->minPoint[m_sortingAxis] > m_AABBArray[i]->maxPoint[m_sortingAxis])
			{
				continue;
			}
			//If AABBS overlap 
			if (AABBvsAABB(m_AABBArray[i], m_AABBArray[j]))
			{
				//Create a dynamic collision pair using each body
				PhysicsDynamicCollision collisionPair(m_AABBArray[i]->body, m_AABBArray[j]->body);

				//Finally do the proper collision check here
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

		//Update axis to test next
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
