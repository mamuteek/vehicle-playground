#include <physics_scene_bullet.h>
#include <physics_callbacks_bullet.h>
#include <iostream>
#include <cassert>



/**
* @brief Broadphase collision callback
*/
bool BroadphaseFilterCallback::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
	// Initial filtering based on Bullet groups (static, default, kinematic ...).
	const bool collides_0_with_1 = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
	const bool collides_1_with_0 = (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask) != 0;
	const bool collides(collides_0_with_1 && collides_1_with_0);

	if (!collides) {
		return false;
	}

	// *** LOG MESSAGE ***
	//const btCollisionObject *const col_object0 = static_cast<const btCollisionObject *>(proxy0->m_clientObject);
	//const btCollisionObject *const col_object1 = static_cast<const btCollisionObject *>(proxy1->m_clientObject);
	//std::cout << "Broadphase contact! First object: " << col_object0->getCollisionShape()->getName() << std::endl;
	//std::cout << "Broadphase contact! Second object: " << col_object1->getCollisionShape()->getName() << std::endl;


	return true;
}


/**
* @brief Allows preventing collisions in the narrowphase
*/
void mNearCollisionCallback(btBroadphasePair &collisionPair, btCollisionDispatcher &dispatcher, const btDispatcherInfo& dispatchInfo)
{
	btCollisionObject* colObj0 = (btCollisionObject*)collisionPair.m_pProxy0->m_clientObject;
	btCollisionObject* colObj1 = (btCollisionObject*)collisionPair.m_pProxy1->m_clientObject;

	if (dispatcher.needsCollision(colObj0, colObj1))
	{
		btCollisionObjectWrapper obj0Wrap(0, colObj0->getCollisionShape(), colObj0, colObj0->getWorldTransform(), -1, -1);
		btCollisionObjectWrapper obj1Wrap(0, colObj1->getCollisionShape(), colObj1, colObj1->getWorldTransform(), -1, -1);

		// *** LOG MESSAGE ***
		//std::cout << "Broadphase contact! First object: " << col_object0->getCollisionShape()->getName() << std::endl;
		//std::cout << "Broadphase contact! Second object: " << col_object1->getCollisionShape()->getName() << std::endl;

		// dispatcher will keep algorithms persistent in the collision pair
		if (!collisionPair.m_algorithm)
		{
			collisionPair.m_algorithm = dispatcher.findAlgorithm(&obj0Wrap, &obj1Wrap, 0, BT_CONTACT_POINT_ALGORITHMS); //BT_CONTACT_POINT_ALGORITHMS
		}

		if (collisionPair.m_algorithm)
		{
			btManifoldResult contactPointResult(&obj0Wrap, &obj1Wrap);

			if (dispatchInfo.m_dispatchFunc == btDispatcherInfo::DISPATCH_DISCRETE)
			{
				//discrete collision detection query

				collisionPair.m_algorithm->processCollision(&obj0Wrap, &obj1Wrap, dispatchInfo, &contactPointResult);
			}
			else
			{
				//continuous collision detection query, time of impact (toi)
				btScalar toi = collisionPair.m_algorithm->calculateTimeOfImpact(colObj0, colObj1, dispatchInfo, &contactPointResult);
				if (dispatchInfo.m_timeOfImpact > toi)
					dispatchInfo.m_timeOfImpact = toi;

			}
		}
	}
}


/**
* @brief Called when new contact is added and if we want special material properties for the shapes
*/
bool mContactAddedCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* obj0, int partId0, int index0,
	const btCollisionObjectWrapper* obj1, int partId1, int index1)
{
	//btAdjustInternalEdgeContacts(cp, obj0, obj1, partId1, index1, 0);

	//bool first_time_contact = true;
	if (cp.m_userPersistentData) {
		mContactDestroyedCallback(cp.m_userPersistentData);
		//first_time_contact = false;
	}

	//contact_info->first_time = first_time_contact;

	//cp.m_userPersistentData = contact_info;

	//std::cout << "Contact added! First object: " << obj0->getCollisionObject()->getCollisionShape()->getName() << std::endl;
	//std::cout << "Contact added! Second object: " << obj1->getCollisionObject()->getCollisionShape()->getName() << std::endl;

	return true;
}


/**
* @brief Called just after a contact has been processed
*/
bool mContactProcessedCallback(btManifoldPoint &cp, void *body0, void *body1)
{
	assert(body0 && body1);

	//cp.m_userPersistentData;

	const btVector3 position_world_0(cp.getPositionWorldOnA());
	const btVector3 normal_world_1(cp.m_normalWorldOnB);

	//std::cout << "Contact processed!" << std::endl;

	return true;
}


/**
* @brief Called when contact is deleted.
*/
bool mContactDestroyedCallback(void *const user_data)
{
	assert(user_data);
	//std::cout << "Contact destroyed!" << std::endl;
	return true;
}


/**
* @brief Called when contact is started.
*/
void mContactStartedCallback(btPersistentManifold* const &manifold)
{
	//std::cout << "Contact started!" << std::endl;
}


/**
* @brief Called when contact is ended.
*/
void mContactEndedCallback(btPersistentManifold* const &manifold)
{
	//std::cout << "Contact ended!" << std::endl;
}
