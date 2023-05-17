#pragma once

#include "Scene\Entity.h"

namespace scene
{

	class GroundObject : public Entity
	{
	public:
		GroundObject();

		void				Initialise();
		void				Shutdown();

		void				Update();
		void				Render();

		float				GetEdgePadding();

	private:

		UINT  m_numVertices = 0;

		float m_groundEdgePadding;

	};

} // namespace scene
