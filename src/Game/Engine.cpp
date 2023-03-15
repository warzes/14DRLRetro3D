#include "stdafx.h"
#include "Engine.h"
//-----------------------------------------------------------------------------
namespace std
{
	template <>
	struct hash<scene::VertexMesh>
	{
		size_t operator()(const scene::VertexMesh& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec2>()(vertex.texCoord) << 1)) >> 1);
		}
	};
} // namespace std
//-----------------------------------------------------------------------------
//=============================================================================
// Global Vars
//=============================================================================
//-----------------------------------------------------------------------------
#if defined(_WIN32)
FILE* LogFile = nullptr;
#endif
bool IsExitRequested = false;
unsigned CurrentShaderProgram = 0;
unsigned CurrentVBO = 0;
unsigned CurrentIBO = 0;
unsigned CurrentVAO = 0;
unsigned CurrentTexture2D[MaxBindingTextures] = { 0 };

// Cache
std::unordered_map<std::string, Texture2D> CacheTextures2D;

GLFWwindow* Window = nullptr;
int WindowWidth = 0;
int WindowHeight = 0;
struct
{
	char currentKeyState[MAX_KEYBOARD_KEYS] = { 0 };
	char previousKeyState[MAX_KEYBOARD_KEYS] = { 0 };

	int keyPressedQueue[MAX_KEY_PRESSED_QUEUE] = { 0 };
	int keyPressedQueueCount = 0; 

	int charPressedQueue[MAX_CHAR_PRESSED_QUEUE] = { 0 };
	int charPressedQueueCount = 0;
} Keyboard;
//-------------------------------------------------------------------------
struct
{
	glm::vec2 currentPosition = glm::vec2(0.0f);
	glm::vec2 previousPosition = glm::vec2(0.0f);

	bool cursorHidden = false;
	bool cursorOnScreen = false;

