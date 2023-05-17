#include "GameProgExercise01.h"
#include "DX\DeviceResources.h"

#include "Utils\Maths.h"

#include "Core.h"
#include "Flower.h"
#include "Scene\EntityHelper.h"

namespace scene
{

	Flower::Flower()
	{
		m_shaderType = Scene::ShaderTypes::Lit;
		m_nectarValue = 0;
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

		m_nectarValue = (utils::Rand() % (m_maxNectar - m_minNectar))+ m_minNectar;
		


		utils::Timers::InitialiseTimers();
		utils::Timers::StartTimer();
		m_flowerSpawnTimerCurrent = utils::Timers::GetFrameTime();
		m_flowerSpawnTimerPrevious = utils::Timers::GetFrameTime();

		m_baseScale = scale;
		m_flowerScale.x = m_baseScale * ((float)m_nectarValue / (float)m_maxNectar);
		m_flowerScale.y = m_baseScale * ((float)m_nectarValue / (float)m_maxNectar);
		m_flowerScale.z = m_baseScale * (float)(m_nectarValue / (float)m_maxNectar);

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
			if ((m_flowerSpawnTimerCurrent - m_flowerSpawnTimerPrevious) >= m_flowerespawnInterval)
			{
				if (m_nectarValue < m_maxNectar)
				{
					m_nectarValue += (utils::Rand() % (m_maxRefill - m_minRefill)) + m_minRefill;
					m_flowerScale.x = m_baseScale * ((float)m_nectarValue / (float)m_maxNectar);
					m_flowerScale.y = m_baseScale * ((float)m_nectarValue / (float)m_maxNectar);
					m_flowerScale.z = m_baseScale * (float)(m_nectarValue / (float)m_maxNectar);

					SetScale(m_flowerScale);
				}
				else
				{
					m_nectarValue = m_maxNectar;
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

	int Flower::GetNectar()
	{
		return m_nectarValue;
	}

	void Flower::SetColourByNectar()
	{
		
		
		if (m_nectarValue != m_PreviousNectarValue)
		{
			float tempaturePercentage = ((float)m_nectarValue / (float)m_maxNectar);
			XMFLOAT4 m_colourBase = { 1.0f, 1.0f, 1.0f, 1.0f };

			switch (m_colourMode)
			{
			case scene::Flower::PURPLE:
				m_colourBase = m_colourBasePurple;
				break;
			case scene::Flower::RED:
				m_colourBase = m_colourBaseRed;
				break;
			case scene::Flower::BlUE:
				m_colourBase = m_colourBaseBlue;
				break;
			case scene::Flower::ORANGE:
				m_colourBase = m_colourBaseOrange;
				break;
			default:
				m_colourBase = m_colourBaseOrange;
				break;
			}

			m_PreviousNectarValue = m_nectarValue;
			XMFLOAT4 colourUpdate = { m_colourBase.x * tempaturePercentage,
									  m_colourBase.y * tempaturePercentage,
									  m_colourBase.z * tempaturePercentage, 1.0f };
			SetColour(colourUpdate);

			if (m_nectarValue <= 0)
			{
				m_flowerRefill = true;
				RandomColour();
			}
		}
	}

	void Flower::ReduceNectar()
	{
		if (m_nectarValue > 0)
		{
			m_nectarValue -= m_nectarReduction;
			m_flowerScale.x = m_baseScale * ((float)m_nectarValue / (float)m_maxNectar);
			m_flowerScale.y = m_baseScale * ((float)m_nectarValue / (float)m_maxNectar);
			m_flowerScale.z = m_baseScale * (float)(m_nectarValue / (float)m_maxNectar);

			SetScale(m_flowerScale);
		}
		else
		{
			m_flowerEmpty = true;
		}
	}

	bool Flower::IsflowerEmpty()
	{

		return m_flowerEmpty;
	}

	int Flower::GetFlowerHeight()
	{
		return m_flowerHeight;
	}

	void Flower::RandomColour()
	{
		int randomColour = utils::Rand() % m_numberOfColour;
		switch (randomColour)
		{
		default:
		case 0:
			m_colourMode = scene::Flower::PURPLE;
			break;
		case 1:
			m_colourMode = scene::Flower::RED;
			break;
		case 2:
			m_colourMode = scene::Flower::BlUE;
			break;
		case 3:
			m_colourMode = scene::Flower::ORANGE;
			break;
		}
	}

} // namespace scene