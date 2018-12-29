#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include "ModulePlayer.h"

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

	Cube cube(4, 4, 6);
	cube.SetPos(0, 2, 20); 

	test = App->physics->AddBody(cube, 0.0f);
	test->collision_listeners.add(this);
	stagePrimitives.add(test);

	Cube ramp(4, 0.5f, 9);
	ramp.SetPos(0, 1, 14);
	ramp.SetRotation(-42, vec3(1, 0, 0));
	stagePrimitives.add(App->physics->AddBody(ramp, 0.0f));

	AddPowerUp(vec3(0, 1, 0));

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

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1 == test && App->player->GetPlayerCar() == body2)
	{
		LOG("DAMN");
	}
}

bool ModuleSceneIntro::AddPowerUp(vec3 position)
{
	PowerUp* power = new PowerUp(position, App);
	powerUps.add(power);
	return true;
}

bool ModuleSceneIntro::Draw()
{

	// Render ground -------------------------------------------------
	Cube ground(100, 2, 200);
	ground.SetPos(0, -1, 0);
	ground.Render();

	// Render all stage primitives -----------------------------------
	for (p2List_item<PhysBody3D*> *item = stagePrimitives.getFirst(); item; item = item->next)
	{
		mat4x4 t;
		item->data->GetBody()->getWorldTransform().getOpenGLMatrix(&t);
		item->data->primitive->transform = t;
		item->data->primitive->Render();
	}

	// Render all power ups ------------------------------------------
	for (p2List_item<PowerUp*> *item = powerUps.getFirst(); item; item = item->next)
	{
		item->data->Render();
	}

	return true;
}

PowerUp::PowerUp(vec3 position, Application * app): position(position)
{
	Cube sensor_cube(1.5f, 1.5f, 1.5f);
	sensor_cube.SetPos(position.x, position.y, position.z);
	sensor = app->physics->AddBody(sensor_cube, 0.0f);
	sensor->collision_listeners.add(app->scene_intro);
	sensor->SetAsSensor(true);
}

PowerUp::~PowerUp()
{
}

void PowerUp::Update()
{
}

void PowerUp::Render()
{
	Sphere sphere(0.4f);
	sphere.color = Red;
	sphere.SetPos(sensor->GetPos().x, sensor->GetPos().y, sensor->GetPos().z);
	sphere.Render();
}
