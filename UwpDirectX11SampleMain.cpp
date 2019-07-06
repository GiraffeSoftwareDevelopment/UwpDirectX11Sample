
#include "pch.h"

#include <Windows.h>

#include "Common/DirectXHelper.h"
#include "UwpDirectX11SampleMain.h"

using namespace UwpWithDirectX11Sample;
using namespace Concurrency;
using namespace DirectX;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;

UwpDirectX11SampleMain *UwpDirectX11SampleMain::Instance = nullptr;

UwpDirectX11SampleMain::UwpDirectX11SampleMain(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	UwpDirectX11SampleMain::Instance = this;

	m_deviceResources = deviceResources;
	m_deviceResources->RegisterDeviceNotify(this);
	m_sceneRenderer3D = std::unique_ptr<SceneRenderer3D>(new SceneRenderer3D());
	m_sceneRenderer2D = std::unique_ptr<SceneRenderer2D>(new SceneRenderer2D());
	m_camera = ref new Camera();
	CoreWindow ^window = Window::Current->CoreWindow;
	m_input = ref new InputProcedure();
	m_input->Init(window);
	m_tracking = false;

	{
		ScopedMutex lock(&m_objectLock);
		RenderObject *obj = nullptr;
		// grid.
		obj = new RenderObjectGrid();
		obj->CreateInstance();
		obj->SetFlag(RenderObject::Resident, true);
		m_objects.push_back(obj);
		// axis.
		obj = new RenderObjectAxis();
		obj->CreateInstance();
		obj->SetFlag(RenderObject::Resident, true);
		obj->SetTranslation(XMFLOAT3(0.0f, 0.0001f, 0.0f));
		m_objects.push_back(obj);
		{
			static const float interval = 1.5f;
			obj = new RenderObjectSampleCube();
			obj->CreateInstance();
			obj->SetName(L"Cube");
			obj->SetFlag(RenderObject::Test, true);
			obj->SetFlag(RenderObject::NamePlate, true);
			obj->SetFlag(RenderObject::Resident, true);
			obj->SetTranslation(XMFLOAT3(interval, 0.5f, interval));
			m_objects.push_back(obj);

			obj = new RenderObjectSampleSquare();
			obj->CreateInstance();
			obj->SetName(L"Billboard");
			obj->SetFlag(RenderObject::NamePlate, true);
			obj->SetFlag(RenderObject::Resident, true);
			obj->SetTranslation(XMFLOAT3(-interval, 0.5f, interval));
			m_objects.push_back(obj);

			obj = new RenderObjectSampleLine();
			obj->CreateInstance();
			obj->SetName(L"Line");
			obj->SetFlag(RenderObject::Test, true);
			obj->SetFlag(RenderObject::NamePlate, true);
			obj->SetFlag(RenderObject::Resident, true);
			obj->SetTranslation(XMFLOAT3(interval, 1.5f, interval * 2.0f));
			m_objects.push_back(obj);

			obj = new RenderObjectAxis();
			obj->CreateInstance();
			obj->SetName(L"Axis");
			obj->SetFlag(RenderObject::Test, true);
			obj->SetFlag(RenderObject::NamePlate, true);
			obj->SetFlag(RenderObject::Resident, true);
			obj->SetTranslation(XMFLOAT3(-interval, 1.5f, interval * 2.0f));
			m_objects.push_back(obj);
		}
	}
}

