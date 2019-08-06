#include <physics_joint_physx.h>
#include <physics_scene.h>
#include <physics_actor.h>

#include "PxPhysicsAPI.h"

using namespace physx;

physics_joint_physx::physics_joint_physx()
{
}


physics_joint_physx::~physics_joint_physx(void)
{
}


void *physics_joint_physx::get_body(void)
{
	return static_cast<void*>(body);
}

void physics_joint_physx::set_body(void *the_body)
{
	body = static_cast<PxJoint*>(the_body);
}



void physics_joint_physx::insert(physics_scene* scene)
{
}

void physics_joint_physx::set_limits(const joint_limit_m & limit)
{
}



physics_joint_6dof_physx::physics_joint_6dof_physx(physics_actor *actor1, physics_actor *actor2, const placement_m &frameInA, const placement_m &frameInB)
{
	PxRigidActor *const bodyA = static_cast<PxRigidActor*>(actor1->getBody());
	PxRigidActor *const bodyB = static_cast<PxRigidActor*>(actor2->getBody());
	const PxTransform px_frame_in_A = to_pxTransform(frameInA);
	const PxTransform px_frame_in_B = to_pxTransform(frameInB);
	PxPhysics &physics = PxGetPhysics();
	body = PxD6JointCreate(physics, bodyA, px_frame_in_A, bodyB, px_frame_in_B);
}