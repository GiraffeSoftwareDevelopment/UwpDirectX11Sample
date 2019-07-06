
#pragma once

ref class Camera
{
internal:
	Camera();

	void SetViewParams(_In_ DirectX::XMFLOAT3 eye, _In_ DirectX::XMFLOAT3 lookAt, _In_ DirectX::XMFLOAT3 up);
	void SetProjParams(_In_ float fieldOfView, _In_ float aspectRatio, _In_ float nearPlane, _In_ float farPlane);

	void LookDirection(_In_ DirectX::XMFLOAT3 lookDirection);
	void Eye(_In_ DirectX::XMFLOAT3 position);

	DirectX::XMFLOAT3 GetDirection();
	void SetAngle(float yaw, float pitch);
	void MoveEyePosition(DirectX::XMFLOAT3 offset);
	void ResetTransform();

	DirectX::XMMATRIX View();
	DirectX::XMMATRIX Projection();
	DirectX::XMMATRIX World();

	bool GetScreenPosition(const DirectX::XMFLOAT3 &pos, const DirectX::XMFLOAT2 &screen, DirectX::XMFLOAT2 &result );

protected private:
	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;

	DirectX::XMFLOAT4X4 m_inverseView;

	DirectX::XMFLOAT3 m_eye;
	DirectX::XMFLOAT3 m_lookAt;
	DirectX::XMFLOAT3 m_up;
	float m_yawAngle;
	float m_pitchAngle;

	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;
};
