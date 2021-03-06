#include "Button.h"

#define BUTTON_PADDING 20

Button::Button() {
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	img = ImageLoader::getImage("Resources\\buttons\\button.png");
	imgSelected = ImageLoader::getImage("Resources\\buttons\\buttonSelected.png");
	glBindTexture(GL_TEXTURE_2D, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, imgSelected);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	font = &fontLarge;
}


Button::~Button() {
}

// Sets the x-position of the button
void Button::setX(int x) {
	this->x = x;
}


// Gets the x-position of the button
int Button::getX() {
	return x;
}


// Sets the y-position of the button
void Button::setY(int y) {
	this->y = y;
}


// Gets the y-position of the button
int Button::getY() {
	return y;
}


// Sets the width of the button
void Button::setWidth(int width) {
	this->width = width;
}


// Gets the width of the button
int Button::getWidth() {
	return width;
}


// Sets the height of the button
void Button::setHeight(int height) {
	this->height = height;
}


// Gets the height of the button
int Button::getHeight() {
	return height;
}


// Sets the label on the button
void Button::setLabel(std::string label) {
	this->label = label;
}


// Gets the label of the button
std::string Button::getLabel() {
	return label;
}


// Draw the button
void Button::draw() {
	glColor3ub(255, 255, 255);
	glPushMatrix();
	glTranslatef((float)x, (float)y, 0.0f);
	glEnable(GL_TEXTURE_2D);
	double mx, my;
	float sX = width * 0.5f;
	float sY = height * 0.5f;
	glfwGetCursorPos(gameWindow, &mx, &my);
	my = sHeight - my;
	//Display a different texture if mouse is over button
	if (mx >= x - sX && mx <= x + sX && my >= y - sY && my <= y + sY) {
		glBindTexture(GL_TEXTURE_2D, imgSelected);
	} else {
		glBindTexture(GL_TEXTURE_2D, img);
	}
	//Draw corners
	for (int i = 0; i < 4; i++) {
		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0);
		glVertex2f(-sX, -sY);
		glTexCoord2d(sX / 32.0, 0.0);
		glVertex2f(0, -sY);
		glTexCoord2d(sX / 32.0, sY / 32.0);
		glVertex2f(0, 0);
		glTexCoord2d(0.0, sY / 32.0);
		glVertex2f(-sX, 0);
		glEnd();
		glRotatef(90.0, 0, 0, 1);
		{ float t = sX; sX = sY; sY = t; }
	}
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glColor3ub(255, 255, 255);
	float centre = freetype::getLength(*font, label.c_str()) * 0.5f;
	//Oh the wonders of fonts and their heights not including the entire letter
	freetype::print(*font, (float)x - centre, (float)y - font->h * 0.375f, label.c_str());
}


// Draw the button with exrapolation
void Button::draw(double ex) {
	draw();
}


// Called when a key event is fired
void Button::keyEvent(GLFWwindow * window, int key, int scan, int action, int mods) {
}


// Sets the function called when the button is pressed
void Button::setCallback(void(*callback)(BaseState* s)) {
	this->callback = callback;
}


// Called when the left mouse button is clicked
bool Button::mouseDown(int x, int y) {
	float w = width*0.5f;
	float h = height*0.5f;
	if (x >= this->x - w && x <= this->x + w && y >= this->y - h && y <= this->y + h) {
		(*callback)(state);
		return true;
	}
	return false;
}
