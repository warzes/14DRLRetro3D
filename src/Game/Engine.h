#pragma once

template<size_t N, typename T>
class Foo;

template<typename T>
class Foo<2, T>
{
	T& operator[](size_t index);

	T t[2];
};

template<> inline float& Foo<2, float>::operator[](size_t index){return t[0];}
template<> inline double& Foo<2, double>::operator[](size_t index) { return t[0]; }


//=============================================================================
// Engine config
//=============================================================================
constexpr int MaxBindingTextures = 16;
constexpr auto MAX_KEYBOARD_KEYS = 512;
constexpr auto MAX_KEY_PRESSED_QUEUE = 16;
constexpr auto MAX_CHAR_PRESSED_QUEUE = 16;
constexpr auto MAX_MOUSE_BUTTONS = 8;
constexpr auto MAX_FRAMES_IN_FLIGHT = 100;

//=============================================================================
// Core
//=============================================================================

// Implementation from "08/02/2015 Better array 'countof' implementation with C++ 11 (updated)" - https://www.g-truc.net/post-0708.html
template<typename T, size_t N>
[[nodiscard]] constexpr unsigned int Countof(T const (&)[N])
{
	return static_cast<unsigned int>(N);
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
	enum class ResourceType : uint8_t
	{
		Unknown,
		ShaderProgram,
		VertexBuffer,
		IndexBuffer,
		VertexArray,
		Texture2D
	};

	enum class ResourceUsage : uint8_t
	{
		Static,
		Dynamic,
		Stream,
	};

	enum class PrimitiveDraw : uint8_t
	{
		Lines,
		Triangles,
		Points,
	};

	enum class TextureMinFilter : uint8_t
	{
		Nearest,
		Linear,
		NearestMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapNearest,
		LinearMipmapLinear,
	};

	enum class TextureMagFilter : uint8_t
	{
		Nearest,
		Linear,
	};

	enum class TextureWrapping : uint8_t
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
#if !defined(__EMSCRIPTEN__) // not support in webGL
		ClampToBorder
#endif
	};

	enum class TexelsFormat : uint8_t
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

	enum class BlendFactor : uint8_t
	{
		Zero,
		One
		//...
	};

	enum class BlendOp : uint8_t
	{
		Add = 1,
		Subrtact = 2,
		//...
	};

	enum class ColorMask : uint8_t
	{
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8,
		All = 0xF
	};
	inline ColorMask operator|(ColorMask a, ColorMask b) { return ColorMask(uint32_t(a) | uint32_t(b)); }
	inline ColorMask operator&(ColorMask a, ColorMask b) { return ColorMask(uint32_t(a) & uint32_t(b)); }
	inline ColorMask operator~(ColorMask a) { return ColorMask(~uint32_t(a)); }
	inline bool operator!(ColorMask a) { return uint32_t(a) == 0; }
	inline bool operator==(ColorMask a, uint32_t b) { return uint32_t(a) == b; }
	inline bool operator!=(ColorMask a, uint32_t b) { return uint32_t(a) != b; }

	enum class RasterFillMode : uint8_t
	{
		Solid,
		Wireframe,
	};

	enum class RasterCullMode : uint8_t
	{
		Back,
		Front,
		Full
	};

	enum class StencilOp : uint8_t
	{
		Keep,
		Zero,
		Replace,
		//...
	};

	enum class ComparisonFunc : uint8_t
	{
		Never,
		Less = 2,
		Equal = 3,
		LessOrEqual = 4,
		Greater = 5,
		NotEqual = 6,
		GreaterOrEqual = 7,
		Always = 8
	};

	struct VertexAttribute
	{
		unsigned location/* = -1*/;  // если -1, то берется индекс массива атрибутов
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

	struct BlendState { /*...*/ };
	struct RasterState { /*...*/ };

	bool operator==(const ShaderProgram& Left, const ShaderProgram& Right) noexcept;
	bool operator==(const Uniform& Left, const Uniform& Right) noexcept;
	bool operator==(const VertexBuffer& Left, const VertexBuffer& Right) noexcept;
	bool operator==(const IndexBuffer& Left, const IndexBuffer& Right) noexcept;
	bool operator==(const VertexArray& Left, const VertexArray& Right) noexcept;
	bool operator==(const Texture2D& Left, const Texture2D& Right) noexcept;

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
	Texture2D CreateTexture2D(const char* fileName, bool useCache = true, const Texture2DInfo& textureInfo = {});
	Texture2D CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo = {});

	void DestroyResource(ShaderProgram& resource);
	void DestroyResource(VertexBuffer& resource);
	void DestroyResource(IndexBuffer& resource);
	void DestroyResource(VertexArray& resource);
	void DestroyResource(Texture2D& resource);

	std::vector<ShaderAttribInfo> GetAttribInfo(const ShaderProgram& resource);
	Uniform GetUniform(const ShaderProgram& program, const char* uniformName);

	void SetUniform(const Uniform& uniform, int value);
	void SetUniform(const Uniform& uniform, float value);
	void SetUniform(const Uniform& uniform, const glm::vec2& value);
	void SetUniform(const Uniform& uniform, const glm::vec3& value);
	void SetUniform(const Uniform& uniform, const glm::vec4& value);
	void SetUniform(const Uniform& uniform, const glm::mat3& value);
	void SetUniform(const Uniform& uniform, const glm::mat4& value);

	void UpdateVertexBuffer(VertexBuffer& vbo, unsigned offset, unsigned vertexCount, unsigned vertexSize, const void* data);
	void UpdateIndexBuffer(IndexBuffer& ibo, unsigned offset, unsigned indexCount, unsigned indexSize, const void* data);

	void ResetState(ResourceType type);
	void ResetState(ResourceType type, unsigned slot);
	void Bind(const ShaderProgram& resource);
	void Bind(const VertexBuffer& resource);
	void Bind(const IndexBuffer& resource);
	void Bind(const VertexAttribute& Attribute);
	void Bind(const Texture2D& resource, unsigned slot = 0);

	void Draw(const VertexArray& vao, PrimitiveDraw primitive = PrimitiveDraw::Triangles);
} // namespace render

