#include "GameProgExercise01.h"
#include "DX\DeviceResources.h"

#include "Utils\Maths.h"

#include "Core.h"
#include "Flower.h"
#include "Scene\EntityHelper.h"

namespace scene
{
	const float Flower::FloweRespawnInterval = 0.8f;
	const float Flower::FloweForceGrowthInterval = 10.0f;
	const float	Flower::NectarReduction = 10.0f;
	const XMFLOAT4 Flower::FlowerColours[NumberOfColour] =
	{
		{ 0.3f, 0.1f, 0.5f, 1.0f },
		{ 0.9f, 0.2f, 0.4f, 1.0f },
		{ 0.2f, 0.1f, 0.9f, 1.0f },
		{ 0.9f, 0.5f, 0.1f, 1.0f }
	};

	const  float Flower::MaxNectar = 100.0f;
	const  float Flower::MinNectar = 20.0f;
	const  float Flower::MaxRefill = 16.0f;
	const  float Flower::MinRefill = 8.0f;

	Flower::Flower()
	{
		m_flowerScale = XMFLOAT3(1.0f, 1.0f, 1.0f);



		m_numVertices = 0;
		m_flowerHeight = 4.0f;

		m_nectarValue = 0.0f;
		m_PreviousNectarValue = 0.0f;

		m_shaderType = Scene::ShaderTypes::Lit;

		m_flowerSpawnTimerCurrent = 0.0f;
		m_flowerSpawnTimerPrevious = 0.0f;
		m_baseScale = 0.0f;

		m_flowerEmpty = false;
		m_flowerRefill = false;

		m_currentColour = utils::Rand() % NumberOfColour;
	}

	void Flower::Initialise(float scale)
	{
		Entity::Initialise();

		Core* const core = Core::Get();

		const DX::DeviceResources* const deviceResources = core->GetDeviceResources();

		HRESULT hr = 0;

		ID3D11Device* const device = deviceResources->GetD3DDevice();

		// Create vertex buffer.
		const Entity::VertexLit* cubeVertices = helper::GetCubeVertices(m_numVertices);



		D3D11_SUBRESOURCE_DATA initialData = {};
		initialData.pSysMem = cubeVertices;

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(VertexLit) * m_numVertices;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.StructureByteStride = sizeof(VertexLit);

		// Note the vertex buffer is released in the base class
		hr = device->CreateBuffer(&bufferDesc, &initialData, &m_vertexBuffer);
		ASSERT_HANDLE(hr);

		m_nectarValue = (float)(utils::Rand() % (int)(MaxNectar - MinNectar)) + (int)MinNectar;
		


		utils::Timers::InitialiseTimers();
		utils::Timers::StartTimer();
		m_flowerSpawnTimerCurrent = utils::Timers::GetFrameTime();
		m_flowerSpawnTimerPrevious = utils::Timers::GetFrameTime();

		m_baseScale = scale;
		m_flowerScale.x = m_baseScale * (m_nectarValue / MaxNectar);
		m_flowerScale.y = m_baseScale * (m_nectarValue / MaxNectar);
		m_flowerScale.z = m_baseScale * (m_nectarValue / MaxNectar);

		SetScale(m_flowerScale);

	}

	void Flower::Shutdown()
	{
		Entity::Shutdown();
	}

	void Flower::Update()
	{
		Entity::Update();
		/*
		Timer for adding a random amount of nectar to the flower, till its full again
		*/

		if (m_flowerRefill == true)
		{

			if ((m_flowerSpawnTimerCurrent - m_flowerSpawnTimerPrevious) >= FloweRespawnInterval)
			{
				if (m_nectarValue < MaxNectar)
				{
					m_nectarValue += (utils::Rand() % (int)(MaxRefill - MinRefill)) + (int)MinRefill;
					m_flowerScale.x = m_baseScale * (m_nectarValue / MaxNectar);
					m_flowerScale.y = m_baseScale * (m_nectarValue / MaxNectar);
					m_flowerScale.z = m_baseScale * (m_nectarValue / MaxNectar);

					SetScale(m_flowerScale);
				}
				else
				{
					m_nectarValue = MaxNectar;
					m_flowerRefill = false;
					m_flowerEmpty = false;
				}
				m_flowerSpawnTimerPrevious = m_flowerSpawnTimerCurrent;
			}
			else
			{
				m_flowerSpawnTimerCurrent += utils::Timers::GetFrameTime();

			}

		}
		else if ((m_flowerSpawnTimerCurrent - m_flowerSpawnTimerPrevious) >= FloweForceGrowthInterval && m_PreviousNectarValue == m_nectarValue)
		{
			m_flowerRefill = true;
			m_flowerSpawnTimerPrevious = m_flowerSpawnTimerCurrent;

		}
		else
		{
			m_flowerSpawnTimerCurrent += utils::Timers::GetFrameTime();

		}

		SetColourByNectar();

	}

	void Flower::Render()
	{
		// Ensure the base class is called as this sets up the shaders for drawing
		Entity::Render();

		Core* const core = Core::Get();

		const DX::DeviceResources* const deviceResources = core->GetDeviceResources();
		ID3D11DeviceContext* const context = deviceResources->GetD3DDeviceContext();

		// Draw the Cube
		UINT strides = sizeof(VertexLit);
		UINT offsets = 0;

		// Define Directx Contexts 
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &strides, &offsets);
		context->Draw(m_numVertices, 0);
	}

	void Flower::SetColourByNectar()
	{

		if (m_nectarValue != m_PreviousNectarValue)
		{

			float tempaturePercentage = ((float)m_nectarValue / (float)MaxNectar);

			XMFLOAT4 m_colourBase = FlowerColours[m_currentColour];

			m_colourBase = {	m_colourBase.x * tempaturePercentage,
								m_colourBase.y * tempaturePercentage,
								m_colourBase.z* tempaturePercentage, 1.0f };
	
			SetColour(m_colourBase);
			m_PreviousNectarValue = m_nectarValue;

			if (m_nectarValue <= 0)
			{
				m_flowerRefill = true;
				m_currentColour = utils::Rand() % NumberOfColour;

			}
		}
	}

	void Flower::ReduceNectar()
	{
		if (m_nectarValue > 0)
		{
			m_nectarValue -= NectarReduction * utils::Timers::GetFrameTime();
			m_flowerScale.x = m_baseScale * ((float)m_nectarValue / (float)MaxNectar);
			m_flowerScale.y = m_baseScale * ((float)m_nectarValue / (float)MaxNectar);
			m_flowerScale.z = m_baseScale * (float)(m_nectarValue / (float)MaxNectar);

			SetScale(m_flowerScale);

			//We do this to stop it refilling as its been eaten 
			m_flowerRefill = false;
			m_flowerSpawnTimerPrevious = m_flowerSpawnTimerCurrent;
		}
		else
		{
			m_flowerEmpty = true;
		}
	}

} // namespace scene