#pragma once
#include "LearningDxEngine.h"

namespace Engine {
	using namespace DirectX::SimpleMath;

	struct Transform {
		Vector3 Position;
		Vector3 Rotation;
		Vector3 Scale;
	};

	struct Camera {
		Vector3 Position = Vector3(0.0f, 0.0f, -2.0f);
		Vector3 Front = Vector3(0.0f, 0.0f, -1.0f);
		Vector3 Up = Vector3(0.0f, -1.0f, 0.0f);
		Vector3 Right = Vector3(1.0f, 0.0f, 0.0f);

		Matrix Projection = Matrix::Identity;

		PUBLICAPI Camera();
		PUBLICAPI void UpdateRollPitchYaw(float roll, float pitch, float yaw);

		Matrix GetView();
		PUBLICAPI float GetYaw();
		PUBLICAPI float GetRoll();
		PUBLICAPI float GetPitch();

	private:
		float mRoll = 0.0f;
		float mPitch = 0.0f;
		float mYaw = 0.0f;
	};

}