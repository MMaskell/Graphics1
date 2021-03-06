#include <iostream>
#include <filesystem>
#include "Globals.h"
#include "BaseState.h"
#include "MainMenu.h"
//----------------------------------Globals----------------------------------//
BaseState* state;
BaseState* newState;
GLFWwindow* gameWindow;
int sWidth;
int sHeight;
freetype::font_data fontLarge;
freetype::font_data fontSmall;
GLFWcursor* cursorNormal;
GLFWcursor* cursorPan;
//-------------------Callback functions and state handlers-------------------//
///Handles key presses
void keyHandler(GLFWwindow* window, int key, int scan, int action, int mods) {
	state->keyEvent(window, key, scan, action, mods);
}
///Handles mouse buttons
void mouseHandler(GLFWwindow* window, int button, int action, int mods) {
	state->mouseEvent(window, button, action, mods);
}
///Handles mouse movement
void mouseMoveHandler(GLFWwindow* window, double x, double y) {
	state->mouseMoveEvent(window, x, y);
}
///Handles text input
void textHandler(GLFWwindow* window, unsigned int ch) {
	state->textEvent(window, ch);
}
///Starts the game and loads anything that needs loading
void init() {
	//Load the key bindings
	KeyConfig::loadBindings();
	//Allow KeyConfig to get key states
	KeyConfig::win = gameWindow;
	//Initialise the texture loader
	ImageLoader::makeMissingTexture();
	//Create the font
	fontLarge.init(FONT_NAME, FONT_SIZE_LARGE);
	fontSmall.init(FONT_NAME, FONT_SIZE_SMALL);
	//Enable alpha channel for OpenGL
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//Create cursors
	cursorNormal = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	cursorPan = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
	//Set state to main menu
	state = new MainMenu();
	//Ensure level folder is present
	std::experimental::filesystem::create_directory("Levels");
}
///Draws the scene
void draw(double ex) {
	//For the sake of checking the background is correct clear to purple
	glClearColor(1.0, 0.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3ub(255, 255, 255);
	glPushMatrix();
	state->draw(ex);
	glPopMatrix();
}
///Updates the game
void update() {
	state->update();
}
///Resizes the window
void resize(GLFWwindow* window, int width, int height) {
	//Fix the viewport
	sWidth = width;
	sHeight = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//Monkey around with the fonts
	fontLarge.clean();
	fontLarge.init(FONT_NAME, height / 25);
	fontSmall.clean();
	fontSmall.init(FONT_NAME, height / 40);
	state->resizeEvent(window, width, height);
}
//-----------------------Window creation and game loop-----------------------//
int main() {
	//Load sounds first, to avoid noticable pause as resources load
	SoundLoader::init();
	if (!glfwInit()) {
		std::cerr << "Failed to initialise GLFW";
		exit(EXIT_FAILURE);
	}
	//GLFW needs to know which version of opengl I am targeting
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	gameWindow = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, TITLE, NULL, NULL);
	glfwMakeContextCurrent(gameWindow);
	//I am given to understand this is glfw's way of enabling v-sync
	glfwSwapInterval(1);
	//Handle window events
	glfwSetWindowSizeCallback(gameWindow, resize);
	//Behold a magical bodge that turns an OpenGL image into a GLFWimage
	int width = 256, height = 256;
	GLuint icon = ImageLoader::getImage("Resources\\icon.png");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(1.0, 0.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3ub(255, 255, 255);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, icon);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 1.0);
	glVertex2i(0, 0);
	glTexCoord2d(1.0, 1.0);
	glVertex2i(width / 2, 0);
	glTexCoord2d(1.0, 0.0);
	glVertex2i(width / 2, height / 2);
	glTexCoord2d(0.0, 0.0);
	glVertex2i(0, height / 2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	GLFWimage glfwIcon;
	glfwIcon.width = width;
	glfwIcon.height = height;
	unsigned char *data = new unsigned char[4 * width * height];
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	unsigned char r = data[0];
	glfwIcon.pixels = data;
	//Set window icon
	glfwSetWindowIcon(gameWindow, 1, &glfwIcon);
	if (!gameWindow) {
		std::cerr << "Failed to create window";
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	//Initialise the game
	init();
	//Call resize to initialise display
	resize(gameWindow, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glfwSetKeyCallback(gameWindow, keyHandler);
	glfwSetMouseButtonCallback(gameWindow, mouseHandler);
	glfwSetCursorPosCallback(gameWindow, mouseMoveHandler);
	glfwSetCharCallback(gameWindow, textHandler);
	//Game loop, calls update every TICKRATE ms and renders whenever it can
	//I didn't invent this type of main loop, I found it online a while ago
	//I did, however, implement it
	double previous = glfwGetTime();
	double lastUpdate = 0.0f;
	while (!glfwWindowShouldClose(gameWindow)) {
		//Calculate time taken for last frame
		double current = glfwGetTime();
		double elapsed = current - previous;
		previous = current;
		//Track total time since last update
		lastUpdate += elapsed;
		//Do any input things here
		glfwPollEvents();
		//Handle the state of the game
		if (newState != NULL) {
			delete state;
			state = newState;
			newState = NULL;
			//Ensure new state has correct window dimensions
			state->resizeEvent(gameWindow, sWidth, sHeight);
		}
		//Call updates until the game has caught up
		//But don't update if not enough time has elapsed
		while (lastUpdate >= TICKRATE) {
			update();
			lastUpdate -= TICKRATE;
		}
		//Draw the game, allowing for any extrapolation
		draw(lastUpdate);
		//Display the back buffer
		glfwSwapBuffers(gameWindow);
	}
	//Shutdown properly
	ImageLoader::cleanup();
	SoundLoader::cleanup();
	glfwDestroyWindow(gameWindow);
	glfwTerminate();
}