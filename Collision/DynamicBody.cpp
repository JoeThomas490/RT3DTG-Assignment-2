#include "DynamicBody.h"


DynamicBody::DynamicBody()
	: m_pMesh(nullptr), m_massData(1), m_fRadius(0)
{
	m_vPosition = XMVectorSet(0, 0, 0, 0);
	m_vVelocity = XMVectorSet(0, 0, 0, 0);
	m_vForce = XMVectorSet(0, 0, 0, 0);
}

DynamicBody::DynamicBody(CommonMesh * mMesh, float mRadius)
	: m_massData(1.0f)
{
	m_vPosition = XMVectorSet(0, 0, 0, 0);
	m_vVelocity = XMVectorSet(0, 0, 0, 0);
	m_vForce = XMVectorSet(0, 0, 0, 0);

	m_pMesh = mMesh;
	m_fRadius = mRadius;
}

DynamicBody::~DynamicBody()
{
	//Delete mesh
	m_pMesh = nullptr;
}

void DynamicBody::IntegratePosition()
{
	float dTime = Application::s_pApp->m_fDTime;

	//REFERENCE NOTE : FROM GAMEDEVTUTS.COM
	//NUMERICAL INTEGRATION, SPRING ENERGY

	//Add all forces onto current velocity (multiplying by dTime to smooth movement over different fps)
	m_vVelocity += (m_massData.inv_mass * m_vForce) * dTime;

	//Increment position by overall velocity
	m_vPosition += m_vVelocity * dTime;

	//Reset force after every frame
	m_vForce = XMVectorSet(0, 0, 0, 0);
}


//Apply a force to the body (Adds force onto m_vForce)
//Params : XMVECTOR of force to be added
void DynamicBody::ApplyForce(const XMVECTOR & mForce)
{
	m_vForce += mForce;
}

//REFERENCE NOTE : FROM GAMEDEVTUTS.COM

//Calculate impulse velocity when passed a collision normal
//Params : XMVECTOR of collision normal (must be normalized)
void DynamicBody::ResolveCollision(const XMVECTOR& mCollisionNormal)
{
	//Calculate negative velocity (opposite movement)
	XMVECTOR relativeVelocity = -m_vVelocity;

	//Calculate dot product between relative velocity and collision normal
	XMVECTOR velAlongNormal = XMVector3Dot(relativeVelocity, mCollisionNormal);

	//If the dot product < 0 (i.e object is moving away from normal)
	//Then return out and don't resolve
	if (XMVectorGetX(velAlongNormal) < 0)
	{
		return;
	}

	//Coefficent of restitution
	float e = 0.6f;

	//Calculate amount of impulse
	float j = -(1 + e) * XMVectorGetX(velAlongNormal);

	//Calculate velocity
	XMVECTOR impulse = j * mCollisionNormal;

	//Modify velocity by this impulse velocty
	m_vVelocity -= impulse;
}

//REFERENCE NOTE : FROM GAMEDEVTUTS.COM

//Calculate position correction when colliding with the heightmap
//Params : Penetration depth between body and heightmap, XMVECTOR of collision normal
void DynamicBody::PositionalCorrectionHeightmap(float mPenetration, const XMVECTOR& mCollisionNormal)
{
	//Percent to move position (high percent causes more jitter but stops overlap)
	const float percent = 0.2f;
	
	//If penetration is less than slop value then don't correct
	const float slop = 0.01f;

	//Calulcate positional correction vector
	XMVECTOR correction = max(mPenetration - slop, 0.0f) * percent * mCollisionNormal;

	//Modify position based on correction calculated
	m_vPosition += correction;
}

//*********************** Getters / Setters ************************************

void DynamicBody::SetMesh(CommonMesh * mMesh)
{
	m_pMesh = mMesh;
}

void DynamicBody::SetPosition(const XMVECTOR& mPos)
{
	m_vPosition = mPos;
}

XMVECTOR DynamicBody::GetPosition()
{
	return m_vPosition;
}

void DynamicBody::SetVelocity(const XMVECTOR & mVelocity)
{
	m_vVelocity = mVelocity;
}

XMVECTOR DynamicBody::GetVelocity()
{
	return m_vVelocity;
}

float DynamicBody::GetRadius()
{
	return m_fRadius;
}

void DynamicBody::SetRadius(float mRadius)
{
	m_fRadius = mRadius;
}

bool DynamicBody::GetActive()
{
	return m_bIsActive;
}

void DynamicBody::SetActive(bool isActive)
{
	m_bIsActive = isActive;
}

//******************************************************************************

