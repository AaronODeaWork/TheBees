#pragma once

#include "Scene\Entity.h"

using namespace DirectX;

namespace scene
{
	class Flower;
	class Bee : public Entity
	{
	public:
		Bee();

		void				Initialise();
		void				Shutdown();

		void				Update();
		void				Render();

		DirectX::XMVECTOR	GetExitPosition();
		void				SetExitPosition(DirectX::XMVECTOR position);
		bool				CheckHasFlower();
		bool				CheckForDeathMark();

		void				SetOrintationLarp();

		void				SetFlower(Flower* selectedFlower);
		Flower*				GetFlower();
							

	private:

		void Spawn();// actual Spawn and set up handled in scene
		void ToFlower();
		void CollectNectar();
		void FromFlower();
		void ToEndPoint();
		void Die(); // actual deleation is handled in scene



		enum MODE { SPAWN, TO_FLOWER, COLLECT_NECTAR, FROM_FLOWER, TO_END_POINT, DIE };
		MODE m_currentMode;

		UINT m_numVertices = 0;

		XMVECTOR	m_exitPositionWorld;
		XMVECTOR	m_exitPositionFlower;

		XMVECTOR	m_desiredDirection;
		XMVECTOR	m_flowerPosition;

		XMVECTOR	m_flowerDropRadius;
		XMVECTOR	m_flowerFinalRadius;

		XMFLOAT3	m_BeeScale = XMFLOAT3(0.2f, 0.2f, 0.4f);

		const UINT	m_minSpeed = 2;
		const UINT	m_maxSpeed = 5;

		bool		m_isFlowerSelcted = false;
		bool		m_markedForDeath = false;
		bool		m_bellyFull = false;

		float		m_speed;
		float		m_DefaultHeight = 5.0f;
		float		m_lerpRate = 5.0f;
		float		m_DropRate = 2.0f;

		float		m_stomachSizeMax = 100.0f;
		float		m_stomachSizeCurrent = 0.0f;
		float		m_eatRate = 10.0f;



		Flower* m_currentFlower;


	};

} // namespace scene
