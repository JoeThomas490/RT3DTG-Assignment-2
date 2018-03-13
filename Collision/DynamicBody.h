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
};

 __declspec(align(16)) class DynamicBody
{
public:
	DynamicBody();
	~DynamicBody();


private:

	CommonMesh* m_pMesh;

	Material m_physMaterial;

	MassData m_massData;

	XMVECTOR m_vVelocity;
	XMVECTOR m_vForce;
	XMVECTOR m_vPosition;
	
	XMMATRIX m_mWorldMatrix;

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