using render::ShaderProgram;
using render::Uniform;
using render::VertexBuffer;
using render::IndexBuffer;
using render::VertexArray;
using render::Texture2D;

//=============================================================================
// Scene System
//=============================================================================
namespace scene
{
	struct Camera
	{
		glm::vec3 position;
		glm::vec3 viewPoint;
		glm::vec3 upVector;
	};

	glm::mat4 GetCameraViewMatrix(const Camera& camera);
	void CameraMoveBy(Camera& camera, float distance); // перемещение вперед/назад
	void CameraStrafeBy(Camera& camera, float distance); // перемещение влево/вправо
	void CameraRotateLeftRight(Camera& camera, float angleInDegrees); // поворот влево/вправо
	void CameraRotateUpDown(Camera& camera, float angleInDegrees); // поворот вверх/вниз

	struct GeometryBuffer
	{
		GeometryBuffer() = default;
		GeometryBuffer(GeometryBuffer&& ref);
		GeometryBuffer(const GeometryBuffer& ref);
		GeometryBuffer& operator=(GeometryBuffer&& ref);
		GeometryBuffer& operator=(const GeometryBuffer& ref);

		VertexBuffer vb;
		IndexBuffer ib;
		VertexArray vao;
	};

	GeometryBuffer CreateGeometryBuffer(
		render::ResourceUsage usage,
		/*vertex*/unsigned vertexCount, unsigned vertexSize, const void* vertexData,
		/*index*/unsigned indexCount, unsigned indexSize, const void* indexData,
		const ShaderProgram& shaders
	);
	GeometryBuffer CreateGeometryBuffer(render::ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, const ShaderProgram& shaders);
	void Destroy(GeometryBuffer& buffer);

	bool IsValid(const GeometryBuffer& buffer);


	class Material
	{
	public:
		Texture2D diffuseTexture;

		glm::vec3 ambientColor = glm::vec3{ 1.0f };
		glm::vec3 diffuseColor = glm::vec3{ 1.0f };
		glm::vec3 specularColor = glm::vec3{ 1.0f };
		float shininess = 1.0f;
	};

	struct VertexMesh
	{
		bool operator==(const VertexMesh& v) const { return position == v.position && normal == v.normal && color == v.color && texCoord == v.texCoord; }

		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 texCoord;
	};

	struct Mesh
	{
		std::vector<VertexMesh> vertices;
		std::vector<uint32_t> indices;
		Material material;

		VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;
		VertexArray vao;

		// bouncing box
		glm::vec3 min;
		glm::vec3 max;
	};

	struct Model
	{
		std::vector<Mesh> subMeshes;
		// bouncing box
		glm::vec3 min;
		glm::vec3 max;
	};

	Model CreateModel(const char* fileName, const char* pathMaterialFiles = "./");
	Model CreateModel(std::vector<Mesh>&& meshes);
	void Destroy(Model& model);
	void Draw(const Model& model);
	std::vector<glm::vec3> GetVertexInMesh(const Mesh& mesh);
	std::vector<glm::vec3> GetVertexInModel(const Model& model);
}

using scene::Camera;
using scene::GeometryBuffer;
using scene::Material;
using scene::VertexMesh;
using scene::Mesh;
using scene::Model;



//=============================================================================
// App System
//=============================================================================
namespace app
{
	enum KeyboardKey
	{
		KEY_NULL = 0,            // Key: NULL, used for no key pressed
		// Alphanumeric keys
		KEY_APOSTROPHE = 39,     // Key: '
		KEY_COMMA = 44,          // Key: ,
		KEY_MINUS = 45,          // Key: -
		KEY_PERIOD = 46,         // Key: .
		KEY_SLASH = 47,          // Key: /
		KEY_ZERO = 48,           // Key: 0
		KEY_ONE = 49,            // Key: 1
		KEY_TWO = 50,            // Key: 2
		KEY_THREE = 51,          // Key: 3
		KEY_FOUR = 52,           // Key: 4
		KEY_FIVE = 53,           // Key: 5
		KEY_SIX = 54,            // Key: 6
		KEY_SEVEN = 55,          // Key: 7
		KEY_EIGHT = 56,          // Key: 8
		KEY_NINE = 57,           // Key: 9
		KEY_SEMICOLON = 59,      // Key: ;
		KEY_EQUAL = 61,          // Key: =

