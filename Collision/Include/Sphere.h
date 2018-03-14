#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "../Application.h"

class HeightMap;

//*********************************************************************************************
// File:			Sphere.h
// Description:					
// Todo:			-	
//					
//*********************************************************************************************

__declspec(align(16)) class Sphere
{
public:

	Sphere();
	~Sphere();

	static void LoadResources();
	static void DeleteResources();

	void Update();
	void Draw();

	void Reset();
	void Reset(XMFLOAT3 mPos, XMFLOAT3 mVel, XMFLOAT3 mAccel);

	void SetHeightmapPtr(HeightMap* pHeightmap);
	void SetPosition(const XMFLOAT3& mNewPos);

	void ResolveCollision(XMVECTOR mCollisionPos, XMVECTOR mCollisionNormal);
	void PositionalCorrection();

	XMFLOAT3 m_v3Position;

private:
	void UpdateMatrices();

public :

	bool m_bHasCollided;

	
private:

	static CommonMesh* s_pMesh;

	HeightMap* m_pHeightMap;

	float m_fRadius = 1.0f;
	float m_fSpeed;

	bool m_bIsVisible;

	XMFLOAT3 m_v3Velocity;
	XMFLOAT3 m_v3Acceleration;

	XMVECTOR m_vAcceleration;
	XMVECTOR m_vCollisionPos;
	XMVECTOR m_vCollisionNormal;

	XMMATRIX m_mWorldMatrix;

	float m_fPenetration;

public:

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}
};


#endif