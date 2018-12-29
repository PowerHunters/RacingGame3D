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
	test->SetAsSensor(true);
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

bool ModuleSceneIntro::Draw()
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	return true;
}