#include "stdafx.h"
#include "Engine.h"
//-----------------------------------------------------------------------------
#define GL_CLAMP 0x2900
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
	case render::TextureWrapping::Clamp:          return GL_CLAMP;
	case render::TextureWrapping::ClampToEdge:    return GL_CLAMP_TO_EDGE;
	case render::TextureWrapping::ClampToBorder:  return GL_CLAMP_TO_BORDER;
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
		format = GL_RG;
		internalFormat = GL_RG8;
		oglType = GL_UNSIGNED_BYTE;
		const GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
		glTexParameteriv(textureType, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask); // TODO: могут быть проблемы с браузерами, тогда только грузить stb с указанием нужного формата
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
		Bind(attribs[i]);

	if( resource.ibo ) Bind(*resource.ibo);

	glBindVertexArray(CurrentVAO); // restore VAO

	return resource;
}
//-----------------------------------------------------------------------------
VertexArray render::CreateVertexArray(VertexBuffer* vbo, IndexBuffer* ibo, const ShaderProgram& shaders)
{
	auto attribInfo = GetAttribInfo(shaders);
	if( attribInfo.empty() ) return {};

	size_t offset = 0;
	std::vector<VertexAttribute> attribs(attribInfo.size());
	for( size_t i = 0; i < attribInfo.size(); i++ )
	{
		attribs[i].location = attribInfo[i].location;
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
		attribs[i].stride = (unsigned int)offset;
	}

	return CreateVertexArray(vbo, ibo, attribs);
}
//-----------------------------------------------------------------------------
Texture2D render::CreateTexture2D(const char* fileName, const Texture2DInfo& textureInfo)
{
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

	const size_t imageDataSize = (size_t)width * height * nrChannels;

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

	Texture2D ret = CreateTexture2D(createInfo, textureInfo);
	stbi_image_free((void*)pixelData);
	return ret;
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
	getTextureFormatType(createInfo.format, GL_TEXTURE_2D, format, internalFormat, oglType);

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
	if( CurrentShaderProgram == resource.id ) return;
	CurrentShaderProgram = resource.id;
	glUseProgram(resource.id);
}
//-----------------------------------------------------------------------------
void render::Bind(const VertexBuffer& resource)
{
	if( CurrentVBO == resource.id ) return;
	CurrentVBO = resource.id;
	glBindBuffer(GL_ARRAY_BUFFER, resource.id);
}
//-----------------------------------------------------------------------------
void render::Bind(const IndexBuffer& resource)
{
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
	if( CurrentTexture2D[slot] == resource.id ) return;
	CurrentTexture2D[slot] = resource.id;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, resource.id);
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
void render::Draw(const VertexArray& vao, PrimitiveDraw primitive)
{
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