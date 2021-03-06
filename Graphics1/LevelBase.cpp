#include "LevelBase.h"
#include "Globals.h"
#include "PointGiver.h"
#include "BombEntity.h"
#include "ShieldGiver.h"
#include "Spike.h"
#include "StompableEntity.h"
#include "MovingPlatform.h"
#include "TextItem.h"
#include "Slope.h"
#include "Turret.h"
#include "BreakablePlatform.h"
#include "Lever.h"
#include "ANDGate.h"
#include "ORGate.h"
#include "XORGate.h"
#include "NANDGate.h"
#include "NORGate.h"
#include "XNORGate.h"
#include "PhasingPlatform.h"
#include "ButtonEntity.h"
#include "Crate.h"
#include "Clock.h"
#include "TimedButton.h"
#include "Light.h"
#define NUM_PANELS 8
#define PARALLAX 0.5
#define PANELS_X 9
#define PANELS_Y 6

LevelBase::LevelBase() {
	//Load background textures
	planet = ImageLoader::getImage("Resources\\planet.png");
	stars = ImageLoader::getImage("Resources\\stars.png");
	backing = ImageLoader::getImage("Resources\\backing.png");
	spawnBack = ImageLoader::getImage("Resources\\spawnBack.png");
	goalActive = ImageLoader::getImage("Resources\\goalActive.png");
	goalInactive = ImageLoader::getImage("Resources\\goalInactive.png");
	glBindTexture(GL_TEXTURE_2D, backing);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Set default values for variables
	spawn = Vec2D(0.0, 0.0);
	goal = Vec2D(1.0, 0.0);
	spawnAngle = 0;
	goalAngle = 0;
	defaultGravity = 0;
	targetTime = 0;
	goalOpen = true;
	goalChannel = -1;
}


LevelBase::~LevelBase() {
	for (Entity* e: entities) {
		delete e;
	}
	for (Platform* p: platforms) {
		delete p;
	}
	for (GravityField* f: gravFields) {
		delete f;
	}
	for (Scenery* s : scenery) {
		delete s;
	}
}


