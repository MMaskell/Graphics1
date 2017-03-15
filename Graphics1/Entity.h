#pragma once
#include "Vec2D.h"
#include "Collider.h"
#include "Selectable.h"

using namespace std;

class Entity:
	public Collider,
	public Selectable {
public:
	Entity();
	~Entity();
protected:
	Vec2D pos;
	Vec2D vel;
	double angle;
	double visAngle;
	double width;
	double height;
	bool onGround;
	double maxSpeed;
	bool flip = false;
	GLuint idle;
public:
	// Gets the x-coordinate of the entity
	double getX();
	// Gets the y-coordinate of the entity
	double getY();
	// Sets the x-coordinate of the entity
	void setX(double x);
	// Sets the y-coordinate of the entity
	void setY(double y);
	// Gets the horizontal velocity of the entity
	double getVelX();
	// Gets the vertical velocity of the entity
	double getVelY();
	// Sets the horizontal velocity of the entity
	void setVelX(double x);
	// Sets the vertical velocity of the entity
	void setVelY(double x);
	// Updates the entity
	virtual void update();
	// Draws the entity
	virtual void draw(double ex);
	// Adds the given value to the horizontal velocity
	void addVelX(double x);
	// Adds the given vaue to the vertical velocity
	void addVelY(double y);
	// Adds the value to the horizontal position
	void addPosX(double x);
	// Adds the value to the vertical position
	void addPosY(double y);
	// Sets the entity's max speed
	void setMaxSpeed(double speed);
	// Gets the entity's max speed
	double getMaxSpeed();
	// Gets the horizontal (relative to the rotation) velocity
	double getVelRelX(double theta);
	// Gets the vertical (relative to the rotation) velocity
	double getVelRelY(double theta);
	// Gets the angle of the entity
	double getAngle();
	// Sets the angle of the entity
	void setAngle(double angle);
	// Gets the normal vectors needed to check collision
	virtual Vec2D* getNormals(int* numNormals);
	// Gets the vertices of the bounding box
	virtual Vec2D* getVertices(int* numVertices);
	// Called when a collision occurs
	virtual void onCollide(Collider* other);
	// Sets the width of the entity
	void setWidth(double width);
	// Gets the width of the entity
	double getWidth();
	// Sets the height of the entity
	void setHeight(double height);
	// Gets the height of the entity
	double getHeight();
	// Gets the current position of the entity
	Vec2D getPos();
	// Sets the position of the entity
	void setPos(Vec2D pos);
	// Sets whether the entity is currently on the ground
	void setOnGround(bool onGround);
	// Gets if the entity is currently on the ground
	bool getOnGround();
	// Gets the angle the entity seen at
	double getVisAngle();
	// Sets the angle the entity is seen at
	void setVisAngle(double angle);
	// Gets the updated visible angle for ex seconds of motion
	double updatedVisAngle(double ex);
	// Gets the current velocity of the entity
	Vec2D getVel();
	// Called when the selectable is moved
	virtual bool onMove(double dX, double dY);
	// Called when the selectable is rotated
	virtual bool onRotate(double dAngle);
	// Returns if the selectable can be moved
	virtual bool canMove();
	// Returns if the selectable can be rotated
	virtual bool canResize();
	// Returns if the selectable can be rotated
	virtual bool canRotate();
	// Returns if the selectable is selected
	virtual bool isInBoundingBox(double x, double y);
	// Returns a DataObject representing the storable object
	virtual DataObject save();
	// Loads the storable object from the DataObject
	virtual void load(DataObject obj);
};

