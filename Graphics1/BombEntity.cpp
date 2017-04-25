#include "BombEntity.h"
#include "Level.h"
#include "Explosion.h"


#define TRIGGER_RADIUS 2.0
#define FUSE_TIME 4.0
#define BOMB_FLASHES 5
#define TICK_DECREASE 0.75

BombEntity::BombEntity() {
	id = "bomb";
	idleAnim = Animation();
	idleAnim.setSpriteSheet("Resources\\entities\\bomb.png");
	idleAnim.setSpritesheetSize(2, 1);
	idleAnim.setSize(0.5, 0.5);
	idleAnim.setRepeat(false);
	idleAnim.addFrame(0, 1);
	currentAnim = &idleAnim;
	width = 0.5;
	height = 0.5;
	explodeAnim = Animation();
	explodeAnim.setSpriteSheet("Resources\\entities\\bomb.png");
	explodeAnim.setSpritesheetSize(2, 1);
	explodeAnim.setSize(0.5, 0.5);
	explodeAnim.setRepeat(false);
	//Each frame lasts 75% as long as the last
	double time = TICK_DECREASE * (FUSE_TIME / 2.0);
	maxTick = time * 2;
	for (int i = 0; i < BOMB_FLASHES; i++) {
		explodeAnim.addFrame(0, time);
		explodeAnim.addFrame(1, time);
		time *= 0.5;
	}
	triggered = false;
	fuse = FUSE_TIME;
	tickSound = SoundLoader::getSound("Resources\\sounds\\tick.wav");
	tickTime = 0;
}


BombEntity::~BombEntity() {
	alDeleteSources(1, &tickSound);
}


//Updates the entity
void BombEntity::update() {
	Level* l = (Level*)state;
	Entity* player = l->getPlayer();
	//Explode if in range
	if (!triggered && player != NULL) {
		//Check distance to player
		double d = player->getPos().subtract(pos).magnitudeSquare();
		if (d < TRIGGER_RADIUS * TRIGGER_RADIUS) {
			triggered = true;
			currentAnim = &explodeAnim;
			explodeAnim.setTime(0);
		}
	} else if (triggered) {
		fuse -= TICKRATE;
		tickTime -= TICKRATE;
		if (fuse <= 0) {
			explode();
		} else if (tickTime <= 0) {
			alSourcePlay(tickSound);
			maxTick *= TICK_DECREASE;
			tickTime = maxTick;
		}
	}
	EntityAI::update();
}


void BombEntity::explode() {
	Level* l = (Level*)state;
	l->safeDelete(this);
	Explosion* exp = new Explosion();
	exp->setRadius(2);
	exp->setPos(pos);
	exp->setMaxAge(1);
	l->safeAdd(exp);
	tickTime = maxTick;
}


// Called when damage is inflicted on the object
void BombEntity::onDamage(Damage d) {
	explode();
}
