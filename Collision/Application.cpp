#include "Application.h"
#include "HeightMap.h"
#include "PhysicsWorld.h"
#include "Sphere.h"

Application* Application::s_pApp = NULL;

const int CAMERA_TOP = 0;
const int CAMERA_ROTATE = 1;
const int CAMERA_MAX = 2;

const int DEBUG_FRAME_COUNT = 30;

std::vector<Sphere*> m_pSphereArray;

HeightMap** m_heightMapArray;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool Application::HandleStart()
{
	s_pApp = this;
	m_frameCount = 0;

	m_pSphereMesh = CommonMesh::NewSphereMesh(this, 1.0f, 16, 16);

	m_bWireframe = true;

	m_heightMapArr[0] = new HeightMap("Resources/heightmap_0.bmp", 2.0f, 0.75f);
	m_heightMapArr[1] = new HeightMap("Resources/heightmap_1.bmp", 2.0f, 0.75f);
	m_heightMapArr[2] = new HeightMap("Resources/heightmap_2.bmp", 2.0f, 0.75f);
	m_heightMapArr[3] = new HeightMap("Resources/heightmap_3.bmp", 2.0f, 0.75f);

	m_pActiveHeightMap = m_heightMapArr[0];

	m_pPhysicsWorld = new PhysicsWorld(m_pActiveHeightMap);

	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		m_pSphereArray.push_back(new Sphere(m_pSphereMesh, 1.0f));
		m_pSphereArray[i]->SetPosition(GetRandomPosition());
		m_pSphereArray[i]->SetActive(false);
		m_pPhysicsWorld->AddBody(m_pSphereArray[i]);
	}

	m_pSphereArray[0]->SetActive(true);
	m_pSphereArray[0]->SetPosition(GetRandomPosition());

	m_iCurrentSphereIndx = 0;

	m_bDebugMode = false;

	m_cameraZ = 50.0f;
	m_rotationAngle = 0.f;

	m_reload = false;

	ReloadShaders();

	if (!this->CommonApp::HandleStart())
		return false;

	this->SetRasterizerState(false, m_bWireframe);

	m_cameraState = CAMERA_ROTATE;
	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleStop()
{
	m_pActiveHeightMap = nullptr;
	delete m_heightMapArr[0];
	delete m_heightMapArr[1];
	delete m_heightMapArr[2];
	delete m_heightMapArr[3];

	if (m_pPhysicsWorld != nullptr)
	{
		delete m_pPhysicsWorld;
		m_pPhysicsWorld = nullptr;
	}

	this->CommonApp::HandleStop();
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::ReloadShaders()
{
	if (m_pActiveHeightMap->ReloadShader() == false)
		this->SetWindowTitle("Reload Failed - see Visual Studio output window. Press F5 to try again.");
	else
		this->SetWindowTitle("Collision: Zoom / Rotate Q, A / O, P, Camera C, Drop Sphere R, U, I and D,  Wire W, Change HeightMap M");
}

void Application::HandleUpdate()
{
	HandleCameraInput();
	HandleDebugInput();
	HandleSphereInput();

	static bool dbH = false;
	static bool toggleHole = false;
	if (IsKeyPressed('H'))
	{
		if (dbH == false)
		{
			dbH = true;

			toggleHole = !toggleHole;

			if (toggleHole)
			{
				m_pActiveHeightMap->DisableBelowLevel(3.0f);
			}
			else
			{
				m_pActiveHeightMap->EnableAll();
			}
		}
	}
	else
	{
		dbH = false;
	}

	static bool dbM = false;
	if (IsKeyPressed('M'))
	{
		if (dbM == false)
		{
			dbM = true;

			m_iCurrentHeightMapIndx++;
			if (m_iCurrentHeightMapIndx >= MAX_HEIGHTMAPS)
			{
				m_iCurrentHeightMapIndx = 0;
			}

			m_pActiveHeightMap = m_heightMapArr[m_iCurrentHeightMapIndx];

			m_pPhysicsWorld->SetHeightMapPtr(m_pActiveHeightMap);
		}
	}
	else
	{
		dbM = false;
	}



	if (!m_bDebugMode)
	{
		m_pPhysicsWorld->UpdateWorld();
		for (auto sphere : m_pSphereArray)
		{
			if (sphere->GetActive())
			{
				sphere->Update();
			}
		}
	}
	else
	{
		if ((int)m_frameCount % DEBUG_FRAME_COUNT == 0)
		{
			m_pPhysicsWorld->UpdateWorld();
			for (auto sphere : m_pSphereArray)
			{
				if (sphere->GetActive())
				{
					sphere->Update();
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleRender()
{
	XMVECTOR vCamera, vLookat;
	XMVECTOR vUpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX matProj, matView;

	switch (m_cameraState)
	{
	case CAMERA_TOP:
		vCamera = XMVectorSet(0.0f, 100.0f, 0.1f, 0.0f);
		vLookat = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		matView = XMMatrixLookAtLH(vCamera, vLookat, vUpVector);
		matProj = XMMatrixOrthographicLH(64, 36, 1.5f, 5000.0f);
		break;
	case CAMERA_ROTATE:
		vCamera = XMVectorSet(sin(m_rotationAngle)*m_cameraZ, (m_cameraZ*m_cameraZ) / 50, cos(m_rotationAngle)*m_cameraZ, 0.0f);
		vLookat = XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f);
		matView = XMMatrixLookAtLH(vCamera, vLookat, vUpVector);
		matProj = XMMatrixPerspectiveFovLH(float(D3DX_PI / 7), 2, 1.5f, 5000.0f);
		break;
	}

	this->EnableDirectionalLight(1, XMFLOAT3(-1.f, -1.f, -1.f), XMFLOAT3(0.55f, 0.55f, 0.65f));
	this->EnableDirectionalLight(2, XMFLOAT3(1.f, -1.f, 1.f), XMFLOAT3(0.15f, 0.15f, 0.15f));

	this->SetViewMatrix(matView);
	this->SetProjectionMatrix(matProj);

	this->Clear(XMFLOAT4(0.05f, 0.05f, 0.5f, 1.f));

	XMMATRIX worldMtx;

	worldMtx = XMMatrixTranslation(mSpherePos.x, mSpherePos.y, mSpherePos.z);

	SetDepthStencilState(false, true);
	m_pActiveHeightMap->Draw(m_frameCount);

	this->SetWorldMatrix(worldMtx);
	SetDepthStencilState(true, true);

	for (auto sphere : m_pSphereArray)
	{
		if (sphere->GetActive())
		{
			sphere->Draw();
		}
	}

	m_frameCount++;
}

void Application::HandleCameraInput()
{
	if (m_cameraState == CAMERA_ROTATE)
	{
		if (this->IsKeyPressed('Q') && m_cameraZ > 38.0f)
			m_cameraZ -= 1.0f;

		if (this->IsKeyPressed('A'))
			m_cameraZ += 1.0f;

		if (this->IsKeyPressed('O'))
			m_rotationAngle -= .01f;

		if (this->IsKeyPressed('P'))
			m_rotationAngle += .01f;
	}


	static bool dbC = false;

	if (this->IsKeyPressed('C'))
	{
		if (!dbC)
		{
			if (++m_cameraState == CAMERA_MAX)
				m_cameraState = CAMERA_TOP;

			dbC = true;
		}
	}
	else
	{
		dbC = false;
	}
}

void Application::HandleDebugInput()
{

	static bool dbW = false;
	if (this->IsKeyPressed('W'))
	{
		if (!dbW)
		{
			m_bWireframe = !m_bWireframe;
			this->SetRasterizerState(false, m_bWireframe);
			dbW = true;
		}
	}
	else
	{
		dbW = false;
	}


	if (this->IsKeyPressed(VK_F5))
	{
		if (!m_reload)
		{
			ReloadShaders();
			m_reload = true;
		}
	}
	else
		m_reload = false;



	if (this->IsKeyPressed(32))
	{
		m_bDebugMode = true;
	}
	else
	{
		m_bDebugMode = false;
	}
}

void Application::HandleSphereInput()
{
	static bool dbUp = false;

	if (this->IsKeyPressed(38))
	{
		if (dbUp == false)
		{
			dbUp = true;

			AddSphere();
		}
	}
	else
	{
		dbUp = false;
	}

	static bool dbDown = false;

	if (this->IsKeyPressed(40))
	{
		if (dbDown == false)
		{
			dbDown = true;

			RemoveSphere();
		}
	}
	else
	{
		dbDown = false;
	}



	static bool dbR = false;
	if (this->IsKeyPressed('R'))
	{
		if (dbR == false)
		{
			dbR = true;

			for (int i = 0; i <= m_iCurrentSphereIndx; i++)
			{
				Sphere* s = m_pSphereArray[i];
				s->SetActive(true);
				s->SetPosition(GetRandomPosition());
				s->SetVelocity(XMVectorSet(0, 0, 0, 0));
			}
		}
	}
	else
	{
		dbR = false;
	}

	static int faceCounter = 0;
	static int vertexCounter = 0;

	static bool dbU = false;
	if (this->IsKeyPressed('U'))
	{
		if (dbU == false)
		{
			faceCounter++;
			if (faceCounter > m_pActiveHeightMap->m_iFaceCount)
			{
				faceCounter = 0;
			}

			vertexCounter = 0;

			XMFLOAT3 newVel = XMFLOAT3(0.0f, 0.2f, 0.0f);
			XMFLOAT3 newAccel = XMFLOAT3(0.0f, -0.05f, 0.0f);

			XMFLOAT3 newPos;
			newPos = m_pActiveHeightMap->GetPositionOnFace(faceCounter, vertexCounter);

			newPos.y += 10.0f;

			m_pSphereArray[0]->SetPosition(XMVectorSet(newPos.x, newPos.y, newPos.z, 1));
			m_pSphereArray[0]->SetVelocity(XMVectorSet(0, 0, 0, 0));

			dbU = true;
		}
	}
	else
	{
		dbU = false;
	}

	static bool dbI = false;
	if (this->IsKeyPressed('I'))
	{
		if (dbI == false)
		{
			faceCounter--;
			if (faceCounter < 0)
			{
				faceCounter = m_pActiveHeightMap->m_iFaceCount;
			}

			vertexCounter = 0;

			XMFLOAT3 newVel = XMFLOAT3(0.0f, 0.2f, 0.0f);
			XMFLOAT3 newAccel = XMFLOAT3(0.0f, -0.05f, 0.0f);

			XMFLOAT3 newPos;
			newPos = m_pActiveHeightMap->GetPositionOnFace(faceCounter, vertexCounter);

			newPos.y += 10.0f;

			m_pSphereArray[0]->SetPosition(XMVectorSet(newPos.x, newPos.y, newPos.z, 1));
			m_pSphereArray[0]->SetVelocity(XMVectorSet(0, 0, 0, 0));

			dbI = true;
		}
	}
	else
	{
		dbI = false;
	}

	static bool dbD = false;
	if (this->IsKeyPressed('D'))
	{
		if (dbD == false)
		{
			dbD = true;
			vertexCounter++;
			if (vertexCounter > 3)
			{
				vertexCounter = 0;
			}


			XMFLOAT3 newVel = XMFLOAT3(0.0f, 0.2f, 0.0f);
			XMFLOAT3 newAccel = XMFLOAT3(0.0f, -0.05f, 0.0f);

			XMFLOAT3 newPos;
			newPos = m_pActiveHeightMap->GetPositionOnFace(faceCounter, vertexCounter);

			newPos.y += 10.0f;

			m_pSphereArray[0]->SetPosition(XMVectorSet(newPos.x, newPos.y, newPos.z, 1));
			m_pSphereArray[0]->SetVelocity(XMVectorSet(0, 0, 0, 0));

		}
	}
	else
	{
		dbD = false;
	}


	static bool dbF = false;

	if (IsKeyPressed('F'))
	{
		if (dbF == false)
		{
			dbF = true;

			if(m_iCurrentSphereIndx == 0)
			{
				AddSphere();
			}

			m_pSphereArray[0]->SetPosition(XMVectorSet(15, 20, 0, 0));
			m_pSphereArray[0]->SetVelocity(XMVectorSet(0, 0, 0, 0));

			m_pSphereArray[1]->SetPosition(XMVectorSet(-15, 20, 0, 0));
			m_pSphereArray[1]->SetVelocity(XMVectorSet(0, 0, 0, 0));
		}
	}
	else
	{
		dbF = false;
	}
}

void Application::AddSphere()
{
	if (m_iCurrentSphereIndx < MAX_OBJECTS)
	{
		m_iCurrentSphereIndx++;
		m_pSphereArray[m_iCurrentSphereIndx]->SetActive(true);
		m_pSphereArray[m_iCurrentSphereIndx]->SetPosition(GetRandomPosition());
		m_pSphereArray[m_iCurrentSphereIndx]->SetVelocity(XMVectorSet(0, 0, 0, 0));
	}

}

void Application::RemoveSphere()
{
	if (m_iCurrentSphereIndx != 0)
	{
		m_pSphereArray[m_iCurrentSphereIndx]->SetActive(false);
		m_iCurrentSphereIndx--;
	}
}

XMVECTOR Application::GetRandomPosition()
{
	XMFLOAT3 newPos = XMFLOAT3((float)((rand() % 24 - 12.0f) - 0.5), 22.0f, (float)((rand() % 24 - 12.0f) - 0.5));
	return XMVectorSet(newPos.x, newPos.y, newPos.z, 1);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application application;

	Run(&application);

	return 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
