#pragma once

#include <vehicle_physics.h>
#include <physics_globals.h>

class physics_actor;
class physics_shape;
class physics_server;
class physics_scene;


enum vehicle_type {
	vehicle_type_TRUCK = 0,
	vehicle_type_TRAILER = 1
};


class shared_vehicle
{
public:
	shared_vehicle(physics_scene* scene, physics_server* physics, const placement_m &transform, const float mass, const vehicle_type type);
	~shared_vehicle();

	void physics_update(void);
	void physics_post_update(void);

	void addWheels(const vec3_m& extents);

	void updateWheels(void);

	physics_actor			*get_actor(void);
	vehicle_physics		*getSuspension(void);

protected:
	physics_actor			*m_chassisActor;
	vehicle_physics		*m_suspension;

	physics_shape			*m_wheelShape[4];
	physics_actor			*m_wheelActor[4];

	physics_scene			*m_scene;
	physics_server			*m_physics;

	placement_m initial_transform;
};