#include <physics_actor_physx.h>
#include <physics_scene_physx.h>
#include <physics_shape_physx.h>

#include <physics_globals.h>

#include "PxPhysicsAPI.h"

using namespace physx;

physics_actor_physx::physics_actor_physx(void)
	: body()
{
}


physics_actor_physx::~physics_actor_physx(void)
{
}

void *physics_actor_physx::getBody(void)
{
	return static_cast<void*>(body);
}

void physics_actor_physx::setBody(void* the_body)
{
	body = static_cast<PxRigidDynamic*>(the_body);
}

float physics_actor_physx::get_mass(void)
{
	return body->getMass();
}

void physics_actor_physx::set_mass(const float mass)
{
	body->setMass(mass);
}

void physics_actor_physx::auto_set_inertia(void)
{
}

void physics_actor_physx::set_inertia(const vec3_m &inertia)
{
	const PxVec3 px_inertia = to_pxVec3(inertia);
	body->setMassSpaceInertiaTensor(px_inertia);
}

void physics_actor_physx::attach_shape(physics_shape *const shape, const placement_m &rel_transform)
{
	PxShape *const shape_body = static_cast<PxShape*>(shape->get_body());
	const PxTransform physx_transform = to_pxTransform(rel_transform);
	shape_body->setLocalPose(physx_transform);

	body->attachShape(*shape_body);
	shape_body->release();
}

void physics_actor_physx::set_collision_group(const unsigned collision_group)
{
	PxSetGroup(*body, collision_group);
}

void physics_actor_physx::add_rel_force_at_rel_pos(const vec3_m &force, const vec3_m &rel_pos)
{
	PxVec3 px_force = to_pxVec3(force);
	PxVec3 px_rel_pos = to_pxVec3(rel_pos);
	PxRigidBodyExt::addLocalForceAtLocalPos(*body, px_force, px_rel_pos);
}

void physics_actor_physx::add_force_at_rel_pos(const vec3_m &force, const vec3_m &rel_pos)
{
	PxVec3 px_force = to_pxVec3(force);
	PxVec3 px_rel_pos = to_pxVec3(rel_pos);
	PxRigidBodyExt::addForceAtLocalPos(*body, px_force, px_rel_pos);
}

vec3_m physics_actor_physx::get_velocity_at_rel_pos(const vec3_m & rel_pos)
{
	const PxVec3 px_rel_pos = to_pxVec3(rel_pos);
	const PxVec3 local_velocity = PxRigidBodyExt::getLocalVelocityAtLocalPos(*body, px_rel_pos);
	const PxVec3 global_velocity = body->getGlobalPose().q.rotate(local_velocity);
	return to_vec3_m(global_velocity);
}

vec3_m physics_actor_physx::get_rel_velocity_at_rel_pos(const vec3_m &rel_pos)
{
	const PxVec3 px_rel_pos = to_pxVec3(rel_pos);
	const PxVec3 px_rel_vel = PxRigidBodyExt::getLocalVelocityAtLocalPos(*body, px_rel_pos);
	return to_vec3_m(px_rel_vel);
}


void physics_actor_physx::set_gravity(const float gravity)
{
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !gravity);
}


void physics_actor_physx::set_sleep_thresholds(const float linear_threshold, const float angular_threshold)
{
	body->setSleepThreshold(linear_threshold);
}


void physics_actor_physx::set_placement(const placement_m &transform)
{
	const PxTransform physx_transform = to_pxTransform(transform);
	body->setGlobalPose(physx_transform);
}


void physics_actor_physx::set_kinematic(void)
{
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
}

placement_m physics_actor_physx::get_placement(void)
{
	return to_placement_m(body->getGlobalPose());
}


void physics_actor_physx::setup_filtering(const unsigned group, const unsigned mask)
{
	PxFilterData filterData;
	filterData.word0 = group;
	const PxU32 numShapes = body->getNbShapes();
	PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*)*numShapes);
	body->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++) {
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
		shape->setQueryFilterData(filterData);
	}

	free(shapes);
}