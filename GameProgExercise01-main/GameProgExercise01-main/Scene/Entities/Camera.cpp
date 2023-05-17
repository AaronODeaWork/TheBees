#include "GameProgExercise01.h"
#include "DX\DeviceResources.h"
#include "Core.h"

#include "DX\View.h" 
#include "DX\Input.h"

#include "Camera.h"


namespace scene
{
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
		const float frameTime = utils::Timers::GetFrameTime();

		const Core* const core = Core::Get();
		DX::View* const view = core->GetView();
		DX::Input* input = core->GetInput();
		

		// We take in the users input and convert to radians to use with the direct X system
		XMVECTOR userInput = XMVECTOR{	(ToRadian(input->GetUpDown()   * m_cameraRotationSpeed) )* frameTime,
										(ToRadian(input->GetLeftRight()* m_cameraRotationSpeed) )* frameTime,
										(ToRadian(input->GetInOut()    * m_cameraRotationSpeed) )* frameTime,
										1.0f };

		m_cameraAngleRadians = XMVectorAdd(m_cameraAngleRadians , userInput);

		// We check the Yaw to make sure its not out of bounds of the min and max radians of a circle.
		if (m_cameraAngleRadians.m128_f32[1] < 0.0f)
		{
			m_cameraAngleRadians.m128_f32[1] += XM_2PI;
		}
		else if (m_cameraAngleRadians.m128_f32[1] > XM_2PI)
		{
			m_cameraAngleRadians.m128_f32[1] -= XM_2PI;
		}


		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_cameraAngleRadians.m128_f32[0], m_cameraAngleRadians.m128_f32[1], m_cameraAngleRadians.m128_f32[2]);
		XMVECTOR transformVector = XMVector3Transform(m_viewOffset, rotationMatrix);
			 
		m_cameraPosition = XMVectorAdd(m_originPoint, transformVector);

		// We negate the transform vector, Because the vector is pointing towards the camera,
		// and we want it to point at the origin. We then normalize it to get a unit vector.
		XMVECTOR viewDirectionNormalized = XMVector3Normalize( XMVectorNegate(transformVector) );
		

		view->SetViewDirection(viewDirectionNormalized);
		view->SetViewPosition(m_cameraPosition);

	}

	float Camera::ToRadian(float degree)
	{
		
		return degree * (XM_PI / 180);
	}

	void Camera::checkCameraBounds()
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
