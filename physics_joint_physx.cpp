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

void physics_joint_physx::set_limits(const joint_limit_m& limit)
{
	// TODO: implement properly, now hacked to provide
	// correct behavior for the 6DOF hook joint only
	PxD6Joint *d6_body = static_cast<PxD6Joint*>(body);

	/*
	linear_min[0] = limit.linear_x_min;
	linear_max[0] = limit.linear_x_max;
	linear_min[1] = limit.linear_y_min;
	linear_max[1] = limit.linear_y_max;
	linear_min[2] = limit.linear_z_min;
	linear_max[2] = limit.linear_z_max;

	const PxJointLinearLimit;
	d6_body->setDistanceLimit();*/

	const PxJointAngularLimitPair twist_lim(limit.angular_y_min, limit.angular_y_max);
	const PxJointLimitCone swing_lim(limit.angular_x_max, limit.angular_z_max);

	d6_body->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	d6_body->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
	d6_body->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);
	//d6_body->setTwistLimit(twist_lim);
	d6_body->setSwingLimit(swing_lim);
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