	char currentButtonState[MAX_MOUSE_BUTTONS] = { 0 };
	char previousButtonState[MAX_MOUSE_BUTTONS] = { 0 };
	glm::vec2 currentWheelMove = glm::vec2(0.0f);
	glm::vec2 previousWheelMove = glm::vec2(0.0f);
} Mouse;
double LastFrameTime = 0.0;
double TimeDelta = 0.0;
uint32_t CurrentFrame = 0;
//-----------------------------------------------------------------------------
//=============================================================================
// Logging
//=============================================================================
//-----------------------------------------------------------------------------
void LogCreate(const std::string& fileName)
{
#if defined(_WIN32)
	errno_t fileErr = fopen_s(&LogFile, fileName.c_str(), "w");
	if( fileErr != 0 || !LogFile )
	{
		LogError("LogCreate() failed!!!");
		LogFile = nullptr;
	}
#endif
}
//-----------------------------------------------------------------------------
void LogDestroy()
{
#if defined(_WIN32)
	if( LogFile )
	{
		fclose(LogFile);
		LogFile = nullptr;
	}
#endif
}
//-----------------------------------------------------------------------------
void LogPrint(const std::string& msg)
{
	puts(msg.c_str());
#if defined(_WIN32)
	if( LogFile ) fputs((msg + "\n").c_str(), LogFile);
#endif
}
//-----------------------------------------------------------------------------
void LogWarning(const std::string& msg)
{
	LogPrint("Warning: " + msg);
}
//-----------------------------------------------------------------------------
void LogError(const std::string& msg)
{
	LogPrint("Error: " + msg);
}
//-----------------------------------------------------------------------------
void Fatal(const std::string& msg)
{
	IsExitRequested = true;
	LogError(msg);
}
//-----------------------------------------------------------------------------
//=============================================================================
// Render Resource
//=============================================================================
//-----------------------------------------------------------------------------
inline GLenum translateToGL(render::ResourceUsage usage)
{
	switch( usage )
	{
	case render::ResourceUsage::Static:  return GL_STATIC_DRAW;
	case render::ResourceUsage::Dynamic: return GL_DYNAMIC_DRAW;
	case render::ResourceUsage::Stream:  return GL_STREAM_DRAW;
	}
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum translateToGL(render::PrimitiveDraw p)
{
	switch( p )
	{
	case render::PrimitiveDraw::Lines:     return GL_LINES;
	case render::PrimitiveDraw::Triangles: return GL_TRIANGLES;
	case render::PrimitiveDraw::Points:    return GL_POINTS;
	}
	return 0;
}
//-----------------------------------------------------------------------------
inline GLint translateToGL(render::TextureWrapping wrap)
{
	switch( wrap )
	{
	case render::TextureWrapping::Repeat:         return GL_REPEAT;
	case render::TextureWrapping::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case render::TextureWrapping::ClampToEdge:    return GL_CLAMP_TO_EDGE;
#if !defined(__EMSCRIPTEN__)
	case render::TextureWrapping::ClampToBorder:  return GL_CLAMP_TO_BORDER;
#endif
	}
	return 0;
}
//-----------------------------------------------------------------------------
inline GLint translateToGL(render::TextureMinFilter filter)
{
	switch( filter )
	{
	case render::TextureMinFilter::Nearest:              return GL_NEAREST;
	case render::TextureMinFilter::Linear:               return GL_LINEAR;
	case render::TextureMinFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
	case render::TextureMinFilter::NearestMipmapLinear:  return GL_NEAREST_MIPMAP_LINEAR;
	case render::TextureMinFilter::LinearMipmapNearest:  return GL_LINEAR_MIPMAP_NEAREST;
	case render::TextureMinFilter::LinearMipmapLinear:   return GL_LINEAR_MIPMAP_LINEAR;
	}
	return 0;
}
//-----------------------------------------------------------------------------
inline constexpr GLint translateToGL(render::TextureMagFilter filter)
{
	switch( filter )
	{
	case render::TextureMagFilter::Nearest: return GL_NEAREST;
	case render::TextureMagFilter::Linear:  return GL_LINEAR;
	}
	return 0;
}
//-----------------------------------------------------------------------------
inline bool getTextureFormatType(render::TexelsFormat inFormat, GLenum textureType, GLenum& format, GLint& internalFormat, GLenum& oglType)
{
	if( inFormat == render::TexelsFormat::R_U8 )
	{
		format = GL_RED;
		internalFormat = GL_R8;
		oglType = GL_UNSIGNED_BYTE;
	}
	else if( inFormat == render::TexelsFormat::RG_U8 )
	{
#if defined(_WIN32)
		format = GL_RG;
		internalFormat = GL_RG8;
		oglType = GL_UNSIGNED_BYTE;
		const GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
		glTexParameteriv(textureType, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask); // TODO: могут быть проблемы с браузерами, тогда только грузить stb с указанием нужного формата
#endif // _WIN32
#if defined(__EMSCRIPTEN__)
		Fatal("TexelsFormat::RG_U8 not support in web platform");
		return false;
#endif
	}
	else if( inFormat == render::TexelsFormat::RGB_U8 )
	{
		format = GL_RGB;
		internalFormat = GL_RGB8;
		oglType = GL_UNSIGNED_BYTE;
	}
	else if( inFormat == render::TexelsFormat::RGBA_U8 )
	{
		format = GL_RGBA;
		internalFormat = GL_RGBA8;
		oglType = GL_UNSIGNED_BYTE;
	}
	else if( inFormat == render::TexelsFormat::Depth_U16 )
	{
		format = GL_DEPTH_COMPONENT;
		internalFormat = GL_DEPTH_COMPONENT16;
		oglType = GL_UNSIGNED_SHORT;
	}
	else if( inFormat == render::TexelsFormat::DepthStencil_U16 )
	{
		format = GL_DEPTH_STENCIL;
		internalFormat = GL_DEPTH24_STENCIL8;
		oglType = GL_UNSIGNED_SHORT;
	}
	else if( inFormat == render::TexelsFormat::Depth_U24 )
	{
		format = GL_DEPTH_COMPONENT;
		internalFormat = GL_DEPTH_COMPONENT24;
		oglType = GL_UNSIGNED_INT;
	}
	else if( inFormat == render::TexelsFormat::DepthStencil_U24 )
	{
		format = GL_DEPTH_STENCIL;
		internalFormat = GL_DEPTH24_STENCIL8;
		oglType = GL_UNSIGNED_INT;
	}
	else
	{
		LogError("unknown texture format");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
unsigned createShader(GLenum openGLshaderType, const std::string& source)
{
	const char* shaderText = source.data();
	const GLint lenShaderText = static_cast<GLint>(source.size());
	const GLuint shaderId = glCreateShader(openGLshaderType);
	glShaderSource(shaderId, 1, &shaderText, &lenShaderText);
	glCompileShader(shaderId);

	GLint compileStatus = GL_FALSE;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
	if( compileStatus == GL_FALSE )
	{
		GLint infoLogSize;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogSize);
		std::vector<GLchar> errorInfo(static_cast<size_t>(infoLogSize));
		glGetShaderInfoLog(shaderId, (GLsizei)errorInfo.size(), nullptr, &errorInfo[0]);
		glDeleteShader(shaderId);

		std::string shaderName;
		switch( openGLshaderType )
		{
		case GL_VERTEX_SHADER: shaderName = "Vertex "; break;
		case GL_FRAGMENT_SHADER: shaderName = "Fragment "; break;
		}
		LogError(shaderName + "Shader compilation failed : " + std::string(&errorInfo[0]) + ", Source: " + source);
		return 0;
	}

	return shaderId;
}
// если какая-то структура была изменена, надо не забыть внести изменения в operator==
static_assert(sizeof(ShaderProgram) == 4, "ShaderProgram changed!!!");
static_assert(sizeof(Uniform) == 8, "Uniform changed!!!");
static_assert(sizeof(VertexBuffer) == 16, "VertexBuffer changed!!!");
static_assert(sizeof(IndexBuffer) == 16, "IndexBuffer changed!!!");
static_assert(sizeof(VertexArray) == 32, "VertexArray changed!!!");
static_assert(sizeof(Texture2D) == 16, "Texture2D changed!!!");
//-----------------------------------------------------------------------------
bool render::operator==(const ShaderProgram& Left, const ShaderProgram& Right) noexcept
{
	return Left.id == Right.id;
}
//-----------------------------------------------------------------------------
bool render::operator==(const Uniform& Left, const Uniform& Right) noexcept
{
	return Left.location == Right.location && Left.programId == Right.programId;
}
//-----------------------------------------------------------------------------
bool render::operator==(const VertexBuffer& Left, const VertexBuffer& Right) noexcept
{

	return Left.count == Right.count && Left.id == Right.id && Left.size == Right.size && Left.usage == Right.usage;
}
//-----------------------------------------------------------------------------
bool render::operator==(const IndexBuffer& Left, const IndexBuffer& Right) noexcept
{
	return Left.count == Right.count && Left.id == Right.id && Left.size == Right.size && Left.usage == Right.usage;
}
//-----------------------------------------------------------------------------
bool render::operator==(const VertexArray& Left, const VertexArray& Right) noexcept
{
	return Left.attribsCount == Right.attribsCount && Left.ibo == Right.ibo && Left.id == Right.id && Left.vbo == Right.vbo;
}
//-----------------------------------------------------------------------------
bool render::operator==(const Texture2D& Left, const Texture2D& Right) noexcept
{
	return Left.format == Right.format && Left.height == Right.height && Left.id == Right.id && Left.width == Right.width;
}
//-----------------------------------------------------------------------------
bool render::IsReadyUniform(const Uniform& uniform)
{
	return IsValid(uniform) && uniform.programId == CurrentShaderProgram;
}
//-----------------------------------------------------------------------------
ShaderProgram render::CreateShaderProgram(const std::string& vertexShaderMemory, const std::string& fragmentShaderMemory)
{
	ShaderProgram resource;
	const GLuint glShaderVertex = createShader(GL_VERTEX_SHADER, vertexShaderMemory);
	const GLuint glShaderFragment = createShader(GL_FRAGMENT_SHADER, fragmentShaderMemory);

	if( glShaderVertex > 0 && glShaderFragment > 0 )
	{
		resource.id = glCreateProgram();
		glAttachShader(resource.id, glShaderVertex);
		glAttachShader(resource.id, glShaderFragment);
		glLinkProgram(resource.id);

		GLint linkStatus = 0;
		glGetProgramiv(resource.id, GL_LINK_STATUS, &linkStatus);
		if( linkStatus == GL_FALSE )
		{
			GLint errorMsgLen;
			glGetProgramiv(resource.id, GL_INFO_LOG_LENGTH, &errorMsgLen);

			std::vector<GLchar> errorInfo(static_cast<size_t>(errorMsgLen));
			glGetProgramInfoLog(resource.id, errorMsgLen, nullptr, &errorInfo[0]);
			LogError("OPENGL: Shader program linking failed: " + std::string(&errorInfo[0]));
			glDeleteProgram(resource.id);
			resource.id = 0;
		}
		glDetachShader(resource.id, glShaderVertex);
		glDetachShader(resource.id, glShaderFragment);
	}
	glDeleteShader(glShaderVertex);
	glDeleteShader(glShaderFragment);

	return resource;
}
//-----------------------------------------------------------------------------
VertexBuffer render::CreateVertexBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data)
{
	VertexBuffer resource;
	resource.count = vertexCount;
	resource.size = vertexSize;
	resource.usage = usage;

	glGenBuffers(1, &resource.id);
	glBindBuffer(GL_ARRAY_BUFFER, resource.id);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, data, translateToGL(usage));
	glBindBuffer(GL_ARRAY_BUFFER, CurrentVBO); // restore current vb
	return resource;
}
//-----------------------------------------------------------------------------
IndexBuffer render::CreateIndexBuffer(ResourceUsage usage, unsigned indexCount, unsigned indexSize, const void* data)
{
	IndexBuffer resource;
	resource.count = indexCount;
	resource.size = indexSize;
	resource.usage = usage;

	glGenBuffers(1, &resource.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, data, translateToGL(usage));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CurrentIBO); // restore current ib
	return resource;
}
//-----------------------------------------------------------------------------
VertexArray render::CreateVertexArray(VertexBuffer* vbo, IndexBuffer* ibo, const std::vector<VertexAttribute>& attribs)
{
	VertexArray resource;

	resource.vbo = vbo;
	resource.ibo = ibo;
	resource.attribsCount = static_cast<unsigned>(attribs.size());

	glGenVertexArrays(1, &resource.id);
	glBindVertexArray(resource.id);

	Bind(*resource.vbo);
	for( size_t i = 0; i < attribs.size(); i++ )
	{
		Bind(attribs[i]);
	}

	if( resource.ibo ) Bind(*resource.ibo);

	glBindVertexArray(CurrentVAO); // restore VAO

	return resource;
}
//-----------------------------------------------------------------------------
VertexArray render::CreateVertexArray(VertexBuffer* vbo, IndexBuffer* ibo, const ShaderProgram& shaders)
{
	assert(IsValid(shaders));
	if (!IsValid(shaders)) return {};

	auto attribInfo = GetAttribInfo(shaders);
	if( attribInfo.empty() ) return {};

	size_t offset = 0;
	std::vector<VertexAttribute> attribs(attribInfo.size());
	for( size_t i = 0; i < attribInfo.size(); i++ )
	{
		assert(attribInfo[i].location>-1);
		attribs[i].location = (unsigned)attribInfo[i].location; // TODO: attribInfo[i].location может быть -1, тогда как в attribs[i].location unsigned. нужно исправить
		attribs[i].normalized = false;

		size_t sizeType = 0;
		switch( attribInfo[i].typeId )
		{
		case GL_FLOAT:
			attribs[i].size = 1;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC2:
			attribs[i].size = 2;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC3:
			attribs[i].size = 3;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC4:
			attribs[i].size = 4;
			sizeType = attribs[i].size * sizeof(float);
			break;

		default:
			LogError("Shader attribute type: " + attribInfo[i].name + " not support!");
			return {};
		}

		attribs[i].offset = (void*)+offset;
		offset += sizeType;
	}
	for( size_t i = 0; i < attribs.size(); i++ )
	{
		attribs[i].stride = (int)offset;
	}

	return CreateVertexArray(vbo, ibo, attribs);
}
//-----------------------------------------------------------------------------
Texture2D render::CreateTexture2D(const char* fileName, bool useCache, const Texture2DInfo& textureInfo)
{
	if( useCache )
	{
		auto it = CacheTextures2D.find(fileName);
		if( it != CacheTextures2D.end() )
			return it->second;
	}

	LogPrint("Load texture: " + std::string(fileName));

	//stbi_set_flip_vertically_on_load(verticallyFlip ? 1 : 0);
	const int desiredСhannels = STBI_default;
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	stbi_uc* pixelData = stbi_load(fileName, &width, &height, &nrChannels, desiredСhannels);
	if( !pixelData || nrChannels < STBI_grey || nrChannels > STBI_rgb_alpha || width == 0 || height == 0 )
	{
		LogError("Image loading failed! Filename='" + std::string(fileName) + "'");
		stbi_image_free((void*)pixelData);
		return {};
	}
	Texture2DCreateInfo createInfo;
	{
		if( nrChannels == STBI_grey ) createInfo.format = TexelsFormat::R_U8;
		else if( nrChannels == STBI_grey_alpha ) createInfo.format = TexelsFormat::RG_U8;
		else if( nrChannels == STBI_rgb ) createInfo.format = TexelsFormat::RGB_U8;
		else if( nrChannels == STBI_rgb_alpha ) createInfo.format = TexelsFormat::RGBA_U8;

		createInfo.width = static_cast<uint16_t>(width);
		createInfo.height = static_cast<uint16_t>(height);
		createInfo.pixelData = pixelData;
	}
	Texture2D texture = CreateTexture2D(createInfo, textureInfo);

	stbi_image_free((void*)pixelData);
	CacheTextures2D[fileName] = texture;
	return texture;
}
//-----------------------------------------------------------------------------
Texture2D render::CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo)
{
	Texture2D texture;
	texture.width = createInfo.width;
	texture.height = createInfo.height;

	// gen texture res
	glGenTextures(1, &texture.id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, translateToGL(textureInfo.wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, translateToGL(textureInfo.wrapT));

	// set texture filtering parameters
	TextureMinFilter minFilter = textureInfo.minFilter;
	if( !textureInfo.mipmap )
	{
		if( textureInfo.minFilter == TextureMinFilter::NearestMipmapNearest ) minFilter = TextureMinFilter::Nearest;
		else if( textureInfo.minFilter != TextureMinFilter::Nearest ) minFilter = TextureMinFilter::Linear;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, translateToGL(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, translateToGL(textureInfo.magFilter));

	// set texture format
	texture.format = createInfo.format;
	GLenum format = GL_RGB;
	GLint internalFormat = GL_RGB;
	GLenum oglType = GL_UNSIGNED_BYTE;
	if( !getTextureFormatType(createInfo.format, GL_TEXTURE_2D, format, internalFormat, oglType) )
	{
		glDeleteTextures(1, &texture.id);
		glBindTexture(GL_TEXTURE_2D, CurrentTexture2D[0]);
		return {};
	}		

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, (GLsizei)texture.width, (GLsizei)texture.height, 0, format, oglType, createInfo.pixelData);

	if( textureInfo.mipmap )
		glGenerateMipmap(GL_TEXTURE_2D);

	// restore prev state
	glBindTexture(GL_TEXTURE_2D, CurrentTexture2D[0]);

	return texture;
}
//-----------------------------------------------------------------------------
void render::DestroyResource(ShaderProgram& resource)
{
	if( resource.id == 0 ) return;
	if( CurrentShaderProgram == resource.id ) ResetState(ResourceType::ShaderProgram);
	glDeleteProgram(resource.id);
	resource.id = 0;
}
//-----------------------------------------------------------------------------
void render::DestroyResource(VertexBuffer& resource)
{
	if( resource.id == 0 ) return;
	if( CurrentVBO == resource.id ) ResetState(ResourceType::VertexBuffer);
	glDeleteBuffers(1, &resource.id);
	resource.id = 0;
}
//-----------------------------------------------------------------------------
void render::DestroyResource(IndexBuffer& resource)
{
	if( resource.id == 0 ) return;
	if( CurrentIBO == resource.id ) ResetState(ResourceType::IndexBuffer);
	glDeleteBuffers(1, &resource.id);
	resource.id = 0;
}
//-----------------------------------------------------------------------------
void render::DestroyResource(VertexArray& resource)
{
	if( resource.id == 0 ) return;
	if( CurrentVAO == resource.id ) ResetState(ResourceType::VertexArray);
	glDeleteVertexArrays(1, &resource.id);
	resource.id = 0;
}
//-----------------------------------------------------------------------------
void render::DestroyResource(Texture2D& resource)
{
	if( resource.id == 0 ) return;
	for( unsigned i = 0; i < MaxBindingTextures; i++ )
	{
		if( CurrentTexture2D[i] == resource.id )
			ResetState(ResourceType::Texture2D, i);
	}
	glDeleteTextures(1, &resource.id);
	resource.id = 0;
}
//-----------------------------------------------------------------------------
std::vector<render::ShaderAttribInfo> render::GetAttribInfo(const ShaderProgram& resource)
{
	if( !IsValid(resource) ) return {};

	int activeAttribs = 0;
	glGetProgramiv(resource.id, GL_ACTIVE_ATTRIBUTES, &activeAttribs);
	int maxLength = 0;
	glGetProgramiv(resource.id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

	std::string name;
	name.resize(static_cast<size_t>(maxLength));

	std::vector<ShaderAttribInfo> attribs(static_cast<size_t>(activeAttribs));
	for( int i = 0; i < activeAttribs; i++ )
	{
		GLint size;
		GLenum type = 0;
		glGetActiveAttrib(resource.id, (GLuint)i, maxLength, nullptr, &size, &type, name.data());

		attribs[i] = {
			.typeId = type,
			.name = name,
			.location = glGetAttribLocation(resource.id, name.c_str())
		};
	}

	std::sort(attribs.begin(), attribs.end(), [](const ShaderAttribInfo& a, const ShaderAttribInfo& b) {return a.location < b.location; });

	return attribs;
}
//-----------------------------------------------------------------------------
Uniform render::GetUniform(const ShaderProgram& program, const char* uniformName)
{
	if( !IsValid(program) || uniformName == nullptr) return {};

	if ( CurrentShaderProgram != program.id ) glUseProgram(program.id);
	Uniform uniform;
	uniform.location = glGetUniformLocation(program.id, uniformName);
	uniform.programId = program.id;
	glUseProgram(CurrentShaderProgram); // restore prev shader program
	return uniform;
}
//-----------------------------------------------------------------------------
void render::SetUniform(const Uniform& uniform, int value)
{
	assert(IsReadyUniform(uniform));
	glUniform1i(uniform.location, value);
}
//-----------------------------------------------------------------------------
void render::SetUniform(const Uniform& uniform, float value)
{
	assert(IsReadyUniform(uniform));
	glUniform1f(uniform.location, value);
}
//-----------------------------------------------------------------------------
void render::SetUniform(const Uniform& uniform, const glm::vec2& value)
{
	assert(IsReadyUniform(uniform));
	glUniform2fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void render::SetUniform(const Uniform& uniform, const glm::vec3& value)
{
	assert(IsReadyUniform(uniform));
	glUniform3fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void render::SetUniform(const Uniform& uniform, const glm::vec4& value)
{
	assert(IsReadyUniform(uniform));
	glUniform4fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void render::SetUniform(const Uniform& uniform, const glm::mat3& value)
{
	assert(IsReadyUniform(uniform));
	glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void render::SetUniform(const Uniform& uniform, const glm::mat4& value)
{
	assert(IsReadyUniform(uniform));
	glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void render::UpdateVertexBuffer(VertexBuffer& vbo, unsigned offset, unsigned vertexCount, unsigned vertexSize, const void* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id);

	if( vbo.count != vertexCount || vbo.size != vertexSize || vbo.usage != ResourceUsage::Dynamic )
	{
		glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, data, translateToGL(ResourceUsage::Dynamic));
		vbo.usage = ResourceUsage::Dynamic;
	}
	else
	{
		glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount * vertexSize, data);
	}
	vbo.count = vertexCount;
	vbo.size = vertexSize;

	glBindBuffer(GL_ARRAY_BUFFER, CurrentVBO); // restore current vb
}
//-----------------------------------------------------------------------------
void render::UpdateIndexBuffer(IndexBuffer& ibo, unsigned offset, unsigned indexCount, unsigned indexSize, const void* data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.id);
	if( ibo.count != indexCount || ibo.size != indexSize || ibo.usage != ResourceUsage::Dynamic )
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, data, translateToGL(ResourceUsage::Dynamic));
		ibo.usage = ResourceUsage::Dynamic;
	}
	else
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, indexCount * indexSize, data);
	}
	ibo.count = indexCount;
	ibo.size = indexSize;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CurrentIBO); // restore current ib
}
//-----------------------------------------------------------------------------
void render::ResetState(ResourceType type)
{
	if( type == ResourceType::ShaderProgram )
	{
		CurrentShaderProgram = 0;
		glUseProgram(0);
	}
	else if( type == ResourceType::VertexBuffer )
	{
		CurrentVBO = 0;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else if( type == ResourceType::IndexBuffer )
	{
		CurrentIBO = 0;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else if( type == ResourceType::VertexArray )
	{
		ResetState(ResourceType::VertexBuffer);
		ResetState(ResourceType::IndexBuffer);
		CurrentVAO = 0;
		glBindVertexArray(0);
	}
	else if( type == ResourceType::Texture2D )
	{
		for( unsigned i = 0; i < MaxBindingTextures; i++ )
		{
			ResetState(type, i);
		}
	}
}
//-----------------------------------------------------------------------------
void render::ResetState(ResourceType type, unsigned slot)
{
	if( type == ResourceType::Texture2D )
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, 0);
		CurrentTexture2D[slot] = 0;
	}
}
//-----------------------------------------------------------------------------
void render::Bind(const ShaderProgram& resource)
{
	assert(IsValid(resource));
	if( CurrentShaderProgram == resource.id ) return;
	CurrentShaderProgram = resource.id;
	glUseProgram(resource.id);
}
//-----------------------------------------------------------------------------
void render::Bind(const VertexBuffer& resource)
{
	assert(IsValid(resource));
	if( CurrentVBO == resource.id ) return;
	CurrentVBO = resource.id;
	glBindBuffer(GL_ARRAY_BUFFER, resource.id);
}
//-----------------------------------------------------------------------------
void render::Bind(const IndexBuffer& resource)
{
	assert(IsValid(resource));
	if( CurrentIBO == resource.id ) return;
	CurrentIBO = resource.id;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource.id);
}
//-----------------------------------------------------------------------------
void render::Bind(const VertexAttribute& attribute)
{
	const GLuint oglLocation = static_cast<GLuint>(attribute.location);
	glEnableVertexAttribArray(oglLocation);
	glVertexAttribPointer(
		oglLocation, 
		attribute.size, 
		GL_FLOAT, 
		(GLboolean)(attribute.normalized ? GL_TRUE : GL_FALSE), 
		attribute.stride, 
		attribute.offset);
}
//-----------------------------------------------------------------------------
void render::Bind(const Texture2D& resource, unsigned slot)
{
	assert(IsValid(resource));
	if( CurrentTexture2D[slot] == resource.id ) return;
	CurrentTexture2D[slot] = resource.id;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, resource.id);
}
//-----------------------------------------------------------------------------
void render::Draw(const VertexArray& vao, PrimitiveDraw primitive)
{
	assert(IsValid(vao));
	if( CurrentVAO != vao.id )
	{
		CurrentVAO = vao.id;
		glBindVertexArray(vao.id);
		Bind(*vao.vbo);
		if( vao.ibo ) Bind(*vao.ibo);
	}

	if( vao.ibo )
	{
		const GLenum indexSizeType = (GLenum)(vao.ibo->size == sizeof(uint32_t) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT);
		glDrawElements(translateToGL(primitive), (GLsizei)vao.ibo->count, indexSizeType, nullptr);
	}
	else
	{
		glDrawArrays(translateToGL(primitive), 0, (GLsizei)vao.vbo->count);
	}
}
//-----------------------------------------------------------------------------
//=============================================================================
// Scene System
//=============================================================================
//-----------------------------------------------------------------------------
glm::vec3 getNormalizedViewVector(const scene::Camera& camera)
{
	return glm::normalize(camera.viewPoint - camera.position);
}
//-----------------------------------------------------------------------------
glm::mat4 scene::GetCameraViewMatrix(const Camera& camera)
{
	return glm::lookAt(camera.position, camera.viewPoint, camera.upVector);
}
//-----------------------------------------------------------------------------
void scene::CameraMoveBy(Camera& camera, float distance)
{
	const glm::vec3 vOffset = getNormalizedViewVector(camera) * distance;
	camera.position += vOffset;
	camera.viewPoint += vOffset;
}
//-----------------------------------------------------------------------------
void scene::CameraStrafeBy(Camera& camera, float distance)
{
	const glm::vec3 strafeVector = glm::normalize(glm::cross(getNormalizedViewVector(camera), camera.upVector)) * distance;
	camera.position += strafeVector;
	camera.viewPoint += strafeVector;
}
//-----------------------------------------------------------------------------
void scene::CameraRotateLeftRight(Camera& camera, float angleInDegrees)
{
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleInDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 rotatedViewVector = rotationMatrix * glm::vec4(getNormalizedViewVector(camera), 0.0f);
	camera.viewPoint = camera.position + glm::vec3(rotatedViewVector);
}
//-----------------------------------------------------------------------------
void scene::CameraRotateUpDown(Camera& camera, float angleInDegrees)
{
	const glm::vec3 viewVector = getNormalizedViewVector(camera);
	const glm::vec3 viewVectorNoY = glm::normalize(glm::vec3(viewVector.x, 0.0f, viewVector.z));

	float currentAngleDegrees = glm::degrees(acos(glm::dot(viewVectorNoY, viewVector)));
	if( viewVector.y < 0.0f ) currentAngleDegrees = -currentAngleDegrees;

	float newAngleDegrees = currentAngleDegrees + angleInDegrees;
	if( newAngleDegrees > -85.0f && newAngleDegrees < 85.0f )
	{
		glm::vec3 rotationAxis = glm::cross(getNormalizedViewVector(camera), camera.upVector);
		rotationAxis = glm::normalize(rotationAxis);

		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleInDegrees), rotationAxis);
		glm::vec4 rotatedViewVector = rotationMatrix * glm::vec4(getNormalizedViewVector(camera), 0.0f);

		camera.viewPoint = camera.position + glm::vec3(rotatedViewVector);
	}
}
//-----------------------------------------------------------------------------
scene::GeometryBuffer::GeometryBuffer(scene::GeometryBuffer&& ref)
{
	vb = ref.vb;
	ib = ref.ib;
	vao = ref.vao;
	// ref будет удален, поэтому надо обновить ссылки
	if( ref.vao.vbo ) vao.vbo = &vb;
	if( ref.vao.ibo ) vao.ibo = &ib;
}
//-----------------------------------------------------------------------------
scene::GeometryBuffer::GeometryBuffer(const scene::GeometryBuffer& ref)
{
	vb = ref.vb;
	ib = ref.ib;
	vao = ref.vao;
	// ref будет удален, поэтому надо обновить ссылки
	if( ref.vao.vbo ) vao.vbo = &vb;
	if( ref.vao.ibo ) vao.ibo = &ib;
}
//-----------------------------------------------------------------------------
scene::GeometryBuffer& scene::GeometryBuffer::operator=(scene::GeometryBuffer&& ref)
{
	vb = ref.vb;
	ib = ref.ib;
	vao = ref.vao;
	// ref будет удален, поэтому надо обновить ссылки
	if( ref.vao.vbo ) vao.vbo = &vb;
	if( ref.vao.ibo ) vao.ibo = &ib;
	return *this;
}
//-----------------------------------------------------------------------------
scene::GeometryBuffer& scene::GeometryBuffer::operator=(const scene::GeometryBuffer& ref)
{
	vb = ref.vb;
	ib = ref.ib;
	vao = ref.vao;
	// ref будет удален, поэтому надо обновить ссылки
	if( ref.vao.vbo ) vao.vbo = &vb;
	if( ref.vao.ibo ) vao.ibo = &ib;
	return *this;
}
//-----------------------------------------------------------------------------
GeometryBuffer scene::CreateGeometryBuffer(render::ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, unsigned indexCount, unsigned indexSize, const void* indexData, const ShaderProgram& shaders)
{
	assert(render::IsValid(shaders));

	GeometryBuffer geom;

	geom.vb = render::CreateVertexBuffer(usage, vertexCount, vertexSize, vertexData);
	if( !render::IsValid(geom.vb) )
	{
		LogError("GeometryBuffer::VertexBuffer create failed!!");
		return {};
	}

	bool isIndexBuffer = indexCount > 0 && indexSize > 0;

	if( isIndexBuffer )
	{
		geom.ib = render::CreateIndexBuffer(usage, indexCount, indexSize, indexData);
		if( !render::IsValid(geom.ib) )
		{
			LogError("GeometryBuffer::IndexBuffer create failed!!");
			render::DestroyResource(geom.vb);
			return {};
		}
	}

	geom.vao = render::CreateVertexArray(&geom.vb, isIndexBuffer ? &geom.ib : nullptr, shaders);
	if( !render::IsValid(geom.vao) )
	{
		LogError("GeometryBuffer::VertexArray create failed!!");
		render::DestroyResource(geom.vb);
		if ( isIndexBuffer) render::DestroyResource(geom.ib);
		return {};
	}
	return geom;
}
//-----------------------------------------------------------------------------
GeometryBuffer scene::CreateGeometryBuffer(render::ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, const ShaderProgram& shaders)
{
	return CreateGeometryBuffer(usage, vertexCount, vertexSize, vertexData, 0, 0, nullptr, shaders);
}
//-----------------------------------------------------------------------------
void scene::Destroy(GeometryBuffer& buffer)
{
	render::DestroyResource(buffer.vb);
	render::DestroyResource(buffer.ib);
	render::DestroyResource(buffer.vao);
}
//-----------------------------------------------------------------------------
bool scene::IsValid(const GeometryBuffer& buffer)
{
	const bool validVB = IsValid(buffer.vb);
	const bool validIB = (buffer.ib.count > 0 && buffer.ib.size > 0) ? IsValid(buffer.ib) : true;
	const bool validVAO = IsValid(buffer.vao);
	const bool vbEqual = buffer.vb == *buffer.vao.vbo;
	const bool ibEqual = buffer.ib == *buffer.vao.ibo;

	return validVB && validIB && validVAO && vbEqual && ibEqual;
}
//-----------------------------------------------------------------------------
Model createMeshBuffer(std::vector<Mesh>&& meshes)
{
	const std::vector<render::VertexAttribute> formatVertex =
	{
		{.location = 0, .size = 3, .normalized = false, .stride = sizeof(VertexMesh), .offset = (void*)offsetof(VertexMesh, position)},
		{.location = 1, .size = 3, .normalized = false, .stride = sizeof(VertexMesh), .offset = (void*)offsetof(VertexMesh, normal)},
		{.location = 2, .size = 3, .normalized = false, .stride = sizeof(VertexMesh), .offset = (void*)offsetof(VertexMesh, color)},
		{.location = 3, .size = 2, .normalized = false, .stride = sizeof(VertexMesh), .offset = (void*)offsetof(VertexMesh, texCoord)}
	};

	Model model;
	model.subMeshes = std::move(meshes);
	model.min = model.subMeshes[0].min;
	model.max = model.subMeshes[0].max;

	for( size_t i = 0; i < model.subMeshes.size(); i++ )
	{
		model.subMeshes[i].vertexBuffer = render::CreateVertexBuffer(render::ResourceUsage::Static, model.subMeshes[i].vertices.size(), sizeof(model.subMeshes[i].vertices[0]), model.subMeshes[i].vertices.data());
		if( !render::IsValid(model.subMeshes[i].vertexBuffer) )
		{
			LogError("VertexBuffer create failed!");
			Destroy(model);
			return {};
		}

		model.subMeshes[i].indexBuffer = render::CreateIndexBuffer(render::ResourceUsage::Static, model.subMeshes[i].indices.size(), sizeof(model.subMeshes[i].indices[0]), model.subMeshes[i].indices.data());
		if( !render::IsValid(model.subMeshes[i].indexBuffer) )
		{
			LogError("IndexBuffer create failed!");
			Destroy(model);
			return {};
		}

		model.subMeshes[i].vao = render::CreateVertexArray(&model.subMeshes[i].vertexBuffer, &model.subMeshes[i].indexBuffer, formatVertex);
		if( !render::IsValid(model.subMeshes[i].vao) )
		{
			LogError("VAO create failed!");
			Destroy(model);
			return {};
		}

		// compute AABB
		{
			model.min = glm::min(model.min, model.subMeshes[i].min);
			model.max = glm::min(model.max, model.subMeshes[i].max);
		}
	}
	return model;
}
//-----------------------------------------------------------------------------
Model loadObjFile(const char* fileName, const char* pathMaterialFiles = "./")
{
	tinyobj::ObjReaderConfig readerConfig;
	readerConfig.mtl_search_path = pathMaterialFiles; // Path to material files

	tinyobj::ObjReader reader;
	if( !reader.ParseFromFile(fileName, readerConfig) )
	{
		if( !reader.Error().empty() )
			LogError("TinyObjReader: " + reader.Error());
		return {};
	}
	if( !reader.Warning().empty() )
		LogWarning("TinyObjReader: " + reader.Warning());

	auto& attributes = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();
	const bool isFindMaterials = !materials.empty();

	std::vector<Mesh> tempMesh(materials.size());
	std::vector<std::unordered_map<VertexMesh, uint32_t>> uniqueVertices(materials.size());
	if( tempMesh.empty() )
	{
		tempMesh.resize(1);
		uniqueVertices.resize(1);
	}

	// Loop over shapes
	for( size_t shapeId = 0; shapeId < shapes.size(); shapeId++ )
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for( size_t faceId = 0; faceId < shapes[shapeId].mesh.num_face_vertices.size(); faceId++ )
		{
			const size_t fv = static_cast<size_t>(shapes[shapeId].mesh.num_face_vertices[faceId]);

			// per-face material
			int materialId = shapes[shapeId].mesh.material_ids[faceId];
			if( materialId < 0 ) materialId = 0;

			// Loop over vertices in the face.
			for( size_t v = 0; v < fv; v++ )
			{
				// access to vertex
				const tinyobj::index_t idx = shapes[shapeId].mesh.indices[index_offset + v];

				// vertex position
				const tinyobj::real_t vx = attributes.vertices[3 * size_t(idx.vertex_index) + 0];
				const tinyobj::real_t vy = attributes.vertices[3 * size_t(idx.vertex_index) + 1];
				const tinyobj::real_t vz = attributes.vertices[3 * size_t(idx.vertex_index) + 2];

				// Check if `normal_index` is zero or positive. negative = no normal data
				tinyobj::real_t nx = 0.0f;
				tinyobj::real_t ny = 0.0f;
				tinyobj::real_t nz = 0.0f;
				if( idx.normal_index >= 0 )
				{
					nx = attributes.normals[3 * size_t(idx.normal_index) + 0];
					ny = attributes.normals[3 * size_t(idx.normal_index) + 1];
					nz = attributes.normals[3 * size_t(idx.normal_index) + 2];
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				tinyobj::real_t tx = 0;
				tinyobj::real_t ty = 0;
				if( idx.texcoord_index >= 0 )
				{
					tx = attributes.texcoords[2 * size_t(idx.texcoord_index) + 0];
					ty = attributes.texcoords[2 * size_t(idx.texcoord_index) + 1];
				}

				// vertex colors
				const tinyobj::real_t r = attributes.colors[3 * size_t(idx.vertex_index) + 0];
				const tinyobj::real_t g = attributes.colors[3 * size_t(idx.vertex_index) + 1];
				const tinyobj::real_t b = attributes.colors[3 * size_t(idx.vertex_index) + 2];

				VertexMesh vertex = { 
					.position = { vx, vy, vz },
					.normal = { nx, ny, nz },
					.color = { r, g, b },
					.texCoord = { tx,ty }
				};

				if( uniqueVertices[materialId].count(vertex) == 0 )
				{
					uniqueVertices[materialId][vertex] = static_cast<uint32_t>(tempMesh[materialId].vertices.size());
					tempMesh[materialId].vertices.emplace_back(vertex);
				}

				tempMesh[materialId].indices.emplace_back(uniqueVertices[materialId][vertex]);
			}
			index_offset += fv;
		}
	}

	// load materials
	if( isFindMaterials )
	{
		for( size_t i = 0; i < materials.size(); i++ )
		{
			if( materials[i].diffuse_texname.empty() ) continue;

			std::string diffuseMap = pathMaterialFiles + materials[i].diffuse_texname;
			tempMesh[i].material.diffuseTexture = render::CreateTexture2D(diffuseMap.c_str(), true);
		}
	}

	// compute AABB
	{
		for( size_t i = 0; i < tempMesh.size(); i++ )
		{
			tempMesh[0].min = tempMesh[i].vertices[0].position;
			tempMesh[0].max = tempMesh[i].vertices[0].position;
			for( size_t j = 0; j < tempMesh[i].vertices.size(); j++ )
			{
				tempMesh[i].min = glm::min(tempMesh[0].min, tempMesh[i].vertices[j].position);
				tempMesh[i].max = glm::max(tempMesh[0].max, tempMesh[i].vertices[j].position);
			}
		}
	}

	return createMeshBuffer(std::move(tempMesh));
}
//-----------------------------------------------------------------------------
Model scene::CreateModel(const char* fileName, const char* pathMaterialFiles)
{
	if( std::string(fileName).find(".obj") != std::string::npos )
		return loadObjFile(fileName, pathMaterialFiles);

	return {};
}
//-----------------------------------------------------------------------------
Model scene::CreateModel(std::vector<Mesh>&& meshes)
{
	return createMeshBuffer(std::move(meshes));
}
//-----------------------------------------------------------------------------
void scene::Destroy(Model& model)
{
	for( size_t i = 0; i < model.subMeshes.size(); i++ )
	{
		model.subMeshes[i].vertices.clear();
		model.subMeshes[i].indices.clear();

		render::DestroyResource(model.subMeshes[i].vertexBuffer);
		render::DestroyResource(model.subMeshes[i].indexBuffer);
		render::DestroyResource(model.subMeshes[i].vao);
	}
	model.subMeshes.clear();
}
//-----------------------------------------------------------------------------
void scene::Draw(const Model& model)
{
	for( size_t i = 0; i < model.subMeshes.size(); i++ )
	{
		if( render::IsValid(model.subMeshes[i].vao) )
		{
			const Texture2D& diffuseTexture = model.subMeshes[i].material.diffuseTexture;
			if( render::IsValid(diffuseTexture) )
				render::Bind(diffuseTexture, 0);
			render::Draw(model.subMeshes[i].vao, render::PrimitiveDraw::Triangles);
		}
	}
}
//-----------------------------------------------------------------------------
std::vector<glm::vec3> scene::GetVertexInMesh(const Mesh& mesh)
{
	std::vector<glm::vec3> v;
	v.reserve(mesh.indices.size());
	// востановление треугольников по индексному буферу
	for( size_t i = 0; i < mesh.indices.size(); i++ )
		v.push_back(mesh.vertices[mesh.indices[i]].position);
	return v;
}
//-----------------------------------------------------------------------------
std::vector<glm::vec3> scene::GetVertexInModel(const Model& model)
{
	std::vector<glm::vec3> v;
	for( size_t i = 0; i < model.subMeshes.size(); i++ )
	{
		auto subV = GetVertexInMesh(model.subMeshes[i]);
		// https://www.techiedelight.com/ru/concatenate-two-vectors-cpp/
		std::move(subV.begin(), subV.end(), std::back_inserter(v));
	}
	return v;
}
//-----------------------------------------------------------------------------
//=============================================================================
// App System
//=============================================================================
//-----------------------------------------------------------------------------
void errorCallback(int error, const char* description) noexcept
{
	Fatal("GLFW Error (" + std::to_string(error) + "): " + std::string(description));
}
//-----------------------------------------------------------------------------
void windowSizeCallback(GLFWwindow* /*window*/, int width, int height) noexcept
{
	WindowWidth = width;
	WindowHeight = height;
}
//-----------------------------------------------------------------------------
void keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/) noexcept
{
	if( key < 0 ) return;

	if( action == GLFW_RELEASE ) Keyboard.currentKeyState[key] = 0;
	else Keyboard.currentKeyState[key] = 1;

	if( (Keyboard.keyPressedQueueCount < MAX_KEY_PRESSED_QUEUE) && (action == GLFW_PRESS) )
	{
		Keyboard.keyPressedQueue[Keyboard.keyPressedQueueCount] = key;
		Keyboard.keyPressedQueueCount++;
	}
}
//-----------------------------------------------------------------------------
void charCallback(GLFWwindow* /*window*/, unsigned int key) noexcept
{
	if( Keyboard.charPressedQueueCount < MAX_KEY_PRESSED_QUEUE )
	{
		Keyboard.charPressedQueue[Keyboard.charPressedQueueCount] = (int)key;
		Keyboard.charPressedQueueCount++;
	}
}
//-----------------------------------------------------------------------------
void mouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) noexcept
{
	Mouse.currentButtonState[button] = (char)action;
}
//-----------------------------------------------------------------------------
void mouseCursorPosCallback(GLFWwindow* /*window*/, double x, double y) noexcept
{
	Mouse.currentPosition.x = (float)x;
	Mouse.currentPosition.y = (float)y;
}
//-----------------------------------------------------------------------------
void mouseScrollCallback(GLFWwindow* /*window*/, double xoffset, double yoffset) noexcept
{
	Mouse.currentWheelMove = { (float)xoffset, (float)yoffset };
}
//-----------------------------------------------------------------------------
void cursorEnterCallback(GLFWwindow* /*window*/, int enter) noexcept
{
	Mouse.cursorOnScreen = (enter == GLFW_TRUE);
}
//-----------------------------------------------------------------------------
bool app::Create(const CreateInfo& info)
{
	LogCreate("../log.txt");

	glfwSetErrorCallback(errorCallback);

	if( !glfwInit() )
	{
		LogError("GLFW: Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
#if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, info.window.resizable ? GL_TRUE : GL_FALSE);
	Window = glfwCreateWindow(info.window.width, info.window.height, info.window.title, nullptr, nullptr);
	if( !Window )
	{
		LogError("GLFW: Failed to initialize Window");
		return false;
	}

	glfwSetWindowSizeCallback(Window, windowSizeCallback);

	glfwSetKeyCallback(Window, keyCallback);
	glfwSetCharCallback(Window, charCallback);
	glfwSetMouseButtonCallback(Window, mouseButtonCallback);
	glfwSetCursorPosCallback(Window, mouseCursorPosCallback);
	glfwSetScrollCallback(Window, mouseScrollCallback);
	glfwSetCursorEnterCallback(Window, cursorEnterCallback);

	glfwMakeContextCurrent(Window);

#if defined(_WIN32)
	if( !gladLoadGL(glfwGetProcAddress) )
	{
		LogError("GLAD: Cannot load OpenGL extensions");
		return false;
	}
#endif // _WIN32

	glfwSwapInterval(info.window.vsync ? 1 : 0);

	glfwGetFramebufferSize(Window, &WindowWidth, &WindowHeight);

	Mouse.currentPosition.x = (float)WindowWidth / 2.0f;
	Mouse.currentPosition.y = (float)WindowHeight / 2.0f;

	LogPrint("OpenGL: OpenGL device information:");
	LogPrint("    > Vendor:   " + std::string((const char*)glGetString(GL_VENDOR)));
	LogPrint("    > Renderer: " + std::string((const char*)glGetString(GL_RENDERER)));
	LogPrint("    > Version:  " + std::string((const char*)glGetString(GL_VERSION)));
	LogPrint("    > GLSL:     " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));

	return true;
}
//-----------------------------------------------------------------------------
void app::Destroy()
{
	for( auto& it : CacheTextures2D )
		render::DestroyResource(it.second);
	CacheTextures2D.clear();

	glfwDestroyWindow(Window);
	glfwTerminate();
	LogDestroy();
}
//-----------------------------------------------------------------------------
void app::BeginFrame()
{
	CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	const auto currentTime = glfwGetTime();
	TimeDelta = currentTime - LastFrameTime;
	LastFrameTime = currentTime;

	glViewport(0, 0, WindowWidth, WindowHeight);
	glClearColor(0.2f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
//-----------------------------------------------------------------------------
void app::EndFrame()
{
	glfwSwapBuffers(Window);

	Keyboard.keyPressedQueueCount = 0;
	Keyboard.charPressedQueueCount = 0;

	for( int i = 0; i < MAX_KEYBOARD_KEYS; i++ ) Keyboard.previousKeyState[i] = Keyboard.currentKeyState[i];
	for( int i = 0; i < MAX_MOUSE_BUTTONS; i++ ) Mouse.previousButtonState[i] = Mouse.currentButtonState[i];

	Mouse.previousWheelMove = Mouse.currentWheelMove;
	Mouse.currentWheelMove = { 0.0f, 0.0f };
	Mouse.previousPosition = Mouse.currentPosition;

	glfwPollEvents();
}
//-----------------------------------------------------------------------------
bool app::IsClose()
{
	return IsExitRequested || glfwWindowShouldClose(Window) == GLFW_TRUE;
}
//-----------------------------------------------------------------------------
void app::Exit()
{
	IsExitRequested = true;
}
//-----------------------------------------------------------------------------
int app::GetWindowWidth()
{
	return WindowWidth;
}
//-----------------------------------------------------------------------------
int app::GetWindowHeight()
{
	return WindowHeight;
}
//-----------------------------------------------------------------------------
float app::GetDeltaTime()
{
	return (float)TimeDelta;
}
//-----------------------------------------------------------------------------
uint32_t app::GetCurrentFrameId()
{
	return CurrentFrame;
}
//-----------------------------------------------------------------------------
bool app::IsKeyPressed(int key)
{
	if( (Keyboard.previousKeyState[key] == 0) && (Keyboard.currentKeyState[key] == 1) ) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool app::IsKeyDown(int key)
{
	if( Keyboard.currentKeyState[key] == 1 ) return true;
	else return false;
}
//-----------------------------------------------------------------------------
bool app::IsKeyReleased(int key)
{
	if( (Keyboard.previousKeyState[key] == 1) && (Keyboard.currentKeyState[key] == 0) ) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool app::IsKeyUp(int key)
{
	if( Keyboard.currentKeyState[key] == 0 ) return true;
	else return false;
}
//-----------------------------------------------------------------------------
int app::GetKeyPressed()
{
	int value = 0;

	if( Keyboard.keyPressedQueueCount > 0 )
	{
		value = Keyboard.keyPressedQueue[0];

		for( int i = 0; i < (Keyboard.keyPressedQueueCount - 1); i++ )
			Keyboard.keyPressedQueue[i] = Keyboard.keyPressedQueue[i + 1];

		Keyboard.keyPressedQueue[Keyboard.keyPressedQueueCount] = 0;
		Keyboard.keyPressedQueueCount--;
	}

	return value;
}
//-----------------------------------------------------------------------------
int app::GetCharPressed()
{
	int value = 0;

	if( Keyboard.charPressedQueueCount > 0 )
	{
		value = Keyboard.charPressedQueue[0];

		for( int i = 0; i < (Keyboard.charPressedQueueCount - 1); i++ )
			Keyboard.charPressedQueue[i] = Keyboard.charPressedQueue[i + 1];

		Keyboard.charPressedQueue[Keyboard.charPressedQueueCount] = 0;
		Keyboard.charPressedQueueCount--;
	}

	return value;
}
//-----------------------------------------------------------------------------
bool app::IsMouseButtonPressed(int button)
{
	if( (Mouse.currentButtonState[button] == 1) && (Mouse.previousButtonState[button] == 0) ) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool app::IsMouseButtonDown(int button)
{
	if( Mouse.currentButtonState[button] == 1 ) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool app::IsMouseButtonReleased(int button)
{
	if( (Mouse.currentButtonState[button] == 0) && (Mouse.previousButtonState[button] == 1) ) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool app::IsMouseButtonUp(int button)
{
	return !IsMouseButtonDown(button);
}
//-----------------------------------------------------------------------------
glm::vec2 app::GetMousePosition()
{
	return
	{
		Mouse.currentPosition.x,
		Mouse.currentPosition.y
	};
}
//-----------------------------------------------------------------------------
glm::vec2 app::GetMouseDelta()
{
	return
	{
		Mouse.currentPosition.x - Mouse.previousPosition.x,
		Mouse.currentPosition.y - Mouse.previousPosition.y
	};
}
//-----------------------------------------------------------------------------
void app::SetMousePosition(int x, int y)
{
	Mouse.currentPosition = { (float)x, (float)y };
	Mouse.previousPosition = Mouse.currentPosition;
	glfwSetCursorPos(Window, Mouse.currentPosition.x, Mouse.currentPosition.y);
}
//-----------------------------------------------------------------------------
float app::GetMouseWheelMove()
{
	float result = 0.0f;
	if( fabsf(Mouse.currentWheelMove.x) > fabsf(Mouse.currentWheelMove.y) ) result = (float)Mouse.currentWheelMove.x;
	else result = (float)Mouse.currentWheelMove.y;
	return result;
}
//-----------------------------------------------------------------------------
glm::vec2 app::GetMouseWheelMoveV()
{
	return Mouse.currentWheelMove;
}
//-----------------------------------------------------------------------------
void app::SetMouseLock(bool lock)
{
	if (Mouse.cursorHidden == lock) return;

#if defined(__EMSCRIPTEN__)
	if( lock ) emscripten_request_pointerlock("#canvas", 1);
	else emscripten_exit_pointerlock();
#else
	glfwSetInputMode(Window, GLFW_CURSOR, lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
#endif
	SetMousePosition(WindowWidth / 2, WindowHeight / 2);

	Mouse.cursorHidden = lock;
}
//-----------------------------------------------------------------------------
bool app::IsCursorOnScreen()
{
	return Mouse.cursorOnScreen;
}
//-----------------------------------------------------------------------------