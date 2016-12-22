#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define CHECKPOINT_NUM 7

struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void CreateStage();
	void AddTires(int posx, int posy, int posz);

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void RestartStage();

public:
	Plane sky;
	Cube line;

	p2List<Cube> cubes;
	p2List<Cube> invisibleWalls;
	p2List<Cylinder> cylinders;
	p2List<PhysBody3D*> cylinderBodies;
	Sphere ball;
	PhysBody3D* ballbody;
	Cube stick;
	PhysBody3D* stickbody;
	Cube roof;
	PhysBody3D* roofbody;

	Sphere ball2;
	PhysBody3D* ballbody2;
	Cube stick2;
	PhysBody3D* stickbody2;
	Cube roof2;
	PhysBody3D* roofbody2;

	const char* themes[6];
	int music_time;
	int current_time;
	int ball_time;
	int ballforce=25;
	int ball2force = -25;
	char* winner;

	uint lap;
	uint victory;
	uint respawn;
	uint start;
	int checkpoint_id=0;

	PhysBody3D* sensor;
	PhysBody3D* deathsensor;
	PhysBody3D* checkpoints[CHECKPOINT_NUM];
};
