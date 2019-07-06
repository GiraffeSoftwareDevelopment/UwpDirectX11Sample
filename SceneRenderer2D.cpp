#include "pch.h"

#include "SceneRenderer2D.h"

#include "Common/DirectXHelper.h"

using namespace UwpWithDirectX11Sample;
using namespace Microsoft::WRL;
using namespace DirectX;

TextObject::TextObject()
{
}

TextObject::~TextObject()
{
	ReleaseDeviceDependentResources();
}

void TextObject::Create(const std::wstring &text, const int color)
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));
		ComPtr<IDWriteTextFormat> textFormat;
		DX::ThrowIfFailed(device->GetDWriteFactory()->CreateTextFormat(
				L"Segoe UI",
				nullptr,
				DWRITE_FONT_WEIGHT_LIGHT,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				16.0f,
				L"en-US",
				&textFormat));

		DX::ThrowIfFailed(textFormat.As(&m_textFormat));
		DX::ThrowIfFailed(m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
		DX::ThrowIfFailed(m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
		CreateDeviceDependentResources(color);

		SetText(text);
		float maxWidth = 1024.0f;
		float maxHeight = 50.0f;
		{
			ComPtr<IDWriteTextLayout> textLayout;
			DX::ThrowIfFailed(device->GetDWriteFactory()->CreateTextLayout(m_text.c_str(), (uint32)(m_text.length()), m_textFormat.Get(), maxWidth, maxHeight,&textLayout));
			DX::ThrowIfFailed(textLayout.As(&m_textLayout));
			DX::ThrowIfFailed(m_textLayout->GetMetrics(&m_textMetrics));
		}
		{
			ComPtr<IDWriteTextLayout> textLayout;
			maxWidth = m_textMetrics.width;
			DX::ThrowIfFailed(device->GetDWriteFactory()->CreateTextLayout(m_text.c_str(), (uint32) m_text.length(), m_textFormat.Get(), maxWidth, maxHeight, &textLayout));
			DX::ThrowIfFailed(textLayout.As(&m_textLayout));
			DX::ThrowIfFailed(m_textLayout->GetMetrics(&m_textMetrics));
		}
	}
}

void TextObject::SetText(const std::wstring &text)
{
	m_text = text;
}

void TextObject::CreateDeviceDependentResources(const int color)
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		DX::ThrowIfFailed(device->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(color, 1.0f), &m_brush));
	}
}
void TextObject::ReleaseDeviceDependentResources()
{
	m_brush.Reset();
}

SceneRenderer2D::SceneRenderer2D()
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		DX::ThrowIfFailed(device->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock));
	}
}

void SceneRenderer2D::ClearObjects()
{
	ScopedMutex lock(&m_objectLock);
	for(TextObject *obj : m_textObjects)
	{
		delete obj;
	}
	m_textObjects.clear();
}

void SceneRenderer2D::Update(const DX::StepTimer &timer)
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		Camera ^camera = GetSampleMain()->GetCamera();
		const auto &objects = GetSampleMain()->GetObjects();
		// clear.
		ClearObjects();

		ScopedMutex lock(&m_objectLock);
		// create.
		Windows::Foundation::Size logicalSize = device->GetLogicalSize();
		// fps.
		{
			wchar_t buff[1024];
			uint32 fps = timer.GetFramesPerSecond();
			swprintf_s(buff, 1024, L" %02d : FPS ", fps);
			TextObject *obj = new TextObject();
			obj->Create(buff, 0xffffff);
			obj->SetPosition(XMFLOAT2(logicalSize.Width - obj->GetTextMetrics().width, logicalSize.Height - obj->GetTextMetrics().height));
			m_textObjects.push_back(obj);
		}
		// name.
		{
			const auto &outputSize = device->GetOutputSize();
			const XMFLOAT2 screen(outputSize.Width, outputSize.Height);
			for(RenderObject *obj : objects)
			{
				if(true == obj->GetFlag(RenderObject::NamePlate))
				{
					XMFLOAT2 pos(0.0f, 0.0f);
					if(true == camera->GetScreenPosition(obj->GetTranslation(), screen, pos))
					{
						TextObject *tobj = new TextObject();
						tobj->Create(obj->GetName(), 0xffffff);
						pos.x -= (tobj->GetTextMetrics().width * 0.5f);
						pos.y -= (tobj->GetTextMetrics().height * 0.5f);
						tobj->SetPosition(pos);
						m_textObjects.push_back(tobj);
					}
				}
			}
		}
	}
}

void SceneRenderer2D::Render()
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		ScopedMutex lock(&m_objectLock);
		ID2D1DeviceContext* context = device->GetD2DDeviceContext();
		Windows::Foundation::Size logicalSize = device->GetLogicalSize();

		context->SaveDrawingState(m_stateBlock.Get());
		context->BeginDraw();
		{
			for(TextObject *obj : m_textObjects)
			{
				D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(obj->GetPosition().x, obj->GetPosition().y);
				context->SetTransform(screenTranslation * device->GetOrientationTransform2D());
				context->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), obj->GetTextLayout().Get(), obj->GetBrush().Get());
			}
		}
		HRESULT hr = context->EndDraw();
		if (hr == D2DERR_RECREATE_TARGET)
		{
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}
		context->RestoreDrawingState(m_stateBlock.Get());
	}
}

void SceneRenderer2D::CreateDeviceDependentResources()
{
}

void SceneRenderer2D::ReleaseDeviceDependentResources()
{
	ClearObjects();
}
