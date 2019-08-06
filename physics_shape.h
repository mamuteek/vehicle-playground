#pragma once
#include <btBulletCollisionCommon.h>
#include <core.h>

class physics_scene;


class physics_shape
{
public:
	physics_shape();
	virtual ~physics_shape(void);

	//physics_scene *scene;

	shapeType type;

	virtual void*	get_body(void) = 0;
	virtual void	set_body(void* the_body) = 0;

	virtual btVector3 calculateInertia(const float mass) = 0;

	virtual void createBox(btVector3 halfExtents) = 0;
	virtual void createCylinder(float radius, float width) = 0;
	virtual void createPlane(btVector3 normal, float distance) = 0;
	virtual void createHeightfield(int width, int length, void *data, float minHeight, float maxheight) = 0;
};
