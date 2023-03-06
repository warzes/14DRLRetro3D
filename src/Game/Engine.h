#pragma once

//=============================================================================
// Engine config
//=============================================================================
constexpr int MaxBindingTextures = 16;

//=============================================================================
// Core
//=============================================================================

// Implementation from "08/02/2015 Better array 'countof' implementation with C++ 11 (updated)" - https://www.g-truc.net/post-0708.html
template<typename T, size_t N>
[[nodiscard]] constexpr size_t Countof(T const (&)[N])
{
	return N;
}

//=============================================================================
// Logging System
//=============================================================================
void LogCreate(const std::string& fileName);
void LogDestroy();
void LogPrint(const std::string& msg);
void LogWarning(const std::string& msg);
void LogError(const std::string& msg);
void Fatal(const std::string& msg);

//=============================================================================
// Render System
//=============================================================================

namespace render
{
	enum class ResourceType
	{
		Unknown,
		ShaderProgram,
		VertexBuffer,
		IndexBuffer,
		VertexArray,
		Texture2D
	};

	enum class ResourceUsage
	{
		Static,
		Dynamic,
		Stream,
	};

	enum class PrimitiveDraw
	{
		Lines,
		Triangles,
		Points,
	};

	enum class TextureMinFilter
	{
		Nearest,
		Linear,
		NearestMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapNearest,
		LinearMipmapLinear,
	};

	enum class TextureMagFilter
	{
		Nearest,
		Linear,
	};

	enum class TextureWrapping
	{
		Repeat,
		MirroredRepeat,
		Clamp,
		ClampToEdge,
		ClampToBorder
	};

	enum class TexelsFormat
	{
		None = 0,
		R_U8,
		RG_U8,
		RGB_U8,
		RGBA_U8,
		Depth_U16,
		DepthStencil_U16,
		Depth_U24,
		DepthStencil_U24,
	};

	struct VertexAttribute
	{
		int location = -1;  // если -1, то берется индекс массива атрибутов
		int size;
		//unsigned type;
		bool normalized;
		int stride;         // sizeof Vertex
		const void* offset; // (void*)offsetof(Vertex, TexCoord)}
	};

	struct ShaderAttribInfo
	{
		unsigned typeId;
		std::string name;
		int location;
	};

	struct Texture2DInfo
	{
		ResourceUsage usage = ResourceUsage::Static;

		TextureMinFilter minFilter = TextureMinFilter::NearestMipmapNearest;
		TextureMagFilter magFilter = TextureMagFilter::Nearest;
		TextureWrapping wrapS = TextureWrapping::Repeat;
		TextureWrapping wrapT = TextureWrapping::Repeat;

		bool mipmap = true;
	};

	struct Texture2DCreateInfo
	{
		TexelsFormat format = TexelsFormat::RGBA_U8;
		uint16_t width = 1;
		uint16_t height = 1;
		uint8_t* pixelData = nullptr;
		unsigned mipMapCount = 1; // TODO: only compressed
	};

	struct ShaderProgram { unsigned id = 0; };
	struct Uniform { int location = -1; unsigned programId = 0; };
	struct VertexBuffer { unsigned id = 0; ResourceUsage usage = ResourceUsage::Static; unsigned count = 0; unsigned size = 0; };
	struct IndexBuffer { unsigned id = 0; ResourceUsage usage = ResourceUsage::Static; unsigned count = 0; unsigned size = 0; };
	struct VertexArray { unsigned id = 0; VertexBuffer *vbo = nullptr; IndexBuffer *ibo = nullptr; unsigned attribsCount = 0; };
	struct Texture2D { unsigned id = 0; unsigned width = 0; unsigned height = 0; TexelsFormat format = TexelsFormat::RGBA_U8; };

	inline bool IsValid(const ShaderProgram& resource) { return resource.id > 0; }
	inline bool IsValid(const Uniform& uniform) { return uniform.location >= 0; }
	inline bool IsValid(const VertexBuffer& resource) { return resource.id > 0; }
	inline bool IsValid(const IndexBuffer& resource) { return resource.id > 0; }
	inline bool IsValid(const VertexArray& resource) { return resource.id > 0; }
	inline bool IsValid(const Texture2D& resource) { return resource.id > 0; }
	bool IsReadyUniform(const Uniform& uniform);

	ShaderProgram CreateShaderProgram(const std::string& vertexShaderMemory, const std::string& fragmentShaderMemory);
	VertexBuffer CreateVertexBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data);
	IndexBuffer CreateIndexBuffer(ResourceUsage usage, unsigned indexCount, unsigned indexSize, const void* data);
	VertexArray CreateVertexArray(VertexBuffer* vbo, IndexBuffer* ibo, const std::vector<VertexAttribute>& attribs);
	VertexArray CreateVertexArray(VertexBuffer* vbo, IndexBuffer* ibo, const ShaderProgram& shaders);
	Texture2D CreateTexture2D(const char* fileName, const Texture2DInfo& textureInfo = {});
	Texture2D CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo = {});

	void DestroyResource(ShaderProgram& resource);
	void DestroyResource(VertexBuffer& resource);
	void DestroyResource(IndexBuffer& resource);
	void DestroyResource(VertexArray& resource);
	void DestroyResource(Texture2D& resource);

	std::vector<ShaderAttribInfo> GetAttribInfo(const ShaderProgram& resource);
	Uniform GetUniform(const ShaderProgram& program, const char* uniformName);

	void ResetState(ResourceType type);
	void ResetState(ResourceType type, unsigned slot);
	void Bind(const ShaderProgram& resource);
	void Bind(const VertexBuffer& resource);
	void Bind(const IndexBuffer& resource);
	void Bind(const VertexAttribute& Attribute);
	void Bind(const Texture2D& resource, unsigned slot = 0);

	void SetUniform(const Uniform& uniform, int value);
	void SetUniform(const Uniform& uniform, float value);
	void SetUniform(const Uniform& uniform, const glm::vec2& value);
	void SetUniform(const Uniform& uniform, const glm::vec3& value);
	void SetUniform(const Uniform& uniform, const glm::vec4& value);
	void SetUniform(const Uniform& uniform, const glm::mat3& value);
	void SetUniform(const Uniform& uniform, const glm::mat4& value);

	void Draw(const VertexArray& vao, PrimitiveDraw primitive = PrimitiveDraw::Triangles);

} // namespace render

using render::ShaderProgram;
using render::Uniform;
using render::VertexBuffer;
using render::IndexBuffer;
using render::VertexArray;
using render::Texture2D;