#ifndef _DYNAMIC_BODY_H_
#define _DYNAMIC_BODY_H_

#include "Application.h"


class PhysicsWorld;


//**********************************************************************************
// Struct : MassData
// Description : Holds the mass and inverse mass for a body. If 0 is passed for the mass
// the inverse is also set as 0
//**********************************************************************************
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

	//Move the position of the body by applying a velocity
	void IntegratePosition();

	//Apply a force to the body (Adds force onto m_vForce)
	//Params : XMVECTOR of force to be added
	void ApplyForce(const XMVECTOR& mForce);
	
	//Calculate impulse velocity when passed a collision normal
	//Params : XMVECTOR of collision normal
	void ResolveCollision(const XMVECTOR& mCollisionNormal);

	//Calculate position correction when colliding with the heightmap
	//Params : Penetration depth between body and heightmap, XMVECTOR of collision normal
	void PositionalCorrectionHeightmap(float mPenetration, const XMVECTOR& mCollisionNormal);

//*********************** Getters / Setters ************************************
	void SetMesh(CommonMesh* mMesh);

	//Set/Get position
	void SetPosition(const XMVECTOR& mPos);
	XMVECTOR GetPosition();

	//Set/Get velocity
	void SetVelocity(const XMVECTOR& mVelocity);
	XMVECTOR GetVelocity();

	//Set/Get radius
	float GetRadius();
	void SetRadius(float mRadius);

	//Set/Get active body
	bool GetActive();
	void SetActive(bool isActive);

//******************************************************************************

protected:

	//Pointer to the mesh associated with this body
	CommonMesh* m_pMesh;

	//Mass data associated with this body (mass and inv_mass)
	MassData m_massData;

	//Current velocity of body
	XMVECTOR m_vVelocity;

	//Current force applied to body
	XMVECTOR m_vForce;

	//Current position of body
	XMVECTOR m_vPosition;

	//Radius of body (As all bodies are currently spheres)
	float m_fRadius;

	//Whether body is currently active or not
	bool m_bIsActive;

public:

XMNEW
XMDELETE

};

#endif

