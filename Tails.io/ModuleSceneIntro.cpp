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

	// Stage Primitives ------------------------------ !!	Add all to stagePrimitives List
	Cube cube(4, 4, 6);
	cube.SetPos(0, 2, 20); 

	test = App->physics->AddBody(cube, 0.0f);
	test->collision_listeners.add(this);
	stagePrimitives.add(test);

	Cube ramp(4, 0.5f, 9);
	ramp.SetPos(0, 1, 14);
	ramp.SetRotation(-42, vec3(1, 0, 0));
	stagePrimitives.add(App->physics->AddBody(ramp, 0.0f));

	// Power Up Spawners Primitives ------------------------------ 

	AddPowerUpSpawner(vec3(0, 1.5f, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	// Delete all stage primitives -----------------------------------
	for (p2List_item<PhysBody3D*> *item = stagePrimitives.getFirst(); item; item = item->next)
	{
		App->physics->DeleteBody(item->data);
	}

	stagePrimitives.clear();


	// Delete all power up spawners ------------------------------------------
	for (p2List_item<PowerUpSpawner*> *item = stageSpawners.getFirst(); item; item = item->next)
	{
		delete item->data;
	}

	stageSpawners.clear();

	// Delete all power ups ------------------------------------------
	for (p2List_item<PowerUp*> *item = powerUps.getFirst(); item; item = item->next)
	{
		App->physics->DeleteBody(item->data->sensor);
		delete item->data;
	}

	powerUps.clear();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{

	p2List_item<PowerUp*> *item = powerUps.getFirst();
	p2List_item<PowerUp*> *iterator = nullptr;

	// Update Power Up Spawners ---------------------------------

	for (p2List_item<PowerUpSpawner*> *item = stageSpawners.getFirst(); item; item = item->next)
	{
		item->data->Update();
	}

	// Update Power Ups -----------------------------------------
	while (item != nullptr)
	{
		if (item->data->toDelete == true)
		{
			iterator = item->next;
			App->physics->DeleteBody(item->data->sensor);
			delete item->data;
			powerUps.del(item);
			item = iterator;
		}
		else
		{
			item = item->next;
		}
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	PhysVehicle3D* player_1 = App->player_1->GetPlayerCar();
	PhysVehicle3D* player_2 = App->player_2->GetPlayerCar();

	if (player_1 == body2 || player_2 == body2)
	{
		PowerUp* powerUp = nullptr;

		for (p2List_item<PowerUp*>* item = powerUps.getFirst(); item; item = item->next)
		{
			if (item->data->sensor == body1)
			{
				powerUp = item->data;
			}
		}

		if (powerUp == nullptr)
		{
			return;
		}

		powerUp->spawner->StartRespawnTime();
		powerUp->toDelete = true;

		if (player_1 == body2)
		{
			LOG("Added 3 ammo player 1");
			App->player_1->AddAmmo();
		}
		else if (player_2 == body2)
		{
			LOG("Added 3 ammo player 2");
			App->player_2->AddAmmo();
		}
	}

}

bool ModuleSceneIntro::AddPowerUpSpawner(vec3 position)
{
	PowerUpSpawner* spawner = new PowerUpSpawner(position, App);
	stageSpawners.add(spawner);
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

void PowerUp::Render()
{
	Sphere sphere(0.4f);
	sphere.color = Red;
	sphere.SetPos(sensor->GetPos().x, sensor->GetPos().y, sensor->GetPos().z);
	sphere.Render();
}

PowerUpSpawner::PowerUpSpawner(vec3 position, Application * app): position(position), App(app)
{
	AddPowerUp(position);
	powerUpTaked = false;

}

PowerUpSpawner::~PowerUpSpawner()
{
}

bool PowerUpSpawner::AddPowerUp(vec3 position)
{
	PowerUp* power = new PowerUp(position, App);
	power->spawner = this;
	App->scene_intro->powerUps.add(power);
	
	return true;
}


void PowerUpSpawner::StartRespawnTime()
{
	spawnTimer.Start();
	powerUpTaked = true;
}

void PowerUpSpawner::Update()
{
	if (spawnTimer.Read() > RESPAWN_TIME && powerUpTaked == true)
	{
		AddPowerUp(position);
		powerUpTaked = false;
	}
}
