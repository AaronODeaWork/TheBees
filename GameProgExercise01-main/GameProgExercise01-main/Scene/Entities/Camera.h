#pragma once
#include "Scene\Entity.h"
#include <DirectXMath.h>
#include "DX\View.h" 


using namespace DirectX;

namespace scene
{

	class Camera : public Entity
	{
	public:
		Camera();
		
		void Initialise();
		void Update();
		void SetOrigin(XMVECTOR newOrigin);

	private:
		float ToRadian(float degree);
			
		void checkCameraBounds();						

		float		m_cameraRotationSpeed = 50.0f;

		XMVECTOR	m_cameraAngleRadians;
		XMVECTOR	m_cameraPosition;

		XMVECTOR	m_originPoint;
		XMVECTOR	m_viewOffset;
		


	};
} // namespace scene