#include "GameProgExercise01.h"
#include "DX\DeviceResources.h"
#include "Core.h"
#include "Utils\File.h"
#include "Utils\Maths.h"
#include "Utils\Timers.h"

#include "Scene\Scene.h"

#include "Scene\Entities\GroundObject.h"
#include "Scene\Entities\Flower.h"
#include "Scene\Entities\Bee.h"
#include "Scene\Entities\Camera.h"



#include "DX\Input.h"
#include "DX\View.h" 

using namespace DirectX;

namespace scene
{

Scene::Scene()
{
	// objects
	m_groundPlane = new GroundObject();
	m_camera = new Camera();

	// bee
	m_aliveBeeCount = 0;

	// flower
	m_flowerScale = 1.0f;
	m_flowerDoubleScale = m_flowerScale * 2.0f;
	m_flowerGridStart = XMVECTOR{ 0.0f, 1.0f, 0.0f };
	// we are doing this so we always get the center of the Flower grid
	m_flowerGridOrigin = XMVECTOR{ -((m_flowerDoubleScale * m_flowerScale) * (m_flowersPerRow - 1.0f)),
								   0.0f,
								 -((m_flowerDoubleScale * m_flowerScale) * (m_flowersPerRow - 1.0f)) };	
	
}

Scene::~Scene()
{	

	delete m_groundPlane;
	delete m_camera;
}

void Scene::Initialise()
{	
	
	utils::Timers::InitialiseTimers();
	utils::Timers::StartTimer();
	m_beeSpawnTimerCurrent = utils::Timers::GetFrameTime();
	m_beeSpawnTimerPrevious = utils::Timers::GetFrameTime();

	InitialiseShaders();
	InitialiseGround();
	InitialiseFlowers();



	m_camera->Initialise();
	m_camera->SetOrigin(XMVectorSet(m_flowerGridOrigin.m128_f32[0], m_groundThickness, m_flowerGridOrigin.m128_f32[2], 1.0f));
}

void Scene::Shutdown()
{
	m_groundPlane->Shutdown();

	containers::List< Flower*>::iterator flowerIterator = m_flowerList.begin();
	while (flowerIterator != m_flowerList.end())
	{
		Flower* flower = *flowerIterator;
		flower->Shutdown();
		delete flower;
		flowerIterator++;
	}
	m_flowerList.clear();

	containers::List< Bee*>::iterator beeIterator = m_beeList.begin();
	while (beeIterator != m_beeList.end())
	{
		Bee* bee = *beeIterator;
		bee->Shutdown();
		delete bee;
		beeIterator++;
	}
	m_beeList.clear();

	for( UINT shaderTypeIndex = 0; shaderTypeIndex < ShaderTypes::NumShaderTypes; ++shaderTypeIndex )
	{
		if( m_shaderData[ shaderTypeIndex ].inputLayout != nullptr )
		{
			m_shaderData[ shaderTypeIndex ].inputLayout->Release();
		}
		if( m_shaderData[ shaderTypeIndex ].vertexShader != nullptr )
		{
			m_shaderData[ shaderTypeIndex ].vertexShader->Release();
		}
		if( m_shaderData[ shaderTypeIndex ].pixelShader != nullptr )
		{
			m_shaderData[ shaderTypeIndex ].pixelShader->Release();
		}
	}
}

void Scene::Update()
{
	m_camera->Update();
	m_groundPlane->Update();
	UpdateBees();
	UpdateFlowers();
}

void Scene::Render()
{
	m_groundPlane->Render();
	
	containers::List< Flower*>::iterator flowerIterator = m_flowerList.begin();
	while (flowerIterator != m_flowerList.end())
	{
		Flower* flower = *flowerIterator;
		flower->Render();
		flowerIterator++;
	}
	
	containers::List< Bee*>::iterator beeIterator = m_beeList.begin();
	while (beeIterator != m_beeList.end())
	{
		Bee* bee = *beeIterator;
		bee->Render();
		beeIterator++;
	}

}

void Scene::InitialiseShaders()
{

	Core* const core = Core::Get();

	const DX::DeviceResources* const deviceResources = core->GetDeviceResources();

	HRESULT hr = 0;
	DWORD size = 0;

	ID3D11Device* const device = deviceResources->GetD3DDevice();

	void* vertShaderData = nullptr;

	HANDLE vsHandle = INVALID_HANDLE_VALUE;
	HANDLE psHandle = INVALID_HANDLE_VALUE;

	// ================== UNLIT SHADERS ==================
	// Load and create the vertex shader for the unlit type of vertices
	vsHandle = utils::file::GetFileData("VertexShader.cso", &vertShaderData, &size);

	hr = device->CreateVertexShader(vertShaderData, size, nullptr, &m_shaderData[ShaderTypes::Unlit].vertexShader);
	ASSERT_HANDLE(hr);

	// Create input layout.
	static const D3D11_INPUT_ELEMENT_DESC s_inputElementDescUnlit[2] =
	{
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA,  0 },
		{ "COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA , 0 },
	};

	hr = device->CreateInputLayout(s_inputElementDescUnlit, _countof(s_inputElementDescUnlit),
		vertShaderData, size,
		&m_shaderData[ShaderTypes::Unlit].inputLayout);
	ASSERT_HANDLE(hr);

	utils::file::CloseFile(vsHandle);

	void* pixelShaderData = nullptr;

	// Handle loading and creating the pixel shader for the unlit type of vertices
	psHandle = utils::file::GetFileData("PixelShader.cso", &pixelShaderData, &size);

	hr = device->CreatePixelShader(pixelShaderData, size,
		nullptr, &m_shaderData[ShaderTypes::Unlit].pixelShader);
	ASSERT_HANDLE(hr);

	utils::file::CloseFile(psHandle);

	// ================== LIT SHADERS ==================
	// Load and create the vertex shader for the unlit type of vertices
	vsHandle = utils::file::GetFileData("VertexShaderLit.cso", &vertShaderData, &size);

	hr = device->CreateVertexShader(vertShaderData, size,
		nullptr, &m_shaderData[ShaderTypes::Lit].vertexShader);
	ASSERT_HANDLE(hr);

	// Create input layout.
	static const D3D11_INPUT_ELEMENT_DESC s_inputElementDescLit[3] =
	{
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA,  0 },
		{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16,  D3D11_INPUT_PER_VERTEX_DATA,  0 },
		{ "COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA , 0 }
	};

	hr = device->CreateInputLayout(s_inputElementDescLit, _countof(s_inputElementDescLit),
		vertShaderData, size,
		&m_shaderData[ShaderTypes::Lit].inputLayout);
	ASSERT_HANDLE(hr);

	utils::file::CloseFile(vsHandle);

	pixelShaderData = nullptr;

	// Handle loading and creating the pixel shader for the unlit type of vertices
	psHandle = utils::file::GetFileData("PixelShaderLit.cso", &pixelShaderData, &size);

	hr = device->CreatePixelShader(pixelShaderData, size,
		nullptr, &m_shaderData[ShaderTypes::Lit].pixelShader);
	ASSERT_HANDLE(hr);

	utils::file::CloseFile(psHandle);

}
void Scene::InitialiseGround()
{
	m_groundPlane->Initialise();
	m_groundPlane->SetColour(GROUND_COLOUR);
	m_groundPlane->SetPosition(XMVectorSet(m_flowerGridOrigin.m128_f32[0], m_flowerGridOrigin.m128_f32[1], m_flowerGridOrigin.m128_f32[2], 0.0f));

	m_groundPlane->SetScale(XMFLOAT3(((m_flowerDoubleScale * m_flowerScale) * m_flowersPerRow),
									   m_groundThickness,
									 ((m_flowerDoubleScale * m_flowerScale) * m_flowersPerRow)));
}
void Scene::InitialiseFlowers()
{

	for (int i = 0; i < (m_flowersPerRow * m_flowersPerRow); i++)
	{
		Flower* flower = new Flower();
		m_flowerList.push_back(flower);
	}

	float flowerGap = m_flowerDoubleScale * m_flowerDoubleScale;
	
	containers::List< Flower*>::iterator iterator = m_flowerList.begin();
	for (int i = 0; i < m_flowersPerRow; i++)
	{
		for (int j = 0; j < m_flowersPerRow; j++)
		{
			Flower* flower = *iterator;
			flower->Initialise(m_flowerScale);
			flower->RandomColour();
			flower->SetPosition(XMVectorSet((m_flowerGridStart.m128_f32[0] - (flowerGap * i)),
											(m_flowerGridStart.m128_f32[1]),
											(m_flowerGridStart.m128_f32[2] - (flowerGap * j)), 1.0f));
			iterator++;
		}
	}
}

void Scene::ActivateShaders(const ShaderTypes shaderType)
{
	ASSERT(shaderType < ShaderTypes::NumShaderTypes, "Shader type ID is out of range.\n");
	Core* const core = Core::Get();

	const DX::DeviceResources* const deviceResources = core->GetDeviceResources();

	ID3D11DeviceContext* const deviceContext = deviceResources->GetD3DDeviceContext();

	// Set input assembler state.
	deviceContext->IASetInputLayout(m_shaderData[shaderType].inputLayout);

	// Set shaders.
	deviceContext->VSSetShader(m_shaderData[shaderType].vertexShader, nullptr, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->PSSetShader(m_shaderData[shaderType].pixelShader, nullptr, 0);
}

void Scene::UpdateBees()
{
	// Spawn bees here 
	if ((m_beeSpawnTimerCurrent - m_beeSpawnTimerPrevious) >= m_beeSpawnInterval)
	{
		if (m_aliveBeeCount < m_minBees)
		{
			SpawnBee();
		}
		else if (m_aliveBeeCount < m_maxBees && (utils::Rand()% 2) == 0)
		{
			SpawnBee();
		}
		m_beeSpawnTimerPrevious = m_beeSpawnTimerCurrent;
	}
	else
	{
		m_beeSpawnTimerCurrent += utils::Timers::GetFrameTime();
	}

	containers::List< Bee*>::iterator beeIterator = m_beeList.begin();
	while (beeIterator != m_beeList.end())
	{
		Bee* bee = *beeIterator;
		bee->Update();
		if (!bee->CheckHasFlower())
		{
			bee->SetFlower(HighNectarFlower());
		}
		if (bee->CheckForDeathMark())
		{
			bee->Shutdown();
			delete bee;
			beeIterator = m_beeList.erase(beeIterator);
			m_aliveBeeCount--;
		}
		else
		{
			beeIterator++;
		}
	}
}
void Scene::UpdateFlowers()
{
	containers::List< Flower*>::iterator flowerIterator = m_flowerList.begin();

	while (flowerIterator != m_flowerList.end())
	{
		Flower* flower = *flowerIterator;
		flower->Update();
		flowerIterator++;
	}
}
void Scene::SpawnBee()
{

	Bee* bee = new Bee();
	bee->Initialise();
	bee->SetColour(BEE_COLOUR);
	
	float angle = (float)(utils::Rand() % 1000000) / 1000000.0f * XM_2PI;
	float distanceFromCentre = (float)(utils::Rand() % 1000000) / 1000000.0f * (m_maxBeeSpawnRadius - m_minBeeSpawnRadius) + m_minBeeSpawnRadius;
	float beePositionX = XMScalarSin(angle) * m_maxBeeSpawnRadius;
	float beePositionY = (float)((utils::Rand() % (m_beeSpawnHeightMax- m_beeSpawnHeightMin)) + m_beeSpawnHeightMin);
	float beePositionZ = XMScalarCos(angle) * m_maxBeeSpawnRadius;

	// TODO ADD IN A BEE HEIGHT
  	bee->SetPosition(		XMVectorSet( beePositionX + (float)m_flowerGridOrigin.m128_f32[0], beePositionY,  beePositionZ + (float)m_flowerGridOrigin.m128_f32[2], 1.0f));
	bee->SetExitPosition(	XMVectorSet(-beePositionX + (float)m_flowerGridOrigin.m128_f32[0], beePositionY, -beePositionZ + (float)m_flowerGridOrigin.m128_f32[2], 1.0f));	
	bee->SetFlower(HighNectarFlower());

	m_beeList.push_back(bee);

	m_aliveBeeCount++;
}

Flower* Scene::HighNectarFlower()
{
	int highestNectar = 0;
	int currentFlowerNectar = 0;

	containers::List< Flower*>::iterator flowerIterator = m_flowerList.begin();
	Flower* selectedFlower = *flowerIterator;

	while (flowerIterator != m_flowerList.end())
	{
		Flower* currentFlower = *flowerIterator;
		currentFlowerNectar = currentFlower->GetNectar();

		if (highestNectar < currentFlowerNectar)
		{
			highestNectar = currentFlowerNectar;
			selectedFlower = currentFlower;
		}

		else if (highestNectar == currentFlowerNectar)
		{
			if ((utils::Rand() % 2) == 0)
			{
				highestNectar = currentFlowerNectar;
				selectedFlower = currentFlower;
			}			
		}

		flowerIterator++;
	}

	return selectedFlower;
}


} // namespace scene