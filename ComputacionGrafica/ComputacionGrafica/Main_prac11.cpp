#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 2.0f, 0.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;


// Light attributes
glm::vec3 lightPos(100.0f, 0.0f, 0.0f);
glm::vec3 PosIni(-95.0f, 1.0f, -45.0f);
bool active;

//Animacion pikachu
float movKitX = 0.0;
float movKitY = 0.0;
float rotKit = 0.0;
bool circuito = true;
bool recorrido1 = true;
bool recorrido2 = false;
bool recorrido3 = false;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX = PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotSilla, rotPikachu;
float movSilla, movMueble, movPikachu;
float rotCompu;

#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ

	float rotSilla;
	float inc1;
	float movSilla;
	float inc2;

	float rotPikachu;
	float inc3;
	float movPikachu;
	float inc4;



}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
bool play2 = false;
bool play3 = false;
bool play3_1 = false;
bool play4 = false;
bool play4_1 = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(5,2.3,-11),
	glm::vec3(100,0,0),
	glm::vec3(100,0,0),
	glm::vec3(100,0,0)
};

glm::vec3 LightP1;




void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotSilla = rotSilla;
	KeyFrame[FrameIndex].movSilla = movSilla;

	KeyFrame[FrameIndex].rotPikachu = rotPikachu;
	KeyFrame[FrameIndex].movPikachu = movPikachu;
	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;


	rotSilla = KeyFrame[0].rotSilla;
	movSilla = KeyFrame[0].movSilla;

	rotPikachu = KeyFrame[0].rotPikachu;
	movPikachu = KeyFrame[0].movPikachu;
}

void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	KeyFrame[playIndex].inc1 = (KeyFrame[playIndex + 1].rotSilla - KeyFrame[playIndex].rotSilla) / i_max_steps;
	KeyFrame[playIndex].inc2 = (KeyFrame[playIndex + 1].movSilla - KeyFrame[playIndex].movSilla) / i_max_steps;
}
void interpolation2(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	KeyFrame[playIndex].inc3 = (KeyFrame[playIndex + 1].rotPikachu - KeyFrame[playIndex].rotPikachu) / i_max_steps;
	KeyFrame[playIndex].inc4 = (KeyFrame[playIndex + 1].movPikachu - KeyFrame[playIndex].movPikachu) / i_max_steps;
}



