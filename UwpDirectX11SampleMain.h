#pragma once

#include <DirectXMath.h>

#include "Common/StepTimer.h"
#include "Common/DeviceResources.h"
#include "SceneRenderer3D.h"
#include "SceneRenderer2D.h"
#include "Camera.h"
#include "InputProcedure.h"

namespace UwpWithDirectX11Sample
{
	#define GetSampleMain() UwpDirectX11SampleMain::Get()
	class UwpDirectX11SampleMain : public DX::IDeviceNotify
	{
	public:
		UwpDirectX11SampleMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~UwpDirectX11SampleMain();
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();
		void CreateWindowSizeDependentResources();
		void StartRenderLoop();
		void StopRenderLoop();

		Concurrency::critical_section& GetCriticalSection() { return m_criticalSection; }
		InputProcedure ^GetInputContainer(){return(m_input);}
		Camera ^GetCamera(){return(m_camera);}
		const std::vector<RenderObject *> &GetObjects(){ return(m_objects); }

		void TestAddObject();
		void TestRemoveObject();

		static UwpDirectX11SampleMain *Get(){ return(Instance); }
		std::shared_ptr<DX::DeviceResources> GetDevice(){ return(m_deviceResources); }

	private:
		void Update();
		bool Render();

		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::unique_ptr<SceneRenderer3D> m_sceneRenderer3D;
		std::unique_ptr<SceneRenderer2D> m_sceneRenderer2D;
		Windows::Foundation::IAsyncAction^ m_renderLoopWorker;
		Concurrency::critical_section m_criticalSection;

		DX::StepTimer m_timer;
		Camera ^m_camera;
		InputProcedure ^m_input;
		bool m_tracking;

		std::vector<RenderObject *> m_objects;
		std::mutex m_objectLock;

		static UwpDirectX11SampleMain *Instance;
	};
}