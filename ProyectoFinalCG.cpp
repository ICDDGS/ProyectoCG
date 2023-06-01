/*---------------------------------------------------------*/
/* ----------------  Proyecto Final             -----------*/
/*-----------------    2023-2   ---------------------------*/
/*------ Cruz Cedillo Daniel Alejandro		  -------------*/
/*------------- No. Cuenta: 316083298---------------*/
/*------ Rueda Aguilar Karla Vanessa		  -------------*/
/*------------- No. Cuenta: 315184945---------------*/
#include <Windows.h>
#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <irrKlang.h> //Sonido


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
Camera camera(glm::vec3(0.0f, 15.0f, 350.0f));//Posicion incial de la camara 
float MovementSpeed = 50.0f;
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

//-------------------------------------------------------------------------------------
// Daclaracion de variables que se usaran para las animaciones
// ------------------------------------------------------------------------------------
//Luz ciclo de dia y noche
float luzx = 0.5f, luzy = 0.8f, luzz = 0.8f, noche = 0.0f;
int dia = 0;

//Sonic
float posxs = 0.0f,
poszs = 0.0f,
posys = 0.0f,
rotsonic =0.0f,
incsonic = 0.0f;
int animsonic = 0;

//Rings
float rotring = 0.0f;

//Freddy
float rotBrazoF = 0.0f;
int animFreddy = 0;

//Chica
float rotBrazoC = 0.0f, 
	rotpanque = 0.0f,
	poszpanque = 18.5f;
int animChica = 0;

//eggman
float eggx = 0.0f,
	eggy = 0.0f,
	eggz = 0.0f,
	rotegg = 0.0f,
	egginc =0.0f;
int animegg=0;

//Cheff
float rotcheff=0.0f,
	poszsar = 13.5f,
	rotsarten=0.0f,
	carnex =0.0f,
	carney = 0.0f,
	carnez =0.0f,
	tempcarne = 0.0f,
	carneinc =1.5f;
int animcheff = 0;

//Bunny
float	rot_bIzqB = 0.0f,
rot_bDerB = 0.0f,
rot_pIzqB = 0.0f,
rot_pDerB = 0.0f;
int animBunny = 0;
float reproduciranimacion, habilitaranimacion,
guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;
//PARA INPUT CON KEYFRAMES 
void inputKeyframes(bool* keys);

// Keyframes
float posX_globo = 0.0, posy_globo = 20.0, posz_globo = -100;
float	movGlobo_x = 0.0f, movGlobo_y = 0.0f, movGlobo_z = 0.0f;
float giroGlobo = 0;

#define MAX_FRAMES 50
int i_max_steps = 270;
int i_curr_steps = 9;

typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movGlobo_x;		//Variable para PosicionX
	float movGlobo_y;		//Variable para PosicionY
	float movGlobo_z;		//Variable para PosicionZ
	float movGlobo_xInc;		//Variable para IncrementoX
	float movGlobo_yInc;		//Variable para IncrementoY
	float movGlobo_zInc;		//Variable para IncrementoZ
	float giroGlobo;
	float giroGloboInc;
}FRAME; //Estructura Frame

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 46;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	printf("frameindex %d\n", FrameIndex);
	KeyFrame[FrameIndex].movGlobo_x = movGlobo_x;
	printf("KeyFrame[FrameIndex].X = Valor; %d\n", KeyFrame[FrameIndex].movGlobo_x);
	KeyFrame[FrameIndex].movGlobo_y = movGlobo_y;
	printf("KeyFrame[FrameIndex].Y = Valor; %d\n", KeyFrame[FrameIndex].movGlobo_y);
	KeyFrame[FrameIndex].movGlobo_z = movGlobo_z;
	printf("KeyFrame[FrameIndex].Z = Valor; %d\n", KeyFrame[FrameIndex].movGlobo_z);
	KeyFrame[FrameIndex].giroGlobo = giroGlobo;
	printf("KeyFrame[FrameIndex].giroGlobo = Valor; %d\n", KeyFrame[FrameIndex].giroGlobo);
	FrameIndex++;
}

//Punto de incio para reproducir
void resetElements(void)
{
	movGlobo_x = KeyFrame[0].movGlobo_x;
	movGlobo_y = KeyFrame[0].movGlobo_y;
	movGlobo_y = KeyFrame[0].movGlobo_z;
	giroGlobo = KeyFrame[0].giroGlobo;
}

