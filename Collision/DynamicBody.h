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

//**********************************************************************************
// Class : DynamicBody
// Description : Holds all relevant data to do with a singular dynamic body including
// position, current velocity, mass etc.
// Also holds functionality to resolve it's own collisions and integrate position
//**********************************************************************************
XMALIGN class DynamicBody
{
public:

	DynamicBody(CommonMesh* mMesh);
	~DynamicBody();

	void IntegratePosition();

	void ApplyForce(const XMVECTOR& mForce);
	void ResolveCollision(const XMVECTOR& mCollisionPos, const XMVECTOR& mCollisionNormal);
	void PositionalCorrection(float mPenetration, const XMVECTOR& mCollisionNormal);

	void SetMesh(CommonMesh* mMesh);

	void ResetPosition(const XMVECTOR& mPos);
	XMVECTOR GetPosition();

	void SetVelocity(const XMVECTOR& mVelocity);
	XMVECTOR GetVelocity();

	float GetRadius();

protected:

	CommonMesh* m_pMesh;

	Material m_physMaterial;

	MassData m_massData;

	XMVECTOR m_vVelocity;
	XMVECTOR m_vForce;
	XMVECTOR m_vPosition;

	XMMATRIX m_mWorldMatrix;

	float m_fRadius;

public:

XMNEW
XMDELETE

};

#endif

