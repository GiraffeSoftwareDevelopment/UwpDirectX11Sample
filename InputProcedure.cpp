
#include "pch.h"

#include "InputProcedure.h"

using namespace UwpWithDirectX11Sample;
using namespace DirectX;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;

InputProcedure::InputProcedure()
{
	m_pointerWheelDelta = 0;
}

void InputProcedure::OnKeyDown(_In_ CoreWindow ^sender, _In_ KeyEventArgs ^args)
{
	Windows::System::VirtualKey key = args->VirtualKey;
	uint32 index = 0;
	if(false == m_inputs->IndexOf(key, &index))
	{
		m_inputs->Append(key);
	}
}

void InputProcedure::OnKeyUp(_In_ CoreWindow ^sender, _In_ KeyEventArgs ^args)
{
	Windows::System::VirtualKey key = args->VirtualKey;
	uint32 index = 0;
	if(true == m_inputs->IndexOf(key, &index))
	{
		m_inputs->RemoveAt(index);
	}
}

void InputProcedure::OnPointerPressed(_In_ CoreWindow ^sender, _In_ PointerEventArgs ^args)
{
	StartTracking();
}

void InputProcedure::OnPointerReleased(_In_ CoreWindow ^sender, _In_ PointerEventArgs ^args)
{
	StopTracking();
}

void InputProcedure::OnPointerMoved(_In_ CoreWindow ^sender, _In_ PointerEventArgs ^args)
{
	PointerPoint^ point = args->CurrentPoint;
	m_pointerPositionPrev = m_pointerPositionCurrent;
	m_pointerPositionCurrent = XMFLOAT2(point->Position.X, point->Position.Y);
}

void InputProcedure::OnPointerWheelChanged(_In_ CoreWindow ^sender, _In_ PointerEventArgs ^args)
{
	PointerPoint^ point = args->CurrentPoint;
	m_pointerWheelDelta = point->Properties->MouseWheelDelta;
}

XMFLOAT2 InputProcedure::GetPointerVelocity()
{
	XMVECTOR current = XMLoadFloat2(&m_pointerPositionCurrent);
	XMVECTOR prev = XMLoadFloat2(&m_pointerPositionPrev);
	XMVECTOR vResult = XMVectorSubtract(current, prev);
	XMFLOAT2 fResult;
	XMStoreFloat2(&fResult, vResult);
	ResetPointerVelocity();
	return(fResult);
}

void InputProcedure::ResetPointerVelocity()
{
	m_pointerPositionPrev = m_pointerPositionCurrent;
}

int InputProcedure::GetPointerWheelDelta()
{
	int result = m_pointerWheelDelta;
	ResetPointerWheelDelta();
	return(result);
}

void InputProcedure::ResetPointerWheelDelta()
{
	m_pointerWheelDelta = 0;
}

void InputProcedure::Init(_In_ CoreWindow ^window)
{
	if(window)
	{
		m_inputs = ref new Vector<Windows::System::VirtualKey>();
		window->KeyDown += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &InputProcedure::OnKeyDown);
		window->KeyUp += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &InputProcedure::OnKeyUp);

		m_pointerPositionPrev = XMFLOAT2(0.0f, 0.0f);
		m_pointerPositionCurrent = XMFLOAT2(0.0f, 0.0f);

		m_tracking = false;
	}
}

bool InputProcedure::IsKeyDown(Windows::System::VirtualKey key)
{
	uint32 index = 0;
	return(m_inputs->IndexOf(key, &index));
}
