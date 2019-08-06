#include <core.h>
#include <physics_scene_physx.h>
#include <physics_actor_physx.h>
#include <debugDrawPhysX.h>

#include <physics_server.h>
#include <physics_globals.h>

#include <string>
#include <cmath>
#include <iostream>

#include "PxPhysicsAPI.h"

using namespace physx;

static PxFilterFlags CustomFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1,
	PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through, and do any other prefiltering you need.
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	PxU32 ShapeGroup0 = filterData0.word0 & 31;
	PxU32 ShapeGroup1 = filterData1.word0 & 31;
	PxU32* groupCollisionFlags = (PxU32*)constantBlock;

	if ((groupCollisionFlags[ShapeGroup0] & (1 << ShapeGroup1)) == 0)
		return PxFilterFlag::eSUPPRESS;
	else
		return PxFilterFlag::eDEFAULT;
}



physics_scene_physx::physics_scene_physx(physics_server* server)
	: physics_scene(server),
	mDebugDrawer(0),

	flagsOn(false),
	reconnectFlag(false),
	connected(true),
	groupCollisionFlags()
{
	PxPhysics *mPhysics = static_cast<PxPhysics*>(server->get_body());
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	mDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = mDispatcher;
	// setup filter shader

	//init all group pairs to true:
	for (unsigned i = 0; i < 32; i++) {
		groupCollisionFlags[i] = 0xffffffff;
	}

	sceneDesc.filterShader = CustomFilterShader;
	sceneDesc.filterShaderData = groupCollisionFlags;
	sceneDesc.filterShaderDataSize = 32 * sizeof(PxU32);
	setup_collision_groups();
	mScene = mPhysics->createScene(sceneDesc);

	mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	//mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, 1.0f);
	mScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
	//mScene->setVisualizationParameter(PxVisualizationParameter::eBODY_MASS_AXES, 1.0f);
	mScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	mScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);

	mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
}

physics_scene_physx::~physics_scene_physx()
{
	mScene->release();
	mDispatcher->release();
}


void physics_scene_physx::init()
{
	mDebugDrawer = new debugDrawPhysX(mScene);
}



void physics_scene_physx::physics_tick(const float timestep)
{
	physics_update();
	mScene->simulate(timestep);
	mScene->fetchResults(true);
	physics_post_update();
}


void physics_scene_physx::render_update(void)
{
}


void physics_scene_physx::debug_draw_lines(void)
{
	mDebugDrawer->debugDrawLines();
}


void physics_scene_physx::debug_draw_triangles(void)
{
	mDebugDrawer->debugDrawTriangles();
}

void *physics_scene_physx::get_debug_drawer(void)
{
	return nullptr;
}

void physics_scene_physx::insert(physics_actor *actor, int collisionGroup, int collisionMask)
{
	PxActor *const actor_body = static_cast<PxActor*>(actor->getBody());
	actor->setup_filtering(collisionGroup, collisionMask);
	mScene->addActor(*actor_body);
}


void physics_scene_physx::ray_test(const vec3_m &source, const vec3_m &target, physics_actor **shape, vec3_m &hit_point, vec3_m &hit_normal, float &hit_fraction)
{
	const PxVec3 px_source = to_pxVec3(source);
	const PxVec3 px_target = to_pxVec3(target);
	PxVec3 dir = px_target - px_source;
	const PxVec3 ndir = dir.getNormalized();
	const PxReal ray_length = dir.magnitude();
	PxRaycastBuffer hit;
	PxHitFlags flags(PxHitFlag::eDEFAULT);
	PxQueryFilterData filter_data(PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC);

	filter_data.data.word0 = COL_GROUND;

	mScene->raycast(px_source, ndir, ray_length, hit, flags, filter_data);

	hit_point = to_vec3_m(hit.block.position);
	hit_normal = to_vec3_m(hit.block.normal);
	hit_fraction = hit.block.distance / ray_length;

	if (hit.block.actor) {
		assert(hit.block.actor->userData);
		*shape = static_cast<physics_actor*>(hit.block.actor->userData);
	}
	else {
		*shape = nullptr;
	}
}


void physics_scene_physx::setU32CollisionFlag(unsigned group1, unsigned group2, bool enable)
{
	PX_ASSERT(groups1 < 32 && groups2 < 32);
	if (enable)
	{
		//be symmetric:
		groupCollisionFlags[group1] |= (1 << group2);
		groupCollisionFlags[group2] |= (1 << group1);
	}
	else
	{
		groupCollisionFlags[group1] &= ~(1 << group2);
		groupCollisionFlags[group2] &= ~(1 << group1);
	}
}


void physics_scene_physx::setup_collision_groups(void)
{
	setU32CollisionFlag(COL_TRUCK, COL_WHEEL, false);
	setU32CollisionFlag(COL_TRAILER, COL_WHEEL, false);
}


void *physics_scene_physx::get_world(void)
{
	return nullptr;
}


void physics_scene_physx::physics_update(void)
{
}


void physics_scene_physx::physics_post_update(void)
{
}