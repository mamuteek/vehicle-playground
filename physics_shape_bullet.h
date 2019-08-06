#pragma once

#include <physics_shape.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

class physics_shape_bullet : public physics_shape
{
public:
	physics_shape_bullet();
	~physics_shape_bullet(void);

	btCollisionShape *geom;

	void*	get_body(void) override;
	void	set_body(void* the_body) override;

	btVector3 calculateInertia(const float mass) override;

	void createBox(btVector3 halfExtents) override;
	void createCylinder(float radius, float width) override;
	void createPlane(btVector3 normal, float distance) override;
	void createHeightfield(int width, int length, void *data, float minHeight, float maxheight) override;
};