// Draws the level
void LevelBase::draw(double ex) {
	Vec2D p = getCameraAt(ex);
	double pX = p.getX();
	double pY = p.getY();
	/*
	Camera displays a box of fixed world size
	If screen is larger then box is scaled to fit
	If resolution is wrong (most likely the case) the smallest dimension is box size
	*/
	float resize = (float)WORLD_SIZE / (float)(sWidth < sHeight ? sWidth : sHeight);
	//Move the camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, sWidth * resize, 0.0, sHeight * resize, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glTranslated(0.5 * sWidth * resize, 0.5 * sHeight * resize, 0.0);
	glRotated(-getCameraAngleAt(ex), 0.0, 0.0, 1.0);
	glTranslated(-pX, -pY, 0.0);
	//Draw the background
	glPushMatrix();
	double pHeight = sqrt(sWidth * sWidth + sHeight * sHeight) * resize;
	glTranslated(pX - pHeight, pY - pHeight * 0.5, 0.0);
	glEnable(GL_TEXTURE_2D);
	//Draw the stars
	glBindTexture(GL_TEXTURE_2D, stars);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(0.0, 0.0);
	glTexCoord2d(4.0, 0.0);
	glVertex2d(pHeight * 2, 0.0);
	glTexCoord2d(4.0, 2.0);
	glVertex2d(pHeight * 2, pHeight);
	glTexCoord2d(0.0, 2.0);
	glVertex2d(0.0, pHeight);
	glEnd();
	//Draw the planet
	glBindTexture(GL_TEXTURE_2D, planet);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(0.0, 0.0);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(pHeight * 2, 0.0);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(pHeight * 2, pHeight * 0.75);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(0.0, pHeight * 0.75);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	//Draw the parallax backing
	double size = pHeight / NUM_PANELS;
	double parX = pX * PARALLAX;
	double parY = pY * PARALLAX;
	int minX = (int)((parX - pHeight * 0.5) / size);
	int minY = (int)((parY - pHeight * 0.5) / size);
	//To Prevent edges unloading too soon
	minX--;
	minY--;
	//For tiles min to max draw tile
	glPushMatrix();
	glTranslated(parX + minX * size, parY + minY * size, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, backing);
	double u, v;
	int tX = minX, tY = minY;
	//Keep panels in range of [0-PANELS_X]
	if (tX < 0) {
		tX *= -1;
		tX %= PANELS_X;
		tX = PANELS_X - tX;
	}
	//Keep panels in range of [0-PANELS_Y]
	if (tY < 0) {
		tY *= -1;
		tY %= PANELS_Y;
		tY = PANELS_Y - tY;
	}
	//For each visible panel on the screen determine texture and draw
	for (int y = 0; y < NUM_PANELS + 2; y++) {
		tY++;
		tY %= PANELS_Y;
		for (int x = 0; x < NUM_PANELS + 2; x++) {
			tX++;
			tX %= PANELS_X;
			if (tY > 4 || tX > 6) {
				u = 0.0;
				v = 0.75;
			} else {
				//Set texture coord based on position
				if (tY == 0) {
					v = 0.25;
				} else if (tY < 4) {
					v = 0.5;
				} else {
					v = 0.75;
				}
				if (tX == 0) {
					u = 0.25;
				} else if (tX < 6) {
					u = 0.5;
				} else {
					u = 0.75;
				}
			}
			//Draw individual panel
			glBegin(GL_QUADS);
			glTexCoord2d(u, v);
			glVertex2d(0.0, 0.0);
			glTexCoord2d(u + 0.25, v);
			glVertex2d(size, 0.0);
			glTexCoord2d(u + 0.25, v + 0.25);
			glVertex2d(size, size);
			glTexCoord2d(u, v + 0.25);
			glVertex2d(0.0, size);
			glEnd();
			glTranslated(size, 0.0, 0.0);
		}
		tX = minX;
		if (tX < 0) {
			tX *= -1;
			tX %= PANELS_X;
			tX = PANELS_X - tX;
		}
		glTranslated(-size * (NUM_PANELS + 2), size, 0.0);
	}
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	//Draw the gravity fields
	for (GravityField* f : gravFields) {
		f->draw(ex);
	}
	//Draw the scenery
	for (Scenery* s : scenery) {
		s->draw(ex);
	}
	//Draw the platforms
	for (Platform* p : platforms) {
		p->draw(ex);
	}
	//Draw the spawn and goal
	//Spawn
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spawnBack);
	glPushMatrix();
	glColor3ub(255, 255, 255);
	glTranslated(spawn.getX(), spawn.getY(), 0.0);
	glRotated(spawnAngle, 0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(-SPAWN_WIDTH * 0.5, -SPAWN_HEIGHT * 0.5);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(-SPAWN_WIDTH * 0.5, SPAWN_HEIGHT * 0.5);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(SPAWN_WIDTH * 0.5, SPAWN_HEIGHT * 0.5);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(SPAWN_WIDTH * 0.5, -SPAWN_HEIGHT * 0.5);
	glEnd();
	glPopMatrix();
	//Goal
	glBindTexture(GL_TEXTURE_2D, goalOpen ? goalActive : goalInactive);
	glPushMatrix();
	glTranslated(goal.getX(), goal.getY(), 0.0);
	glRotated(goalAngle, 0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(-SPAWN_WIDTH * 0.5, -SPAWN_HEIGHT * 0.5);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(-SPAWN_WIDTH * 0.5, SPAWN_HEIGHT * 0.5);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(SPAWN_WIDTH * 0.5, SPAWN_HEIGHT * 0.5);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(SPAWN_WIDTH * 0.5, -SPAWN_HEIGHT * 0.5);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	//Draw the entities
	for (Entity* e : entities) {
		e->draw(ex);
	}
	glColor3ub(0, 0, 0);
}


// Saves the level to the given file
void LevelBase::saveLevel(string filePath) {
	DataObject lvl = DataObject();
	//General level information
	lvl.add("name", levelName);
	lvl.add("nextLevel", nextLevelPath);
	lvl.add("grav", defaultGravity);
	lvl.add("spawnX", spawn.getX());
	lvl.add("spawnY", spawn.getY());
	lvl.add("spawnAngle", spawnAngle);
	lvl.add("goalX", goal.getX());
	lvl.add("goalY", goal.getY());
	lvl.add("goalAngle", goalAngle);
	lvl.add("targetTime", targetTime);
	lvl.add("goalChannel", goalChannel);
	//For each type of object create a DO listing them
	DataObject objs = DataObject();
	int i = 0;
	for (Entity* e : entities) {
		objs.add(to_string(i), e->save());
		i++;
	}
	for (GravityField* g : gravFields) {
		objs.add(to_string(i), g->save());
		i++;
	}
	for (Platform* p : platforms) {
		objs.add(to_string(i), p->save());
		i++;
	}
	for (Scenery* s : scenery) {
		objs.add(to_string(i), s->save());
		i++;
	}
	lvl.add("objects", objs);

	//Save the level to a file
	lvl.saveToFile(filePath);
}


// Loads a level from the given file
void LevelBase::loadLevel(string filePath) {
	//Load the level from file
	DataObject lvl = DataObject();
	lvl.loadFromFile(filePath);
	//Get data, attempting to save any failures
	bool exists;
	levelName = lvl.getString("name", exists);
	if (!exists) {
		printf("Error loading level: No name found\n");
		levelName = "Unknown Level";
	}
	nextLevelPath = lvl.getString("nextLevel", exists);
	if (!exists) {
		printf("Error loading next level: No name found\n");
		nextLevelPath = "";
	}
	defaultGravity = lvl.getDouble("grav", exists);
	if (!exists) {
		printf("Error loading level: No default gravity found\n");
		defaultGravity = 0;
	}
	spawn.setX(lvl.getDouble("spawnX", exists));
	if (!exists) {
		printf("Error loading level: No spawn x coordinate found\n");
		spawn.setX(0.0);
	}
	spawn.setY(lvl.getDouble("spawnY", exists));
	if (!exists) {
		printf("Error loading level: No spawn y coordinate found\n");
		spawn.setY(0.0);
	}
	spawnAngle = lvl.getDouble("spawnAngle", exists);
	if (!exists) {
		printf("Error loading level: No spawn angle found\n");
		spawnAngle = (0.0);
	}
	goal.setX(lvl.getDouble("goalX", exists));
	if (!exists) {
		printf("Error loading level: No goal x coordinate found\n");
		goal.setX(1.0);
	}
	goal.setY(lvl.getDouble("goalY", exists));
	if (!exists) {
		printf("Error loading level: No goal y coordinate found\n");
		goal.setY(0.0);
	}
	goalAngle = lvl.getDouble("goalAngle", exists);
	if (!exists) {
		printf("Error loading level: No goal angle found\n");
		goalAngle = (0.0);
	}
	targetTime = lvl.getDouble("targetTime", exists);
	if (!exists) {
		printf("Error loading level: No target time found\n");
		targetTime = 0;
	}
	goalChannel = lvl.getInt("goalChannel", exists);
	if (!exists) {
		printf("Error loading level: No goal channel found\n");
		goalChannel = -1;
	}
	//Get list of platforms/entities/gravity fields
	DataObject objs = lvl.getDataObject("objects", exists);
	if (!exists) {
		objs = DataObject();
	}
	//Wipe any existing data
	for (GravityField* f: gravFields) {
		delete f;
	}
	gravFields.clear();
	for (Platform* p: platforms) {
		delete p;
	}
	platforms.clear();
	for (Entity* e: entities) {
		delete e;
	}
	entities.clear();
	for (Scenery* s : scenery) {
		delete s;
	}
	scenery.clear();
	//Load data from DataObjects
	int i = 0;
	DataObject item = objs.getDataObject(to_string(i), exists);
	while (exists) {
		string id = item.getString("id", exists);
		//ID found, parse object
		if (exists) {
			//Go through all valid ids
			if (id == "gravfield") {
				GravityField* f = new GravityField();
				f->load(item);
				gravFields.push_back(f);
			} else if (id == "platform") {
				Platform* p = new Platform();
				p->load(item);
				platforms.push_back(p);
			} else if (id == "points") {
				PointGiver* p = new PointGiver();
				p->load(item);
				entities.push_back(p);
			} else if (id == "bomb") {
				BombEntity* b = new BombEntity();
				b->load(item);
				entities.push_back(b);
			} else if (id == "shieldgiver") {
				ShieldGiver* s = new ShieldGiver();
				s->load(item);
				entities.push_back(s);
			} else if (id == "spike") {
				Spike* s = new Spike();
				s->load(item);
				platforms.push_back(s);
			} else if (id == "stomp") {
				StompableEntity* s = new StompableEntity();
				s->load(item);
				entities.push_back(s);
			} else if (id == "movingplatform") {
				MovingPlatform* m = new MovingPlatform();
				m->load(item);
				platforms.push_back(m);
			} else if (id == "text") {
				TextItem* t = new TextItem();
				t->load(item);
				scenery.push_back(t);
			} else if (id == "slope") {
				Slope* s = new Slope();
				s->load(item);
				platforms.push_back(s);
			} else if (id == "turret") {
				Turret* t = new Turret();
				t->load(item);
				entities.push_back(t);
			} else if (id == "breakable") {
				BreakablePlatform* b = new BreakablePlatform();
				b->load(item);
				platforms.push_back(b);
			} else if (id == "lever") {
				Lever* l = new Lever();
				l->load(item);
				scenery.push_back(l);
			} else if (id == "ANDGate") {
				ANDGate* g = new ANDGate();
				g->load(item);
				scenery.push_back(g);
			} else if (id == "NANDGate") {
				NANDGate* g = new NANDGate();
				g->load(item);
				scenery.push_back(g);
			} else if (id == "ORGate") {
				ORGate* g = new ORGate();
				g->load(item);
				scenery.push_back(g);
			} else if (id == "NORGate") {
				NORGate* g = new NORGate();
				g->load(item);
				scenery.push_back(g);
			} else if (id == "XORGate") {
				XORGate* g = new XORGate();
				g->load(item);
				scenery.push_back(g);
			} else if (id == "XNORGate") {
				XNORGate* g = new XNORGate();
				g->load(item);
				scenery.push_back(g);
			} else if (id == "phase") {
				PhasingPlatform* p = new PhasingPlatform();
				p->load(item);
				platforms.push_back(p);
			} else if (id == "button") {
				ButtonEntity* b = new ButtonEntity();
				b->load(item);
				entities.push_back(b);
			} else if (id == "crate") {
				Crate* c = new Crate();
				c->load(item);
				entities.push_back(c);
			} else if (id == "clock") {
				Clock* c = new Clock();
				c->load(item);
				scenery.push_back(c);
			} else if (id == "timedbutton") {
				TimedButton* t = new TimedButton();
				t->load(item);
				scenery.push_back(t);
			} else if (id == "light") {
				Light* l = new Light();
				l->load(item);
				scenery.push_back(l);
			}
		}
		//Get next item
		i++;
		item = objs.getDataObject(to_string(i), exists);
	}
	levelPath = filePath;
}


// Gets the camera position ex seconds after last update
Vec2D LevelBase::getCameraAt(double ex) {
	return Vec2D(0.0, 0.0);
}


// Gets the angle of the camera ex seconds after last update
double LevelBase::getCameraAngleAt(double ex) {
	return 0.0;
}


// Gets the world coordinates from the screen coordinates
Vec2D LevelBase::getWorldCoordinates(Vec2D screen) {
	//Offset from the centre of the screen
	Vec2D world = Vec2D(screen.getX(), screen.getY());
	world.subtractFrom(Vec2D(sWidth * 0.5, sHeight * 0.5));
	//Scale coordinates
	double scale = WORLD_SIZE / (double)(sWidth < sHeight ? sWidth : sHeight);
	world.multiplyBy(scale);
	//Rotate coordinates
	double cTheta = cos(getCameraAngleAt(0) * DEG_TO_RAD);
	double sTheta = sin(getCameraAngleAt(0) * DEG_TO_RAD);
	double tX = world.getX();
	world.setX(world.getX() * cTheta - world.getY() * sTheta);
	world.setY(world.getY() * cTheta + tX * sTheta);
	//Offset from the world origin
	world.addTo(getCameraAt(0));
	return world;
}


// Gets the screen coordinates from the world coordinates
Vec2D LevelBase::getScreenCoordinates(Vec2D world) {
	//Offset from world origin
	Vec2D screen = world.subtract(getCameraAt(0));
	//Scale coordinates
	double scale = (double)(sWidth < sHeight ? sWidth : sHeight) / WORLD_SIZE;
	screen.multiplyBy(scale);
	//Rotate coordinates
	double cTheta = cos(-getCameraAngleAt(0) * DEG_TO_RAD);
	double sTheta = sin(-getCameraAngleAt(0) * DEG_TO_RAD);
	double tX = screen.getX();
	screen.setX(screen.getX() * cTheta - screen.getY() * sTheta);
	screen.setY(screen.getY() * cTheta + tX * sTheta);
	//Offset from the centre of the screen
	screen.addTo(Vec2D(sWidth * 0.5, sHeight * 0.5));
	return screen;
}


// Sets the position of the spawn point
void LevelBase::setSpawn(Vec2D pos) {
	spawn = pos;
}


// Gets the position of the spawn point
Vec2D LevelBase::getSpawn() {
	return spawn;
}


// Sets the position of the goal
void LevelBase::setGoal(Vec2D pos) {
	goal = pos;
}


// Gets the position of the goal
Vec2D LevelBase::getGoal() {
	return goal;
}


// Sets the angle of the spawn point
void LevelBase::setSpawnAngle(double a) {
	spawnAngle = a;
}


// Gets the angle of the spawn point
double LevelBase::getSpawnAngle() {
	return spawnAngle;
}


// Sets the angle of the goal
void LevelBase::setGoalAngle(double a) {
	goalAngle = a;
}


// Gets the angle of the goal
double LevelBase::getGoalAngle() {
	return goalAngle;
}


// Adds an entity to the level
void LevelBase::addEntity(Entity* entity) {
	entities.push_back(entity);
}


// Removes the entity from the level if it exists
void LevelBase::removeEntity(Entity* entity) {
	auto ptr = entities.begin();
	while (ptr != entities.end()) {
		if (*ptr == entity) {
			ptr = entities.erase(ptr);
			delete entity;
			break;
		} else {
			ptr++;
		}
	}
}


// Adds an entity to the level
void LevelBase::addPlatform(Platform* platform) {
	platforms.push_back(platform);
}


// Removes the entity from the level if it exists
void LevelBase::removePlatform(Platform* platform) {
	auto ptr = platforms.begin();
	while (ptr != platforms.end()) {
		if (*ptr == platform) {
			ptr = platforms.erase(ptr);
			delete platform;
			break;
		} else {
			ptr++;
		}
	}
}


// Adds a gravity field to the level
void LevelBase::addGravityField(GravityField* field) {
	gravFields.push_back(field);
}



// Removes the gravity field from the level if it exists
void LevelBase::removeGravityField(GravityField* field) {
	auto ptr = gravFields.begin();
	while (ptr != gravFields.end()) {
		if (*ptr == field) {
			ptr = gravFields.erase(ptr);
			delete field;
			break;
		} else {
			ptr++;
		}
	}
}


// Adds scenery to the level
void LevelBase::addScenery(Scenery* s) {
	scenery.push_back(s);
}



// Removes the scenery from the level if it exists
void LevelBase::removeScenery(Scenery* s) {
	auto ptr = scenery.begin();
	while (ptr != scenery.end()) {
		if (*ptr == s) {
			ptr = scenery.erase(ptr);
			delete s;
			break;
		} else {
			ptr++;
		}
	}
}
