#ifndef physics_scene_bullet_H_
#define physics_scene_bullet_H_

#include <physics_scene.h>


class debugDrawBullet;
class physics_shape;
class physics_actor;
class btSoftRigidDynamicsWorld;
struct btDbvtBroadphase;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btSoftBodySolver;

class physics_scene_bullet : public physics_scene
{
public:
	physics_scene_bullet(physics_server* server);
	~physics_scene_bullet();

	void	init	(void) override;
	void	physics_tick	(const float timestep) override;

	void	render_update	(void) override;

	debugDrawBullet* mDebugDrawer;


	void	debug_draw_lines(void) override;
	void	debug_draw_triangles(void) override;

	void	*get_debug_drawer(void) override;

	void	insert(physics_actor* actor, int collisionGroup, int collisionMask) override;

	void	ray_test(const vec3_m &source, const vec3_m &target, physics_actor **shape, vec3_m &hit_point, vec3_m &hit_normal, float &hit_fraction) override;

	void setup_collision_groups(void) override;

	btSoftRigidDynamicsWorld *mPhysicsWorld;

	void *get_world(void) override;

protected:

	void	physics_update(void) override;
	void	physics_post_update(void) override;

private:

	btDbvtBroadphase* mBroadphase;
	btDefaultCollisionConfiguration *mCollisionConfiguration;
	btCollisionDispatcher *mDispatcher;
	btSequentialImpulseConstraintSolver *mConstraintSolver;
	btSoftBodySolver *mSoftBodySolver;
};




#endif /* physics_scene_bullet_H_ */
