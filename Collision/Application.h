#ifndef APPLICATION_H
#define APPLICATION_H

#define WIN32_LEAN_AND_MEAN

#include <assert.h>

#include <stdio.h>
#include <windows.h>
#include <d3d11.h>

#include "Macros.h"
#include "Constants.h"

#include "CommonApp.h"
#include "CommonMesh.h"

class HeightMap;
class PhysicsWorld;
class Sphere;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
// An example of creating a shader of your own, that fits in
// relatively neatly with the CommonApp functionality.
//
// Edit the shader as the program runs, then Alt+Tab back to it and
// press F5 to reload the shader. Instant feedback!
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class Application:
public CommonApp
{
public:
	static Application* s_pApp;
protected:
	bool HandleStart();
	void HandleStop();
	void HandleUpdate();
	void HandleRender();

	void HandleCameraInput();
	void HandleDebugInput();
	void HandleSphereInput();

private:

	void AddSphere();
	void RemoveSphere();

	XMVECTOR GetRandomPosition();

private:

	float m_frameCount;

	bool m_reload;

	float m_rotationAngle;
	float m_cameraZ;
	bool m_bWireframe;

	bool m_bDebugMode;

	int m_cameraState;

	HeightMap* m_pHeightMap;

	CommonMesh *m_pSphereMesh;

	PhysicsWorld* m_pPhysicsWorld;

	int m_iSphereCount = 0;


	XMFLOAT3 mSpherePos;
	XMFLOAT3 mSphereVel;
	float mSphereSpeed;
	bool mSphereCollided;
	XMFLOAT3 mGravityAcc;

	void ReloadShaders();
};

#endif