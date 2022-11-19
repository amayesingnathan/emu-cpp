#pragma once

#include "Core/Common.h"

namespace Emu {

	enum class ShaderDataType : uint8_t
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	static uint ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:		return 4;
		case ShaderDataType::Float2:	return 4 * 2;
		case ShaderDataType::Float3:	return 4 * 3;
		case ShaderDataType::Float4:	return 4 * 4;
		case ShaderDataType::Mat3:		return 4 * 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4 * 4;
		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Int2:		return 4 * 2;
		case ShaderDataType::Int3:		return 4 * 3;
		case ShaderDataType::Int4:		return 4 * 4;
		case ShaderDataType::Bool:		return 1;
		}

		GL_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		usize offset;
		uint size;
		bool normalised;

		BufferElement() = default;
		BufferElement(ShaderDataType t, const std::string& n, bool norm = false)
			: name(n), type(t), size(ShaderDataTypeSize(t)), offset(0), normalised(norm)
		{}

		uint getComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:		return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;
			case ShaderDataType::Mat3:		return 3;
			case ShaderDataType::Mat4:		return 4;
			case ShaderDataType::Int:		return 1;
			case ShaderDataType::Int2:		return 2;
			case ShaderDataType::Int3:		return 3;
			case ShaderDataType::Int4:		return 4;
			case ShaderDataType::Bool:		return 1;
			}

			GL_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(std::initializer_list<BufferElement> elements)
			: mElements(elements)
		{
			CalculateOffsetsAndStride();
		}

		const std::vector<BufferElement>& getElements() const { return mElements; }
		uint getStride() const { return mStride; }

		std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
		std::vector<BufferElement>::iterator end() { return mElements.end(); }
		std::vector<BufferElement>::reverse_iterator rbegin() { return mElements.rbegin(); }
		std::vector<BufferElement>::reverse_iterator rend() { return mElements.rend(); }

		std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
		std::vector<BufferElement>::const_iterator end()	const { return mElements.end(); }
		std::vector<BufferElement>::const_reverse_iterator rbegin() const { return mElements.rbegin(); }
		std::vector<BufferElement>::const_reverse_iterator rend() const { return mElements.rend(); }

	private:
		void CalculateOffsetsAndStride()
		{
			usize offset = 0;
			mStride = 0;
			for (auto& element : mElements)
			{
				element.offset = offset;
				offset += element.size;
				mStride += element.size;
			}
		}

	private:
		std::vector<BufferElement> mElements;
		uint mStride = 0;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(uint32_t size);
		VertexBuffer(float* vertices, uint32_t size);
		~VertexBuffer();

		void bind() const;
		void unbind() const;

		void setData(const void* data, uint size);

		const BufferLayout& getLayout() const { return mLayout; }
		void setLayout(const BufferLayout& layout) { mLayout = layout; }

		static Ref<VertexBuffer> Create(uint32_t size) { return std::make_shared<VertexBuffer>(size); }
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size) { return std::make_shared<VertexBuffer>(vertices, size); }

	private:
		uint mRendererID;
		BufferLayout mLayout;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t* indices, uint32_t count);
		~IndexBuffer();

		void bind() const;
		void unbind() const;

		void setData(const void* data, uint size);

		uint getCount() const { return mCount; }

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count) { return std::make_shared<IndexBuffer>(indices, count); }

	private:
		uint mRendererID;
		uint mCount;
	};
}