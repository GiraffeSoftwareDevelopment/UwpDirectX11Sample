
#include "pch.h"

#include <string>

#include "Common/DirectXHelper.h"
#include "SceneRenderer3D.h"

using namespace UwpWithDirectX11Sample;
using namespace DirectX;
using namespace Windows::Foundation;

XMFLOAT3 SceneRenderer3D::Float3One = XMFLOAT3(1.0f, 1.0f, 1.0f);
XMFLOAT3 SceneRenderer3D::Float3Zero = XMFLOAT3(0.0f, 0.0f, 0.0f);
XMVECTOR SceneRenderer3D::VectorOne = XMLoadFloat3(&Float3One);
XMVECTOR SceneRenderer3D::VectorZero = XMLoadFloat3(&Float3Zero);

SceneRenderer3D::SceneRenderer3D() :
	m_initialized(false),
	m_outputSize(1.0f, 1.0f),
	m_aspectRatio(1.0f),
	m_fovAngleY(1.0f)
{
	CreateDeviceDependentResources();
	UpdateOutputSize();
}

void SceneRenderer3D::CreateDeviceDependentResources()
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		ID3D11RasterizerState *rsState;
		CD3D11_DEFAULT default_state;
		CD3D11_RASTERIZER_DESC rsdesc(default_state);
		device->GetD3DDevice()->CreateRasterizerState(&rsdesc, &rsState);
		device->GetD3DDeviceContext()->RSSetState(rsState);
		safe_release(rsState);

		Concurrency::task<std::vector<byte>> loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
		Concurrency::task<std::vector<byte>> loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");
		Concurrency::task<void> createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
		{
			static const D3D11_INPUT_ELEMENT_DESC vertexDesc [] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			auto device = GetSampleMain()->GetDevice();
			if(device)
			{
				DX::ThrowIfFailed(device->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_vertexShader));
				DX::ThrowIfFailed(device->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(),&m_inputLayout));
			}
		});
		Concurrency::task<void> createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
		{
			auto device = GetSampleMain()->GetDevice();
			if(device)
			{
				CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);
				DX::ThrowIfFailed(device->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_pixelShader));
				DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer));
			}
		});
		Concurrency::task<void> initializedTrueTask = (createPSTask && createVSTask).then([this] ()
		{
			m_initialized = true;
		});
	}
}

void SceneRenderer3D::ReleaseDeviceDependentResources()
{
	m_initialized = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
}

void SceneRenderer3D::UpdateOutputSize()
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		m_outputSize = device->GetOutputSize();
		m_aspectRatio = m_outputSize.Width / m_outputSize.Height;
		m_fovAngleY = 70.0f * XM_PI / 180.0f;
	}
}

void SceneRenderer3D::Update(DX::StepTimer const& timer)
{
}

void SceneRenderer3D::Render()
{
	if (!m_initialized)
	{
		return;
	}
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		Camera ^camera = GetSampleMain()->GetCamera();
		const auto objects = GetSampleMain()->GetObjects();
		// projection.
		XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(camera->Projection()));
		// view.
		XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(camera->View()));
		for (RenderObject *obj : objects)
		{
			XMVECTOR vTranslation = XMLoadFloat3(&(obj->GetTranslation()));
			XMVECTOR vRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&(obj->GetRotation())));
			if(true == obj->GetFlag(RenderObject::Billboard))
			{
				vRotation = XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, camera->View()));
			}
			else
			{
				vRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&(obj->GetRotation())));
			}
			XMMATRIX mat = XMMatrixTransformation(
				VectorZero,    // ScalingOrigin,
				VectorZero,    // ScalingOrientationQuaternion.
				VectorOne,     // Scaling.
				VectorZero,    // RotationOrigin.
				vRotation,     // RotationQuaternion.
				vTranslation); // Translation.
			// model.
			XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(mat));

			auto context = device->GetD3DDeviceContext();
			auto d3dDdevice = device->GetD3DDevice();
			context->UpdateSubresource1(m_constantBuffer.Get(), 0, nullptr, &m_constantBufferData, 0, 0, 0);

			UINT stride = sizeof(VertexPositionColor);
			UINT offset = 0;
			context->IASetVertexBuffers(0, 1, obj->GetVertexBuffer().GetAddressOf(), &stride, &offset);
			context->IASetIndexBuffer(obj->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
			context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)(obj->GetTopology()));
			context->IASetInputLayout(m_inputLayout.Get());
			// vertex shader.
			context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
			// constant buffer.
			context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
			// pixel shader.
			context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
			{
				ID3D11RasterizerState *rasterState = nullptr;
				context->RSGetState(&rasterState);
				D3D11_RASTERIZER_DESC RasterDesc;
				rasterState->GetDesc(&RasterDesc);
				safe_release(rasterState);

				RasterDesc.CullMode = (D3D11_CULL_MODE)(obj->GetCullMode());
				d3dDdevice->CreateRasterizerState(&RasterDesc, &rasterState);
				context->RSSetState(rasterState);
				safe_release(rasterState);
			}
			context->DrawIndexed(obj->GetIndexCount(), 0, 0);
		}
	}
}
