
#include "pch.h"

#include "Camera.h"
#include "UwpDirectX11SampleMain.h"

using namespace UwpWithDirectX11Sample;
using namespace DirectX;
using namespace Windows::Graphics::Display;

Camera::Camera()
{
	ResetTransform();
	SetProjParams(XM_PI / 2, 1.0f, 0.01f, 1000.0f);
}

void Camera::ResetTransform()
{
	m_eye = XMFLOAT3(0.0f, 1.5f, -2.0f);
	m_lookAt = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_yawAngle = 0.0f;
	m_pitchAngle = 0.0f;
	LookDirection(GetDirection());
}

void Camera::LookDirection(_In_ XMFLOAT3 lookDirection)
{
	XMFLOAT3 lookAt;
	lookAt.x = m_eye.x + lookDirection.x;
	lookAt.y = m_eye.y + lookDirection.y;
	lookAt.z = m_eye.z + lookDirection.z;
	SetViewParams(m_eye, lookAt, m_up);
}

XMFLOAT3 Camera::GetDirection()
{
	XMFLOAT3 dir;
	float r = cosf(m_pitchAngle); // In the plane.
	dir.y = sinf(m_pitchAngle);   // Vertical.
	dir.z = r * cosf(m_yawAngle); // Fwd-back.
	dir.x = r * sinf(m_yawAngle); // Left-right.
	return(dir);
}

void Camera::SetAngle(float yaw, float pitch)
{
	static const float speed = 0.05f;
	m_pitchAngle += (pitch * speed);
	m_yawAngle   -= (yaw * speed);

	float limit = XM_PI / 2.0f - 0.01f;
	m_pitchAngle = __max(-limit, m_pitchAngle);
	m_pitchAngle = __min(+limit, m_pitchAngle);

	if (m_yawAngle >  XM_PI)
	{
		m_yawAngle -= XM_PI * 2.0f;
	}
	else if (m_yawAngle < -XM_PI)
	{
		m_yawAngle += XM_PI * 2.0f;
	}
}

void Camera::MoveEyePosition(DirectX::XMFLOAT3 offset)
{
	XMVECTOR vDirection = XMVector3Rotate(XMLoadFloat3(&offset), XMQuaternionRotationRollPitchYaw(-m_pitchAngle, m_yawAngle, 0.0f));
	XMStoreFloat3(&m_eye, XMVectorAdd(XMLoadFloat3(&m_eye), vDirection));
}

void Camera::Eye(_In_ XMFLOAT3 eye)
{
	SetViewParams(eye, m_lookAt, m_up);
}

void Camera::SetViewParams(_In_ XMFLOAT3 eye, _In_ XMFLOAT3 lookAt, _In_ XMFLOAT3 up)
{
	m_eye = eye;
	m_lookAt = lookAt;
	m_up = up;
	XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&m_eye), XMLoadFloat3(&m_lookAt), XMLoadFloat3(&m_up));
	XMVECTOR det;
	XMMATRIX inverseView = XMMatrixInverse(&det, view);
	XMStoreFloat4x4(&m_viewMatrix, view);
	XMStoreFloat4x4(&m_inverseView, inverseView);
}

void Camera::SetProjParams(_In_ float fieldOfView, _In_ float aspectRatio, _In_ float nearPlane, _In_ float farPlane)
{
	// Set attributes for the projection matrix.
	m_fieldOfView = fieldOfView;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio,m_nearPlane, m_farPlane));
}

DirectX::XMMATRIX Camera::View()
{
	return XMLoadFloat4x4(&m_viewMatrix);
}

DirectX::XMMATRIX Camera::Projection()
{
	return XMLoadFloat4x4(&m_projectionMatrix);
}

DirectX::XMMATRIX Camera::World()
{
	return XMLoadFloat4x4(&m_inverseView);
}

bool Camera::GetScreenPosition(const XMFLOAT3 &pos, const XMFLOAT2 &screen, XMFLOAT2 &result )
{
	bool isInScren = false;
	auto device = GetSampleMain()->GetDevice();
	if (device)
	{
		const XMFLOAT4 pos4 = XMFLOAT4(pos.x, pos.y, pos.z, 1.0f);
		const XMVECTOR posV = XMLoadFloat4(&pos4);
		const XMVECTOR viewPos = XMVector4Transform(posV, View());
		XMVECTOR projPos = XMVector4Transform(viewPos, Projection());
		projPos /= projPos.m128_f32[3];
		result = XMFLOAT2((1.0f + projPos.m128_f32[0]) / 2.0f * screen.x, (1.0f - projPos.m128_f32[1]) / 2.0f * screen.y);
		{
			const float dpi = DisplayProperties::LogicalDpi;
			const float displayScale = dpi / 96.0f;
			result.x /= displayScale;
			result.y /= displayScale;
		}
		if (((0.0f <= result.x) && (result.x <= screen.x)) && ((0.0f <= result.y) && (result.y <= screen.y)))
		{
			const XMVECTOR vAt3 = XMLoadFloat3(&m_lookAt);
			const XMVECTOR vEye3 = XMLoadFloat3(&m_eye);
			const XMVECTOR vPos3 = XMLoadFloat3(&pos);
			const XMVECTOR vDp3 = XMVector3Dot(XMVector3NormalizeEst(vAt3 - vEye3), XMVector3Normalize(vPos3 - vEye3));
			if(0.0f <= vDp3.m128_f32[0])
			{
				isInScren = true;
			}
		}
		//{
		//	const XMVECTOR vAt = XMLoadFloat3(&m_lookAt);
		//	const XMVECTOR vEye = XMLoadFloat3(&m_eye);
		//	const XMVECTOR vPos = XMLoadFloat3(&pos);
		//	const XMVECTOR vDir1 = XMVector3Normalize(vAt - vEye);
		//	const XMVECTOR vDir2 = XMVector3Normalize(vPos - vEye);
		//	const XMVECTOR vDp = XMVector3Dot(vDir1, vDir2);
		//
		//	wchar_t message[1024];
		//	swprintf_s(message, L"%s : x : %f/ y : %f/dp : %f\n", (true == isInScren ? L"In " : L"Out"), result.x, result.y, vDp.m128_f32[0]);
		//	OutputDebugStringW(message);
		//}
	}
	return(isInScren);
}
