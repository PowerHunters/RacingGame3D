#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_SNAKE 2
#define PU_LEVITATION_OFFSET 0.4F

struct PhysBody3D;
struct PhysVehicle3D;
struct PhysMotor3D;
class ModuleSceneIntro;

class PowerUp
{
public:
	PowerUp(vec3 position, Application* app);
	~PowerUp();

	void Update();
	void Render();

private:
	vec3 position;
	bool toDelete = false;
	PhysBody3D* sensor = nullptr;

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
	bool AddPowerUp(vec3 position);

private:

	p2List<PhysBody3D*> stagePrimitives;
	p2List<PowerUp*>    powerUps;
};
