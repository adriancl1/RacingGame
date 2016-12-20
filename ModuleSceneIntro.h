#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_SNAKE 2
#define CHECKPOINT_NUM 5

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

	void AddFence(vec3 size, float posx, float posy, float posz, float angle = 0, vec3 rot = { 0,0,0 });
	void AddInvisibleWall(vec3 size, int posx, int posy, int posz);

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	/*
	PhysBody3D* pb_snake[MAX_SNAKE];
	Sphere s_snake[MAX_SNAKE];

	PhysBody3D* pb_snake2[MAX_SNAKE];
	Sphere s_snake2[MAX_SNAKE];
	*/

	p2List<Cube> cubes;
	p2List<Cube> invisibleWalls;

	int checkpoint_id=0;

	Cube s;
	PhysBody3D* sensor;
	PhysBody3D* deathsensor;
	PhysBody3D* checkpoints[CHECKPOINT_NUM];
};
