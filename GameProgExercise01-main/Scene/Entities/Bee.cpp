#include "GameProgExercise01.h"
#include "DX\DeviceResources.h"

#include "Core.h"
#include "Bee.h"

#include "Utils\File.h"
#include "Utils\Maths.h"
#include "Utils\Timers.h"

#include "Scene\EntityHelper.h"
#include "Scene\Entities\Flower.h"


namespace scene
{
	const UINT	Bee::MinSpeed = 2;
	const UINT	Bee::MaxSpeed = 5;
	const float Bee::LerpRate = 5.0f;
	const float Bee::DropRate = 2.0f;
	const XMFLOAT3	Bee::BeeScaleLimit = { 0.4f, 0.4f, 0.6f };
	const XMFLOAT3	Bee::BeeScale = { 0.2f, 0.2f, 0.4f };

	const float	Bee::StomachSizeMax = 100.0f;
	const float	Bee::NectarEatRate = 30.0f;

	static const float	DefaultHeight = 5.0f; // the height the bee will return to after eating nectar

	Bee::Bee(Scene* scene)
	{
		m_currentScene = scene;

		m_exitPositionWorld = XMVECTOR{ 0.0f,0.0f,0.0f };
		m_exitPositionFlower = XMVECTOR{ 0.0f,0.0f,0.0f };

		m_desiredDirection = XMVECTOR{ 0.0f,0.0f,0.0f };
		m_flowerPosition = XMVECTOR{ 0.0f,0.0f,0.0f };
		m_flowerDropRadius = XMVECTOR{ 4.0f,4.0f,4.0f };
		m_flowerFinalRadius = XMVECTOR{ 0.5f,0.5f,0.5f };

		m_currentFlower = NULL;

		m_numVertices = 0;

		m_speed = 1.0f;
		m_defaultHeight = 5.0f;
		m_stomachSizeCurrent = 0.0f;


		m_isFlowerSelcted = false;
		m_markedForDeath = false;
		m_bellyFull = false;

		m_shaderType = Scene::ShaderTypes::Lit;

		m_currentMode = scene::Bee::SPAWN;

	}

	void Bee::Initialise()
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


		m_currentMode = scene::Bee::SPAWN;

