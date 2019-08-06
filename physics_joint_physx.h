#pragma once
#include <physics_joint.h>
#include <physics_globals.h>


class physics_scene;
class physics_actor;

namespace physx {
	class PxJoint;
}

class physics_joint_physx : public physics_joint
{
public:
	physics_joint_physx();
	~physics_joint_physx();

	physx::PxJoint *body;

	void*	get_body(void) override;
	void	set_body(void* the_body) override;

	virtual void insert(physics_scene* scene) override;
	virtual void set_limits(const joint_limit_m &limit) override;
};

class physics_joint_6dof_physx : public physics_joint_physx
{
public:
	physics_joint_6dof_physx(physics_actor* actor1, physics_actor *actor2, const placement_m& frameInA, const placement_m& frameInB);
};
