#include "Sphere.h"
Sphere::Sphere()
{
	DynamicBody();
	m_bIsVisible = false;
}

Sphere::Sphere(CommonMesh* mMesh, float mRadius)
{
	m_bIsVisible = true;
	m_pMesh = mMesh;
	m_fRadius = mRadius;

	m_vPosition = XMVectorSet(0, 0, 0, 0);
	m_vVelocity = XMVectorSet(0, 0, 0, 0);
	m_vForce = XMVectorSet(0, 0, 0, 0);
}

Sphere::~Sphere()
{

}

void Sphere::Update()
{
	m_fSpeed = XMVectorGetX(XMVector3Length(GetVelocity()));
	UpdateMatrices();
}



void Sphere::UpdateMatrices()
{
	XMMATRIX mTrans;

	mTrans = XMMatrixTranslationFromVector(GetPosition());

	m_mWorldMatrix = mTrans;
}

void Sphere::Draw()
{
	if (m_bIsVisible)
	{
		Application::s_pApp->SetWorldMatrix(m_mWorldMatrix);
		Application::s_pApp->SetDepthStencilState(true, true);
		m_pMesh->Draw();
	}
}
