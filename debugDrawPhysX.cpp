#include <debugDrawPhysX.h>
#include "PxPhysicsAPI.h"

using namespace physx;

debugDrawPhysX::debugDrawPhysX(PxScene *const theScene) :
	vao(),
	lines_buff(),
	triangles_buff(),
	mScene(theScene)
{
	const int vertexSize = sizeof(PxVec3) + sizeof(PxU32);
	// We use a single vao, so there is no reason to unbind it anywhere.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &lines_buff);
	glBindBuffer(GL_ARRAY_BUFFER, lines_buff);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, vertexSize, reinterpret_cast<void*>(sizeof(PxVec3)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &triangles_buff);
	glBindBuffer(GL_ARRAY_BUFFER, triangles_buff);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_UNSIGNED_INT, GL_FALSE, vertexSize, reinterpret_cast<void*>(sizeof(PxVec3)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


debugDrawPhysX::~debugDrawPhysX()
{
	glDeleteBuffers(1, &triangles_buff);
	glDeleteBuffers(1, &lines_buff);
	glDeleteVertexArrays(1, &vao);
}


void debugDrawPhysX::debugDrawLines(void)
{
	const PxRenderBuffer& rb = mScene->getRenderBuffer();
	const PxDebugLine* lines = rb.getLines();
	const PxU32 lineCount = rb.getNbLines();

	glBindBuffer(GL_ARRAY_BUFFER, lines_buff);
	glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(PxDebugLine), lines, GL_STREAM_DRAW);
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(2 * lineCount)); //fixme: why the 2 is needed?
}


void debugDrawPhysX::debugDrawTriangles(void)
{
	const PxRenderBuffer& rb = mScene->getRenderBuffer();
	const PxDebugTriangle* triangles= rb.getTriangles();
	const PxU32 triangleCount = rb.getNbTriangles();

	glBindBuffer(GL_ARRAY_BUFFER, triangles_buff);
	glBufferData(GL_ARRAY_BUFFER, triangleCount * sizeof(PxDebugTriangle), triangles, GL_STREAM_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(3 * triangleCount));

}