void interpolation(void)
{
	KeyFrame[playIndex].movGlobo_xInc = (KeyFrame[playIndex + 1].movGlobo_x - KeyFrame[playIndex].movGlobo_x) / i_max_steps;
	KeyFrame[playIndex].movGlobo_yInc = (KeyFrame[playIndex + 1].movGlobo_y - KeyFrame[playIndex].movGlobo_y) / i_max_steps;
	KeyFrame[playIndex].movGlobo_zInc = (KeyFrame[playIndex + 1].movGlobo_z - KeyFrame[playIndex].movGlobo_z) / i_max_steps;
	KeyFrame[playIndex].giroGloboInc = (KeyFrame[playIndex + 1].giroGlobo - KeyFrame[playIndex].giroGlobo) / i_max_steps;
}

//-----------------------------------------------------------------------
//Animaciones
//-------------------------------------------------------------------

void animate(void)
{
	//animacion para el ciclo de dia y de noche
	/* El ciclo de dia y de noche sera aproximadamente de 1 min, en le caso 0 es para 
	hacer de dia, el caso 1 es para oscurecer 	y el caso 2 no cambia la iluminacion 
	pero si mantiene los valores ´para simularla */
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
	//--------------------------------------------------------------------------------
	//Animacion por keyframes
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			printf("Frame [%d] reproducido \n", playIndex - 1);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("Ultimo frame es [%d] \n", FrameIndex - 1);
				//fprintf(guardarFrames,"FrameIndex [%d]" ,FrameIndex);
				printf("Termina animaci�n\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				//printf("entro aqu�\n");
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			movGlobo_x += KeyFrame[playIndex].movGlobo_xInc;
			movGlobo_y += KeyFrame[playIndex].movGlobo_yInc;
			movGlobo_z += KeyFrame[playIndex].movGlobo_zInc;
			giroGlobo += KeyFrame[playIndex].giroGloboInc;
			i_curr_steps++;
		}
	}
	//--------------------------------------------------------------------------------
	//Animacion Sonic
	/*En esta animacion se usa a sonic hecho bolita y tiene que recorrer una parte del mapa, en le 
	caso 0 sonic avanza al inicio de la rampa, en le caso 1 sonic recorre la rampa circular, en el
	caso 2 sonic continua el camino restante, despues en los casoso 3,4 y 5realiza de forma 
	inversa el recorrido, tambien se le dio una rotacion, para simular que esta girando mientras avanaza*/
	rotsonic += 1.5f;
	switch (animsonic) {
	case 0:
		posys += 0.6f;
			if(posys >= 150)
				animsonic = 1;
		break;
	case 1:
		//Escala * 3 
		poszs = 95 + ( -95 *cos(incsonic));//altura circ 
		posys =  150 + ( 95 *sin(incsonic));//largo circ 
		incsonic += 0.01;

		posxs -= 0.1;
		if (incsonic >= 6.5) { //3
			//incsonic = 6.5f;	//1.5
			animsonic = 2;
		}
		if (posxs <= -66) {
			posxs = -66.0f;
		}
		break;
	case 2:
		posys += 0.6f;
		if (posys >= 350)
			//posys = 350;
			animsonic = 3;
		break;

	case 3:
		posys -= 0.6f;
		if (posys <= 150)
			animsonic = 4;
		break;
	case 4:
		poszs = 95 + (-95 * cos(incsonic));//altura circ 
		posys = 150 + (95 * sin(incsonic));//largo circ 
		incsonic -= 0.01;

		posxs += 0.1;
		if (incsonic <= 0.0) { 
			incsonic = 0.0f;	//1.5
			animsonic = 5;
		}
		break;
	case 5:
		posys -= 0.6f;
		if (posys <= 0) {
			posys = 0;
			animsonic = 0;
		}
			
		break;
	}
	//--------------------------------------------------------------------------------
	//Animacion Ring
	/*Esta animacion se le da giro al ring*/
	rotring += 2.5;
	if (rotring >= 180)
		rotring = 0.0f;
	//--------------------------------------------------------------------------------
	//Animacion Saludo Freddy
	/*Se busca darle una animacion de saludo, para ello se le da animacion al brazo realiando 
	rotaciones en el brazo, en el caso 0 el brazo sube por medioD de una rotacion y en el 
	caso 1 el brazo deciende por emdio de otra rotacion*/
	switch (animFreddy) {
	case 0:
		rotBrazoF += 1.0f;
		if (rotBrazoF >= 45)
			animFreddy = 1;
		break;
	case 1:
		
		rotBrazoF -= 1.0f;
		if (rotBrazoF <= -45)
			animFreddy = 0;
		break;
	}
	//--------------------------------------------------------------------------------
	//Animacion Eggman
	/*En este caso se busco darle la animacon de eggman elevandose al cielo, y despues
	de hacerlo que de vueltas a lo largo del edificio, el caso 1, 2, 3  y 4 se encargan
	de posiconarlo en el lugar de despegue, en el caso 5 se le da la animacion de 
	despegue y en el caso 6 se le da la animacion de dar vueltas al edificio*/
	switch (animegg) {
		case 0:
			eggy += 0.7f;
			if (eggy >= 70)
				animegg = 1;
			break;
		case 1:
			rotegg += 0.7f;

			if (rotegg >= 90)
				animegg = 2;
			break;
		case 2:
			eggx += 0.7f;
			if (eggx >= 200)
				animegg = 3;
		break;
		case 3:
			rotegg -= 0.7f;

			if (rotegg <= 0)
				animegg = 4;
			break;
		case 4:
			eggy -= 0.7f;
			if (eggy <= 40)
				animegg = 5;
			break;
		case 5:
			eggz += 0.3f;
			eggx = 200 * cos(egginc);
			eggz += 0.3f;
			eggy = 200 * sin(egginc);
			eggz += 0.3f;
			egginc += 0.01f;
			rotegg -= 0.4f;
			if (eggz >= 100)
				animegg = 6;
			break;
		case 6:
			rotegg -= 0.46f;
			eggx = 200 * cos(egginc);
			eggy = 200 * sin(egginc);
			egginc += 0.008f;
			break;
	}
	//--------------------------------------------------------------------------------
	//Animacio Chica
	/*En esta animacion se busca que chica lance un panque al aire y que de vueltas
	a lo largo del recorrido, en el caso 0 el panque y el brazo se mueven al mismo 
	tiempo, y empieza la rotacion del panque, en le caso 1 el panque se eleva y rota
	y en el caso 2 	el panque deciende con el brazo de chica mientras termina su 
	rotacion*/
	switch (animChica) {
	case 0:
		rotBrazoC -= 0.3f;
		poszpanque += 0.1f;
		rotpanque += 2.7;
		if (rotBrazoC <= -20)
			animChica = 1;
		break;
	case 1:
		poszpanque += 0.3f;
		if (poszpanque >= 25)
			animChica = 2;
		break;
	case 2:
		rotBrazoC += 0.3f;
		rotpanque += 2.7;
		poszpanque -= 0.105f;
		if (rotpanque >= 360)
			rotpanque = 0;
		if (rotBrazoC >= 0)
			animChica = 0;
		break;
	}
	//--------------------------------------------------------------------------------
	//Animacion Cheff
	/*La animacion consiste en que el cheff lanza el sarten al aire con la carne,
	y la misma vuela hasta un plato que hay sobre la mesa, en el caso 0 el cheff 
	lanza el sarten por medio de una rotacion en sus brazos, el sarten aumenta su
	altura mientras tiene una pequeña rotacion, la carne tambien aumenta su altura
	mientras realiza un trayectoria curva, en el caso 1 se hace una pequeña pausa
	en el caso 2 y 3 cambia a otra rotacion y continua la animacion para llegar 
	al plato, en el caso 4 la carne se da una pequeña pausa a la carn en el 
	plato antes de repetir la animacion*/
	switch(animcheff){
	case 0:
		rotcheff += 0.3f;
		poszsar += 0.1;
		rotsarten += 0.25;
		carnez = -15.0 * cos(carneinc);
		carney = 12.0 * sin(carneinc);
		carneinc += 0.015;
		if (carneinc >= 3) { //3
			carneinc = 3.0f;	//1.5
		}
		if (rotcheff >= 30)
			animcheff = 1;
		break;
	case 1:
		carneinc += 0.04;
		if (carneinc >= 3.1) {
			animcheff = 2;
		}
		break;
	case 2:
		carnez = -15.0 * cos(carneinc);
		carney = 70 * sin(carneinc);
		carneinc += 0.005;
		rotcheff -= 0.3f;
		rotsarten -= 0.25;
		poszsar -= 0.1;

		if (rotcheff <= 0) {
			animcheff = 3;
		}		
		break;
	case 3:
		carnez = -15.0 * cos(carneinc);
		carney = 70 * sin(carneinc);
		carneinc += 0.008;
		if (carneinc >= 4.85) {
			animcheff = 4;
		}
		break;
	case 4:
		carneinc += 0.01;
		if (carneinc >= 7) {
			carneinc = 1.5f;
			animcheff = 0;
		}
		break;
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

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// --------------------
	// glfw window creation
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
	//-------------------------------------------------------------------------
	//Sonido de fondo
	//---------------------------------------------------------------------
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
	
	if (!engine) {
		printf("Could not startup engine ");
		return 0; //eror inciando engine
	}

	engine->play2D("fnaf.mp3", true);
	engine->setSoundVolume(0.4f);
	//--------------------------------
	// configure global opengl state
	// ------------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/rightcity.jpg", 
		"resources/skybox/leftcity.jpg",
		"resources/skybox/topcity.jpg",
		"resources/skybox/bottomcity.jpg",
		"resources/skybox/frontcity.jpg", 
		"resources/skybox/backcity.jpg" 
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	
	//--------------------------------------------------------------------------------
	//Modelos Proyecto
	//-------------------------------------------------------------------------------
	//Elementos sin animacion
	//--------------------------------------------------------------------------------
	Model restaurante("resources/objects/restaurante/rest.obj");
	Model mesa("resources/objects/mesa/mesa.obj");
	Model silla("resources/objects/silla/silla.obj");
	Model pastel("resources/objects/pastel/pastel.obj");
	Model micro("resources/objects/microfono/micro.obj");
	Model globor("resources/objects/globos/globor.obj");
	Model globop("resources/objects/globos/globop.obj");
	Model globon("resources/objects/globos/globon.obj");
	Model globodec("resources/objects/globos/globodec.obj");
	Model cocina("resources/objects/cocina/cocina.obj");
	Model bar("resources/objects/bar/bar.obj");
	Model cortina("resources/objects/cortina/cortina.obj");
	Model Arcade1("resources/objects/arcade/a1.obj");
	Model Arcade2("resources/objects/arcade/a2.obj");
	Model Arcade3("resources/objects/arcade/a3.obj");
	//--------------------------------------------------------------------------------
	//Modelos con animacion
	//--------------------------------------------------------------------------------
	//Sonic
	Model mapa("resources/objects/sonic/mapa.obj");
	Model sonic("resources/objects/sonic/sonic.obj");
	//ring
	Model ring("resources/objects/ring/ring.obj");
	//EggMAn
	Model Eggman("resources/objects/Eggman/Eggman.obj");
	//Freddy
	Model Freddy("resources/objects/Freddy/Freddy.obj");
	Model FreddyBrazo("resources/objects/Freddy/FreddyBrazo.obj");
	//Chica
	Model Chica("resources/objects/Chica/chica.obj");
	Model ChicaBrazo("resources/objects/Chica/chicabrazo.obj");
	Model panque("resources/objects/Chica/panque.obj");
	//Cheff
	Model cheff("resources/objects/cocinero/cheff.obj");
	Model cheffbd("resources/objects/cocinero/cheffbd.obj");
	Model cheffbi("resources/objects/cocinero/cheffbi.obj");
	Model sarten("resources/objects/cocinero/sarten.obj");
	Model carne("resources/objects/cocinero/carne.obj");
	Model plato("resources/objects/cocinero/plato.obj");

	//Bunny
	Model Bunny("resources/objects/Bunny/cuerpoBunny.obj");
	Model BunnyBrazoIzq("resources/objects/Bunny/bIzqBunny.obj");
	Model BunnyBrazoDer("resources/objects/Bunny/bDerBunny.obj");
	Model BunnyPieIzq("resources/objects/Bunny/pIzqBunny.obj");
	Model BunnyPieDer("resources/objects/Bunny/pDerBunny.obj");
	//Globo
	Model globo("resources/objects/globos/globodec.obj");

	//para keyframes
	animate();
	//Inicialización de KeyFrames

	KeyFrame[0].movGlobo_x = 0.0f;
	KeyFrame[0].movGlobo_y = 0.0f;
	KeyFrame[0].movGlobo_z = 0.0f;
	KeyFrame[0].giroGlobo = 0;


	KeyFrame[1].movGlobo_x = 0.0f;
	KeyFrame[1].movGlobo_y = 10.0f;
	KeyFrame[1].movGlobo_z = 0.0f;
	KeyFrame[1].giroGlobo = 0;


	KeyFrame[2].movGlobo_x = 5.0f;
	KeyFrame[2].movGlobo_y = 6.0f;
	KeyFrame[2].movGlobo_z = 0.0f;
	KeyFrame[2].giroGlobo = 0;


	KeyFrame[3].movGlobo_x = 7.0f;
	KeyFrame[3].movGlobo_y = 10.0f;
	KeyFrame[3].movGlobo_z = 0.0f;
	KeyFrame[3].giroGlobo = 0;

	KeyFrame[4].movGlobo_x = 9.0f;
	KeyFrame[4].movGlobo_y = 6.0f;
	KeyFrame[4].movGlobo_z = 0.0f;
	KeyFrame[4].giroGlobo = 0.0f;

	KeyFrame[5].movGlobo_x = 11.0f;
	KeyFrame[5].movGlobo_y = 10.0f;
	KeyFrame[5].movGlobo_z = 0.0f;
	KeyFrame[5].giroGlobo = 0;

	KeyFrame[6].movGlobo_x = 13.0f;
	KeyFrame[6].movGlobo_y = 6.0f;
	KeyFrame[6].movGlobo_z = 0.0f;
	KeyFrame[6].giroGlobo = 0;

	KeyFrame[7].movGlobo_x = 15.0f;
	KeyFrame[7].movGlobo_y = 10.0f;
	KeyFrame[7].movGlobo_z = 0.0f;
	KeyFrame[7].giroGlobo = 0;

	KeyFrame[8].movGlobo_x = 17.0f;
	KeyFrame[8].movGlobo_y = 6.0f;
	KeyFrame[8].movGlobo_z = 0.0f;
	KeyFrame[8].giroGlobo = 90;

	KeyFrame[9].movGlobo_x = 19.0f;
	KeyFrame[9].movGlobo_y = 10.0f;
	KeyFrame[9].movGlobo_z = -4.0f;
	KeyFrame[9].giroGlobo = 90;

	KeyFrame[10].movGlobo_x = 19.0f;
	KeyFrame[10].movGlobo_y = 6.0f;
	KeyFrame[10].movGlobo_z = -8.0f;
	KeyFrame[10].giroGlobo = 90;

	KeyFrame[11].movGlobo_x = 19.0f;
	KeyFrame[11].movGlobo_y = 10.0f;
	KeyFrame[11].movGlobo_z = -12.0f;
	KeyFrame[11].giroGlobo = 90;

	KeyFrame[12].movGlobo_x = 19.0f;
	KeyFrame[12].movGlobo_y = 6.0f;
	KeyFrame[12].movGlobo_z = -16.0f;
	KeyFrame[12].giroGlobo = 90;

	KeyFrame[13].movGlobo_x = 19.0f;
	KeyFrame[13].movGlobo_y = 10.0f;
	KeyFrame[13].movGlobo_z = -20.0f;
	KeyFrame[13].giroGlobo = 90;

	KeyFrame[14].movGlobo_x = 19.0f;
	KeyFrame[14].movGlobo_y = 6.0f;
	KeyFrame[14].movGlobo_z = -24.0f;
	KeyFrame[14].giroGlobo = 90;

	KeyFrame[15].movGlobo_x = 19.0f;
	KeyFrame[15].movGlobo_y = 10.0f;
	KeyFrame[15].movGlobo_z = -28.0f;
	KeyFrame[15].giroGlobo = 90;

	KeyFrame[16].movGlobo_x = 19.0f;
	KeyFrame[16].movGlobo_y = 6.0f;
	KeyFrame[16].movGlobo_z = -32.0f;
	KeyFrame[16].giroGlobo = 90;

	KeyFrame[17].movGlobo_x = 19.0f;
	KeyFrame[17].movGlobo_y = 10.0f;
	KeyFrame[17].movGlobo_z = -36.0f;
	KeyFrame[17].giroGlobo = 90;

	KeyFrame[18].movGlobo_x = 19.0f;
	KeyFrame[18].movGlobo_y = 6.0f;
	KeyFrame[18].movGlobo_z = -40.0f;
	KeyFrame[18].giroGlobo = 90;

	KeyFrame[19].movGlobo_x = 19.0f;
	KeyFrame[19].movGlobo_y = 5.0f;
	KeyFrame[19].movGlobo_z = -44.0f;
	KeyFrame[19].giroGlobo = 90;

	KeyFrame[20].movGlobo_x = 19.0f;
	KeyFrame[20].movGlobo_y = 4.0f;
	KeyFrame[20].movGlobo_z = -48.0f;
	KeyFrame[20].giroGlobo = 90;

	KeyFrame[21].movGlobo_x = 19.0f;
	KeyFrame[21].movGlobo_y = 3.0f;
	KeyFrame[21].movGlobo_z = -52.0f;
	KeyFrame[21].giroGlobo = 90;

	KeyFrame[22].movGlobo_x = 19.0f;
	KeyFrame[22].movGlobo_y = 2.0f;
	KeyFrame[22].movGlobo_z = -56.0f;
	KeyFrame[22].giroGlobo = 90;

	KeyFrame[23].movGlobo_x = 19.0f;
	KeyFrame[23].movGlobo_y = 1.0f;
	KeyFrame[23].movGlobo_z = -60.0f;
	KeyFrame[23].giroGlobo = 90;

	KeyFrame[24].movGlobo_x = 19.0f;
	KeyFrame[24].movGlobo_y = 0.0f;
	KeyFrame[24].movGlobo_z = -64.0f;
	KeyFrame[24].giroGlobo = 90;

	KeyFrame[25].movGlobo_x = 19.0f;
	KeyFrame[25].movGlobo_y = 0.0f;
	KeyFrame[25].movGlobo_z = -68.0f;
	KeyFrame[25].giroGlobo = 90;
	
	KeyFrame[26].movGlobo_x = 19.0f;
	KeyFrame[26].movGlobo_y = 0.0f;
	KeyFrame[26].movGlobo_z = -68.0f;
	KeyFrame[26].giroGlobo = 180;
	
	KeyFrame[27].movGlobo_x = 15.0f;
	KeyFrame[27].movGlobo_y = 1.0f;
	KeyFrame[27].movGlobo_z = -68.0f;
	KeyFrame[27].giroGlobo = 180;

	KeyFrame[28].movGlobo_x = 11.0f;
	KeyFrame[28].movGlobo_y = 2.f;
	KeyFrame[28].movGlobo_z = -68.0f;
	KeyFrame[28].giroGlobo = 180;

	KeyFrame[29].movGlobo_x = 7.0f;
	KeyFrame[29].movGlobo_y = 3.0f;
	KeyFrame[29].movGlobo_z = -68.0f;
	KeyFrame[29].giroGlobo = 180;

	KeyFrame[30].movGlobo_x = 3.0f;
	KeyFrame[30].movGlobo_y = 4.0f;
	KeyFrame[30].movGlobo_z = -68.0f;
	KeyFrame[30].giroGlobo = 180;

	KeyFrame[31].movGlobo_x = -1.0f;
	KeyFrame[31].movGlobo_y = 5.0f;
	KeyFrame[31].movGlobo_z = -68.0f;
	KeyFrame[31].giroGlobo = 180;

	KeyFrame[32].movGlobo_x = -5.0f;
	KeyFrame[32].movGlobo_y = 6.0f;
	KeyFrame[32].movGlobo_z = -68.0f;
	KeyFrame[32].giroGlobo = 180;
	
	KeyFrame[33].movGlobo_x = -5.0f;
	KeyFrame[33].movGlobo_y = 6.0f;
	KeyFrame[33].movGlobo_z = -68.0f;
	KeyFrame[33].giroGlobo = 270;
	
	KeyFrame[34].movGlobo_x = -5.0f;
	KeyFrame[34].movGlobo_y = 10.0f;
	KeyFrame[34].movGlobo_z = -60.0f;
	KeyFrame[34].giroGlobo = 270;

	KeyFrame[35].movGlobo_x = -5.0f;
	KeyFrame[35].movGlobo_y = 6.0f;
	KeyFrame[35].movGlobo_z = -52.0f;
	KeyFrame[35].giroGlobo = 270;

	KeyFrame[36].movGlobo_x = -5.0f;
	KeyFrame[36].movGlobo_y = 10.0f;
	KeyFrame[36].movGlobo_z = -44.0f;
	KeyFrame[36].giroGlobo = 270;

	KeyFrame[37].movGlobo_x = -5.0f;
	KeyFrame[37].movGlobo_y = 6.0f;
	KeyFrame[37].movGlobo_z = -36.0f;
	KeyFrame[37].giroGlobo = 270;

	KeyFrame[38].movGlobo_x = -5.0f;
	KeyFrame[38].movGlobo_y = 10.0f;
	KeyFrame[38].movGlobo_z = -28.0f;
	KeyFrame[38].giroGlobo = 270;

	KeyFrame[39].movGlobo_x = -5.0f;
	KeyFrame[39].movGlobo_y = 6.0f;
	KeyFrame[39].movGlobo_z = -20.0f;
	KeyFrame[39].giroGlobo = 270;

	KeyFrame[40].movGlobo_x = -5.0f;
	KeyFrame[40].movGlobo_y = 10.0f;
	KeyFrame[40].movGlobo_z = -12.0f;
	KeyFrame[40].giroGlobo = 270;

	KeyFrame[41].movGlobo_x = -5.0f;
	KeyFrame[41].movGlobo_y = 6.0f;
	KeyFrame[41].movGlobo_z = -4.0f;
	KeyFrame[41].giroGlobo = 270;

	KeyFrame[42].movGlobo_x = -5.0f;
	KeyFrame[42].movGlobo_y = 5.0f;
	KeyFrame[42].movGlobo_z = 0.0f;
	KeyFrame[42].giroGlobo = 270;
	
	KeyFrame[43].movGlobo_x = -5.0f;
	KeyFrame[43].movGlobo_y = 5.0f;
	KeyFrame[43].movGlobo_z = 0.0f;
	KeyFrame[43].giroGlobo = 360;
	
	KeyFrame[44].movGlobo_x = -5.0f;
	KeyFrame[44].movGlobo_y = 0.0f;
	KeyFrame[44].movGlobo_z = 0.0f;
	KeyFrame[44].giroGlobo = 360;
	
	KeyFrame[45].movGlobo_x = 0.0f;
	KeyFrame[45].movGlobo_y = 0.0f;
	KeyFrame[45].movGlobo_z = 0.0f;
	KeyFrame[45].giroGlobo = 360;


	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{

	}


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

		//animShader.setVec3("material.specular", glm::vec3(0.5f));
		//animShader.setFloat("material.shininess", 32.0f);
		//animShader.setVec3("light.ambient", ambientColor);
		//animShader.setVec3("light.diffuse", diffuseColor);
		//animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		//animShader.setVec3("light.direction", lightDirection);
		//animShader.setVec3("viewPos", camera.Position);


		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);


		// Restaurante
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.7f, -100.0f));
		model = glm::scale(model, glm::vec3(4.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		restaurante.Draw(staticShader);

		//Mesas
		// -------------------------------------------------------------------------------------------------------------------------
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

		//Pastel
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 11.0f, -170.0f));
		model = glm::scale(model, glm::vec3(2.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		pastel.Draw(staticShader);

		//Sillas
		// -------------------------------------------------------------------------------------------------------------------------
		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 0.0f));
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
		silla.Draw(staticShader);*/

		//Sonic
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, 5.0f, 150.0f));
		model = glm::scale(model, glm::vec3(8.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		mapa.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(posxs + 340.0f, poszs + 11.0f, posys)); //(340,10,0)
		model = glm::scale(model, glm::vec3(3.0));
		model = glm::rotate(model, glm::radians(rotsonic), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		sonic.Draw(staticShader);

		//Rings
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(340.0f, 10.0f, 150.0f));
		model = glm::scale(model, glm::vec3(4.0));
		model = glm::rotate(model, glm::radians(rotring), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		ring.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(340.0f, 10.0f, 100.0f));
		model = glm::scale(model, glm::vec3(4.0));
		model = glm::rotate(model, glm::radians(rotring), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		ring.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(340.0f, 10.0f, 50.0f));
		model = glm::scale(model, glm::vec3(4.0));
		model = glm::rotate(model, glm::radians(rotring), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		ring.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 10.0f, 150.0f));
		model = glm::scale(model, glm::vec3(4.0));
		model = glm::rotate(model, glm::radians(rotring), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		ring.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 10.0f, 200.0f));
		model = glm::scale(model, glm::vec3(4.0));
		model = glm::rotate(model, glm::radians(rotring), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		ring.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 10.0f, 250.0f));
		model = glm::scale(model, glm::vec3(4.0));
		model = glm::rotate(model, glm::radians(rotring), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		ring.Draw(staticShader);

		//Microfono
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 7.5f, -110.0f));
		model = glm::scale(model, glm::vec3(150.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		micro.Draw(staticShader);

		//Globos
		// -------------------------------------------------------------------------------------------------------------------------
		/*model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 25.0f, 40.0f));
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
		globop.Draw(staticShader);*/

		//Cocina
		// -------------------------------------------------------------------------------------------------------------------------
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

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, 0.0f, -70.0f));
		model = glm::scale(model, glm::vec3(6.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		mesa.Draw(staticShader);

		//Mesa bar
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-55.0f, 0.0f, -10.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		bar.Draw(staticShader);

		//Cortinas Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(123.0f, 7.0f, -115.0f));
		model = glm::scale(model, glm::vec3(11.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cortina.Draw(staticShader);

		//Arcade
		// -------------------------------------------------------------------------------------------------------------------------
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

		//Freddy
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 0.0f, 50.0f));
		model = glm::scale(model, glm::vec3(10.0));
		staticShader.setMat4("model", model);
		Freddy.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(47.0f, 34.5f, 48.0f));
		model = glm::scale(model, glm::vec3(10.0));
		model = glm::rotate(model, glm::radians(rotBrazoF), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		FreddyBrazo.Draw(staticShader);

		//Eggman
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(eggx, eggz, eggy));
		model = glm::scale(model, glm::vec3(3.0));
		model = glm::rotate(model, glm::radians(rotegg), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		Eggman.Draw(staticShader);

		//Chica
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -220.0f));
		model = glm::scale(model, glm::vec3(0.3));
		staticShader.setMat4("model", model);
		Chica.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f, 17.0f, -218.5f));
		model = glm::scale(model, glm::vec3(0.3));
		model = glm::rotate(model, glm::radians(rotBrazoC), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		ChicaBrazo.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5f, poszpanque, -212.0f));
		model = glm::scale(model, glm::vec3(0.025));
		model = glm::rotate(model, glm::radians(rotpanque), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		panque.Draw(staticShader);

		//cheff
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, 0.0f, 0.0f));//(-180,0,0)
		model = glm::scale(model, glm::vec3(14.0f));
		staticShader.setMat4("model", model);
		cheff.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-182.0f, 13.5f, 0.0f));//(-180,0,0)
		model = glm::scale(model, glm::vec3(14.0f));
		model = glm::rotate(model, glm::radians(rotcheff), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		cheffbd.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-178.0f, 13.5f, 0.0f));//(-180,0,0)
		model = glm::scale(model, glm::vec3(14.0f));
		model = glm::rotate(model, glm::radians(-1*rotcheff), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		cheffbd.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, poszsar, 7.0f));//(-180,13.5,0)
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotsarten), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		sarten.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, 11.2f, -70.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		plato.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, carnez + 13.5, carney));//(-180,13.5,12.0)
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		carne.Draw(staticShader);

		//Bunny
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-85.0f, -0.5f, -10.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0));
		staticShader.setMat4("model", model);
		Bunny.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-85.0f, -0.5f, -10.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0));
		model = glm::rotate(model, glm::radians(rot_bIzqB), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		BunnyBrazoIzq.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-85.0f, -0.5f, -10.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0));
		model = glm::rotate(model, glm::radians(rot_bDerB), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		BunnyBrazoDer.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-85.0f, -0.5f, -10.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0));
		model = glm::rotate(model, glm::radians(rot_pIzqB), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		BunnyPieIzq.Draw(staticShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-85.0f, -0.5f, -10.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0));
		model = glm::rotate(model, glm::radians(rot_pDerB), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		BunnyPieDer.Draw(staticShader);

		//Globo
		// -------------------------------------------------------------------------------------------------------------------------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(posX_globo + movGlobo_x, posy_globo + movGlobo_y, posz_globo));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3));
		model = glm::rotate(model, glm::radians(giroGlobo), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		globo.Draw(staticShader);

		//Pasto Diorama
		// -------------------------------------------------------------------------------------------------------------------------
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
	

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;
			}
		}
	}

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movGlobo_x es: %f\n", movGlobo_x);
			//printf("movGlobo_y es: %f\n", movGlobo_y);
			printf(" \npresiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
		}
	}


	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		if (ciclo < 1)
		{
			//printf("movGlobo_x es: %f\n", movGlobo_x);
			movGlobo_x += 1.0f;
			printf("\n movGlobo_x es: %f\n", movGlobo_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 2\n");
		}

	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
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