#include <physics_actor_bullet.h>
#include <physics_scene_bullet.h>
#include <physics_shape_bullet.h>

#include <btBulletDynamicsCommon.h>



physics_actor_bullet::physics_actor_bullet(void)
	: body()
{
}


physics_actor_bullet::~physics_actor_bullet(void)
{
	//if (body && body->getBroadphaseHandle()) {
	//	scene->removeRigidBody(body);
	//}
}

void *physics_actor_bullet::getBody(void)
{
	return static_cast<void*>(body);
}

void physics_actor_bullet::setBody(void * the_body)
{
	body = static_cast<btRigidBody*>(the_body);
}


float physics_actor_bullet::get_mass(void)
{
	return 1.0f / body->getInvMass();
}

void physics_actor_bullet::set_mass(const float mass)
{
	const btVector3 inertia = body->getLocalInertia();
	body->setMassProps(mass, inertia);
}

void physics_actor_bullet::auto_set_inertia(void)
{
	const float mass = 1.0f / body->getInvMass();
	btVector3 inertia;
	body->getCollisionShape()->calculateLocalInertia(mass, inertia);
	inertia[2] *= 10.0f;
	body->setMassProps(mass, inertia);
}

void physics_actor_bullet::set_inertia(const vec3_m &inertia)
{
	const btScalar mass = 1.0f / body->getInvMass();
	const btVector3 bt_inertia = to_btVector3(inertia);
	body->setMassProps(mass, bt_inertia);
}

void physics_actor_bullet::attach_shape(physics_shape *const shape, const placement_m &rel_transform)
{
	const btTransform bt_rel_transform = to_btTransform(rel_transform);
	btCollisionShape *const actor_shape = body->getCollisionShape();
	btCollisionShape *const added_shape = static_cast<btCollisionShape*>(shape->get_body());
	if (! actor_shape) {
		// If the shape is a non-compound, create a new compound proxy and
		// add both shapes to it.
		btCompoundShape *const new_compound = new btCompoundShape();
		body->setCollisionShape(new_compound);
		new_compound->addChildShape(bt_rel_transform, added_shape);
	}
	else if (actor_shape->isCompound()) {
		// If the shape is set as compound, just add the new shape as child.
		btCompoundShape *const compound_shape = static_cast<btCompoundShape*>(actor_shape);
		compound_shape->addChildShape(bt_rel_transform, added_shape);
	}
	else {
		assert("Unsupported shape addition.");
	}
}

void physics_actor_bullet::set_collision_group(const unsigned collision_group)
{
}

void physics_actor_bullet::add_rel_force_at_rel_pos(const vec3_m &force, const vec3_m &rel_pos)
{
	const btVector3 bt_impulse = to_btVector3(force);
	const btVector3 bt_rel_pos = to_btVector3(rel_pos);
	body->applyForce(bt_impulse, bt_rel_pos);
}

void physics_actor_bullet::add_force_at_rel_pos(const vec3_m &force, const vec3_m &rel_pos)
{
	const btVector3 bt_impulse = to_btVector3(force);
	const btVector3 bt_rel_pos = to_btVector3(rel_pos);
	body->applyForce(bt_impulse, bt_rel_pos);
}

vec3_m physics_actor_bullet::get_velocity_at_rel_pos(const vec3_m & rel_pos)
{
	const btVector3 bt_rel_pos = to_btVector3(rel_pos);
	const btVector3 velocity_global = body->getVelocityInLocalPoint(bt_rel_pos);
	return to_vec3_m(velocity_global);
}

vec3_m physics_actor_bullet::get_rel_velocity_at_rel_pos(const vec3_m &rel_pos)
{
	const btVector3 bt_rel_pos = to_btVector3(rel_pos);
	const btVector3 velocity_global =  body->getVelocityInLocalPoint(bt_rel_pos);
	const btQuaternion rel_rotation = body->getWorldTransform().getRotation().inverse();
	const btVector3 velocity_local = quatRotate(rel_rotation, velocity_global);
	return to_vec3_m(velocity_local);
}


void physics_actor_bullet::set_gravity(const float gravity)
{
	body->setGravity(btVector3(0.0f, gravity, 0.0f));
}


void physics_actor_bullet::set_sleep_thresholds(const float linear_threshold, const float angular_threshold)
{
	body->setSleepingThresholds(linear_threshold, angular_threshold);
}


void physics_actor_bullet::set_placement(const placement_m &transform)
{
	const btTransform bt_transform = to_btTransform(transform);
	body->setWorldTransform(bt_transform);
}


void physics_actor_bullet::set_kinematic(void)
{
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
}

placement_m physics_actor_bullet::get_placement(void)
{
	return to_placement_m(body->getWorldTransform());
}

void physics_actor_bullet::setup_filtering(unsigned group, unsigned mask)
{
}
