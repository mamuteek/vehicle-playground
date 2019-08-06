#pragma once

#include <physics_globals.h>

class physics_scene;
class physics_shape;

class physics_actor
{
public:

	physics_actor(void);
	~physics_actor(void);

	virtual void *getBody(void) = 0;
	virtual void setBody(void* the_body) = 0;

	virtual void set_gravity	(const float gravity) = 0;
	virtual void set_sleep_thresholds	(const float linear_threshold, const float angular_threshold) = 0;
	virtual void set_placement	(const placement_m &transform) = 0;
	virtual void set_kinematic	(void) = 0;

	virtual float get_mass (void) = 0;
	virtual void set_mass	(const float mass) = 0;

	virtual void auto_set_inertia	(void) = 0;
	virtual void set_inertia	(const vec3_m &inertia) = 0;

	virtual void set_collision_group	(const unsigned collision_group) = 0;

	virtual void attach_shape	(physics_shape *const shape, const placement_m &rel_transform) = 0;

	virtual void add_rel_force_at_rel_pos	(const vec3_m &force, const vec3_m &rel_pos) = 0;
	virtual void add_force_at_rel_pos		(const vec3_m &force, const vec3_m &rel_pos) = 0;
	virtual vec3_m get_velocity_at_rel_pos	(const vec3_m &rel_pos) = 0;
	virtual vec3_m get_rel_velocity_at_rel_pos(const vec3_m &rel_pos) = 0;
	virtual placement_m get_placement(void) = 0;

	virtual void setup_filtering(const unsigned group, const unsigned mask) = 0;

private:

	physics_scene *scene;
};
