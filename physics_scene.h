#ifndef physics_scene_H_
#define physics_scene_H_

#include <core.h>
#include <physics_globals.h>

class physics_actor;
class physics_shape;
class physics_joint;

class physics_scene
{
public:
	physics_scene(physics_server* server);
	virtual ~physics_scene();

	virtual void	init(void) = 0;
	virtual void	physics_tick(const float timestep) = 0;

	virtual void	render_update(void) = 0;

	virtual void	debug_draw_lines(void) = 0;
	virtual void	debug_draw_triangles(void) = 0;

	virtual void	*get_debug_drawer(void) = 0;

	virtual void	insert(physics_actor* actor, int collisionGroup, int collisionMask) = 0;

	virtual void	ray_test(const vec3_m &source, const vec3_m &target, physics_actor **shape, vec3_m &hit_point, vec3_m &hit_normal, float &hit_fraction) = 0;

	virtual void setup_collision_groups(void) = 0;
	virtual void*	get_world(void) = 0;
protected:

	virtual void	physics_update(void) = 0;
	virtual void	physics_post_update(void) = 0;

};

#endif /* physics_scene_H_ */