UwpDirectX11SampleMain::~UwpDirectX11SampleMain()
{
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

void UwpDirectX11SampleMain::OnDeviceLost()
{
	m_sceneRenderer3D->ReleaseDeviceDependentResources();
	m_sceneRenderer2D->ReleaseDeviceDependentResources();
	{
		ScopedMutex lock(&m_objectLock);
		for(RenderObject *obj : m_objects)
		{
			obj->ReleaseInstance();
		}
		m_objects.clear();
	}
}

void UwpDirectX11SampleMain::OnDeviceRestored()
{
	m_sceneRenderer3D->CreateDeviceDependentResources();
	m_sceneRenderer2D->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void UwpDirectX11SampleMain::CreateWindowSizeDependentResources() 
{
	m_sceneRenderer3D->UpdateOutputSize();
	m_camera->SetProjParams(m_sceneRenderer3D->GetFovAngleY(), m_sceneRenderer3D->GetAspectRatio(), 0.01f, 1000.0f);
}

void UwpDirectX11SampleMain::StartRenderLoop()
{
	if (m_renderLoopWorker != nullptr && m_renderLoopWorker->Status == AsyncStatus::Started)
	{
		return;
	}
	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
	{
		while (action->Status == AsyncStatus::Started)
		{
			critical_section::scoped_lock lock(m_criticalSection);
			Update();
			if (Render())
			{
				m_deviceResources->Present();
			}
		}
	});
	m_renderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void UwpDirectX11SampleMain::StopRenderLoop()
{
	m_renderLoopWorker->Cancel();
}

void UwpDirectX11SampleMain::Update() 
{
	{
		if(m_input->IsTracking() != m_tracking)
		{
			m_tracking = m_input->IsTracking();
			m_input->ResetPointerVelocity();
			m_input->ResetPointerWheelDelta();
		}
		float unit = 0.025f;
		int wheelDelta = m_input->GetPointerWheelDelta();
		if(0 != wheelDelta)
		{
			XMFLOAT3 offset(0.0f, 0.0f, 0.0f);
			if (0 < wheelDelta) { offset.z += (unit * 4); }
			if (0 > wheelDelta) { offset.z -= (unit * 4); }
			m_camera->MoveEyePosition(offset);
			m_camera->LookDirection(m_camera->GetDirection());
		}
		if(true == m_tracking)
		{
			XMFLOAT2 velocity = m_input->GetPointerVelocity();
			m_camera->SetAngle(-velocity.x * 0.25f, -velocity.y * 0.25f);

			XMFLOAT3 offset(0.0f, 0.0f, 0.0f);
			if(true == m_input->IsKeyDown(Windows::System::VirtualKey::D)){offset.x += unit;}
			if(true == m_input->IsKeyDown(Windows::System::VirtualKey::A)){offset.x -= unit;}
			if(true == m_input->IsKeyDown(Windows::System::VirtualKey::S)){offset.z -= unit;}
			if(true == m_input->IsKeyDown(Windows::System::VirtualKey::W)){offset.z += unit;}
			if(true == m_input->IsKeyDown(Windows::System::VirtualKey::E)){offset.y += unit;}
			if(true == m_input->IsKeyDown(Windows::System::VirtualKey::Q)){offset.y -= unit;}
			m_camera->MoveEyePosition(offset);
			m_camera->LookDirection(m_camera->GetDirection());
		}
		{
			ScopedMutex lock(&m_objectLock);
			for(RenderObject *obj : m_objects)
			{
				if(true == obj->GetFlag(RenderObject::Flag::Test))
				{
					float yRot = (1.0f * XM_PI / 180.0f);
					XMFLOAT3 rot = obj->GetRotation();
					rot.y = ((XM_2PI <= (rot.y + yRot)) ? 0.0f : (rot.y + yRot));
					obj->SetRotation(rot);
				}
			}
		}
	}
	m_timer.Tick([&]()
	{
		{
			ScopedMutex lock(&m_objectLock);
			m_sceneRenderer3D->Update(m_timer);
			m_sceneRenderer2D->Update(m_timer);
		}
	});
}

bool UwpDirectX11SampleMain::Render() 
{
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto viewport = m_deviceResources->GetScreenViewport();
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Black);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	{
		ScopedMutex lock(&m_objectLock);
		m_sceneRenderer3D->Render();
		m_sceneRenderer2D->Render();
	}
	return true;
}

void UwpDirectX11SampleMain::TestAddObject()
{
	if(m_sceneRenderer3D)
	{
		int offset = 0;
		for(auto obj : m_objects)
		{
			if(true == obj->GetFlag(RenderObject::Resident))
			{
				offset++;
			}
		}
		ScopedMutex lock(&m_objectLock);
		RenderObject *obj = (0 == (m_objects.size() % 2)) ? (RenderObject *)(new RenderObjectSampleCube()) : (RenderObject *)(new RenderObjectSampleSquare());
		obj->CreateInstance();
		obj->SetTranslation(XMFLOAT3((float)(m_objects.size() - offset) * 1.25f, 0.0f, 4.0f));
		obj->SetFlag(RenderObject::Test, true);
		obj->SetFlag(RenderObject::NamePlate, true);
		m_objects.push_back(obj);
	}
}

void UwpDirectX11SampleMain::TestRemoveObject()
{
	if(m_sceneRenderer3D)
	{
		ScopedMutex lock(&m_objectLock);
		if(2 < m_objects.size())
		{
			RenderObject *obj = m_objects.back();
			if((obj) && (false == obj->GetFlag(RenderObject::Resident)))
			{
				delete obj;
				m_objects.pop_back();
			}
		}
	}
}