		SetScale(BeeScale);

	}

	void Bee::Shutdown()
	{
		Entity::Shutdown();
	}

	void Bee::Update()
	{
		Entity::Update();

		if (!CheckHasFlower())
		{
			SetTargetFlower(m_currentScene->HighNectarFlower());
		}

		switch (m_currentMode)
		{
		case scene::Bee::SPAWN:
			Spawn();
			break;
		case scene::Bee::TO_FLOWER:
			ToFlower();
			break;
		case scene::Bee::COLLECT_NECTAR:
			CollectNectar();
			break;
		case scene::Bee::FROM_FLOWER:
			FromFlower();
			break;
		case scene::Bee::TO_END_POINT:
			ToEndPoint();
			break;
		case scene::Bee::DIE:
			Die();
			break;
		default:
			break;
		}


	}

	void Bee::Render()
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

	void Bee::SetExitPosition(DirectX::XMVECTOR position)
	{
		m_exitPositionWorld = position;
		m_defaultHeight = position.m128_f32[1];
	}

	void Bee::SetOrintationLarp()
	{
		float lerpAmount = LerpRate * utils::Timers::GetFrameTime();

		if (lerpAmount > 1.0f)
		{
			lerpAmount = 1.0f;
		}

		XMVECTOR newOrientation = XMVectorLerp(m_orientation.r[2], m_desiredDirection, lerpAmount);
		newOrientation = XMVector3Normalize(newOrientation);
		SetOrientation(newOrientation);
	}

	void Bee::SetTargetFlower(Flower* selectedFlower)
	{
		m_currentFlower = selectedFlower;

		m_flowerPosition = m_currentFlower->GetPosition();		
		m_flowerPosition.m128_f32[1] = m_defaultHeight;
		m_desiredDirection = XMVector3Normalize(m_flowerPosition - m_position);
		SetOrintationLarp();
		XMVECTOR newPosition = (m_position + (m_desiredDirection * m_speed) * utils::Timers::GetFrameTime());
		SetPosition(newPosition);

		m_isFlowerSelcted = true;
	}

	/* Enum function */
	void Bee::Spawn()
	{
		m_speed = (float)(utils::Rand() % 1000000) / 1000000.0f * (MaxSpeed - MinSpeed) + MinSpeed;

		if (CheckHasFlower())
		{
			m_currentMode = scene::Bee::TO_FLOWER;
		}
	}
	void Bee::ToFlower()
	{
		
		if (m_currentFlower->IsFlowerEmpty())
		{
			m_isFlowerSelcted = false;
		}
		else
		{
			/* Checks if the Bee is in wihtin a N radius of the flower to begin decent  else go to flower */
			if (XMVector3NearEqual(m_position, m_flowerPosition, m_flowerDropRadius))
			{
				m_exitPositionFlower = m_exitPositionWorld;
				m_exitPositionFlower.m128_f32[1] = m_defaultHeight;

				m_currentMode = scene::Bee::COLLECT_NECTAR;
			}
			else
			{
				m_desiredDirection = XMVector3Normalize(m_flowerPosition - m_position);
				SetOrintationLarp();

				XMVECTOR newPosition = (m_position + (m_desiredDirection * m_speed) * utils::Timers::GetFrameTime());
				SetPosition(newPosition);
			}
		}

	}
	void Bee::CollectNectar()
	{

		/* Checks if the Bee is in near the Flower to begin Nectar counsumption else go to flower */
		if (XMVector3NearEqual(m_position, m_flowerPosition, m_flowerFinalRadius))
		{

			if (m_currentFlower->IsFlowerEmpty())
			{
				m_currentMode = scene::Bee::TO_FLOWER;
			}
			else
			{

				if (m_stomachSizeCurrent >= StomachSizeMax)
				{
					m_bellyFull = true;
					m_currentMode = scene::Bee::FROM_FLOWER;
				}
				else
				{
					m_currentFlower->ReduceNectar();
					m_stomachSizeCurrent += NectarEatRate * utils::Timers::GetFrameTime();

					XMFLOAT3 newBeeScale = {	BeeScale.x + ((BeeScaleLimit.x - BeeScale.x) * (m_stomachSizeCurrent / StomachSizeMax)),
												BeeScale.y + ((BeeScaleLimit.y - BeeScale.y) * (m_stomachSizeCurrent / StomachSizeMax)),
												BeeScale.z + ((BeeScaleLimit.z - BeeScale.z) * (m_stomachSizeCurrent / StomachSizeMax))};
					SetScale(newBeeScale);
				}
			}
		}
		else
		{
			if (m_flowerPosition.m128_f32[1] >= m_currentFlower->GetFlowerHeight())
			{
				m_flowerPosition.m128_f32[1] -= DropRate;
			}

			m_desiredDirection = XMVector3Normalize(m_flowerPosition - m_position);
			SetOrintationLarp();

			XMVECTOR newPosition = (m_position + (m_desiredDirection * m_speed) * utils::Timers::GetFrameTime());
			SetPosition(newPosition);

		}		
	}
	void Bee::FromFlower()
	{


		/* When leaving flower go to the origin spawn height,
		   While mantaing the same direction as the exit point */
		if (XMVector3NearEqual(m_position, m_exitPositionFlower, m_flowerDropRadius))
		{
			if (!m_bellyFull)
			{
				m_isFlowerSelcted = false;
						
				
				m_currentMode = scene::Bee::TO_FLOWER;

			}
			else
			{
				m_currentMode = scene::Bee::TO_END_POINT;
			}
		}
		else
		{	
			if (m_exitPositionFlower.m128_f32[1] <= m_exitPositionWorld.m128_f32[1])
			{
				m_exitPositionFlower.m128_f32[1] += DropRate * utils::Timers::GetFrameTime();
			}
			m_desiredDirection = XMVector3Normalize(m_exitPositionFlower - m_position);
			SetOrintationLarp();

			XMVECTOR newPosition = (m_position + (m_desiredDirection * m_speed) * utils::Timers::GetFrameTime());
			SetPosition(newPosition);
		}
	}
	void Bee::ToEndPoint()
	{

		/* Go to the exit point once near start despawn process */
		if (XMVector3NearEqual(m_position, m_exitPositionWorld, m_flowerFinalRadius))
		{
			m_currentMode = scene::Bee::DIE;
		}
		else
		{
			m_desiredDirection = XMVector3Normalize(m_exitPositionWorld - m_position);
			SetOrintationLarp();

			XMVECTOR newPosition = (m_position + (m_desiredDirection * m_speed) * utils::Timers::GetFrameTime());
			SetPosition(newPosition);
		}

	}
	void Bee::Die()
	{
		m_markedForDeath = true;
	}

} // namespace scene