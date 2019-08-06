#pragma once

#include <physics_shape.h>



namespace physx {
	class PxShape;
}

class physics_shape_physx : public physics_shape
{
public:
	physics_shape_physx();
	~physics_shape_physx(void);

	physx::PxShape* geom;

	void*	get_body(void) override;
	void	set_body(void* the_body) override;

	btVector3 calculateInertia(const float mass) override;

	void createBox(btVector3 halfExtents) override;
	void createCylinder(float radius, float width) override;
	void createPlane(btVector3 normal, float distance) override;
	void createHeightfield(int width, int length, void *data, float minHeight, float maxheight) override;
};
