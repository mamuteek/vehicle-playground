#pragma once

#include <physics_server.h>
// Todo: do not include this batch of files, instead manually pick the needed ones.
#include "PxPhysicsAPI.h"

using namespace physx;

class physics_server_physx : public physics_server
{
public:
	physics_server_physx(const float timestep);
	virtual ~physics_server_physx(void);

	physics_joint*	create_joint_6dof(physics_actor *const actor1, physics_actor *const actor2, const placement_m &frameInA, const placement_m &frameInB) override;

	physics_actor*	create_new_actor(const placement_m &transform) override;
	physics_actor*	new_actor_static(const placement_m &transform) override;

	physics_shape*	create_plane(const vec3_m &normal, const float dist) override;
	physics_shape*	create_heightfield(const int width, const int length, void *const data, const float minHeight, const float maxHeight) override;
	physics_shape*	create_box(const vec3_m &halfExtents) override;
	physics_shape*	create_capsule(const float radius, const float width) override;
	physics_shape*	create_sphere(const float radius) override;

	physics_shape* create_compound(void) override;

	void*	get_body(void) override;
	physics_scene* create_scene(void) override;
	void	physics_tick(void) override;

private:

	PxFoundation*	mFoundation;
	PxPhysics*	mPhysics;
	PxCooking*	mCooking;

	PxDefaultAllocator		mAllocator;
	PxDefaultErrorCallback	mErrorCallback;
};