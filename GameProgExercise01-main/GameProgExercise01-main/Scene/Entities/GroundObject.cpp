#include "GameProgExercise01.h"
#include "DX\DeviceResources.h"

#include "Core.h"
#include "GroundObject.h"
#include "Scene\EntityHelper.h"

namespace scene
{

	GroundObject::GroundObject()
	{
		m_shaderType = Scene::ShaderTypes::Lit;
		m_groundEdgePadding = 2.0f;
	}

	void GroundObject::Initialise()
	{
		Entity::Initialise();

		Core* const core = Core::Get();

		const DX::DeviceResources* const deviceResources = core->GetDeviceResources();

		HRESULT hr = 0;

		ID3D11Device* const device = deviceResources->GetD3DDevice();

		// Create vertex buffer.
		const Entity::VertexLit* cubeVertices = helper::GetCubeVertices(m_numVertices);
		


		D3D11_SUBRESOURCE_DATA initialData = {};
		//initialData.pSysMem = cubeVertices;
		initialData.pSysMem = cubeVertices;

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(VertexLit)* m_numVertices;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.StructureByteStride = sizeof(VertexLit);

		// Note the vertex buffer is released in the base class
		hr = device->CreateBuffer(&bufferDesc, &initialData, &m_vertexBuffer);
		ASSERT_HANDLE(hr);

		SetScale(1.0f);
	}

	void GroundObject::Shutdown()
	{
		Entity::Shutdown();
	}

	void GroundObject::Update()
	{
		Entity::Update();
	}

	void GroundObject::Render()
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

	float GroundObject::GetEdgePadding()
	{
		return m_groundEdgePadding;
	}

} // namespace scene