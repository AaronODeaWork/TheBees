#include "GameProgExercise01.h"
#include "DX\DeviceResources.h"
#include "Core.h"

#include "DX\View.h" 
#include "DX\Input.h"

#include "Camera.h"


namespace scene
{
	const float	Camera::MaxZoom = 50.0f;
	const float	Camera::MinZoom = 5.0f;
	const float	Camera::ZoomRate = 0.5f;
	const float	Camera::CameraRotationSpeed = 50.0f;
	Camera::Camera()
	{									
		// Angles for the camera ( Pitch , Yaw, Roll)
		m_cameraAngleRadians = XMVECTOR{ -0.52f, 0.0f, 0.0f, 1.0f};
		m_cameraPosition = XMVECTOR{ 0.0f, 0.0f, 0.0f,1.0f };

		m_originPoint = XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f };
		m_viewOffset = XMVECTOR{ 0.0f, 0.0f, 25.0f, 1.0f };

	}


	void Camera::Initialise()
	{
		const Core* const core = Core::Get();
		DX::View* const view = core->GetView();

		view->SetViewPosition(m_cameraPosition);
		view->SetViewDirection(XMVector3Normalize(XMVectorNegate(m_viewOffset)));
	}

	void Camera::Update()
	{
		const Core* const core = Core::Get();
		DX::View* const view = core->GetView();
		DX::Input* const input = core->GetInput();
		
		float correctedRotationSpeed = CameraRotationSpeed *utils::Timers::GetFrameTime();

		// We take in the users input and convert to radians to use with the direct X system
		XMVECTOR userInput = XMVECTOR{	(-XMConvertToRadians(input->GetUpDown()   * correctedRotationSpeed) ),
										(-XMConvertToRadians(input->GetLeftRight()* correctedRotationSpeed) ),
										(-XMConvertToRadians(input->GetInOut()    * correctedRotationSpeed) ),
										1.0f };

		if (input->GetInOut() == 1)
		{
			if (m_viewOffset.m128_f32[2] <= MaxZoom)
			{
				m_viewOffset.m128_f32[2] += ZoomRate * utils::Timers::GetFrameTime();
			}
		}
		else if (input->GetInOut() == -1)
		{
			if (m_viewOffset.m128_f32[2] >= MinZoom)
			{
				m_viewOffset.m128_f32[2] -= ZoomRate * utils::Timers::GetFrameTime();
			}
		}

		m_cameraAngleRadians = XMVectorAdd(m_cameraAngleRadians , userInput);

		CheckCameraBounds();


		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_cameraAngleRadians.m128_f32[0], m_cameraAngleRadians.m128_f32[1], m_cameraAngleRadians.m128_f32[2]);
		XMVECTOR transformVector = XMVector3Transform(m_viewOffset, rotationMatrix);
			 
		m_cameraPosition = XMVectorAdd(m_originPoint, transformVector);

		// We negate the transform vector, Because the vector is pointing towards the camera,
		// and we want it to point at the origin. We then normalize it to get a unit vector.
		XMVECTOR viewDirectionNormalized = XMVector3Normalize( XMVectorNegate(transformVector) );
		

		view->SetViewDirection(viewDirectionNormalized);
		view->SetViewPosition(m_cameraPosition);

	}

	void Camera::CheckCameraBounds()
	{
		// Pitch
		if (m_cameraAngleRadians.m128_f32[0] < 0.0f)
		{
			m_cameraAngleRadians.m128_f32[0] += XM_2PI;
		}
		else if (m_cameraAngleRadians.m128_f32[0] > XM_2PI)
		{
			m_cameraAngleRadians.m128_f32[0] -= XM_2PI;
		}

		// Yew
		if (m_cameraAngleRadians.m128_f32[1] < 0.0f)
		{
			m_cameraAngleRadians.m128_f32[1] += XM_2PI;
		}
		else if (m_cameraAngleRadians.m128_f32[1] > XM_2PI)
		{
			m_cameraAngleRadians.m128_f32[1] -= XM_2PI;
		}

		// Roll
		if (m_cameraAngleRadians.m128_f32[1] < 0.0f)
		{
			m_cameraAngleRadians.m128_f32[1] += XM_2PI;
		}
		else if (m_cameraAngleRadians.m128_f32[1] > XM_2PI)
		{
			m_cameraAngleRadians.m128_f32[1] -= XM_2PI;
		}

	}

	void Camera::SetOrigin(XMVECTOR newOrigin)
	{
		m_originPoint = newOrigin;
	}


	

} // namespace scene
