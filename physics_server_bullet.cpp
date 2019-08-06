#include <physics_server_bullet.h>
#include <physics_scene_bullet.h>
#include <physics_actor_bullet.h>
#include <physics_shape_bullet.h>
#include <physics_joint_bullet.h>

#include <physics_globals.h>

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

physics_server_bullet::physics_server_bullet(const float timestep)
	: physics_server(timestep)
{
}

physics_server_bullet::~physics_server_bullet(void)
{
}

physics_joint *physics_server_bullet::create_joint_6dof(physics_actor *const actor1, physics_actor *const actor2, const placement_m &frameInA, const placement_m &frameInB)
{
	physics_joint* joint = new physics_joint_6dof_bullet(actor1, actor2, frameInA, frameInB);
	return joint;
}

physics_actor* physics_server_bullet::create_new_actor(const placement_m &transform)
{
	physics_actor* actor = new physics_actor_bullet();
	const btTransform bt_transform = to_btTransform(transform);
	btMotionState* motionState = new btDefaultMotionState(bt_transform);
	btRigidBody* body = new btRigidBody(0.0f, motionState, nullptr, btVector3(0.0f, 0.0f, 0.0f));
	body->setUserPointer(actor);
	actor->setBody(body);
	return actor;
}

physics_actor *physics_server_bullet::new_actor_static(const placement_m &transform)
{
	// Bullet does not differentiate between creating dynamic and static actors?
	return create_new_actor(transform);
}


physics_shape * physics_server_bullet::create_plane(const vec3_m &normal, const float dist)
{
	return nullptr;
}

physics_shape *physics_server_bullet::create_heightfield(const int width, const int length, 
		void *const data, const float minHeight, const float maxHeight)
{
	physics_shape* shape = new physics_shape_bullet();
	btCollisionShape* geom = new btHeightfieldTerrainShape(width, length, data, 1 /* scale */, btScalar(minHeight), btScalar(maxHeight), 1, PHY_UCHAR, true);
	geom->setLocalScaling(btVector3(50.f, 1.f, 50.f));
	geom->setUserPointer(this);
	shape->set_body(geom);
	return shape;
}

physics_shape *physics_server_bullet::create_box(const vec3_m &halfExtents)
{
	physics_shape* shape = new physics_shape_bullet();
	const btVector3 bt_half_extents = to_btVector3(halfExtents);
	btCollisionShape* geom = new btBoxShape(bt_half_extents);
	geom->setUserPointer(this);
	shape->set_body(geom);
	return shape;
}

physics_shape *physics_server_bullet::create_capsule(const float radius, const float width)
{
	physics_shape* shape = new physics_shape_bullet();
	btCollisionShape* geom = new btCylinderShapeX(btVector3(radius / 2, radius / 2, width));
	geom->setUserPointer(this);
	shape->set_body(geom);
	return shape;
}

physics_shape * physics_server_bullet::create_sphere(const float radius)
{
	return nullptr;
}

physics_shape *physics_server_bullet::create_compound(void)
{
	physics_shape* shape = new physics_shape_bullet();
	btCollisionShape* geom = new btCompoundShape();
	geom->setUserPointer(this);
	shape->set_body(geom);
	return shape;
}



void *physics_server_bullet::get_body(void)
{
	return nullptr;
}

physics_scene *physics_server_bullet::create_scene(void)
{
	physics_scene* scene = new physics_scene_bullet(this);
	scenes.push_back(scene);
	return scene;
}

void physics_server_bullet::physics_tick(void)
{
	for (std::vector<physics_scene*>::iterator it = scenes.begin(); it != scenes.end(); ++it) {
		(*it)->physics_tick(timestep);
	}
}
