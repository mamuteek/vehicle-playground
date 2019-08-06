#ifndef debugDrawePhysX_h__
#define debugDrawePhysX_h__

// TODO: move GL includes to some more local place

// We are linking to the static version of GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw3.h>

// Glm, to be replaced
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include <physics_scene_physx.h>

namespace physx {
	class PxScene;
}

class debugDrawPhysX
{
public:
	debugDrawPhysX(physx::PxScene *const theScene);
	~debugDrawPhysX();
	void	 debugDrawLines(void);
	void	 debugDrawTriangles(void);

private:
	GLuint vao;
	GLuint lines_buff;
	GLuint triangles_buff;

	physx::PxScene *mScene;
};


#endif // DebugDrawer_h__

