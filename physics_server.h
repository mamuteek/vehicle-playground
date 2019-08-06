#pragma once

#include <vector>
#include <core.h>
#include <physics_globals.h>

class physics_scene;
class physics_actor;
class physics_shape;


class physics_server
{
public:
	physics_server(const float the_timestep);
	virtual ~physics_server(void);

	float timestep;
	std::vector<physics_scene*> scenes;

	virtual	physics_joint*	create_joint_6dof	(physics_actor *const actor1, physics_actor *const actor2, const placement_m &frameInA, const placement_m &frameInB) = 0;
	virtual physics_actor*	create_new_actor(const placement_m &transform) = 0;
	virtual physics_actor*	new_actor_static(const placement_m &transform) = 0;

	virtual physics_shape*	create_plane(const vec3_m &normal, const float dist) = 0;
	virtual	physics_shape*	create_heightfield(const int width, const int length, void *const data, const float minHeight, const float maxHeight) = 0;
	virtual	physics_shape*	create_box(const vec3_m &halfExtents) = 0;
	virtual	physics_shape*	create_capsule(const float radius, const float width) = 0;
	virtual	physics_shape*	create_sphere(const float radius) = 0;

	virtual	physics_shape*	create_compound(void) = 0;

	virtual void*	get_body(void) = 0;
	virtual physics_scene* create_scene(void) = 0;
	virtual void	physics_tick	(void) = 0;
};