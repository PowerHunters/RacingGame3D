#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	Cylinder wheel;

	wheel.color = Blue;

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[i].radius;
		wheel.height = info.wheels[i].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

		wheel.Render();
	}

	// Rotation info -----------------------------


	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	mat4x4 t;
	btVector3 offset;

	vehicle->getChassisWorldTransform().getOpenGLMatrix(&t);
	offset.setValue(0, 0.7f,0);

	t[12] += offset.getX();
	t[13] += offset.getY();
	t[14] += offset.getZ();


	// Main chasis -------------------------------
	Cube chassis_1(1.5f ,0.6, 1);
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

	offset.setValue( 0.0f, 0.1f, 2.0f );
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
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}