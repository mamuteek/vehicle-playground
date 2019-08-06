#ifndef physics_scene_physx_H_
#define physics_scene_physx_H_


#include <physics_scene.h>

class debugDrawPhysX;

namespace physx {
	class PxScene;
	class PxDefaultCpuDispatcher;
	class PxMaterial;
}


class physics_scene_physx : public physics_scene
{
public:
	physics_scene_physx(physics_server *server);
	~physics_scene_physx();

	void	init	(void) override;
	void	physics_tick	(const float timestep) override;

	void	render_update	(void) override;

	void	debug_draw_lines		(void) override;
	void	debug_draw_triangles	(void) override;

	void	*get_debug_drawer	(void) override;

	void	insert		(physics_actor* actor, int collisionGroup, int collisionMask) override;

	void	ray_test	(const vec3_m &source, const vec3_m &target, physics_actor **shape, vec3_m &hit_point, vec3_m &hit_normal, float &hit_fraction) override;

	void setU32CollisionFlag	(unsigned group1, unsigned group2, bool enable);
	void setup_collision_groups	(void) override;

	void *get_world	(void) override;

	debugDrawPhysX* mDebugDrawer;
	bool flagsOn;
	bool reconnectFlag;

	bool connected;
protected:

	void	physics_update	(void) override;
	void	physics_post_update	(void) override;

private:
	unsigned groupCollisionFlags[32];

	physx::PxDefaultCpuDispatcher*	mDispatcher;
	physx::PxScene*				mScene;
	physx::PxMaterial*				mMaterial;
};




#endif /* physics_scene_bullet_H_ */