		KEY_A = 65,              // Key: A | a
		KEY_B = 66,              // Key: B | b
		KEY_C = 67,              // Key: C | c
		KEY_D = 68,              // Key: D | d
		KEY_E = 69,              // Key: E | e
		KEY_F = 70,              // Key: F | f
		KEY_G = 71,              // Key: G | g
		KEY_H = 72,              // Key: H | h
		KEY_I = 73,              // Key: I | i
		KEY_J = 74,              // Key: J | j
		KEY_K = 75,              // Key: K | k
		KEY_L = 76,              // Key: L | l
		KEY_M = 77,              // Key: M | m
		KEY_N = 78,              // Key: N | n
		KEY_O = 79,              // Key: O | o
		KEY_P = 80,              // Key: P | p
		KEY_Q = 81,              // Key: Q | q
		KEY_R = 82,              // Key: R | r
		KEY_S = 83,              // Key: S | s
		KEY_T = 84,              // Key: T | t
		KEY_U = 85,              // Key: U | u
		KEY_V = 86,              // Key: V | v
		KEY_W = 87,              // Key: W | w
		KEY_X = 88,              // Key: X | x
		KEY_Y = 89,              // Key: Y | y
		KEY_Z = 90,              // Key: Z | z
		KEY_LEFT_BRACKET = 91,   // Key: [
		KEY_BACKSLASH = 92,      // Key: '\'
		KEY_RIGHT_BRACKET = 93,  // Key: ]
		KEY_GRAVE = 96,          // Key: `
		// Function keys
		KEY_SPACE = 32,          // Key: Space
		KEY_ESCAPE = 256,        // Key: Esc
		KEY_ENTER = 257,         // Key: Enter
		KEY_TAB = 258,           // Key: Tab
		KEY_BACKSPACE = 259,     // Key: Backspace
		KEY_INSERT = 260,        // Key: Ins
		KEY_DELETE = 261,        // Key: Del
		KEY_RIGHT = 262,         // Key: Cursor right
		KEY_LEFT = 263,          // Key: Cursor left
		KEY_DOWN = 264,          // Key: Cursor down
		KEY_UP = 265,            // Key: Cursor up
		KEY_PAGE_UP = 266,       // Key: Page up
		KEY_PAGE_DOWN = 267,     // Key: Page down
		KEY_HOME = 268,          // Key: Home
		KEY_END = 269,           // Key: End
		KEY_CAPS_LOCK = 280,     // Key: Caps lock
		KEY_SCROLL_LOCK = 281,   // Key: Scroll down
		KEY_NUM_LOCK = 282,      // Key: Num lock
		KEY_PRINT_SCREEN = 283,  // Key: Print screen
		KEY_PAUSE = 284,         // Key: Pause
		KEY_F1 = 290,            // Key: F1
		KEY_F2 = 291,            // Key: F2
		KEY_F3 = 292,            // Key: F3
		KEY_F4 = 293,            // Key: F4
		KEY_F5 = 294,            // Key: F5
		KEY_F6 = 295,            // Key: F6
		KEY_F7 = 296,            // Key: F7
		KEY_F8 = 297,            // Key: F8
		KEY_F9 = 298,            // Key: F9
		KEY_F10 = 299,           // Key: F10
		KEY_F11 = 300,           // Key: F11
		KEY_F12 = 301,           // Key: F12
		KEY_LEFT_SHIFT = 340,    // Key: Shift left
		KEY_LEFT_CONTROL = 341,  // Key: Control left
		KEY_LEFT_ALT = 342,      // Key: Alt left
		KEY_LEFT_SUPER = 343,    // Key: Super left
		KEY_RIGHT_SHIFT = 344,   // Key: Shift right
		KEY_RIGHT_CONTROL = 345, // Key: Control right
		KEY_RIGHT_ALT = 346,     // Key: Alt right
		KEY_RIGHT_SUPER = 347,   // Key: Super right
		KEY_KB_MENU = 348,       // Key: KB menu
		// Keypad keys
		KEY_KP_0 = 320,          // Key: Keypad 0
		KEY_KP_1 = 321,          // Key: Keypad 1
		KEY_KP_2 = 322,          // Key: Keypad 2
		KEY_KP_3 = 323,          // Key: Keypad 3
		KEY_KP_4 = 324,          // Key: Keypad 4
		KEY_KP_5 = 325,          // Key: Keypad 5
		KEY_KP_6 = 326,          // Key: Keypad 6
		KEY_KP_7 = 327,          // Key: Keypad 7
		KEY_KP_8 = 328,          // Key: Keypad 8
		KEY_KP_9 = 329,          // Key: Keypad 9
		KEY_KP_DECIMAL = 330,    // Key: Keypad .
		KEY_KP_DIVIDE = 331,     // Key: Keypad /
		KEY_KP_MULTIPLY = 332,   // Key: Keypad *
		KEY_KP_SUBTRACT = 333,   // Key: Keypad -
		KEY_KP_ADD = 334,        // Key: Keypad +
		KEY_KP_ENTER = 335,      // Key: Keypad Enter
		KEY_KP_EQUAL = 336,      // Key: Keypad =
		// Android key buttons
		KEY_BACK = 4,            // Key: Android back button
		KEY_MENU = 82,           // Key: Android menu button
		KEY_VOLUME_UP = 24,      // Key: Android volume up button
		KEY_VOLUME_DOWN = 25     // Key: Android volume down button
	};

	struct CreateInfo
	{
		struct
		{
			int width = 1024;
			int height = 768;
			const char* title = "game";
			bool resizable = true;
			bool vsync = true;
		} window;
	};

	bool Create(const CreateInfo& info);
	void Destroy();

	void BeginFrame();
	void EndFrame();
	bool IsClose();

	void Exit();

	int GetWindowWidth();
	int GetWindowHeight();

	float GetDeltaTime();
	uint32_t GetCurrentFrameId();

	bool IsKeyPressed(int key);
	bool IsKeyDown(int key);
	bool IsKeyReleased(int key);
	bool IsKeyUp(int key);
	int GetKeyPressed();
	int GetCharPressed();

	bool IsMouseButtonPressed(int button);
	bool IsMouseButtonDown(int button);
	bool IsMouseButtonReleased(int button);
	bool IsMouseButtonUp(int button);
	glm::vec2 GetMousePosition();
	glm::vec2 GetMouseDelta();

	void SetMousePosition(int x, int y);
	float GetMouseWheelMove();
	glm::vec2 GetMouseWheelMoveV();
	void SetMouseLock(bool lock);
	bool IsCursorOnScreen();
}