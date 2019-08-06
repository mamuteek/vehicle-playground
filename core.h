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


enum shapeType {
	SHAPE_BOX,
	SHAPE_CYLINDER,
	SHAPE_COMPOUND,
	SHAPE_HEIGHTFIELD
};

enum motionFlag {
	DEFAULT,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	C,
	ZERO
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

		void create_physics_objects(void);

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
		unsigned mHeightmap[900];

		motionFlag		vehicleMotionFlag;

		shared_vehicle	*m_truck;
		shared_vehicle	*m_trailer;
		physics_joint*	m_hook_joint;

		physics_shape	*m_ground_shape;
		physics_actor	*m_ground_actor;
};


#endif /* CORE_H_ */
