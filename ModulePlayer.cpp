#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, int num, bool start_enabled) : Module(app, start_enabled), num(num), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
	if (num == 1) {
		active = true;
	}
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(2, 2, 4);
	car.chassis_offset.Set(0, 1.5, 0);
	car.windows_offset.Set(0, 1.7, 0.02);
	car.mass = 1000.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car, num);
	if (num == 1) {
		vehicle->SetPos(0, 12, 45);
	}
	else if (num == 2) {
		vehicle->SetPos(0, 12, 49);
		vehicle->SetAsSensor(true);
	}
	if (num == 3) {
		vehicle->SetPos(0, 12, 53);
		vehicle->SetAsSensor(true);
	}
	else if (num == 4) {
		vehicle->SetPos(0, 12, 57);
		vehicle->SetAsSensor(true);
	}
	vehicle->collision_listeners.add(this);
	mat4x4 initial;
	vehicle->GetTransform(&initial);
	initial.rotate(-90, { 0,1,0 });
	vehicle->SetTransform(&initial);

	vehicle->last_checkpoint = App->scene_intro->checkpoints[0];

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;
	if (active) {
		if (num == 1) {
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				if (vehicle->GetKmh() >= -1) {
					acceleration = MAX_ACCELERATION;
				}
				else {
					brake = BRAKE_POWER;
				}
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				if (turn < TURN_DEGREES)
					turn += TURN_DEGREES;
			}

			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				if (turn > -TURN_DEGREES)
					turn -= TURN_DEGREES;
			}

			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				if (vehicle->GetKmh() <= 0) {
					acceleration = -MAX_ACCELERATION;
				}
				else {
					brake = BRAKE_POWER;
				}
			}
			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) {
				vehicle->Respawn();
			}
		}

		if (num == 2) {
			if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			{
				if (vehicle->GetKmh() >= -1) {
					acceleration = MAX_ACCELERATION;
				}
				else {
					brake = BRAKE_POWER;
				}
			}

			if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			{
				if (turn < TURN_DEGREES)
					turn += TURN_DEGREES;
			}

			if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			{
				if (turn > -TURN_DEGREES)
					turn -= TURN_DEGREES;
			}

			if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			{
				if (vehicle->GetKmh() <= 0) {
					acceleration = -MAX_ACCELERATION;
				}
				else {
					brake = BRAKE_POWER;
				}
			}
			if (App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_DOWN) {
				vehicle->Respawn();
			}
		}
		if (num == 3) {
			if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT)
			{
				if (vehicle->GetKmh() >= -1) {
					acceleration = MAX_ACCELERATION;
				}
				else {
					brake = BRAKE_POWER;
				}
			}

			if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
			{
				if (turn < TURN_DEGREES)
					turn += TURN_DEGREES;
			}

			if (App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
			{
				if (turn > -TURN_DEGREES)
					turn -= TURN_DEGREES;
			}

			if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
			{
				if (vehicle->GetKmh() <= 0) {
					acceleration = -MAX_ACCELERATION;
				}
				else {
					brake = BRAKE_POWER;
				}
			}
			if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN) {
				vehicle->Respawn();
			}
		}
		if (num == 4) {
			if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_REPEAT)
			{
				if (vehicle->GetKmh() >= -1) {
					acceleration = MAX_ACCELERATION;
				}
				else {
					brake = BRAKE_POWER;
				}
			}

			if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_REPEAT)
			{
				if (turn < TURN_DEGREES)
					turn += TURN_DEGREES;
			}

			if (App->input->GetKey(SDL_SCANCODE_KP_3) == KEY_REPEAT)
			{
				if (turn > -TURN_DEGREES)
					turn -= TURN_DEGREES;
			}

			if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_REPEAT)
			{
				if (vehicle->GetKmh() <= 0) {
					acceleration = -MAX_ACCELERATION;
				}
				else {
					brake = BRAKE_POWER;
				}
			}
			if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_DOWN) {
				vehicle->Respawn();
			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && num == 2 && active == false) {
		active = true;
		vehicle->SetAsSensor(false);
		vehicle->SetPos(0, 12, 49);
	}
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN && num == 3 && active == false && App->player2->active==true) {
		active = true;
		vehicle->SetAsSensor(false);
		vehicle->SetPos(0, 12, 53);
	}
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN && num == 4 && active == false && App->player3->active == true) {
		active = true;
		vehicle->SetAsSensor(false);
		vehicle->SetPos(0, 12, 57);
	}
	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);
	if (active) {
		vehicle->Render();
	}


	return UPDATE_CONTINUE;
}

void ModulePlayer::OnCollision(PhysBody3D* body1, PhysBody3D* body2) {
	
}


