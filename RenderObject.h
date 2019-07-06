#pragma once

namespace UwpWithDirectX11Sample
{
	//------------------------------------------------------------------------------------------------
	// RenderObject.
	//------------------------------------------------------------------------------------------------
	class RenderObject
	{
	public:
		RenderObject();
		virtual void CreateInstance();
		void ReleaseInstance();

		Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer(){ return(m_vertexBuffer); }
		Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer(){ return(m_indexBuffer); }
		uint32  GetIndexCount(){ return(m_indexCount); }

		DirectX::XMFLOAT3 &GetTranslation(){ return(m_translation); }
		DirectX::XMFLOAT3 &GetRotation(){ return(m_rotation); }
		void SetTranslation(DirectX::XMFLOAT3 &translation){ m_translation = translation; }
		void SetRotation(DirectX::XMFLOAT3 &rotation){ m_rotation = rotation; }

		enum Flag
		{
			Test      = (1 << 0),
			Resident  = (1 << 1),
			Billboard = (1 << 2),
			NamePlate = (1 << 3),
		};
		void ClearFlag()
		{
			m_flag = 0;
		}
		void SetFlag(uint32 flag, bool value)
		{
			m_flag = (true == value) ? (m_flag | flag) : (m_flag & ~flag);
		}
		bool GetFlag(uint32 flag)
		{
			return(((m_flag & flag) == 0) ? false : true);
		}

		const uint32 GetCullMode(){return(m_cullMode);}
		const uint32 GetTopology(){return(m_topology);}

		void SetName(const std::wstring &name){ m_name = name; }
		const std::wstring &GetName(){ return(m_name); }

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		uint32 m_indexCount;

		DirectX::XMFLOAT3 m_translation;
		DirectX::XMFLOAT3 m_rotation;

		GUID m_guid;
		uint32 m_cullMode;
		uint32 m_topology;
		uint32 m_flag;
		std::wstring m_name;
	};

	//------------------------------------------------------------------------------------------------
	// RenderObjectSampleCube.
	//------------------------------------------------------------------------------------------------
	class RenderObjectSampleCube : public RenderObject
	{
	public:
		RenderObjectSampleCube();
		void CreateInstance();

	private:

	};

	//------------------------------------------------------------------------------------------------
	// RenderObjectSampleSquare.
	//------------------------------------------------------------------------------------------------
	class RenderObjectSampleSquare : public RenderObject
	{
	public:
		RenderObjectSampleSquare();
		void CreateInstance();

	private:

	};

	//------------------------------------------------------------------------------------------------
	// RenderObjectSampleLine.
	//------------------------------------------------------------------------------------------------
	class RenderObjectSampleLine : public RenderObject
	{
	public:
		RenderObjectSampleLine();
		void CreateInstance();

	private:

	};
	//------------------------------------------------------------------------------------------------
	// RenderObjectAxis.
	//------------------------------------------------------------------------------------------------
	class RenderObjectAxis : public RenderObject
	{
	public:
		RenderObjectAxis();
		void CreateInstance();

	private:

	};
	//------------------------------------------------------------------------------------------------
	// RenderObjectGrid.
	//------------------------------------------------------------------------------------------------
	class RenderObjectGrid : public RenderObject
	{
	public:
		RenderObjectGrid();
		void CreateInstance();

	private:

	};
}
