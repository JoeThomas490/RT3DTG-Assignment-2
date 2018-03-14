#include "Sphere.h"
#include "../HeightMap.h"

CommonMesh* Sphere::s_pMesh = nullptr;

Sphere::Sphere()
{
	m_bIsVisible = true;
	m_v3Position = XMFLOAT3(-14.0f, 20.0f, -14.0f);
	m_v3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Sphere::~Sphere()
{

}

void Sphere::LoadResources()
{
	s_pMesh = CommonMesh::NewSphereMesh(Application::s_pApp, 1.0f, 16, 16);
}

void Sphere::DeleteResources()
{
	if (s_pMesh != nullptr)
	{
		delete s_pMesh;
		s_pMesh = nullptr;
	}
}

void Sphere::Update()
{
	XMVECTOR vSPos = XMLoadFloat3(&m_v3Position);
	XMVECTOR vSVel = XMLoadFloat3(&m_v3Velocity);
	XMVECTOR vSAcc = XMLoadFloat3(&m_v3Acceleration);

	//if (!m_bHasCollided)
	{

		if (m_pHeightMap != nullptr)
		{

			float dTime = Application::s_pApp->m_fDTime;

			vSPos += vSVel; // Really important that we add LAST FRAME'S velocity as this was how fast the collision is expecting the ball to move
			vSVel += vSAcc; // The new velocity gets passed through to the collision so it can base its predictions on our speed NEXT FRAME

			//dprintf("DTIME = %f\n", dTime);
			XMStoreFloat3(&m_v3Velocity, vSVel);
			XMStoreFloat3(&m_v3Position, vSPos);

			m_fSpeed = XMVectorGetX(XMVector3Length(vSVel));

			float distance;
			//m_bHasCollided = m_pHeightMap->RayCollision(vSPos, vSVel, m_fSpeed, m_vCollisionPos, m_vCollisionNormal);
			m_bHasCollided = m_pHeightMap->SphereTriangle(vSPos, 1.0f, m_vCollisionPos, m_vCollisionNormal, distance);
			//m_fPenetration = XMVector3Length(vSPos - m_vCollisionPos).m128_f32[0];


			if (m_bHasCollided)
			{
				ResolveCollision(m_vCollisionPos, m_vCollisionNormal);
				//PositionalCorrection();
			}
		}


	}



	UpdateMatrices();
}



void Sphere::UpdateMatrices()
{
	XMMATRIX mTrans;

	mTrans = XMMatrixTranslationFromVector(XMLoadFloat3(&m_v3Position));

	m_mWorldMatrix = mTrans;
}

void Sphere::Draw()
{
	if (m_bIsVisible)
	{
		Application::s_pApp->SetWorldMatrix(m_mWorldMatrix);
		Application::s_pApp->SetDepthStencilState(true, true);
		s_pMesh->Draw();
	}
}

void Sphere::Reset()
{
	m_v3Position = XMFLOAT3(-14.0f, 20.0f, -14.0f);
	m_v3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_v3Acceleration = XMFLOAT3(0.0f, -0.05f, 0.0f);

	m_bHasCollided = false;
}

void Sphere::Reset(XMFLOAT3 mPos, XMFLOAT3 mVel, XMFLOAT3 mAccel)
{
	m_v3Position = mPos;
	m_v3Velocity = mVel;
	m_v3Acceleration = mAccel;

	m_bHasCollided = false;
}

void Sphere::SetHeightmapPtr(HeightMap * pHeightmap)
{
	m_pHeightMap = pHeightmap;
}

void Sphere::SetPosition(const XMFLOAT3 & mNewPos)
{
	m_v3Position = mNewPos;
}

void Sphere::ResolveCollision(XMVECTOR mCollisionPos, XMVECTOR mCollisionNormal)
{
	XMVECTOR relativeVelocity = -XMLoadFloat3(&m_v3Velocity);

	XMVECTOR velAlongNormal = XMVector3Dot(relativeVelocity, mCollisionNormal);

	//if (velAlongNormal.m128_f32[0] > 0)
	//{
	//	return;
	//}

	float e = 0.1f;

	float j = -(1 + e) * velAlongNormal.m128_f32[0];

	XMVECTOR impulse = j * mCollisionNormal;

	XMVECTOR vel = XMLoadFloat3(&m_v3Velocity);
	vel -= impulse;

	XMStoreFloat3(&m_v3Velocity, vel);
}

void Sphere::PositionalCorrection()
{
	const float percent = 0.2f;
	const float slop = 0.01f;

	XMVECTOR correction = max(m_fPenetration - slop, 0.0f) * m_vCollisionNormal;

	XMVECTOR pos = XMLoadFloat3(&m_v3Position);
	pos -= correction;
	XMStoreFloat3(&m_v3Position, pos);
}

