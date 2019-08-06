#include <physics_joint_bullet.h>
#include <physics_scene.h>
#include <physics_actor.h>

#include <btBulletDynamicsCommon.h>



physics_joint_bullet::physics_joint_bullet()
{
}


physics_joint_bullet::~physics_joint_bullet(void)
{
}


void *physics_joint_bullet::get_body(void)
{
	return static_cast<void*>(body);
}

void physics_joint_bullet::set_body(void *the_body)
{
	body = static_cast<btTypedConstraint*>(the_body);
}


physics_joint_6dof_bullet::physics_joint_6dof_bullet(physics_actor *actor1, physics_actor *actor2, const placement_m &frameInA, const placement_m &frameInB)
{
	const bool use_local_A_frame = true;
	btRigidBody *const bodyA = static_cast<btRigidBody*>(actor1->getBody());
	btRigidBody *const bodyB = static_cast<btRigidBody*>(actor2->getBody());
	const btTransform bt_frame_in_A = to_btTransform(frameInA);
	const btTransform bt_frame_in_B = to_btTransform(frameInB);

	body = new btGeneric6DofConstraint(*bodyA, *bodyB, bt_frame_in_A, bt_frame_in_B, use_local_A_frame);
	body->setDbgDrawSize(btScalar(5.f));
}


void physics_joint_bullet::insert(physics_scene* scene)
{
	btDiscreteDynamicsWorld *const world = static_cast<btDiscreteDynamicsWorld*>(scene->get_world());
	world->addConstraint(body);
}

void physics_joint_bullet::set_limits(const joint_limit_m &limit)
{
	btVector3 linear_min;
	btVector3 linear_max;
	btVector3 angular_min;
	btVector3 angular_max;

	linear_min[0] = limit.linear_x_min;
	linear_max[0] = limit.linear_x_max;
	linear_min[1] = limit.linear_y_min;
	linear_max[1] = limit.linear_y_max;
	linear_min[2] = limit.linear_z_min;
	linear_max[2] = limit.linear_z_max;

	angular_min[0] = limit.angular_x_min;
	angular_max[0] = limit.angular_x_max;
	angular_min[1] = limit.angular_y_min;
	angular_max[1] = limit.angular_y_max;
	angular_min[2] = limit.angular_z_min;
	angular_max[2] = limit.angular_z_max;

	btGeneric6DofConstraint *const body_6dof = static_cast<btGeneric6DofConstraint *>(body);
	body_6dof->setLinearLowerLimit(linear_min);
	body_6dof->setLinearUpperLimit(linear_max);
	body_6dof->setAngularLowerLimit(angular_min);
	body_6dof->setAngularUpperLimit(angular_max);
}
