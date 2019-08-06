#pragma once

#include <physics_joint.h>
#include <physics_globals.h>

class physics_scene;
class physics_actor;

class btTypedConstraint;

class physics_joint_bullet : public physics_joint
{
public:
	physics_joint_bullet();
	~physics_joint_bullet();

	void*	get_body(void) override;
	void	set_body(void* the_body) override;

	virtual void insert(physics_scene* scene) override;
	virtual void set_limits(const joint_limit_m &limit) override;

protected:
	btTypedConstraint *body;
};


class physics_joint_6dof_bullet : public physics_joint_bullet
{
public:
	physics_joint_6dof_bullet(physics_actor* actor1, physics_actor *actor2, const placement_m& frameInA, const placement_m& frameInB);
};