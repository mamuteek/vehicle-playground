#include <physics_shape_bullet.h>
#include <physics_actor.h>

#include <cassert>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

physics_shape_bullet::physics_shape_bullet()
{
}

physics_shape_bullet::~physics_shape_bullet(void)
{
}

void *physics_shape_bullet::get_body(void)
{
	return static_cast<void*>(geom);
}

void physics_shape_bullet::set_body(void* the_body)
{
	geom = static_cast<btCollisionShape*>(the_body);
}


btVector3 physics_shape_bullet::calculateInertia(const float mass)
{
	btVector3 inertia;
	geom->calculateLocalInertia(mass, inertia);
	return inertia;
}


void physics_shape_bullet::createBox(btVector3 halfExtents)
{
	type = SHAPE_BOX;
	geom = new btBoxShape(halfExtents);
	geom->setUserPointer(this);
}


void physics_shape_bullet::createCylinder(float radius, float width)
{
	type = SHAPE_CYLINDER;
	geom = new btCylinderShapeX(btVector3(radius / 2, radius / 2, width));
	geom->setUserPointer(this);
}


void physics_shape_bullet::createPlane(btVector3 normal, float distance)
{
}


void physics_shape_bullet::createHeightfield(int width, int length, void *data, float minHeight, float maxHeight)
{
	geom = new btHeightfieldTerrainShape(width, length, data, 1 /* ignore */, btScalar(minHeight), btScalar(maxHeight), 1, PHY_FLOAT, true);
	geom->setLocalScaling(btVector3(50.f, 50.f, 50.f));
	//geom->setUseDiamondSubdivision(true);
	geom->setUserPointer(this);
}