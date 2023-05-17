#pragma once
#include <Containers\List.h>

namespace scene
{

class TestObject;

class GroundObject;
class Camera;
class Flower;
class Bee;
class List;

using namespace DirectX;


// Object containing all the entities and related objects in the scene
class Scene sealed
{
public:
	enum ShaderTypes
	{
		Unlit = 0,
		Lit,

		NumShaderTypes
	};

	Scene();
	~Scene();

	void				Initialise();
	void				Shutdown();
	void				Update();
	void				Render();

	void				ActivateShaders( const ShaderTypes shaderType );

private:
	struct ShaderData
	{
		ShaderData():	inputLayout( nullptr ),
						vertexShader( nullptr ),
						pixelShader( nullptr )
		{
		}

		ID3D11InputLayout* inputLayout;
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
	};
	
	ShaderData m_shaderData[ ShaderTypes::NumShaderTypes ];
	
	void				InitialiseShaders();
	void				InitialiseGround();
	void				InitialiseFlowers();

	void				UpdateBees();
	void				UpdateFlowers();

	void				SpawnBee();
	Flower*				HighNectarFlower();



	//Colours
	CONST XMFLOAT4 GROUND_COLOUR = { 0.2f, 0.6f, 0.0f, 1.0f};
	CONST XMFLOAT4 BEE_COLOUR = { 1.0f, 1.0f, 0.0f, 1.0f };

	// Scene Objects 
	GroundObject*	m_groundPlane;
	Camera*			m_camera;

	containers::List<Flower*>	m_flowerList;
	containers::List<Bee*>		m_beeList;

	// Flowers
	XMVECTOR	m_flowerGridStart; //The flower grids origin point is its bottom left corner
	XMVECTOR	m_flowerGridOrigin;
	const int	m_flowersPerRow = 5; // This is used to create a N*N flower grid;
	float		m_flowerDoubleScale;
	float		m_flowerScale;

	// Ground
	const float m_groundThickness = 1.0f;

	// Bee
	const int	m_minBees = 10;
	const int	m_maxBees = 50;
	const int	m_beeSpawnHeightMax = 8;
	const int	m_beeSpawnHeightMin = 5;
	const float m_minBeeSpawnRadius = 8.0f;
	const float m_maxBeeSpawnRadius = 15.0f;
	const float m_beeSpawnInterval = 1.0f;
	float		m_beeSpawnTimerCurrent;
	float		m_beeSpawnTimerPrevious;
	int			m_aliveBeeCount;


	TestObject* m_testObject1;
};

} // namespace scene