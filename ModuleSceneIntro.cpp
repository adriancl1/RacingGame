#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include "ModulePhysics3D.h"
#include <time.h>

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(0.0f, 60.0f, 120.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	themes[0] = "audio/super_bus_racing_theme_1.ogg";
	themes[1] = "audio/super_bus_racing_theme_2.ogg";
	themes[2] = "audio/super_bus_racing_theme_3.ogg";
	themes[3] = "audio/super_bus_racing_theme_1_base.ogg";
	themes[4] = "audio/super_bus_racing_theme_2_base.ogg";
	themes[5] = "audio/super_bus_racing_theme_3_base.ogg";

	lap = App->audio->LoadFx("audio/LAP.wav");
	start = App->audio->LoadFx("audio/super_bus_racing_name.ogg");
	respawn = App->audio->LoadFx("audio/RESPAWN.wav");
	victory = App->audio->LoadFx("audio/FINISH.wav");


	winner = "";
	App->audio->PlayMusic(themes[0]);
	music_time = SDL_GetTicks() + 9000;
	ball_time = SDL_GetTicks() + 3000;
	ballforce = 25;

	srand(time(NULL));

	CreateStage();
	App->audio->PlayFx(start);
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	current_time = SDL_GetTicks();
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		RestartStage();
	}
	//PLAY A RANDOM THEME EVERY 9 SECONDS 
	if (current_time > music_time) {
		int random_theme = rand() % 5;
		App->audio->PlayMusic(themes[random_theme], 0);
		music_time = SDL_GetTicks() + 9000;
	}

	for (p2List_item<Cube>* cubelist = cubes.getFirst(); cubelist; cubelist = cubelist->next) {
		cubelist->data.Render();
	}
	p2List_item<PhysBody3D*>* cylinderbody = cylinderBodies.getFirst();
	for (p2List_item<Cylinder>* cylinderlist = cylinders.getFirst(); cylinderlist; cylinderlist = cylinderlist->next) {
		cylinderbody->data->GetTransform(&(cylinderlist->data.transform));
		cylinderlist->data.Render();
		cylinderbody = cylinderbody->next;
	}
	if (current_time > ball_time) {
		ball_time = SDL_GetTicks() + 3000;
		ballforce = -ballforce;
		ball2force = -ball2force;
		ballbody->GetRigidBody()->setLinearVelocity(btVector3(0, 0, ballforce));
		ballbody2->GetRigidBody()->setLinearVelocity(btVector3(0, 0, ball2force));
	}


	ballbody->GetTransform(&(ball.transform));
	ball.Render();
	stickbody->GetTransform(&(stick.transform));
	stick.Render();
	roof.Render();

	ballbody2->GetTransform(&(ball2.transform));
	ball2.Render();
	stickbody2->GetTransform(&(stick2.transform));
	stick2.Render();
	roof2.Render();

	sky.Render();


	char title[80];
	if (App->player2->active == true && App->player3->active == true && App->player4->active == true) {
		sprintf_s(title, "P1 Laps: %i P2 Laps: %i P3 Laps: %i P4 Laps: %i Last Winner: %s", App->player1->vehicle->laps, App->player2->vehicle->laps, App->player3->vehicle->laps, App->player4->vehicle->laps, winner);
	}
	else if (App->player2->active == true && App->player3->active == true ) {
		sprintf_s(title, "P1 Laps: %i P2 Laps: %i P3 Laps: %i Last Winner: %s", App->player1->vehicle->laps, App->player2->vehicle->laps, App->player3->vehicle->laps, winner);
	}
	else if (App->player2->active == true) {
		sprintf_s(title, "P1 Laps: %i P2 Laps: %i Last Winner: %s", App->player1->vehicle->laps, App->player2->vehicle->laps, winner);
	}
	else {
		sprintf_s(title, "P1 Laps: %i Last Winner: %s", App->player1->vehicle->laps, winner);
	}
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	LOG("Hit!");
	if (body2->IsVehicle()) {
		if (!body1->IsCheckpoint() && body1->IsSensor()) {
			((PhysVehicle3D*)body2)->Respawn();
			App->audio->PlayFx(respawn);
			LOG("CHECK");
		}
		if (body1->IsCheckpoint() && body1 != ((PhysVehicle3D*)body2)->last_checkpoint) {
			int nextid = ((PhysVehicle3D*)body2)->last_checkpoint->CheckPointId() + 1;
			if (body1->CheckPointId() == nextid)
			{
				((PhysVehicle3D*)body2)->last_checkpoint = body1;
			}
			else if (body1->CheckPointId() == 0 && nextid == CHECKPOINT_NUM && ((PhysVehicle3D*)body2)->laps<1) {
				((PhysVehicle3D*)body2)->last_checkpoint = body1;
				((PhysVehicle3D*)body2)->laps++;
				App->audio->PlayFx(lap);
			}
			else if (body1->CheckPointId() == 0 && nextid == CHECKPOINT_NUM && ((PhysVehicle3D*)body2)->laps == 1) {
				((PhysVehicle3D*)body2)->laps++;
				if (((PhysVehicle3D*)body2)->playernum == 1) {
					winner = "P1";
					sky.color = Red;
				}
				else if (((PhysVehicle3D*)body2)->playernum == 2) {
					winner = "P2";
					sky.color = Blue;
				}
				else if (((PhysVehicle3D*)body2)->playernum == 3) {
					winner = "P3";
					sky.color = Green;
				}
				else if (((PhysVehicle3D*)body2)->playernum == 4) {
					winner = "P4";
					sky.color = Yellow;
				}
				App->audio->PlayFx(victory);
				RestartStage();
			}
		}
		if (body2->IsCheckpoint()) {
			LOG("CHECK2");
		}
	}
}

