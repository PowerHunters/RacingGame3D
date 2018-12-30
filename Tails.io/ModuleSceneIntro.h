#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_SNAKE 2
#define PU_LEVITATION_OFFSET 0.4F
#define RESPAWN_TIME 5000.0F

struct PhysBody3D;
struct PhysVehicle3D;
struct PhysMotor3D;
class ModuleSceneIntro;
class PowerUpSpawner;

class PowerUp
{
public:
	PowerUp(vec3 position, Application* app);
	~PowerUp();

	void Render();

private:
	vec3 position;
	bool toDelete = false;
	PhysBody3D* sensor = nullptr;
	PowerUpSpawner* spawner = nullptr;

private:
	friend ModuleSceneIntro;
	friend PowerUpSpawner;
};

class PowerUpSpawner
{
public:
	PowerUpSpawner(vec3 position, Application* app);
	~PowerUpSpawner();

	void StartRespawnTime();
	void Update();

private:
	bool AddPowerUp(vec3 position);

private:
	bool  powerUpTaked = false;
	Timer spawnTimer;
	vec3  position;
	Application* App;
private:
	friend ModuleSceneIntro;
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool Draw();
	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	PhysBody3D* test = nullptr;

private:
	bool AddPowerUpSpawner(vec3 position);

private:
	p2List<PowerUpSpawner*> stageSpawners;
	p2List<PhysBody3D*> stagePrimitives;
	p2List<PowerUp*>    powerUps;

	friend PowerUpSpawner;
};
