#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "../Application.h"
#include "../DynamicBody.h"


class HeightMap;

//*********************************************************************************************
// File:			Sphere.h
// Description:					
// Todo:			-	
//					
//*********************************************************************************************

XMALIGN class Sphere : public DynamicBody
{
public:

	Sphere();
	Sphere(CommonMesh* mMesh, float mRadius);
	~Sphere();

	void Update();
	void Draw();

private:
	void UpdateMatrices();

public :
	
private:

	float m_fSpeed;

	XMMATRIX m_mWorldMatrix;

public:

	XMNEW;
	XMDELETE;
};


#endif