#pragma once
#include <string>
#include <map>
#include <vector>
using std::string;
using std::to_string;
using std::map;
using std::vector;
#include "FreeType.h"
#include "DataObject.h"
#include "ImageLoader.h"
#include "SoundLoader.h"
#include "KeyConfig.h"
#include "Vec2D.h"
//The time in seconds between updates
#define TICKRATE 0.05
//The games title
#define TITLE "Gravitate"
//The width of the game window on launch
#define DEFAULT_WIDTH 800
//The height of the game window on launch
#define DEFAULT_HEIGHT 600
//Constant to multiply an angle by to get it in radians
#define DEG_TO_RAD 0.01745329251
//Constant to multiply an angle by to get it in degrees
#define RAD_TO_DEG 57.2957795131
//Small value to compare floating points to zero with
#define FLOAT_ZERO 1e-5
//How many units in world size the smallest side of the screen sees
#define WORLD_SIZE 10.00
//How many degrees an entity can turn per second (visually) to match gravity
#define ENTITY_ROTATE_SPEED 180
//How thin an object is allowed to be (smaller = more likely for collision to fail)
#define SMALLEST_THICKNESS 0.10
//The default font size used
#define FONT_SIZE_LARGE 24
//The default smaller font size used
#define FONT_SIZE_SMALL 12
//The name of the font being used
#define FONT_NAME "arial.ttf"
//Useful constants
#define PI 3.14159265359
#define TAU 6.28318530718
#define RT2 1.41421356237
#define ONE_OVER_RT2 0.70710678118
//The width of the player
#define PLAYER_WIDTH 0.45
//The height of the player
#define PLAYER_HEIGHT 1.70
//How large the spawn and goal are
#define SPAWN_WIDTH 1.00
#define SPAWN_HEIGHT 2.00
//How many points are given for touching a point giver
#define SCORE_POINTGIVER 100
//How many points are given for killing an enemy
#define SCORE_KILL 50
//The value of each second on the clock when the player reaches the goal
#define TIME_MULTIPLIER 10
//Default speed cap for an entity
#define DEFAULT_ENTITY_MAXSPEED 2.00
//The width of an entity if none is provided
#define DEFAULT_ENTITY_WIDTH 0.10
//The height of an entity if none is provided
#define DEFAULT_ENTITY_HEIGHT 0.10
//The maximum distance between the player and goal to count as in
#define GOAL_DISTANCE_SQR 0.04
//The maximum angle between the player anf goal to count as in
#define GOAL_ANGLE_DIF 5
//The coefficient of friction for platforms (mu)
#define GROUND_FRICTION 0.75
//The velocity at which friction completely stops motion
#define STOP_VELOCITY 0.25
//How fast the player accelerates when pressing a direction key
#define PLAYER_ACCELERATION 0.30
//How fast the player jumps
#define PLAYER_JUMP 6.0
//The cosine of the angle between collision and vector and ground needed to count as standing on it
#define COS_GROUND_ANGLE_MIN -1
#define COS_GROUND_ANGLE_MAX -0.707106
//How many particles make up an explosion
#define EXPLOSION_NEW_PARTICLES 5000
//How long explosions emit particles for after being created
#define EXPLOSION_NEW_PARTICLES_DURATION 0.05
//Maximum number of particles per particle system
#define MAX_PARTICLES 5000
//How long before the spawn beam begins to fade
#define SPAWN_ANIM_BEGIN 0.5
//How long before the spawn doors open
#define SPAWN_ANIM_DOORS 1.0
//The time after which the spawn animation is complete
#define SPAWN_ANIM_END 1.5
//The time after which the goal animation is complete
#define GOAL_ANIM_END 1.0
//The cooldown between shooting entities releasing a projectile
#define SHOOTING_COOLDOWN 3
//The distance from which a shooting entity can see the player
#define SHOOTING_VISION 5.0
//The velocity of projectiles
#define PROJECTILE_SPEED 3.0
//The number of channels available for buttons/levers
#define MAX_CHANNELS 256
//The maximum range at which a player can interact with objects
#define INTERACTION_RANGE 1
//How much of the screen is devoted to the minimap
#define MINIMAP_SIZE 0.2
//Uncomment to enter debug mode in release builds
//#define DEBUG
#ifdef _DEBUG
#define DEBUG
#endif
extern GLFWwindow* gameWindow;
extern int sWidth;
extern int sHeight;
extern freetype::font_data fontLarge;
extern freetype::font_data fontSmall;
class BaseState;
extern BaseState* state;
extern BaseState* newState;
extern GLFWcursor* cursorNormal;
extern GLFWcursor* cursorPan;
enum class Damage {EXPLOSION, SPIKE, ENEMYCOLLISION, PROJECTILE, INSTAKILL};

//Macro function to print centred text
#define printCentre(font,x,y,text, ...)	do { \
									int l = freetype::getLength((font), (text)); \
									freetype::print((font), (float)(x) - l * 0.5f, (float)(y), (text), ##__VA_ARGS__); \
								} while(0)
//Macro function to generate a random double
#define randD(min, max) (min) + (((max) - (min)) * (((double)rand()) / (double)RAND_MAX))

#ifndef DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif // !DEBUG
