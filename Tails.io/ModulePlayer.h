#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

#define MAX_ACCELERATION 400.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 100.0F

#define TIME_TO_DELETE_MISSILE 1000.0f

struct PhysVehicle3D;
class Timer;

class Missile
{
public:
	Missile(ModulePlayer* owner);
	void Update();

private:

	bool toDelete = false;
	PhysBody3D* physBody = nullptr;
	ModulePlayer* owner = nullptr;
	Timer timerToDelete;
	
	friend class ModulePlayer;
};

class Missile;

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, uint playerNum, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	bool Draw();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void AddMissile();
	void AddAmmo();
	void Reset();

	PhysVehicle3D* GetPlayerCar();

public:
	vec3 initPosition;

private:

	int lifes = 3;
	bool isDead = false;

	int  ammo = 0;
	uint playerNum = 0u;

	PhysVehicle3D* playerCar = nullptr;
	p2List<Missile*> missiles;

	Color firstColor;
	Color secondColor;

	float turn;
	float acceleration;
	float brake;
};