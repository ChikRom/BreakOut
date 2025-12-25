#include <GLAD/glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "game.h"
#include "resource_manager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height); // callback function for changing the window 

void processInput(GLFWwindow* window); // function for exit from the window with "escape" on keyboard

//void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 colour); // rendering a text

//void loadCharsAndTextures(FT_Face& face);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// debug function
void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}

// Width of the screen
const unsigned int SCREEN_WIDTH = 2400;
// Height of the screen
const unsigned int SCREEN_HEIGHT = 1200;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main()

{
	//initialize the GLFW library
	glfwInit();

	//set the version and add core features for GFLW window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_DEBUG, true);

	// creating a window
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BreakOut", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// initialize a GLAD library
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Enable blending for characters
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//initialize game
	Breakout.Init();

	// deltaTime variables
	// ------------------
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	//// set up a shader
	//Shader shader("shaders/text.vert", "shaders/text.frag");
	//glm::mat4 projection = glm::ortho(0.0f, 2400.0f, 0.0f, 1200.0f);
	//shader.Use();
	//shader.setMat4("projection", projection);

	// FreeType
	//FT_Library ft;
	//// All functions return a value different than 0 whenever an error occured
	//if (FT_Init_FreeType(&ft))
	//{
	//	std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	//	return -1;
	//}

	//// load font as face
	//FT_Face face;
	//if (FT_New_Face(ft, std::string("resources/fonts/times.ttf").c_str(), 0, &face))
	//{
	//	std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	//	return -1;
	//}
	//else
	//{
	//	loadCharsAndTextures(face);
	//}
	//FT_Done_Face(face);
	//FT_Done_FreeType(ft);


	// enable OpenGL debug context if context allows for debug context
	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	while (!glfwWindowShouldClose(window))
	{

		//processInput(window);

		// calculate delta time
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

		// manage user input
		// -----------------
		Breakout.ProcessInput(deltaTime);

		// update game state
		// -----------------
		Breakout.Update(deltaTime);

		// render
		// ------

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		Breakout.Render();
		

		glfwSwapBuffers(window);
	}
	ResourceManager::Clear();
	glfwTerminate();
	return 0;
	
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// when a user presses the escape key, we set the WindowShouldClose property to true
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			Breakout.Keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			Breakout.Keys[key] = false;
			Breakout.KeysProcessed[key] = false;
		}
			
	}
}


//void loadCharsAndTextures(FT_Face& face)
//{
//	FT_Set_Pixel_Sizes(face, 0, 48);
//
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//	for (unsigned char c = 0; c < 255; c++)
//	{
//		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
//		{
//			std::cout << "ERROR:FREETYPE: Failed to load Glyph" << std::endl;
//			continue;
//		}
//
//		// generate texture
//		unsigned int texture;
//		glGenTextures(1, &texture);
//		glBindTexture(GL_TEXTURE_2D, texture);
//		glTexImage2D(
//			GL_TEXTURE_2D,
//			0,
//			GL_RED,
//			face->glyph->bitmap.width,
//			face->glyph->bitmap.rows,
//			0,
//			GL_RED,
//			GL_UNSIGNED_BYTE,
//			face->glyph->bitmap.buffer);
//		// set texture options
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		// now store character for later use
//		Character character{
//			texture,
//			glm::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
//			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
//			static_cast<unsigned int> (face->glyph->advance.x)
//		};
//		Characters.insert(std::pair<char, Character>(c, character));
//	}
//	glBindTexture(GL_TEXTURE_2D, 0);
//}