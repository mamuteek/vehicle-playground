#ifndef CORE_H_
#define CORE_H_

#include <debugDrawer.h>
#include <shaders.h>

//#define USE_AS_VEHICLE

class physics_server;
class physics_scene;
class physics_shape;
class physics_actor;
class physics_joint;
class GUI;
class shared_vehicle;


enum phys_engine_type {
	PHYS_ENGINE_bullet,
	PHYS_ENGINE_physx
};

enum phys_shape_type {
	SHAPE_BOX,
	SHAPE_CYLINDER,
	SHAPE_COMPOUND,
	SHAPE_HEIGHTFIELD
};

enum vehicle_motion_flag {
	DEFAULT = 1 << 0,
	UP = 1 << 1,
	DOWN = 1 << 2,
	LEFT = 1 << 3,
	RIGHT = 1 << 4,
	BRAKE = 1 << 5,
	ZERO = 1 << 6
};

class coreApp
{
	public:
		coreApp();
		~coreApp();

		void mainLoop(void);

		void handle_simulation	(void);
		void handle_renderer	(const float timestep);
		void handle_input		(void);

		void init_physics	(phys_engine_type engine);
		void create_physics_objects(phys_engine_type engine);

		void physics_update	(void);
		void physics_post_update(void);

		GLFWwindow		*m_window;
		physics_server	*m_physics;
		physics_scene	*m_scene;
		myShaders		*m_shaders;
		GUI				*m_GUI;
		bool			showGUI;

	private:

		bool is_paused;
		bool physics_loaded;
		unsigned mHeightmap[900];

		unsigned		motion_flag;

		shared_vehicle	*m_truck;
		shared_vehicle	*m_trailer;
		physics_joint*	m_hook_joint;

		physics_shape	*m_ground_shape;
		physics_actor	*m_ground_actor;
};


#endif /* CORE_H_ */
