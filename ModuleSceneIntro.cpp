#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"

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

	App->camera->Move(vec3(0.0f, 120.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	Cube death;
	death.size = vec3(180.0f, 0.1f, 180.0f);
	death.SetPos(0, 8, 0);
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
	secondchk.SetPos(-45, 18, 53);
	checkpoints[checkpoint_id] = App->physics->AddBody(secondchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { -45,12,48 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube thirdchk;
	thirdchk.size = vec3(1.0f, 5.0f, 14.0f);
	thirdchk.SetPos(-40, 18, 25);
	checkpoints[checkpoint_id] = App->physics->AddBody(thirdchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { -40,12,25 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube fourthchk;
	fourthchk.size = vec3(1.0f, 5.0f, 14.0f);
	fourthchk.SetPos(0, 18, -50);
	checkpoints[checkpoint_id] = App->physics->AddBody(fourthchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { 0,12,50 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube fifthchk;
	fifthchk.size = vec3(1.0f, 5.0f, 14.0f);
	fifthchk.SetPos(0, 18, 0);
	checkpoints[checkpoint_id] = App->physics->AddBody(fifthchk, 0);
	checkpoints[checkpoint_id]->SetAsCheckpoint(true, checkpoint_id, { 0,12,0 });
	checkpoints[checkpoint_id++]->collision_listeners.add(this);

	Cube floor;
	floor.size = vec3(120.0f, 10.0f, 120.0f);
	floor.SetPos(0, 10, 0);
	floor.color = Brown;

	cubes.add(floor);
	App->physics->AddBody(floor, 0);

	Cylinder cyl1;
	cyl1.SetRotation(90, { 0,0,1 });
	cyl1.radius = 1.2f;
	cyl1.height = 0.7f;
	cyl1.SetPos(-10, 18, 48);
	cylinders.add(cyl1);
	cylinderBodies.add(App->physics->AddBody(cyl1, 200.0f));

	Cylinder cyl2;
	cyl2.SetRotation(90, { 0,0,1 });
	cyl2.radius = 1.2f;
	cyl2.height = 0.7f;
	cyl2.SetPos(-10, 21, 48);
	cyl2.color = DarkRed;
	cylinders.add(cyl2);
	cylinderBodies.add(App->physics->AddBody(cyl2, 200.0f));
	


	App->physics->AddFence({ 10.0f, 7.0f, 14.0f }, -31, 14, 39, 29, { 0,0,1 });//RAMP LEFT


	App->physics->AddFence({ 10.0f, 5.0f, 14.0f }, -23, 17, 39);//RAMP CENTER


	App->physics->AddFence({ 10.0f, 7.0f, 14.0f }, -15, 14, 39, -34, { 0,0,1 });//RAMP RIGHT

	App->physics->AddFence({ 20.0f, 8.0f, 68.0f }, -50, 19, -26); // TOP LEFT 

	App->physics->AddFence({ 34.0f, 8.0f, 20.0f }, -23, 19, -50); // TOP 
	App->physics->AddFence({ 68.0f, 4.0f, 20.0f }, -10, 17, -50); // TOP SMALL
	App->physics->AddFence({ 13.0f, 8.0f, 20.0f }, 21, 19, -50); // TOP 
	App->physics->AddFence({ 14.0f, 5.0f, 20.0f }, -1.5, 17.0f, -50, 34, { 0,0,-1 });//TOP RAMP 2 DOWN

	App->physics->AddFence({ 14.0f, 5.0f, 20.0f }, 10, 17.0f, -50, 34, { 0,0,1 });//TOP RAMP 2 UP

	App->physics->AddFence({ 80.0f, 3.0f, 2.0f }, 0, 16, 45); // BOTTOM 

	App->physics->AddFence({ 2.0f, 3.0f, 30.0f }, 40, 16, 31);//BOTTOM RIGHT

	App->physics->AddFence({ 38.0f, 3.0f, 2.0f }, 41, 16, 0);//CENTER RIGHT

	App->physics->AddFence({ 2.0f, 3.0f, 30.0f }, 21, 16, 14);//CENTER RIGHT DOWN

	App->physics->AddFence({ 2.0f, 3.0f, 88.0f }, 4, 16, 0);//ACROSS MIDDLE

	App->physics->AddFence({ 50.0f, 3.0f, 2.0f }, -35, 16, 33);//RAMP FENCE

	App->physics->AddFence({ 2.0f, 3.0f, 60.0f }, -10, 16, 4);//RAMP FENCE UP

	App->physics->AddFence({ 20.0f, 10.0f, 14.0f }, -50, 15, 11, 34, { 1,0,0 });//TOP RAMP UP

	App->physics->AddFence({ 14.0f, 10.0f, 20.0f }, 31, 15, -50, 34, { 0,0,-1 });//TOP RAMP DOWN

	App->physics->AddInvisibleWall({ 28.0f, 10.0f, 2.0f }, -23, 23, 33);//BOTTOM RAMP WALL

	App->physics->AddInvisibleWall({ 68.0f, 8.0f, 0.10f }, -6, 23, -40);//TOP WALL 


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

	cubes.getFirst();
	for (p2List_item<Cube>* cubelist = cubes.getFirst(); cubelist; cubelist = cubelist->next) {
		cubelist->data.Render();
	}
	p2List_item<PhysBody3D*>* cylinderbody = cylinderBodies.getFirst();
	for (p2List_item<Cylinder>* cylinderlist = cylinders.getFirst(); cylinderlist; cylinderlist = cylinderlist->next) {
		cylinderbody->data->GetTransform(&(cylinderlist->data.transform));
		cylinderlist->data.Render();
		cylinderbody = cylinderbody->next;
	}
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	LOG("Hit!");
	if (body2->IsVehicle()) {
		if (!body1->IsCheckpoint() && body1->IsSensor()) {
			vec3 newpos = ((PhysVehicle3D*)body2)->last_checkpoint->CheckPointPos();
			body2->SetPos(newpos.x, newpos.y, newpos.z);
			LOG("CHECK");
		}
		if (body1->IsCheckpoint() && body1 != ((PhysVehicle3D*)body2)->last_checkpoint) {
			int nextid = ((PhysVehicle3D*)body2)->last_checkpoint->CheckPointId() + 1;
			if (body1->CheckPointId() == nextid)
			{
				((PhysVehicle3D*)body2)->last_checkpoint = body1;
			}
			else if (body1->CheckPointId() == 0 && nextid == CHECKPOINT_NUM) {
				((PhysVehicle3D*)body2)->last_checkpoint = body1;
			}
		}
		if (body2->IsCheckpoint()) {
			LOG("CHECK2");
		}
	}
}
