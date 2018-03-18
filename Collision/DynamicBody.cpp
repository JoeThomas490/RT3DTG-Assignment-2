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

	//dprintf("FORCE: %f , %f , %f \n", XMVectorGetX(m_vForce), XMVectorGetY(m_vForce), XMVectorGetZ(m_vForce));

	//REFERENCE NOTE : FROM GAMEDEVTUTS.COM
	//NUMERICAL INTEGRATION, SPRING ENERGY
	m_vVelocity += (m_massData.inv_mass * m_vForce) * dTime;
	m_vPosition += m_vVelocity * dTime;

	m_vForce = XMVectorSet(0, 0, 0, 0);
}

void DynamicBody::ApplyForce(const XMVECTOR & mForce)
{
	m_vForce += mForce;
}

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

//REFERENCE NOTE : FROM GAMEDEVTUTS.COM
void DynamicBody::ResolveCollision(const XMVECTOR& mCollisionNormal)
{
	XMVECTOR relativeVelocity = -m_vVelocity;

	XMVECTOR velAlongNormal = XMVector3Dot(relativeVelocity, mCollisionNormal);

	if (XMVectorGetX(velAlongNormal) < 0)
	{
		return;
	}

	float e = 0.6f;

	float j = -(1 + e) * XMVectorGetX(velAlongNormal);

	XMVECTOR impulse = j * mCollisionNormal;
	m_vVelocity -= impulse;
}

//REFERENCE NOTE : FROM GAMEDEVTUTS.COM
void DynamicBody::PositionalCorrectionHeightmap(float mPenetration, const XMVECTOR& mCollisionNormal)
{
	const float percent = 0.1f;
	const float slop = 0.01f;

	XMVECTOR correction = max(mPenetration - slop, 0.0f) * percent * mCollisionNormal;
	m_vPosition += correction;
}

