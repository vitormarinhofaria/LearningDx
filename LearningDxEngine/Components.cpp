#include "Components.h"
#include "LearningDxEngine.h"
namespace Engine {
	Camera::Camera() {
		this->UpdateRollPitchYaw(0.0f, 0.0f, 0.0f);
	}
	void Camera::UpdateRollPitchYaw(float roll, float pitch, float yaw)
	{
		mRoll = roll;
		mPitch = pitch;
		mYaw = yaw;
		auto rotation = Quaternion::CreateFromYawPitchRoll({ pitch, yaw, roll });
		Front = DirectX::XMVector3Rotate(Vector3(0.0f, 0.0f, -1.0f), rotation);
		Front.Normalize();
		Right = Up.Cross(Front);
		Right.Normalize();
		return;
	}

	Matrix Camera::GetView()
	{
		auto view = Matrix::CreateLookAt(Position, Position + Front, Up);
		view.Transpose();
		return view;
	}

	float Camera::GetYaw()
	{
		return mYaw;
	}

	float Camera::GetRoll()
	{
		return mRoll;
	}

	float Camera::GetPitch()
	{
		return mPitch;
	}

}