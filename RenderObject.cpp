
#include "pch.h"

#include "Common/DirectXHelper.h"
#include "RenderObject.h"
#include "UwpDirectX11SampleMain.h"

using namespace UwpWithDirectX11Sample;
using namespace DirectX;

//------------------------------------------------------------------------------------------------
// RenderObject.
//------------------------------------------------------------------------------------------------
RenderObject::RenderObject()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_indexCount = 0;

	m_translation = SceneRenderer3D::Float3Zero;
	m_rotation = SceneRenderer3D::Float3Zero;

	m_cullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_flag = 0;

	m_name = L"name";

	CoCreateGuid(&m_guid);
	//{
	//	char guid_cstr[37];
	//	snprintf(guid_cstr, sizeof(guid_cstr),
	//		"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
	//		m_guid.Data1,
	//		m_guid.Data2,
	//		m_guid.Data3,
	//		m_guid.Data4[0], m_guid.Data4[1], m_guid.Data4[2], m_guid.Data4[3],m_guid.Data4[4], m_guid.Data4[5], m_guid.Data4[6], m_guid.Data4[7]);
	//}
}

void RenderObject::CreateInstance()
{
	assert(false);
}

void RenderObject::ReleaseInstance()
{
	if(m_vertexBuffer)
	{
		m_vertexBuffer.Reset();
	}
	if(m_indexBuffer)
	{
		m_indexBuffer.Reset();
	}
	m_indexCount = 0;
}

//------------------------------------------------------------------------------------------------
// RenderObjectSampleCube.
//------------------------------------------------------------------------------------------------
RenderObjectSampleCube::RenderObjectSampleCube()
{
}

void RenderObjectSampleCube::CreateInstance()
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		// vertex buffer.
		static const VertexPositionColor vertices[] = 
		{
			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
			{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
			{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
			{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		};
		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = vertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));
		// index buffer.
		static const unsigned short indices [] =
		{
			1,2,0, // -x
			3,2,1,

			6,5,4, // +x
			6,7,5,

			5,1,0, // -y
			4,5,0,

			7,6,2, // +y
			3,7,2,

			6,4,0, // -z
			2,6,0,

			7,3,1, // +z
			5,7,1,

		};
		m_indexCount = ARRAYSIZE(indices);
		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = indices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));
	}
}

//------------------------------------------------------------------------------------------------
// RenderObjectSampleSquare.
//------------------------------------------------------------------------------------------------
RenderObjectSampleSquare::RenderObjectSampleSquare()
{
	m_cullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	SetFlag(Billboard, true);
}

void RenderObjectSampleSquare::CreateInstance()
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		// vertex buffer.
		static const VertexPositionColor vertices[] = 
		{
			{XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-0.5f,  0.5f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3( 0.5f, -0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{XMFLOAT3( 0.5f,  0.5f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		};
		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = vertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));
		// index buffer.
		static const unsigned short indices [] =
		{
			0,1,2,
			2,1,3,
		};
		m_indexCount = ARRAYSIZE(indices);
		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = indices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));
	}
}

//------------------------------------------------------------------------------------------------
// RenderObjectSampleLine.
//------------------------------------------------------------------------------------------------
RenderObjectSampleLine::RenderObjectSampleLine()
{
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
}

void RenderObjectSampleLine::CreateInstance()
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		// vertex buffer.
		static const VertexPositionColor vertices[] = 
		{
			{XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-0.5f,  0.5f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3( 0.5f, -0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{XMFLOAT3( 0.5f,  0.5f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		};
		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = vertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));
		// index buffer.
		static const unsigned short indices [] =
		{
			0,1,
			1,3,
			3,2,
			2,0,
		};
		m_indexCount = ARRAYSIZE(indices);
		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = indices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));
	}
}

//------------------------------------------------------------------------------------------------
// RenderObjectAxis.
//------------------------------------------------------------------------------------------------
RenderObjectAxis::RenderObjectAxis()
{
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
}

void RenderObjectAxis::CreateInstance()
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		// vertex buffer.
		static const VertexPositionColor vertices[] = 
		{
			{XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
		};
		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = vertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));
		// index buffer.
		static const unsigned short indices [] =
		{
			0,1,
			2,3,
			4,5,
		};
		m_indexCount = ARRAYSIZE(indices);
		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = indices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));
	}
}

//------------------------------------------------------------------------------------------------
// RenderObjectGrid.
//------------------------------------------------------------------------------------------------
RenderObjectGrid::RenderObjectGrid()
{
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
}

void RenderObjectGrid::CreateInstance()
{
	auto device = GetSampleMain()->GetDevice();
	if(device)
	{
		const XMFLOAT3 gridColor(0.25f, 0.25f, 0.25f);
		const float gridSize = 0.5f;
		uint32 gridCount = 40;
		gridCount = (0 == (gridCount % 2)) ? (gridCount + 1) : gridCount;
		const uint32 vertexCount = (gridCount * gridCount);
		// vertex buffer.
		VertexPositionColor *vertices = new VertexPositionColor[vertexCount];
		float start = (-(((float)(gridCount) * gridSize) * 0.5f)) + (gridSize * 0.5f);
		for(uint32 index1 = 0; index1 < gridCount; index1++)
		{
			const float z = ((float)(index1) * gridSize);
			for(uint32 index2 = 0; index2 < gridCount; index2++)
			{
				const uint32 i = ((index1 * gridCount) + index2);
				const float x = ((float)(index2) * gridSize);
				vertices[i].pos = XMFLOAT3(start + x, 0.0f, start + z);
				vertices[i].color = gridColor;
			}
		}
		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = vertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		uint32 vertexSize = ((sizeof(XMFLOAT3) * 2) * vertexCount);
		CD3D11_BUFFER_DESC vertexBufferDesc(vertexSize, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));
		// index buffer.
		uint32 indexCount = (gridCount * 2) * 2;
		unsigned short *indices = new unsigned short[indexCount];
		uint32 pos = 0;
		for(uint32 index1 = 0; index1 < gridCount; index1++)
		{
			indices[pos]     = (index1 * gridCount);
			indices[pos + 1] = (index1 * gridCount) + (gridCount - 1);
			pos += 2;
		}
		for(uint32 index1 = 0; index1 < gridCount; index1++)
		{
			indices[pos]     = index1;
			indices[pos + 1] = (index1 + ((gridCount - 1) * gridCount));
			pos += 2;
		}
		m_indexCount = indexCount;
		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = indices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		uint32 indexSize = (sizeof(unsigned short) * indexCount);
		CD3D11_BUFFER_DESC indexBufferDesc(indexSize, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(device->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));
	}
}
