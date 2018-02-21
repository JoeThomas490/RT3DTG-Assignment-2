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
	if (m_pHeightMap != nullptr)
	{

		if (!m_bHasCollided)
		{
			XMVECTOR vSPos = XMLoadFloat3(&m_v3Position);
			XMVECTOR vSVel = XMLoadFloat3(&m_v3Velocity);
			XMVECTOR vSAcc = XMLoadFloat3(&m_v3Acceleration);

			vSPos += vSVel; // Really important that we add LAST FRAME'S velocity as this was how fast the collision is expecting the ball to move
			vSVel += vSAcc; // The new velocity gets passed through to the collision so it can base its predictions on our speed NEXT FRAME


			XMStoreFloat3(&m_v3Velocity, vSVel);
			XMStoreFloat3(&m_v3Position, vSPos);

			m_fSpeed = XMVectorGetX(XMVector3Length(vSVel));

			m_bHasCollided = m_pHeightMap->RayCollision(vSPos, vSVel, m_fSpeed, m_vCollisionPos, m_vCollisionNormal);

			if (m_bHasCollided)
			{
				m_v3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
				XMStoreFloat3(&m_v3Position, m_vCollisionPos);
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
