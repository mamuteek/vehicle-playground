#pragma once

#include <physics_actor.h>

namespace physx {
	class PxRigidDynamic;
}

class physics_scene_bullet;

class physics_actor_physx : public physics_actor
{
public:
	physics_actor_physx(void);
	~physics_actor_physx(void);

	float get_mass	(void) override;
	void set_mass	(const float mass) override;
	void auto_set_inertia	(void) override;
	void set_inertia	(const vec3_m &inertia) override;
	void attach_shape	(physics_shape *const shape, const placement_m &rel_transform) override;

	void set_collision_group(const unsigned collision_group) override;

	virtual void add_rel_force_at_rel_pos(const vec3_m &force, const vec3_m &rel_pos) override;
	virtual void add_force_at_rel_pos(const vec3_m &force, const vec3_m &rel_pos) override;

	virtual vec3_m get_velocity_at_rel_pos(const vec3_m &rel_pos) override;
	virtual vec3_m get_rel_velocity_at_rel_pos(const vec3_m &rel_pos) override;

	void *getBody(void) override;
	void setBody(void* the_body) override;

	void set_gravity(const float gravity) override;
	void set_sleep_thresholds(const float linear_threshold, const float angular_threshold) override;
	void set_placement(const placement_m &transform) override;
	void set_kinematic(void) override;

	placement_m get_placement(void) override;

	void setup_filtering(const unsigned group, const unsigned mask) override;

private:
	physx::PxRigidDynamic *body;
};
