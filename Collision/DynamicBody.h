#ifndef _DYNAMIC_BODY_H_
#define _DYNAMIC_BODY_H_

#include "Application.h"


struct Material
{
	float density;
	float restitution;
};

struct MassData
{
	float mass;
	float inv_mass;

	MassData(float mMass)
	{
		mass = mMass;
		if (mass == 0)
		{
			inv_mass = 0;
		}
		else
		{
			inv_mass = 1 / mass;
		}
	}

};

XMALIGN class DynamicBody
{
public:

	DynamicBody(CommonMesh* mMesh);
	~DynamicBody();

	void IntegratePosition();

	void ApplyForce(const XMVECTOR& mForce);

	void SetMesh(CommonMesh* mMesh);

	void Reset(const XMVECTOR& mPos);

	void SetPosition(const XMFLOAT3& mNewPos);

	void ResolveCollision(XMVECTOR mCollisionPos, XMVECTOR mCollisionNormal);
	void PositionalCorrection();

	
protected:

	CommonMesh* m_pMesh;

	Material m_physMaterial;

	MassData m_massData;

	XMVECTOR m_vVelocity;
	XMVECTOR m_vForce;
	XMVECTOR m_vPosition;

	XMVECTOR m_vCollisionPosition;
	XMVECTOR m_vCollisionNormal;

	float m_fPenetration;

	XMMATRIX m_mWorldMatrix;

public:

XMNEW
XMDELETE

};

#endif

