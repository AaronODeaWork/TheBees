#pragma once
#include <Containers\List.h>

namespace scene
{

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
	Flower*				HighNectarFlower();

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



	//Colours
	static CONST XMFLOAT4 GROUND_COLOUR;
	static CONST XMFLOAT4 BEE_COLOUR;

	// Scene Objects 
	GroundObject*	m_groundPlane;
	Camera*			m_camera;

	containers::List<Flower*>	m_flowerList;
	containers::List<Bee*>		m_beeList;

	// Flowers
	XMVECTOR	m_flowerGridStart; //The flower grids origin point is its bottom left corner
	XMVECTOR	m_flowerGridOrigin;
	float		m_flowerDoubleScale;
	float		m_flowerScale;
	static const int	FlowersPerRow; // This is used to create a N*N flower grid;

	// Ground
	static const float	GroundThickness;

	// Bee
	static const int	MinBees;
	static const int	MaxBees;
	static const int	BeeSpawnHeightMax;
	static const int	BeeSpawnHeightMin;

	float		m_beeSpawnTimerCurrent;
	float		m_beeSpawnTimerPrevious;
	int			m_aliveBeeCount;

	static const float	MinBeeSpawnRadius;
	static const float	MaxBeeSpawnRadius;
	static const float	BeeSpawnInterval;

};

} // namespace scene