int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Final", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");


	//Modelos de objetos
	Model Computadora1((char*)"Models/Proyecto/Computadora.obj");
	Model Computadora2((char*)"Models/Proyecto/Computadora1.obj");
	Model Libro((char*)"Models/Proyecto/Libro.obj");
	//Model Mueble1((char*)"Models/Proyecto/MuebleTv.obj");
	//Model Mueble2((char*)"Models/Proyecto/muebleTV1.obj");
	//Model Mueble3((char*)"Models/Proyecto/MuebleTV2.obj");
	Model Pikachu((char*)"Models/Proyecto/Pikachu.obj");
	Model Reloj1((char*)"Models/Proyecto/Reloj.obj");
	Model Reloj2((char*)"Models/Proyecto/Reloj1.obj");
	Model Reloj3((char*)"Models/Proyecto/Reloj2.obj");
	Model Silla1((char*)"Models/Proyecto/Silla.obj");
	Model Silla2((char*)"Models/Proyecto/Silla1.obj");
	Model Switch((char*)"Models/Proyecto/Switch.obj");
	Model Television((char*)"Models/Proyecto/Television.obj");

	//Modelo del resto del cuarto
	Model Cuarto((char*)"Models/Cuarto/Cuarto.obj");
	//Modelo de fachada
	Model Fachada((char*)"Models/Proyecto/Fachada.obj");

	// Build and compile our shader program

	//Inicialización de KeyFrames

	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		

		KeyFrame[i].rotSilla = 0;
		KeyFrame[i].inc1 = 0;
		KeyFrame[i].movSilla = 0;
		KeyFrame[i].inc2 = 0;

		KeyFrame[i].rotPikachu = 0;
		KeyFrame[i].inc3 = 0;


	}

	KeyFrame[0].rotSilla = 0;
	KeyFrame[1].rotSilla = 360;
	KeyFrame[0].movSilla = 0;
	KeyFrame[1].movSilla = 3.5;

	KeyFrame[0].rotPikachu = 0;
	KeyFrame[1].rotPikachu = 360;
	//KeyFrame[0].movPikachu = 0;
	//KeyFrame[1].movPikachu = 2;

	




	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/back.tga");
	faces.push_back("SkyBox/front.tga");

	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);
	// ----
	GLuint texture3, texture4;
	glGenTextures(1, &texture3);
	glGenTextures(1, &texture4);

	int textureWidth2, textureHeight2, nrChannels2;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image2;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	// Diffuse map
	image2 = stbi_load("Models/Proyecto/poke.jpg", &textureWidth2, &textureHeight2, &nrChannels2, 0);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth2, textureHeight2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glGenerateMipmap(GL_TEXTURE_2D);
	if (image2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth2, textureHeight2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image2);

	// Specular map
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	image2 = stbi_load("Models/Proyecto/muebleTv.jpg", &textureWidth2, &textureHeight2, &nrChannels2, 0);
	glBindTexture(GL_TEXTURE_2D, texture4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth2, textureHeight2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glGenerateMipmap(GL_TEXTURE_2D);
	if (image2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth2, textureHeight2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image2);

	glBindTexture(GL_TEXTURE_2D, 0);

	//Set textures units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "text3"), 2);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "text4"), 3);
	// ----
	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);


		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), LightP1.x, LightP1.y, LightP1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), LightP1.x, LightP1.y, LightP1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 0.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 0.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();


		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//Bind diffuse map
		glBindTexture(GL_TEXTURE_2D, texture3);

		//Bind specular map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture4);


		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); //Temp
		glm::mat4 model(1);
		
		//Mueble TV
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-10.48f, 0.5f, -1.37f));
		model = glm::scale(model, glm::vec3(2.0f, 1.5f, 7.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//Cajon de mueble
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-9.8f, 0.5f, -3.0f));
		model = glm::scale(model, glm::vec3(1.0, 0.8f, 3.0f));
		model = glm::translate(model, glm::vec3(movMueble, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//Cajon de mueble
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-9.8f, 0.5f, 0.5f));
		model = glm::scale(model, glm::vec3(1.0, 0.8f, 3.0f));
		model = glm::translate(model, glm::vec3(movMueble, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);



		//Carga de modelo 
		//Television
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(-10.54f, 1.0f, -1.30f));
		model = glm::scale(model, glm::vec3(1.0f, 1.83f, 1.57f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Television.Draw(lightingShader);
		////Mueble TV (1)
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//tmp = model = glm::translate(model, glm::vec3(-10.48f, 0.0f, -1.37f));
		//model = glm::scale(model, glm::vec3(1.2f, 1.5f, 2.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Mueble1.Draw(lightingShader);
		////Mueble (2)
		//view = camera.GetViewMatrix();
		//model = glm::translate(model, glm::vec3(movMueble, 0.0f, 0.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Mueble2.Draw(lightingShader);
		////Mueble (3)
		//view = camera.GetViewMatrix();
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Mueble3.Draw(lightingShader);
		//Libro
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 8.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Libro.Draw(lightingShader);
		//Computadora (1)
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(-0.0f, 1.72f, 10.0f));
		model = glm::rotate(model, glm::radians(155.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Computadora1.Draw(lightingShader);
		//Computadora (2)
		view = camera.GetViewMatrix();
		model = glm::translate(model, glm::vec3(-0.0f, 0.0f, -0.3f));
		model = glm::rotate(model, glm::radians(rotCompu), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Computadora2.Draw(lightingShader);
		//Pikachu
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(movKitX, movKitY, 0));
		model = glm::translate(model, glm::vec3(-10.716f, 3.0f, 8.7f));
		model = glm::rotate(model, glm::radians(rotPikachu), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0, 0, -movPikachu));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pikachu.Draw(lightingShader);
		//Switch
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(-9.71f, 2.3f,5.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Switch.Draw(lightingShader);
		//Silla (1)
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(-0.322f, 0.0f, 8.55f));
		model = glm::translate(model, glm::vec3(movSilla, 0, 0));
		model = glm::rotate(model, glm::radians(-rotSilla), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Silla1.Draw(lightingShader);
		//Silla (2)
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(-0.322f, 0.0f, 8.55f));
		model = glm::translate(model, glm::vec3(movSilla, 0, 0));
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Silla2.Draw(lightingShader);
		//Reloj (1)
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(-11.8f, 6.4f, 3.47f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Reloj1.Draw(lightingShader);
		//Reloj (2)
		view = camera.GetViewMatrix();
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::rotate(model, -(float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Reloj3.Draw(lightingShader);
		//Reloj (3)
		view = camera.GetViewMatrix();
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		model = glm::rotate(model, -(float)glfwGetTime() / 50, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Reloj2.Draw(lightingShader);


		//Resto del cuarto
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		//tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cuarto.Draw(lightingShader);
		//Fachada
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Fachada.Draw(lightingShader);

		

		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.25f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);


		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default




		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();




	return 0;
}


void animacion()
{

	//Movimiento del personaje

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += KeyFrame[playIndex].incX;
			posY += KeyFrame[playIndex].incY;
			posZ += KeyFrame[playIndex].incZ;

			rotSilla += KeyFrame[playIndex].inc1;
			movSilla += KeyFrame[playIndex].inc2;
			i_curr_steps++;
		}

	}
	else if (play2)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation2();
			}
		}
		else
		{
			//Draw animation
			posX += KeyFrame[playIndex].incX;
			posY += KeyFrame[playIndex].incY;
			posZ += KeyFrame[playIndex].incZ;
			rotPikachu += KeyFrame[playIndex].inc3;
			movPikachu += KeyFrame[playIndex].inc4;
			i_curr_steps++;
		}

	}
	else if (play3) {
		if (movMueble > 0.5) {
			play3 = false;
		}
		else {
			movMueble += 0.001f;
		}
	}
	else if (play3_1) {
		if (movMueble < 0.0) {
			play3_1 = false;
		}
		else {
			movMueble -= 0.001f;
		}
	}
	else if (play4) {
		if (rotCompu > 45.0) {
			play4 = false;
		}
		else {
			rotCompu += 0.45f;
		}
	}
	else if (play4_1) {
		if (rotCompu < 0.0) {
			play4_1 = false;
		}
		else {
			rotCompu -= 0.45f;
		}
	}
	if (circuito)
	{
		if (recorrido1)
		{
			movKitY += 0.03f;
			if (movKitY > 2.5)
			{
				recorrido1 = false;
				recorrido2 = true;
			}
		}
		if (recorrido2)
		{
			rotKit = 0;
			movKitY -= 0.015f;
			if (movKitY < 0)
			{
				recorrido2 = false;
				recorrido3 = true;

			}
		}
		if (recorrido3)
		{
			rotKit = 0;
			movKitY += 0.03f;
			if (movKitY > 2.5)
			{
				recorrido3 = false;
				recorrido1 = true;
			}
		}
	}
}




// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_L])
	{
		if (play == false)
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	if (keys[GLFW_KEY_K])
	{
		if (play2 == false)
		{

			resetElements();
			//First Interpolation				
			interpolation2();

			play2 = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play2 = false;
		}

	}
	


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
			LightP1 = glm::vec3(2.0f, 2.0f, 2.0f);
		else
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	
	if (keys[GLFW_KEY_I])
	{
		circuito = true;
	}

	if (keys[GLFW_KEY_O])
	{
		movKitY = 0;
		circuito = false;
	}
	if (keys[GLFW_KEY_M])
	{
		play3 = !play3;

	}
	if (keys[GLFW_KEY_N])
	{
		play3_1 = !play3_1;

	}
	if (keys[GLFW_KEY_C])
	{
		play4 = !play4;

	}
	if (keys[GLFW_KEY_V])
	{
		play4_1 = !play4_1;

	}



	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}






}