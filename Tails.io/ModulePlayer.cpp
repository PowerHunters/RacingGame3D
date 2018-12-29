#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include "Timer.h"
#include "ModuleSceneIntro.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), playerCar(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	VehicleInfo car;

	// Car properties ---------------------------------------
	car.chassis_size.Set(1.4F, 0.5f, 3);
	car.chassis_offset.Set(0, 1, 0);
	car.mass = 300.0f;
	car.suspensionStiffness = 80.0F;
	car.suspensionCompression = 0.2f;
	car.suspensionDamping = 2.5F;
	car.maxSuspensionTravelCm = 500.0f;
	car.frictionSlip = 1000.0f;
	car.maxSuspensionForce = 4000.0f;
	// Wheel properties ---------------------------------------
	float connection_height = 0.9f;
	float front_wheel_radius = 0.4f;
	float rear_wheel_radius = 0.5f;

	float wheel_width = 0.3f;
	float suspensionRestLength = 0.25f;

	// Don't change anything below this line ------------------

	float half_width = (car.chassis_size.x + 0.6F)*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - front_wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = front_wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - front_wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = front_wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + rear_wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = rear_wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + rear_wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = rear_wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	playerCar = App->physics->AddVehicle(car);
	playerCar->SetPos(0, 1, 10);
	
	// Test tail ---------------------------
	
	PhysBody3D * tailBall = nullptr;
	PhysBody3D * iterator = nullptr;

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
	// Update Car -------------------------------------------

	turn = acceleration = brake = 0.0f;

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION;
	}

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if(turn < TURN_DEGREES)
			turn +=  TURN_DEGREES;
	}

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if(turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		acceleration = - MAX_ACCELERATION;
	}


	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		AddMissile();
	}

	playerCar->ApplyEngineForce(acceleration);
	playerCar->Turn(turn);
	playerCar->Brake(brake);

	// Update Missiles  -------------------------------------------

	p2List_item<Missile*> * item = missiles.getFirst() ;

	while (item != nullptr)
	{
		item->data->Update();

		if (item->data->toDelete == true)
		{
			p2List_item<Missile*> *iterator = item->next;
			App->physics->DeleteBody(item->data->physBody);
			delete item->data;
			missiles.del(item);
			item = iterator;
		}
		else
		{
			item = item->next;
		}
	}

	return UPDATE_CONTINUE;
}

void ModulePlayer::OnCollision(PhysBody3D * body1, PhysBody3D * body2)
{
	if (App->scene_intro->test == body2)
	{
		for (p2List_item<Missile*> * item = missiles.getFirst(); item; item = item->next)
		{
			if (item->data->physBody == body1)
			{
				LOG("DAMN");
			}
		}
	}
}

void ModulePlayer::AddMissile()
{
	Missile* missile = new Missile(this);
	missiles.add(missile);
}



PhysVehicle3D * ModulePlayer::GetPlayerCar()
{
	return playerCar;
}


Missile::Missile(ModulePlayer* owner)
{
	this->owner = owner;

	timerToDelete.Start();

	Sphere s(1);
	btVector3 offset;
	btQuaternion q = owner->GetPlayerCar()->vehicle->getChassisWorldTransform().getRotation();

	offset.setValue(0, 0.5f, 6);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	// Set init position offset respect the car ---------------------
	vec3 missileInitPos = owner->GetPlayerCar()->GetPos() + vec3(offset.getX(), offset.getY(), offset.getZ());
	s.SetPos(missileInitPos.x, missileInitPos.y, missileInitPos.z);

	// Use car Z vector as reference --------------------------------
	float force = 60.0F;
	btVector3 Z(0, 0, 1);

	Z = Z.rotate(q.getAxis(), q.getAngle());
	Z *= force;

	// Create a missile ---------------------------------------------
	physBody = owner->App->physics->AddBody(s);
	physBody->collision_listeners.add(owner);
	physBody->Push(Z.getX(), Z.getY(), Z.getZ());
}

