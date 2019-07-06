#pragma once

namespace UwpWithDirectX11Sample
{
	ref class InputProcedure
	{
		internal:
			InputProcedure();
			void Init(_In_ Windows::UI::Core::CoreWindow ^window);

			void OnPointerPressed(_In_ Windows::UI::Core::CoreWindow ^sender, _In_ Windows::UI::Core::PointerEventArgs ^args);
			void OnPointerReleased(_In_ Windows::UI::Core::CoreWindow ^sender, _In_ Windows::UI::Core::PointerEventArgs ^args);
			void OnPointerMoved(_In_ Windows::UI::Core::CoreWindow ^sender, _In_ Windows::UI::Core::PointerEventArgs ^args);
			void OnPointerWheelChanged(_In_ Windows::UI::Core::CoreWindow ^sender, _In_ Windows::UI::Core::PointerEventArgs ^args);

			DirectX::XMFLOAT2 GetPointerVelocity();
			void ResetPointerVelocity();

			int GetPointerWheelDelta();
			void ResetPointerWheelDelta();

			void StartTracking() { m_tracking = true; }
			void StopTracking() { m_tracking = false; }
			bool IsTracking() { return(m_tracking); }

			bool IsKeyDown(Windows::System::VirtualKey key);

		private:
			void OnKeyDown(_In_ Windows::UI::Core::CoreWindow ^sender, _In_ Windows::UI::Core::KeyEventArgs ^args);
			void OnKeyUp(_In_ Windows::UI::Core::CoreWindow ^sender, _In_ Windows::UI::Core::KeyEventArgs ^args);

			Platform::Collections::Vector<Windows::System::VirtualKey> ^m_inputs;
			DirectX::XMFLOAT2 m_pointerPositionPrev;
			DirectX::XMFLOAT2 m_pointerPositionCurrent;
			int m_pointerWheelDelta;
			bool m_tracking;

	};
}