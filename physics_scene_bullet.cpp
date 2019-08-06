#include <core.h>


#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btDefaultSoftBodySolver.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btAlignedObjectArray.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include <physics_scene_bullet.h>
#include <physics_actor_bullet.h>
#include <physics_shape_bullet.h>
#include <physics_callbacks_bullet.h>

#include <LinearMath/btIDebugDraw.h>

#include <string>
#include <cmath>
#include <iostream>

physics_scene_bullet::physics_scene_bullet(physics_server* server) :
	physics_scene(server),
	mBroadphase(0),
	mCollisionConfiguration(0),
	mDispatcher(0),
	mConstraintSolver(0),
	mSoftBodySolver(0),
	mPhysicsWorld(0),
	mDebugDrawer(0)
{
}

physics_scene_bullet::~physics_scene_bullet()
{

	delete mDebugDrawer;

	delete mPhysicsWorld;
	delete mSoftBodySolver;
	delete mConstraintSolver;
	delete mDispatcher;
	delete mCollisionConfiguration;
	delete mBroadphase;
}

void physics_scene_bullet::init()
{
	// ** CORE PHYSICS ***
	mBroadphase = new btDbvtBroadphase();
	mCollisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	mConstraintSolver = new btSequentialImpulseConstraintSolver();
	mSoftBodySolver = new btDefaultSoftBodySolver();
	mPhysicsWorld = new btSoftRigidDynamicsWorld(mDispatcher, mBroadphase, mConstraintSolver, mCollisionConfiguration, mSoftBodySolver);
	mPhysicsWorld->setDrawFlags(fDrawFlags::Std + fDrawFlags::Nodes); // + fDrawFlags::Nodes + fDrawFlags::NodeTree

	// *** COLLISION CALLBACKS ***
	mDispatcher->setNearCallback(mNearCollisionCallback);
	gContactAddedCallback = mContactAddedCallback;
	gContactProcessedCallback = mContactProcessedCallback;
	gContactDestroyedCallback = mContactDestroyedCallback;

	// *** EUROTRUCKS CUSTOM SETTINGS ***
	mPhysicsWorld->getSolverInfo().m_splitImpulse = false;
	mPhysicsWorld->getSolverInfo().m_numIterations = 10;
	mPhysicsWorld->getSolverInfo().m_solverMode = SOLVER_USE_WARMSTARTING | SOLVER_SIMD | SOLVER_RANDMIZE_ORDER;
	mPhysicsWorld->getSolverInfo().m_warmstartingFactor = 0.85f;
	mPhysicsWorld->setForceUpdateAllAabbs(true);

	// *** DISPATCHER SETTINGS ***
	mPhysicsWorld->getDispatchInfo().m_useContinuous = true;
	mPhysicsWorld->getDispatchInfo().m_enableSPU = true;
	mPhysicsWorld->getDispatchInfo().m_enableSatConvex = false;
	mPhysicsWorld->getDispatchInfo().m_useEpa = false;
	mPhysicsWorld->getDispatchInfo().m_allowedCcdPenetration = 0.04f;
	mPhysicsWorld->getDispatchInfo().m_useConvexConservativeDistanceUtil = false;
	mPhysicsWorld->getDispatchInfo().m_convexConservativeDistanceThreshold = 0.0f;
	mPhysicsWorld->getDispatchInfo().m_dispatchFunc = btDispatcherInfo::DISPATCH_DISCRETE;

	mDebugDrawer = new debugDrawBullet();
	mDebugDrawer->setDebugMode(2049); // 6145 to add constraint drawing
	mPhysicsWorld->setDebugDrawer(mDebugDrawer);
}


void physics_scene_bullet::physics_tick(const float timestep)
{
	physics_update();
	mPhysicsWorld->stepSimulation(timestep);
	physics_post_update();
}


void physics_scene_bullet::render_update(void)
{
	// Clear vectors
	mDebugDrawer->lines.clear();
	mDebugDrawer->triangles.clear();
	mPhysicsWorld->debugDrawWorld();
}

void physics_scene_bullet::debug_draw_lines(void)
{
	mDebugDrawer->debugDrawLines();
}


void physics_scene_bullet::debug_draw_triangles(void)
{
	mDebugDrawer->debugDrawTriangles();
}


void *physics_scene_bullet::get_debug_drawer(void)
{
	return static_cast<void*>(mDebugDrawer);
}


void physics_scene_bullet::insert(physics_actor *actor, int collisionGroup, int collisionMask)
{
	btRigidBody* body = static_cast<btRigidBody*>(actor->getBody());
	mPhysicsWorld->addRigidBody(body, collisionGroup, collisionMask);
}


void physics_scene_bullet::ray_test(const vec3_m &source, const vec3_m &target, physics_actor **shape, vec3_m &hit_point, vec3_m &hit_normal, float &hit_fraction)
{
	const btVector3 bt_source = to_btVector3(source);
	const btVector3 bt_target = to_btVector3(target);

	btCollisionWorld::ClosestRayResultCallback rayCallback(bt_source, bt_target);
	mPhysicsWorld->rayTest(bt_source, bt_target, rayCallback);
	hit_point = to_vec3_m(rayCallback.m_hitPointWorld);
	hit_normal = to_vec3_m(rayCallback.m_hitNormalWorld);
	hit_fraction = rayCallback.m_closestHitFraction;
	*shape = rayCallback.m_collisionObject ? static_cast<physics_actor*>(rayCallback.m_collisionObject->getUserPointer()) : nullptr;
}

void physics_scene_bullet::setup_collision_groups(void)
{
}


void *physics_scene_bullet::get_world(void)
{
	return static_cast<void*>(mPhysicsWorld);
}



void physics_scene_bullet::physics_update(void)
{
}




void physics_scene_bullet::physics_post_update(void)
{
}


