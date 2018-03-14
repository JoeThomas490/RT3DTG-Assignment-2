#include "DynamicBody.h"


DynamicBody::DynamicBody(CommonMesh * mMesh)
	: m_pMesh(mMesh), m_massData(1)
{

}

DynamicBody::~DynamicBody()
{
	//Delete mesh
	m_pMesh = nullptr;
}

void DynamicBody::IntegratePosition()
{
	float dTime = Application::s_pApp->m_fDTime;

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

void DynamicBody::Reset(const XMVECTOR& mPos)
{
	m_vPosition = mPos;
}

void DynamicBody::SetPosition(const XMFLOAT3 & mNewPos)
{
	m_vPosition = XMVectorSet(mNewPos.x, mNewPos.y, mNewPos.z, 1);
}

void DynamicBody::ResolveCollision(XMVECTOR mCollisionPos, XMVECTOR mCollisionNormal)
{
	XMVECTOR relativeVelocity = -m_vVelocity;

	XMVECTOR velAlongNormal = XMVector3Dot(relativeVelocity, mCollisionNormal);

	//if (velAlongNormal.m128_f32[0] > 0)
	//{
	//	return;
	//}

	float e = 0.5f;

	float j = -(1 + e) * XMVectorGetX(velAlongNormal);

	XMVECTOR impulse = j * mCollisionNormal;

	m_vVelocity -= impulse;
}

void DynamicBody::PositionalCorrection()
{
	const float percent = 0.4f;
	const float slop = 0.05f;

	XMVECTOR correction = max(m_fPenetration - slop, 0.0f) * percent * m_vCollisionNormal;
	m_vPosition -= correction;
}
