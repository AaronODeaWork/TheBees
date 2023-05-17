#pragma once

#include "Scene\Entity.h"

namespace scene
{

	class Flower : public Entity
	{
	public:
		Flower();

		void				Initialise(float scale);
		void				Shutdown();
		void				Update();
		void				Render();

		void				SetColourByNectar();
		void				ReduceNectar();
		bool		inline	IsFlowerEmpty()		{ return m_flowerEmpty; };
		float		inline	GetNectar()			{ return m_nectarValue; };
		float		inline	GetFlowerHeight()	{ return m_flowerHeight;};

	private:
		
		XMFLOAT3	m_flowerScale; // used to rescale the flower based on nectar si

		UINT		m_numVertices;

		bool		m_flowerEmpty;
		bool		m_flowerRefill;

		static const float	FloweRespawnInterval;
		static const float	FloweForceGrowthInterval;

		static const float	NectarReduction;

		static const  float	 MaxNectar;
		static const  float	 MinNectar;

		static const  float	 MaxRefill;
		static const  float	 MinRefill;

		static const int	NumberOfColour = 4; // to add more colours you will have to add case in RandomColour()
		static const XMFLOAT4 FlowerColours[NumberOfColour];

		float		m_flowerSpawnTimerCurrent;
		float		m_flowerSpawnTimerPrevious;
		float		m_nectarValue;
		float		m_PreviousNectarValue;
		float		m_flowerHeight;
		float		m_baseScale;

		int			m_currentColour;

	};

} // namespace scene
