#include "Platform.h"
#include <math.h>
#include "Globals.h"


Platform::Platform() {
	id = "platform";
	textureString = "";
	texture = ImageLoader::getImage(textureString);
	//Store both versions to minimise rounding errors
	//The number of times the texture repeats in each axis
	texX = 1;
	texY = 1;
	//The exact size of 1 texture tile
	texXSize = 1;
	texYSize = 1;
}


Platform::Platform(const Platform& other) : WorldObject(other) {
	textureString = other.textureString;
	texture = other.texture;
	texX = other.texX;
	texY = other.texY;
	texXSize = other.texXSize;
	texYSize = other.texYSize;
}


Platform::~Platform() {
	//If platform is destroyed it can no longer support entities
	for (Entity* e : supporting) {
		e->setOnGround(false);
	}
}


// Sets the width of the platform
void Platform::setWidth(double width) {
	Selectable::setWidth(width);
	texX = width / texXSize;
}


// Sets the height of the platform
void Platform::setHeight(double height) {
	Selectable::setHeight(height);
	texY = height / texYSize;
}


// Gets the normal vectors needed to check collision
Vec2D* Platform::getNormals(int* numNormals) {
	*numNormals = 2;
	Vec2D* r = new Vec2D[2];
	//Calculate the angle of one of the normals
	r[0].set(cos(DEG_TO_RAD * angle), sin(DEG_TO_RAD * angle));
	//Rotate the other by 90 degrees to get the second normal
	r[1].set(-r[0].getY(), r[0].getX());
	return r;
}


// Gets the vertices of the bounding box
Vec2D* Platform::getVertices(int* numVertices) {
	*numVertices = 4;
	Vec2D* r = new Vec2D[4];
	//Get vector in 1 direction at angle
	//Transpose it to get other vector
	Vec2D w = Vec2D(cos(DEG_TO_RAD * angle), sin(DEG_TO_RAD * angle));
	Vec2D h = Vec2D(-w.getY(), w.getX());
	//Scale to represent size of bounding box
	w.multiplyBy(width * 0.5);
	h.multiplyBy(height * 0.5);
	//Use vectors to get 4 corners
	r[0] = pos.add(w).add(h);
	r[1] = pos.subtract(w).add(h);
	r[2] = pos.subtract(w).subtract(h);
	r[3] = pos.add(w).subtract(h);
	return r;
}


// Called when a collision occurs
void Platform::onCollide(Collider* other) {
}


// Draws the platform ex seconds after last update
void Platform::draw(double ex) {
	glColor3ub(255, 255, 255);
	glPushMatrix();
	glTranslated(pos.getX() + vel.getX()*ex, pos.getY() + vel.getY()*ex, 0.0);
	glRotated(angle, 0.0, 0.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(-width / 2, -height / 2);
	glTexCoord2d(0.0, texY);
	glVertex2d(-width / 2, height / 2);
	glTexCoord2d(texX, texY);
	glVertex2d(width / 2, height / 2);
	glTexCoord2d(texX, 0.0);
	glVertex2d(width / 2, -height / 2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
#ifdef DEBUG
	//Draw velocity
	glRotated(-angle, 0.0, 0.0, 1.0);
	glColor3ub(0, 0, 255);
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex2d(0.0, 0.0);
	glVertex2d(vel.getX(), vel.getY());
	glEnd();
	//Draw hitbox
	//Note: The laggy appearance of the hitbox is because the game runs at 20hz
	//and is made to appear smoother by extrapolating graphical positions when
	//the game is drawn
	glTranslated(-pos.getX() - vel.getX()*ex, -pos.getY() - vel.getY()*ex, 0.0);
	glColor3ub(255, 127, 0);
	glLineWidth(2);
	int n;
	Vec2D* vecs = getVertices(&n);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < n; i++) {
		glVertex2d(vecs[i].getX(), vecs[i].getY());
	}
	delete[] vecs;
	glEnd();
	//Draw Normals
	Vec2D* norms = getNormals(&n);
	glColor3ub(0, 255, 255);
	glBegin(GL_LINES);
	for (int i = 0; i < n; i++) {
		glVertex2d(pos.getX(), pos.getY());
		glVertex2d(pos.getX() + norms[i].getX(), pos.getY() + norms[i].getY());
	}
	delete[] norms;
	glEnd();
#endif
	glPopMatrix();
}


// Returns if the selectable can be moved
bool Platform::canMove() {
	return true;
}


// Returns if the selectable can be rotated
bool Platform::canResize() {
	return true;
}


// Returns if the selectable can be rotated
bool Platform::canRotate() {
	return true;
}


// Returns a DataObject representing the storable object
DataObject Platform::save() {
	DataObject platform = Storable::save();
	platform.add("texture", textureString);
	platform.add("texScaleX", texX);
	platform.add("texScaleY", texY);
	return platform;
}


// Loads the storable object from the DataObject
void Platform::load(DataObject obj) {
	Storable::load(obj);
	setTexture(obj.getString("texture"));
	setTexScaleX(obj.getDouble("texScaleX"));
	setTexScaleY(obj.getDouble("texScaleY"));
}


// Sets the texture of the platform
void Platform::setTexture(string tex) {
	texture = ImageLoader::getImage(tex);
	textureString = tex;
}

// Sets the scale of the texture in the x axis
void Platform::setTexScaleX(double x) {
	texX = x;
	texXSize = width / x;
}


// Sets the scale of the texture in the y axis
void Platform::setTexScaleY(double y) {
	texY = y;
	texYSize = height / y;
}


// Sets the scale of the texture in the x axis
double Platform::getTexScaleX() {
	return texY;
}


// Sets the scale of the texture in the y axis
double Platform::getTexScaleY() {
	return texY;
}


// Sets the options for this selectable
void Platform::setOptions(OptionMenu* menu) {
	Selectable::setOptions(menu);
	std::map<string, string> values = menu->getValues();
	string v;
	v = values["Texture"];
	textureString = v;
	texture = ImageLoader::getImage(textureString);
	v = values["Texture Scale X"];
	setTexScaleX(atof(v.c_str()));
	v = values["Texture Scale Y"];
	setTexScaleY(atof(v.c_str()));
}

void Platform::addSupporting(Entity* e) {
	supporting.push_back(e);
}


// Creates an option menu using the current values as defaults
void Platform::createOptions() {
	Selectable::createOptions();
	options->addOption("Texture", false, textureString);
	options->addOption("Texture Scale X", true, to_string(texX));
	options->addOption("Texture Scale Y", true, to_string(texY));
}


// Updates the platform
void Platform::update() {
	pos.addTo(vel.multiply(TICKRATE));
}


// Updates the list of supported entities
void Platform::updateSupporting() {
	auto entIt = supporting.begin();
	while (entIt != supporting.end()) {
		if (!(*entIt)->getOnGround()) {
			entIt = supporting.erase(entIt);
		} else {
			entIt++;
		}

	}
}