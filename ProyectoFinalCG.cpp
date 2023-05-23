/*---------------------------------------------------------*/
/* ----------------  Proyecto Final             -----------*/
/*-----------------    2023-2   ---------------------------*/
/*------ Alumno: Cruz Cedillo Daniel Alejandro-------------*/
/*------------- No. Cuenta: 316083298---------------*/
#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor *monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 90.0f));
float MovementSpeed = 0.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f); //Direccion de la fuente de luz
glm::vec3 myposition02(80.0f, 4.0f, 0.0f);
glm::vec3 myColor01(0.0f, 0.0f, 1.0f);

//Variable global
float myVariable = 0.0f;


// posiciones
float	movAuto_x = 0.0f,
		movAuto_y =-1.0f,
		movAuto_z = 0.0f,
		orienta = 0.0f;
bool	animacion = false;

//Luz ciclo de dia y noche
float luzx = 0.8f, luzy = 0.8f, luzz = 0.8f, noche = 0.0f;
int dia = 0;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		giroMonito = 0.0f,
		movBrazoDer =0.0f,
		movBrzoIzq=0.0f,
		rotRodDer =0.0f,
		rotcabez=0.0f;
//calculo del incremento en la posicion
float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotInc = 0.0f,
		giroMonitoInc = 0.0f,
		incBrzoDer=0.0f,
		incBrazoIzq=0.0f,
		incrotRodDer=0.0f,
		incrotCabeza=0.0f;

//carro avanza
int avanza = 0;

#define MAX_FRAMES 20  //cantidad de cuadros clave que se pueden guardar
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;
	float movBrazoDer;
	float movBrzoIzq;
	float rotRodDer;
	float rotcabez;

}FRAME; //Estructura Frame

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;//4;			//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);

	//Guarda como esta el personaje dentro de la estrucutura
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	KeyFrame[FrameIndex].movBrazoDer = movBrazoDer;
	KeyFrame[FrameIndex].movBrzoIzq = movBrzoIzq;
	KeyFrame[FrameIndex].rotRodDer = rotRodDer;
	KeyFrame[FrameIndex].rotcabez = rotcabez;


	FrameIndex++;
}

//Punto de incio para reproducir
void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
	movBrazoDer = KeyFrame[0].movBrazoDer;
	movBrzoIzq = KeyFrame[0].movBrzoIzq;
	rotRodDer = KeyFrame[0].rotRodDer;
	rotcabez = KeyFrame[0].rotcabez;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;
	incBrzoDer = (KeyFrame[playIndex + 1].movBrazoDer - KeyFrame[playIndex].movBrazoDer) / i_max_steps; //i_max_steps cantidad maxima de cuadros
	incBrazoIzq = (KeyFrame[playIndex + 1].movBrzoIzq - KeyFrame[playIndex].movBrzoIzq) / i_max_steps;
	incrotRodDer = (KeyFrame[playIndex + 1].rotRodDer - KeyFrame[playIndex].rotRodDer) / i_max_steps;
	incrotCabeza = (KeyFrame[playIndex + 1].rotcabez - KeyFrame[playIndex].rotcabez) / i_max_steps;
}



