#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysVehicle3D;
struct PhysMotor3D;

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

	p2List<PhysBody3D*> stagePrimitives;

};
