
#include "pch.h"
#include "DirectXPage.xaml.h"

using namespace UwpWithDirectX11Sample;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

App::App()
{
	InitializeComponent();
	Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
	Resuming += ref new EventHandler<Object^>(this, &App::OnResuming);
}

void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
#if _DEBUG
	if (IsDebuggerPresent())
	{
		DebugSettings->EnableFrameRateCounter = true;
	}
#endif
	auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);
	if (rootFrame == nullptr)
	{
		rootFrame = ref new Frame();
		rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);
		if (rootFrame->Content == nullptr)
		{
			rootFrame->Navigate(TypeName(DirectXPage::typeid), e->Arguments);
		}
		Window::Current->Content = rootFrame;
		Window::Current->Activate();

		auto frame = (Frame^)Window::Current->Content;
		auto page = (DirectXPage^)frame->Content;
		if(page)
		{
			m_directXPage = page;
			if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
			{
				m_directXPage->LoadInternalState(ApplicationData::Current->LocalSettings->Values);
			}
		}
	}
	else
	{
		if (rootFrame->Content == nullptr)
		{
			rootFrame->Navigate(TypeName(DirectXPage::typeid), e->Arguments);
		}
		Window::Current->Activate();
	}
}

void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
	(void) sender;
	(void) e;
	m_directXPage->SaveInternalState(ApplicationData::Current->LocalSettings->Values);
}

void App::OnResuming(Object ^sender, Object ^args)
{
	(void) sender;
	(void) args;
	m_directXPage->LoadInternalState(ApplicationData::Current->LocalSettings->Values);
}

void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
	throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}