void ModuleSceneIntro::CreateStage() {

	sky.color = Green;
	sky.normal = { 0,0,1 };
	sky.SetPos(0, 0, -100);
	sky.SetRotation(90, { 1,0,0 });


	Cube death;
	death.size = vec3(1000.0f, 0.1f, 1000.0f);
	death.SetPos(0, 8, 0);
	death.color = Red;
	cubes.add(death);
	deathsensor = App->physics->AddBody(death, 0);
	deathsensor->SetAsSensor(true);
	deathsensor->collision_listeners.add(this);

	Cube firstchk;
	firstchk.size = vec3(1.0f, 5.0f, 14.0f);
	firstchk.SetPos(0, 18, 53);
	checkpoints[checkpoint_id] = App->physics->AddBody(firstchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { 0,12,48 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube secondchk;
	secondchk.size = vec3(1.0f, 5.0f, 14.0f);
	secondchk.SetPos(-40, 18, 53);
	checkpoints[checkpoint_id] = App->physics->AddBody(secondchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { -45,12,48 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube thirdchk;
	thirdchk.size = vec3(1.0f, 5.0f, 17.0f);
	thirdchk.SetPos(-10, 18, -31);
	checkpoints[checkpoint_id] = App->physics->AddBody(thirdchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { -40,12,25 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube fourthchk;
	fourthchk.size = vec3(19.0f, 5.0f, 1.0f);
	fourthchk.SetPos(-51, 25, -15);
	checkpoints[checkpoint_id] = App->physics->AddBody(fourthchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { -40,12,25 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube fifthchk;
	fifthchk.size = vec3(1.0f, 5.0f, 19.0f);
	fifthchk.SetPos(23, 25, -50);
	checkpoints[checkpoint_id] = App->physics->AddBody(fifthchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { 0,12,0 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube sixthchk;
	sixthchk.size = vec3(19.0f, 5.0f, 1.0f);
	sixthchk.SetPos(30, 18, 20);
	checkpoints[checkpoint_id] = App->physics->AddBody(sixthchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { -40,12,25 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube seventhchk;
	seventhchk.size = vec3(18.0f, 5.0f, 1.0f);
	seventhchk.SetPos(50, 18, 40);
	checkpoints[checkpoint_id] = App->physics->AddBody(seventhchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { 0,12,0 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube floor;
	floor.size = vec3(120.0f, 10.0f, 120.0f);
	floor.SetPos(0, 10, 0);
	floor.color = Brown;

	cubes.add(floor);
	App->physics->AddBody(floor, 0);

	//UPPER TIRES ----
	AddTires(-46, 20, -54);
	AddTires(-54, 20, -45);
	AddTires(-45, 20, -15);
	AddTires(-54, 20, -24);
	AddTires(-46, 20, -35);

	//DOWN TIRES ----
	AddTires(-37, 10, -8);
	AddTires(-35, 10, -8);
	AddTires(-33, 10, -8);
	AddTires(-31, 10, -8);
	AddTires(-29, 10, -8);

	//FINAL TIRES ----
	AddTires(40, 10, 10);
	AddTires(21, 10, 24);
	AddTires(21, 10, -28);
	AddTires(37, 10, -20);
				
	Cube water;
	water.size = { 20.0f, 2.0f, 19.0f };
	water.color = Azure;
	water.SetPos(5, 20, -50);

	cubes.add(water);
	PhysBody3D* waterbody=App->physics->AddBody(water, 0);
	waterbody->GetRigidBody()->setCollisionFlags(4);

	App->physics->AddFence({ 10.0f, 7.0f, 17.0f }, -31, 14, 35, 29, { 0,0,1 });//RAMP LEFT

	App->physics->AddFence({ 10.0f, 5.0f, 17.0f }, -23, 17, 35);//RAMP CENTER

	App->physics->AddFence({ 10.0f, 7.0f, 17.0f }, -15, 14, 35, -29, { 0,0,1 });//RAMP RIGHT

	App->physics->AddFence({ 80.0f, 3.0f, 2.0f }, 0, 16, 43); // BOTTOM 

	App->physics->AddFence({ 2.0f, 3.0f, 32.0f }, 40, 16, 28);//BOTTOM RIGHT

	App->physics->AddFence({ 38.0f, 3.0f, 2.0f }, 41, 16, -7);//CENTER RIGHT

	App->physics->AddFence({ 2.0f, 3.0f, 30.0f }, 21, 16, 7);//CENTER RIGHT DOWN

	App->physics->AddFence({ 2.0f, 3.0f, 90.0f }, 4, 16, -2);//ACROSS MIDDLE

	App->physics->AddFence({ 50.0f, 3.0f, 2.0f }, -35, 16, 26);//RAMP FENCE

	App->physics->AddFence({ 2.0f, 3.0f, 50.0f }, -10, 16, 2);//RAMP FENCE UP

	App->physics->AddFence({ 2.0f, 3.0f, 18.0f }, 37, 16, -31);//JUST AFTER TOP RAMP DOWN

	App->physics->AddFence({ 2.0f, 3.0f, 18.0f }, 21, 16, -17);//JUST AFTER TOP RAMP DOWN

	//App->physics->AddFence({ 9.0f, 2.0f, 2.0f }, 0, 15, 0, 45, { 1,0,0 });//SMALL RAMP

	App->physics->AddFence({ 15.0f, 1.3f, 1.3f }, -3, 15, -7, 45, { 1,0,0 });//SMALL RAMP
	App->physics->AddFence({ 15.0f, 1.3f, 1.3f }, -3, 15, -2, 45, { 1,0,0 });//TSMALL RAMP
	App->physics->AddFence({ 15.0f, 1.3f, 1.3f }, -3, 15, 3, 45, { 1,0,0 });//SMALL RAMP
	App->physics->AddFence({ 15.0f, 1.3f, 1.3f }, -3, 15, 8, 45, { 1,0,0 });//SMALL RAMP

	//TOP ----------
	App->physics->AddFence({ 20.0f, 8.0f, 50.0f }, -50, 19, -35); // TOP LEFT 

	App->physics->AddFence({ 34.0f, 8.0f, 20.0f }, -23, 19, -50); // TOP 
	App->physics->AddFence({ 68.0f, 4.0f, 20.0f }, -10, 17, -50); // TOP SMALL
	App->physics->AddFence({ 13.0f, 8.0f, 20.0f }, 21, 19, -50); // TOP 
	App->physics->AddFence({ 14.0f, 5.0f, 20.0f }, -1.5, 17.0f, -50, 34, { 0,0,-1 });//TOP RAMP 2 DOWN

	App->physics->AddFence({ 14.0f, 5.0f, 20.0f }, 10, 17.0f, -50, 34, { 0,0,1 });//TOP RAMP 2 UP

	App->physics->AddFence({ 20.0f, 10.0f, 14.0f }, -50, 15, -7, 34, { 1,0,0 });//TOP RAMP UP

	App->physics->AddFence({ 14.0f, 10.0f, 20.0f }, 31, 15, -50, 34, { 0,0,-1 });//TOP RAMP DOWN
	//-------------


	App->physics->AddInvisibleWall({ 28.0f, 10.0f, 2.0f }, -23, 23, 26);//BOTTOM RAMP WALL

	App->physics->AddInvisibleWall({ 40.0f, 8.0f, 0.10f }, 15, 23, -40);//TOP WALL 

	App->physics->AddInvisibleWall({ 2.0f, 8.0f, 82.0F }, 4, 20, 2);//MIDDLE WALL */

	//ball
	ball.radius = 3;
	ball.SetPos(-30, 20, -50);
	ball.color = Gray;
	ballbody = App->physics->AddBody(ball, 25000);
	ballbody->SetFriction(0);
	ballbody->GetRigidBody()->setLinearVelocity(btVector3(0, 0, ballforce));

	stick.size = { 0.1f,20.0f,0.1f };
	stick.color = Gray;
	stick.SetPos(-30, 30, -50);
	stick.SetRotation(45, { 0,1,0 });
	stickbody = App->physics->AddBody(stick,25000);
	
	stickbody->SetFriction(0);

	roof.size = { 5.0f,1.0f,5.0f };
	roof.color = Gray;
	roof.SetPos(-30, 50, -50);
	roofbody = App->physics->AddBody(roof, 0);

	App->physics->AddConstraintHinge(*roofbody, *stickbody, { 0,-1.0f,0 }, { 0,10,0 }, { 1,0,0 }, { 1,0,0 });
	App->physics->AddConstraintHinge(*stickbody, *ballbody, { 0,-10,0 }, { 0,3,0 }, { 0,1,0 }, {0,1,0 });

	//BALL 2
	ball2.radius = 3;
	ball2.SetPos(-15, 20, -50);
	ball2.color = Gray;
	ballbody2 = App->physics->AddBody(ball2, 25000);
	ballbody2->SetFriction(0);
	ballbody2->GetRigidBody()->setLinearVelocity(btVector3(0, 0, ball2force));

	stick2.size = { 0.1f,20.0f,0.1f };
	stick2.color = Gray;
	stick2.SetPos(-15, 30, -50);
	stick2.SetRotation(45, { 0,1,0 });
	stickbody2 = App->physics->AddBody(stick2, 25000);

	stickbody2->SetFriction(0);

	roof2.size = { 5.0f,1.0f,5.0f };
	roof2.color = Gray;
	roof2.SetPos(-15, 50, -50);
	roofbody2 = App->physics->AddBody(roof2, 0);

	App->physics->AddConstraintHinge(*roofbody2, *stickbody2, { 0,-1.0f,0 }, { 0,10,0 }, { 1,0,0 }, { 1,0,0 });
	App->physics->AddConstraintHinge(*stickbody2, *ballbody2, { 0,-10,0 }, { 0,3,0 }, { 0,1,0 }, { 0,1,0 });
}

void ModuleSceneIntro::AddTires(int posx, int posy, int posz) {
	Cylinder cyl1;
	cyl1.SetRotation(90, { 0,0,1 });
	cyl1.radius = 1.2f;
	cyl1.height = 0.7f;
	cyl1.SetPos(posx, posy, posz);
	cylinders.add(cyl1);
	cylinderBodies.add(App->physics->AddBody(cyl1, 200.0f));

	Cylinder cyl2;
	cyl2.SetRotation(90, { 0,0,1 });
	cyl2.radius = 1.2f;
	cyl2.height = 0.7f;
	cyl2.SetPos(posx, posy+3, posz);
	cyl2.color = DarkRed;
	cylinders.add(cyl2);
	cylinderBodies.add(App->physics->AddBody(cyl2, 200.0f));

	Cylinder cyl3;
	cyl3.SetRotation(90, { 0,0,1 });
	cyl3.radius = 1.2f;
	cyl3.height = 0.7f;
	cyl3.SetPos(posx, posy+6, posz);
	cylinders.add(cyl3);
	cylinderBodies.add(App->physics->AddBody(cyl3, 200.0f));
}

void ModuleSceneIntro::RestartStage() {
	App->player1->vehicle->last_checkpoint = checkpoints[0];
	App->player1->vehicle->laps = 0;
	App->player1->vehicle->Respawn();
	App->player2->vehicle->last_checkpoint = checkpoints[0];
	App->player2->vehicle->laps = 0;
	App->player2->vehicle->Respawn();
	App->player3->vehicle->last_checkpoint = checkpoints[0];
	App->player3->vehicle->laps = 0;
	App->player3->vehicle->Respawn();
	App->player4->vehicle->last_checkpoint = checkpoints[0];
	App->player4->vehicle->laps = 0;
	App->player4->vehicle->Respawn();

	App->player1->vehicle->SetPos(0, 12, 45);
	App->player2->vehicle->SetPos(0, 12, 49);
	
	App->player3->vehicle->SetPos(0, 12, 53);
	App->player4->vehicle->SetPos(0, 12, 57);

	int newstart = SDL_GetTicks() + 2000;

	App->player1->start_time = newstart;
	App->player2->start_time = newstart;
	App->player3->start_time = newstart;
	App->player4->start_time = newstart;

}