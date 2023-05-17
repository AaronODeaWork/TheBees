#pragma once

#include "Scene\Entity.h"

using namespace DirectX;

namespace scene
{
	class Flower;
	class Scene;

	class Bee : public Entity
	{
	public:
		Bee(Scene* scene);

		void				Initialise();
		void				Shutdown();

		void				Update();
		void				Render();

		void				SetOrintationLarp();

		void				SetExitPosition(DirectX::XMVECTOR position);
		DirectX::XMVECTOR	inline	GetExitPosition()	{ return m_exitPositionWorld; };
		bool				inline	CheckHasFlower()	{ return m_isFlowerSelcted; };
		bool				inline	CheckForDeathMark() { return m_markedForDeath;  };

		Flower* 			GetTargetFlower() { return m_currentFlower; };
		void				SetTargetFlower(Flower* selectedFlower);


	private:

		void Spawn();// actual Spawn and set up handled in scene
		void ToFlower();
		void CollectNectar();
		void FromFlower();
		void ToEndPoint();
		void Die(); // actual deleation is handled in scene



		XMVECTOR	m_exitPositionWorld;
		XMVECTOR	m_exitPositionFlower;

		XMVECTOR	m_desiredDirection;
		XMVECTOR	m_flowerPosition;

		XMVECTOR	m_flowerDropRadius;
		XMVECTOR	m_flowerFinalRadius;

		static const XMFLOAT3	BeeScale;

		static const XMFLOAT3	BeeScaleLimit;

		enum MODE { SPAWN, 
					TO_FLOWER,
					COLLECT_NECTAR,
					FROM_FLOWER,
					TO_END_POINT,
					DIE };

		MODE m_currentMode;

		UINT m_numVertices;


		static const UINT	MinSpeed;
		static const UINT	MaxSpeed;

		bool		m_isFlowerSelcted;
		bool		m_markedForDeath;
		bool		m_bellyFull;

		float		m_speed;
		float		m_defaultHeight; // the height the bee will return to after eating nectar

		static const float	LerpRate;
		static const float	DropRate;

		static const float	StomachSizeMax;
		float	m_stomachSizeCurrent;
		static const float	NectarEatRate;


		Scene*	m_currentScene;
		Flower* m_currentFlower;


	};

} // namespace scene
