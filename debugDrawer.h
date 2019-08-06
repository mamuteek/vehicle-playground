#ifndef debugDrawer_h__
#define debugDrawer_h__

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
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

class debugDrawBullet : public btIDebugDraw
{
public:
	debugDrawBullet(void);
	~debugDrawBullet();
	void	 debugDrawLines(void);
	void	 debugDrawTriangles(void);

	void     drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
	void     drawCross(const btVector3 &origin, const btVector3 &color);
	void	 drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& /*n0*/, const btVector3& /*n1*/, const btVector3& /*n2*/, const btVector3& color, btScalar alpha);
	void     drawTriangle(const btVector3 &v0, const btVector3 &v1, const btVector3 &v2, const btVector3 &color, btScalar);
	void	 drawBox(const btVector3& bbMin, const btVector3& bbMax, const btTransform& trans, const btVector3& color);
	void	 drawCylinder(btScalar radius, btScalar halfHeight, int upAxis, const btTransform& transform, const btVector3 &color);
	void     drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
	void     reportErrorWarning(const char *warningString);
	void     draw3dText(const btVector3 &location, const char *textString);
	void     setDebugMode(int debugMode);
	int      getDebugMode() const;

private:
	GLuint vao;
	GLuint lines_buff;
	GLuint triangles_buff;

public:
	std::vector<float> lines;
	std::vector<float> triangles;

	DebugDrawModes mDebugModes;
};

#endif // DebugDrawer_h__
