#ifndef _DYNAMIC_BODY_H_
#define _DYNAMIC_BODY_H_

#include "Application.h"


class PhysicsWorld;

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

	DynamicBody();
	DynamicBody(CommonMesh* mMesh, float mRadius);
	~DynamicBody();

	void IntegratePosition();

	void ApplyForce(const XMVECTOR& mForce);
	void ResolveCollision(const XMVECTOR& mCollisionNormal);
	void PositionalCorrectionHeightmap(float mPenetration, const XMVECTOR& mCollisionNormal);

	void SetMesh(CommonMesh* mMesh);

	void SetPosition(const XMVECTOR& mPos);
	XMVECTOR GetPosition();

	void SetVelocity(const XMVECTOR& mVelocity);
	XMVECTOR GetVelocity();

	float GetRadius();
	void SetRadius(float mRadius);

	bool GetActive();
	void SetActive(bool isActive);

protected:

	CommonMesh* m_pMesh;

	Material m_physMaterial;

	MassData m_massData;

	XMVECTOR m_vVelocity;
	XMVECTOR m_vForce;
	XMVECTOR m_vPosition;

	float m_fRadius;

	bool m_bIsActive;

public:

XMNEW
XMDELETE

};

#endif

