#pragma once

#include <mutex>
#include <DirectXMath.h>

#include "Common/DeviceResources.h"
#include "Common/StepTimer.h"
#include "Content/ShaderStructures.h"

#include "Camera.h"
#include "RenderObject.h"

namespace UwpWithDirectX11Sample
{
	class SceneRenderer3D
	{
	public:
		SceneRenderer3D();
		void CreateDeviceDependentResources();
		void UpdateOutputSize();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

		bool GetInitialized(){return(m_initialized);}

		float GetAspectRatio(){return(m_aspectRatio);}
		float GetFovAngleY(){return(m_fovAngleY);}

	public:
		static DirectX::XMFLOAT3 Float3One;
		static DirectX::XMFLOAT3 Float3Zero;
		static DirectX::XMVECTOR VectorOne;
		static DirectX::XMVECTOR VectorZero;

	private:
		bool m_initialized;

		Windows::Foundation::Size m_outputSize;
		float m_aspectRatio;
		float m_fovAngleY;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>       m_constantBuffer;
		ModelViewProjectionConstantBuffer m_constantBufferData;
	};
}
