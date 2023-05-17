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
		bool				IsflowerEmpty();
		int					GetNectar();		
		int					GetFlowerHeight();

		void				RandomColour();
	private:

		UINT		m_numVertices = 0;
		int			m_flowerHeight = 4;

		bool		m_flowerEmpty = false;
		bool		m_flowerRefill = false;

		const float m_flowerespawnInterval = 0.8f;
		const int	m_nectarReduction = 10;
		float		m_flowerSpawnTimerCurrent;
		float		m_flowerSpawnTimerPrevious;
		int			m_nectarValue = 0;
		int			m_PreviousNectarValue = 0;

		int			m_maxNectar = 100;
		int			m_minNectar = 70;

		int			m_maxRefill = 16;
		int			m_minRefill = 8;

		enum MODE { PURPLE, RED , BlUE, ORANGE};
		MODE		m_colourMode = PURPLE;

		const int	m_numberOfColour = 4; // to add more colours you will have to add case in RandomColour()
		XMFLOAT4	m_colourBasePurple = { 0.3f, 0.1f, 0.5f, 1.0f };
		XMFLOAT4	m_colourBaseRed = { 0.9f, 0.2f, 0.4f, 1.0f };
		XMFLOAT4	m_colourBaseBlue = { 0.2f, 0.1f, 0.9f, 1.0f };
		XMFLOAT4	m_colourBaseOrange = { 0.9f, 0.5f, 0.1f, 1.0f };

		float		m_baseScale;
		XMFLOAT3	m_flowerScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	};

} // namespace scene
