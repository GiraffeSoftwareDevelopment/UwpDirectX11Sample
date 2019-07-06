#pragma once

#include <string>
#include <DirectXMath.h>
#include "Common/DeviceResources.h"
#include "Common/StepTimer.h"
#include "RenderObject.h"
#include "Camera.h"

namespace UwpWithDirectX11Sample
{
	class TextObject
	{
	public:
		TextObject();
		~TextObject();

		void Create(const std::wstring &text, const int color);
		void SetText(const std::wstring &text);
		void SetPosition(const DirectX::XMFLOAT2 &pos){ m_position = pos; }

		const DWRITE_TEXT_METRICS &GetTextMetrics(){ return(m_textMetrics); }
		const DirectX::XMFLOAT2 &GetPosition(){ return(m_position); }
		const Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> GetBrush(){ return(m_brush); }
		const Microsoft::WRL::ComPtr<IDWriteTextLayout3> GetTextLayout(){ return(m_textLayout); }

		void CreateDeviceDependentResources(const int color);
		void ReleaseDeviceDependentResources();

	private:
		std::wstring                                 m_text;
		DWRITE_TEXT_METRICS	                         m_textMetrics;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;
		Microsoft::WRL::ComPtr<IDWriteTextLayout3>   m_textLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat2>   m_textFormat;
		DirectX::XMFLOAT2 m_position;
	};
	class SceneRenderer2D
	{
	public:
		SceneRenderer2D();
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(const DX::StepTimer &timer);
		void Render();
		void ClearObjects();

	private:
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_stateBlock;
		std::vector<TextObject *> m_textObjects;
		std::mutex m_objectLock;
	};
}