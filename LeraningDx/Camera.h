#pragma once
#include "pch.h"
#include "Transform.h"

struct Camera
{
	Vector3 Position = Vector3(0.0f, 0.0f, -2.0f);
	Vector3 Front = Vector3(0.0f, 0.0f, -1.0f);
	Vector3 Up = Vector3(0.0f, -1.0f, 0.0f);
	Vector3 Right = Vector3(1.0f, 0.0f, 0.0f);
	Vector3 WorldUp = Vector3(0.0f, -1.0f, 0.0f);
	Matrix view = Matrix::Identity;
	Matrix projection = Matrix::Identity;

private:
	float m_roll = 0.0f;
	float m_pitch = 0.0f;
	float m_yaw = 0.0f;

public:
	float move_speed = 2.5f;
	float mouse_sensitivity = 10.0f;
	float zoom = 45.0f;

	Camera() {
		updateRollPitchYaw(m_roll, m_pitch, m_yaw);
		//update();
	}

	void updateRollPitchYaw(float roll, float pitch, float yaw) {
		m_roll = roll;
		m_pitch = pitch;
		m_yaw = yaw;
		auto rotation = Quaternion::CreateFromYawPitchRoll({ pitch, yaw, roll });
		Front = DirectX::XMVector3Rotate(Vector3(0.0f, 0.0f, -1.0f), rotation);
		Front.Normalize();
		Right = Up.Cross(Front);
		Right.Normalize();
		return;
	}

	float getYaw() { return m_yaw; }
	float getPitch() { return m_pitch; }
	float getRoll() { return m_roll; }

	/*void update()
	{
		Vector3 front;
		front.x = cos(DirectX::XMConvertToRadians(yaw)) * cos(DirectX::XMConvertToRadians(pitch));
		front.y = sin(DirectX::XMConvertToRadians(pitch));
		front.z = sin(DirectX::XMConvertToRadians(yaw) * cos(DirectX::XMConvertToRadians(pitch)));
		front.Normalize();
		Front = front;
		Right = Front.Cross(WorldUp);
		Right.Normalize();
		Up = Right.Cross(Front);
		Up.Normalize();
	}*/

	Matrix getView() {
		auto view = Matrix::CreateLookAt(Position, Position + Front, Up);
		return view;
	}
};

