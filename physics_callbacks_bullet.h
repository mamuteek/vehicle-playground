#ifndef PHYSICSCALLBACKS_H_
#define PHYSICSCALLBACKS_H_

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>
#include <core.h>
#include <physics_scene_bullet.h>
#include <iostream>
#include <cassert>

/**
* @brief Custom broad-phase callback.
*/
struct BroadphaseFilterCallback : public btOverlapFilterCallback
{
public:
	//inline		BroadphaseFilterCallback();
	bool		needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const;
};


void	mNearCollisionCallback(btBroadphasePair &collisionPair, btCollisionDispatcher &dispatcher, const btDispatcherInfo& dispatchInfo);
bool	mContactAddedCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);
bool	mContactProcessedCallback(btManifoldPoint &cp, void *body0, void *body1);
bool	mContactDestroyedCallback(void *const user_data);
void	mContactStartedCallback(btPersistentManifold* const &manifold);
void	mContactEndedCallback(btPersistentManifold* const &manifold);


/**
* @brief Callback to store the closest convex cast result.
*/
struct mConvexCallback : public btCollisionWorld::ClosestConvexResultCallback
{

public:
	mConvexCallback(const btVector3 &convex_from_world,
		const btVector3 &convex_to_world,
		const unsigned the_collision_group);
	bool		needsCollision(btBroadphaseProxy *proxy0) const;
	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult &convex, bool normal_in_world_space);

private:
	unsigned	collision_group;

public:
	//physics_shape_t	*collision_shape;
	btCollisionWorld::LocalShapeInfo shape_info;
};




/**
* @brief Custom raycast callback
*/
struct mRayCallback : public btCollisionWorld::ClosestRayResultCallback
{
public:
	unsigned	collision_group;
	//const array_t<const physics_actor_t *> &exceptions;

	//array_dyn_t<physics_shape_t *> collision_shape;
	//array_dyn_t<btCollisionWorld::LocalShapeInfo> shape_info;

public:
	mRayCallback(
	/*const btVector3 &ray_from_world,
	const btVector3 &ray_to_world,
	const unsigned the_collision_group
	const array_t<const physics_actor_t *> *const the_exceptions*/
	);

	bool		needsCollision(btBroadphaseProxy* proxy0) const;
	btScalar	addSingleResult(btCollisionWorld::LocalRayResult &ray_result, bool normal_in_world_space);

};



/**
* @brief Custom contact callback
*/
struct mContactCallback : public btCollisionWorld::ContactResultCallback
{
	mContactCallback(btRigidBody& tgtBody)
		: btCollisionWorld::ContactResultCallback(),
		body(tgtBody)
	{
	}

	btRigidBody& body;

	virtual bool needsCollision(btBroadphaseProxy* proxy) const {
		if (!btCollisionWorld::ContactResultCallback::needsCollision(proxy))
		{
			return false;
		}
		return body.checkCollideWithOverride(static_cast<btCollisionObject*>(proxy->m_clientObject));
	}

	virtual btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0,
		int partId0,
		int index0,
		const btCollisionObjectWrapper* colObj1,
		int partId1,
		int index1)
	{
		//btVector3 pt = cp.m_localPointB;
		btVector3 pt(-15, -2, -6);
		void* test1 = colObj0->getCollisionObject()->getUserPointer(); // the trailer
		void* test2 = colObj1->getCollisionObject()->getUserPointer(); // the corresponding cable
		btRigidBody* trailer = static_cast<btRigidBody*>(test1);
		btSoftBody* rope = static_cast<btSoftBody*>(test2);

		btCollisionObject* colObj = const_cast<btCollisionObject*>(colObj1->getCollisionObject());
		//btRigidBody* body = static_cast<btRigidBody*>(colObj);

		rope->cutLink(32, 33, 0.5);
		//rope->m_anchors.removeAtIndex(0);
		//rope->m_anchors.removeAtIndex(1);
		//rope->m_nodes[32].m_x = pt;
		//rope->m_nodes[32].m_im = 0;
		//rope->m_nodes[33].m_x = pt;
		//rope->m_nodes[33].m_im = 0;

		//rope->appendAnchor(32, body, btVector3(0,0,0), false, 1);
		rope->appendAnchor(33, trailer, pt, false, 1);
		colObj->setUserPointer(NULL);
		std::cout << "Custom contact no. " << index0 << " at (" << pt[0] << ", " << pt[1] << ", " << pt[2] << ")" << std::endl;
		return 0;
	}
};


#endif

