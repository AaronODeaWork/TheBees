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
			
		void CheckCameraBounds();						

		static const float	CameraRotationSpeed;

		static const float	MaxZoom;
		static const float	MinZoom;
		static const float	ZoomRate;


		XMVECTOR	m_cameraAngleRadians;
		XMVECTOR	m_cameraPosition;

		XMVECTOR	m_originPoint;
		XMVECTOR	m_viewOffset;
		


	};
} // namespace scene