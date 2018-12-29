#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

struct TailBall 
{
	Sphere sphere;
	PhysBody3D* physBody;
};


#define MAX_ACCELERATION 1000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 100.0F

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	bool Draw();

	PhysVehicle3D* GetPlayerCar();

private:

	PhysVehicle3D* playerCar = nullptr;

	float turn;
	float acceleration;
	float brake;
};