void animate(void)
{
	switch (dia) {
		case 0:
			luzx += 0.0005f;
			luzy += 0.0005;
			luzz += 0.0005;
			if (luzx >= 0.8f)
				dia = 1;
			break;
		case 1:
			luzx -= 0.0005f;
			luzy -= 0.0005;
			luzz -= 0.0005;
			if (luzx <= 0.1f)
				dia = 2;
			break;
		case 2:
			noche += 0.001;
			if (noche >= 1.0f) {
				dia = 0;
				noche = 0.0f;
			}
		
	}
	

	
	myposition02.x = 200.0f * cos(myVariable);
	myposition02.z = 200.0f * sin(myVariable);
	myVariable += 0.07f;
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
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
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotInc;
			giroMonito += giroMonitoInc;

			movBrazoDer += incBrzoDer;
			movBrzoIzq += incBrazoIzq;
			rotRodDer += incrotRodDer;
			rotcabez += incrotCabeza;

			i_curr_steps++;
		}
	}

	//Vehículo
	if (animacion && avanza==0)
	{
		movAuto_z += 3.0f;
		if (movAuto_z >= 100)
			avanza = 1;
	}
	else {
		if (animacion && avanza==1) {
			movAuto_z -= 3.0f;
			if (movAuto_z <= 0) {
				avanza = 2;
			}	
		}
		else {
			if (animacion && avanza == 2) {
				movAuto_y += 3.0f;
				if (movAuto_y >= 100) {
					avanza = 3;
				}
			}
			else {
				if (animacion && avanza == 3) {
					movAuto_y -= 3.0f;
					if (movAuto_y <= 0) {
						avanza = 0;
						animacion = false;
					}
				}
			}

		}
	}
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/ProyCG/piso/piso.obj");
	
	
	//Modelos Proyecto---------------------------------------------------------------------------------------------------------------------------
	Model restaurante("resources/objects/ProyCG/restaurante/rest.obj");
	Model mesa("resources/objects/ProyCG/mesa/mesa.obj");
	Model silla("resources/objects/ProyCG/silla/silla.obj");
	Model sonic("resources/objects/ProyCG/sonic/Sonic.obj");
	Model bis("resources/objects/ProyCG/sonic/bis.obj");
	Model bds("resources/objects/ProyCG/sonic/bds.obj");
	Model ring("resources/objects/ProyCG/ring/ring.obj");
	Model pastel("resources/objects/ProyCG/pastel/pastel.obj");
	Model micro("resources/objects/ProyCG/microfono/micro.obj");
	Model globor("resources/objects/ProyCG/globos/globor.obj");
	Model globop("resources/objects/ProyCG/globos/globop.obj");
	Model globon("resources/objects/ProyCG/globos/globon.obj");
	Model globodec("resources/objects/ProyCG/globos/globodec.obj");
	Model cocina("resources/objects/ProyCG/cocina/cocina.obj");
	Model bar("resources/objects/ProyCG/bar/bar.obj");
	Model cortina("resources/objects/ProyCG/cortina/cortina.obj");
	Model Arcade1("resources/objects/ProyCG/arcade/a1.obj");
	Model Arcade2("resources/objects/ProyCG/arcade/a2.obj");
	Model Arcade3("resources/objects/ProyCG/arcade/a3.obj");




	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
		KeyFrame[i].movBrazoDer = 0;
		KeyFrame[i].movBrzoIzq = 0;
		KeyFrame[i].rotRodDer = 0;
		KeyFrame[i].rotcabez = 0;
	}

	//Cuadros Clave
	/*KeyFrame[0].posX = 0.0f;
	KeyFrame[0].posY = 0.0f;
	KeyFrame[0].posZ = 0.0f;
	KeyFrame[0].rotRodIzq = 0.0f;
	KeyFrame[0].giroMonito = 0.0f;
	KeyFrame[0].movBrazoDer = 0.0f;

	KeyFrame[1].posX = 30.0f;
	KeyFrame[1].posY = 0.0f;
	KeyFrame[1].posZ = 0.0f;
	KeyFrame[1].rotRodIzq = -45.0f;
	KeyFrame[1].giroMonito = 90.0f;
	KeyFrame[1].movBrazoDer = -45.0f;

	KeyFrame[2].posX = 30.0f;
	KeyFrame[2].posY = 50.0f;
	KeyFrame[2].posZ = 0.0f;
	KeyFrame[2].rotRodIzq = 0.0f;
	KeyFrame[2].giroMonito = 0.0f;
	KeyFrame[2].movBrazoDer = 90.0f;

	KeyFrame[3].posX = 30.0f;
	KeyFrame[3].posY = 0.0f;
	KeyFrame[3].posZ = 0.0f;
	KeyFrame[3].rotRodIzq = 0.0f;
	KeyFrame[3].giroMonito = 0.0f;
	KeyFrame[3].movBrazoDer = 0.0f;*/

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(luzx, luzy, luzz));//Da luz a todo
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));	//Da luz desde un únto
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));	//Brillo sobre una superficie

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(1.0f, 1.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.008f); //Potencia de la luz
		staticShader.setFloat("pointLight[0].linear", 0.009f); //distancia de luz, control mas fino
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);//distancia de luz, es mas brusco, a mas pequeño menor atenuacion mas viaja la luz

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.2f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", myColor01);
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.00000032f);

		staticShader.setVec3("pointLight[2].position", myposition02);
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.2f, 0.0f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 1.0f);
		staticShader.setFloat("pointLight[2].linear", 0.009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.0000032f);

		//fuente de luz reflector
		staticShader.setVec3("spotLight[0].position", glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));//Posicion	
		staticShader.setVec3("spotLight[0].direction", glm::vec3(camera.Front.x, camera.Front.y, camera.Front.z));//Direccion a donde apunta la luz
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.3f, 0.3f, 0.3f));//
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));//Maxima iluminacion
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(20.0f)));//Disminucion de la intensidad
		staticShader.setFloat("spotLight[0].constant", 0.5f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);//Distancia que viajara la luz
		staticShader.setFloat("spotLight[0].quadratic", 0.005);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projection);
		animShader.setMat4("view", view);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);


		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);


		// -------------------------------------------------------------------------------------------------------------------------
		// Restaurante
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.7f, -100.0f));
		model = glm::scale(model, glm::vec3(4.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		restaurante.Draw(staticShader);

		//Mesas
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 0.0f, -170.0f));
		model = glm::scale(model, glm::vec3(6.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		mesa.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 0.0f, -170.0f));
		model = glm::scale(model, glm::vec3(6.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		mesa.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 0.0f, -100.0f));
		model = glm::scale(model, glm::vec3(6.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		mesa.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 0.0f, -100.0f));
		model = glm::scale(model, glm::vec3(6.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		mesa.Draw(staticShader);

		//Pasteles
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 11.0f, -170.0f));
		model = glm::scale(model, glm::vec3(2.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		pastel.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 11.0f, -170.0f));
		model = glm::scale(model, glm::vec3(2.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		pastel.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 11.0f, -100.0f));
		model = glm::scale(model, glm::vec3(2.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		pastel.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 11.0f, -100.0f));
		model = glm::scale(model, glm::vec3(2.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		pastel.Draw(staticShader);

		//Sillas
		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		silla.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(30.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		silla.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, -10.0f));
		model = glm::scale(model, glm::vec3(30.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		silla.Draw(staticShader);

		//Cuerpo sonic
		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(30.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		sonic.Draw(staticShader);

		//Brazo Izquierdo Sonic
		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(30.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		bis.Draw(staticShader);

		//Brazo Derecho Sonic
		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(30.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		bds.Draw(staticShader);

		//Rings
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 20.0f));
		model = glm::scale(model, glm::vec3(5.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		ring.Draw(staticShader);

		//Microfono
		model = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 7.5f, -110.0f));
		model = glm::scale(model, glm::vec3(150.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		micro.Draw(staticShader);

		//Globos
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 25.0f, 40.0f));
		model = glm::scale(model, glm::vec3(0.25));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		globodec.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 25.0f, 40.0f));
		model = glm::scale(model, glm::vec3(0.25));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		globor.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 25.0f, 40.0f));
		model = glm::scale(model, glm::vec3(0.25));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		globon.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 25.0f, 40.0f));
		model = glm::scale(model, glm::vec3(0.25));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		globop.Draw(staticShader);

		//Cocina
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-165.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(13.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cocina.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-220.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(13.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cocina.Draw(staticShader);

		//Mesa bar
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-55.0f, 0.0f, -10.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		bar.Draw(staticShader);

		//Cortinas Escenario
		model = glm::translate(glm::mat4(1.0f), glm::vec3(123.0f, 7.0f, -115.0f));
		model = glm::scale(model, glm::vec3(11.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cortina.Draw(staticShader);

		//Arcade
		model = glm::translate(glm::mat4(1.0f), glm::vec3(180.0f, 0.0f, -10.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Arcade1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(180.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(10.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Arcade1.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(140.0f, 0.0f, -35.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		Arcade2.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(160.0f, 0.0f, -35.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		staticShader.setMat4("model", model);
		Arcade2.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(140.0f, 0.0f, 33.0f));
		model = glm::scale(model, glm::vec3(1.15f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Arcade3.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(160.0f, 0.0f, 33.0f));
		model = glm::scale(model, glm::vec3(1.15f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Arcade3.Draw(staticShader);

		//Pasto Diorama
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -13.25f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);

		
		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS )
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		lightPosition.x--;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		lightPosition.x--;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		lightPosition.x--;

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && myColor01.x <= 1.0)
		myColor01.x++;
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && myColor01.x >= 0.0)
		myColor01.x--;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && myColor01.y <= 1.0)
		myColor01.y++;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && myColor01.y >= 0.0)
		myColor01.y--;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && myColor01.z <= 1.0)
		myColor01.z++;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && myColor01.z >= 0.0)
		myColor01.z--;

	//movimiento brazo
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && myColor01.z <= 1.0)
		movBrazoDer++;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && myColor01.z >= 0.0)
		movBrazoDer--;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && myColor01.z <= 1.0)
		movBrzoIzq++;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && myColor01.z >= 0.0)
		movBrzoIzq--;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && myColor01.z <= 1.0)
		rotRodDer++;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && myColor01.z >= 0.0)
		rotRodDer--;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && myColor01.z <= 1.0)
		rotcabez++;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && myColor01.z >= 0.0)
		rotcabez--;


		//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;	//XOR

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		animacion = false;
		movAuto_z = 0.0f; animacion ^= true;
	}

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
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
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}