void Missile::Update()
{
	if (timerToDelete.Read() > TIME_TO_DELETE_MISSILE)
	{
		toDelete = true;
	}
}

bool ModulePlayer::Draw()
{
	// Draw missiles ================================================
	for (p2List_item<Missile*> * item = missiles.getFirst(); item; item = item->next)
	{
		vec3 vec = item->data->physBody->GetPos();
		Sphere s(1);
		s.SetPos(vec.x, vec.y, vec.z);
		s.Render();
	}
	
	// Draw car ====================================================
	playerCar->Render();

	// Rotation info -----------------------------

	btQuaternion q = playerCar->vehicle->getChassisWorldTransform().getRotation();
	mat4x4 t;
	btVector3 offset;

	playerCar->vehicle->getChassisWorldTransform().getOpenGLMatrix(&t);
	offset.setValue(0, 0.7f, 0);

	t[12] += offset.getX();
	t[13] += offset.getY();
	t[14] += offset.getZ();


	// Main chasis -------------------------------
	Cube chassis_1(1.5f, 0.6, 1);
	chassis_1.transform = t;

	offset.setValue(0, 0.2f, -0.9f);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis_1.transform.M[12] += offset.getX();
	chassis_1.transform.M[13] += offset.getY();
	chassis_1.transform.M[14] += offset.getZ();

	Cube chassis_2(1.1f, 0.6, 1);
	chassis_2.transform = t;

	offset.setValue(0, 0.2f, 0.0f);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis_2.transform.M[12] += offset.getX();
	chassis_2.transform.M[13] += offset.getY();
	chassis_2.transform.M[14] += offset.getZ();

	Cube chassis_3(0.8, 0.4, 1.3f);
	chassis_3.transform = t;

	offset.setValue(0, 0.1f, 1.1f);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis_3.transform.M[12] += offset.getX();
	chassis_3.transform.M[13] += offset.getY();
	chassis_3.transform.M[14] += offset.getZ();

	// Front chasis ------------------------------
	Cube front_chasis(2.3f, 0.15f, 0.5f);
	front_chasis.transform = t;

	offset.setValue(0.0f, 0.1f, 2.0f);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	front_chasis.transform.M[12] += offset.getX();
	front_chasis.transform.M[13] += offset.getY();
	front_chasis.transform.M[14] += offset.getZ();

	// Rear chasis ------------------------------
	Cube rear_chasis(2.1f, 0.15f, 0.5f);
	rear_chasis.transform = t;

	offset.setValue(0.0f, 0.8f, -1.5f);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	rear_chasis.transform.M[12] += offset.getX();
	rear_chasis.transform.M[13] += offset.getY();
	rear_chasis.transform.M[14] += offset.getZ();

	// Sticks  ---------------------------------
	Cube rear_stick_1(0.1, 0.2, 0.1);
	rear_stick_1.transform = t;

	offset.setValue(-0.3f, 0.6f, -1.3f);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	rear_stick_1.transform.M[12] += offset.getX();
	rear_stick_1.transform.M[13] += offset.getY();
	rear_stick_1.transform.M[14] += offset.getZ();


	Cube rear_stick_2(0.1, 0.2, 0.1);
	rear_stick_2.transform = t;

	offset.setValue(0.3f, 0.6f, -1.3f);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	rear_stick_2.transform.M[12] += offset.getX();
	rear_stick_2.transform.M[13] += offset.getY();
	rear_stick_2.transform.M[14] += offset.getZ();

	// Pilot  ---------------------------------
	Sphere pilot;
	pilot.radius = 0.2f;
	pilot.transform = t;

	offset.setValue(0.0f, 0.65f, -0.4f);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	pilot.transform.M[12] += offset.getX();
	pilot.transform.M[13] += offset.getY();
	pilot.transform.M[14] += offset.getZ();

	// Render -----------------------------------
	front_chasis.Render();
	rear_chasis.Render();
	rear_stick_1.Render();
	rear_stick_2.Render();
	chassis_1.Render();
	chassis_2.Render();
	chassis_3.Render();
	pilot.Render();

	return true;
}