#include <debugDrawer.h>


debugDrawBullet::debugDrawBullet(void) :
	vao(),
	lines_buff(),
	triangles_buff(),
	lines(),
	triangles(),
	mDebugModes()
{
	// We use a single vao, so there is no reason to unbind it anywhere.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &lines_buff);
	glBindBuffer(GL_ARRAY_BUFFER, lines_buff);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &triangles_buff);
	glBindBuffer(GL_ARRAY_BUFFER, triangles_buff);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


debugDrawBullet::~debugDrawBullet()
{
	glDeleteBuffers(1, &triangles_buff);
	glDeleteBuffers(1, &lines_buff);
	glDeleteVertexArrays(1, &vao);
}


void debugDrawBullet::debugDrawLines(void)
{
	glBindBuffer(GL_ARRAY_BUFFER, lines_buff);
	glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(float), &lines[0], GL_STREAM_DRAW);
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lines.size()));
}


void debugDrawBullet::debugDrawTriangles(void)
{
	glBindBuffer(GL_ARRAY_BUFFER, triangles_buff);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(float), &triangles[0], GL_STREAM_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(triangles.size()));
}


void debugDrawBullet::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
	lines.push_back(from.getX());
	lines.push_back(from.getY());
	lines.push_back(from.getZ());

	lines.push_back(color.getX());
	lines.push_back(color.getY());
	lines.push_back(color.getZ());

	lines.push_back(to.getX());
	lines.push_back(to.getY());
	lines.push_back(to.getZ());

	lines.push_back(color.getX());
	lines.push_back(color.getY());
	lines.push_back(color.getZ());
}


void debugDrawBullet::drawCross(const btVector3 &origin, const btVector3 &color)
{
	drawLine(origin + btVector3(1,0,0), origin - btVector3(1,0,0), color);
	drawLine(origin + btVector3(0,1,0), origin - btVector3(0,1,0), color);
	drawLine(origin + btVector3(0,0,1), origin - btVector3(0,0,1), color);
}


void debugDrawBullet::drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& /*n0*/, const btVector3& /*n1*/, const btVector3& /*n2*/, const btVector3& color, btScalar alpha)
{
}


void debugDrawBullet::drawTriangle(const btVector3 &v0, const btVector3 &v1, const btVector3 &v2, const btVector3 &color, btScalar alpha)
{
	triangles.push_back(v0.getX());
	triangles.push_back(v0.getY());
	triangles.push_back(v0.getZ());

	triangles.push_back(color.getX());
	triangles.push_back(color.getY());
	triangles.push_back(color.getZ());
	triangles.push_back(alpha);


	triangles.push_back(v1.getX());
	triangles.push_back(v1.getY());
	triangles.push_back(v1.getZ());

	triangles.push_back(color.getX());
	triangles.push_back(color.getY());
	triangles.push_back(color.getZ());
	triangles.push_back(alpha);


	triangles.push_back(v2.getX());
	triangles.push_back(v2.getY());
	triangles.push_back(v2.getZ());

	triangles.push_back(color.getX());
	triangles.push_back(color.getY());
	triangles.push_back(color.getZ());
	triangles.push_back(alpha);
}


void debugDrawBullet::drawBox(const btVector3& bbMin, const btVector3& bbMax, const btTransform& trans, const btVector3& color)
{
	drawLine(trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), btVector3(0, 0, 0));
	drawLine(trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), btVector3(0, 0, 0));

	drawTriangle(trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), color, 0.9f);
	drawTriangle(trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), color, 0.9f);
}


void debugDrawBullet::drawCylinder(btScalar radius, btScalar halfHeight, int upAxis, const btTransform& transform, const btVector3 &color)
{
	btVector3 start = transform.getOrigin();
	btVector3 offsetHeight(0.0f, 0.0f, 0.0f);
	offsetHeight[upAxis] = halfHeight;
	int stepDegrees = 30;
	btVector3 capStart(0.0f, 0.0f, 0.0f);
	capStart[upAxis] = -halfHeight;
	btVector3 capEnd(0.0f, 0.0f, 0.0f);
	capEnd[upAxis] = halfHeight;

	btVector3 capStartNext(0.0f, 0.0f, 0.0f);
	capStartNext[upAxis] = -halfHeight;
	btVector3 capEndNext(0.0f, 0.0f, 0.0f);
	capEndNext[upAxis] = halfHeight;

	for (int i = 0; i < 360; i += stepDegrees)
	{
		capEnd[(upAxis + 1) % 3] = capStart[(upAxis + 1) % 3] = btSin(btScalar(i) * SIMD_RADS_PER_DEG) * radius;
		capEnd[(upAxis + 2) % 3] = capStart[(upAxis + 2) % 3] = btCos(btScalar(i) * SIMD_RADS_PER_DEG) * radius;

		capEndNext[(upAxis + 1) % 3] = capStartNext[(upAxis + 1) % 3] = btSin(btScalar(i + 30) * SIMD_RADS_PER_DEG) * radius;
		capEndNext[(upAxis + 2) % 3] = capStartNext[(upAxis + 2) % 3] = btCos(btScalar(i + 30) * SIMD_RADS_PER_DEG) * radius;

		drawLine(start + transform.getBasis() * capStart, start + transform.getBasis() * capEnd, btVector3(0, 0, 0));

		drawTriangle(start + transform.getBasis() * capStart, start + transform.getBasis() * capEnd, start + transform.getBasis() * capEndNext, color, 0.9f);
		drawTriangle(start + transform.getBasis() * capStart, start + transform.getBasis() * capEndNext, start + transform.getBasis() * capStartNext, color, 0.9f);
	}

	// Drawing top and bottom caps of the cylinder
	btVector3 yaxis(0.0f, 0.0f, 0.0f);
	yaxis[upAxis] = btScalar(1.0f);
	btVector3 xaxis(0.0f, 0.0f, 0.0f);
	xaxis[(upAxis + 1) % 3] = btScalar(1.0f);
	drawArc(start - transform.getBasis() * (offsetHeight), transform.getBasis() * yaxis, transform.getBasis()*xaxis, radius, radius, 0, SIMD_2_PI, btVector3(0, 0, 0), false, btScalar(10.0));
	drawArc(start + transform.getBasis() * (offsetHeight), transform.getBasis() * yaxis, transform.getBasis()*xaxis, radius, radius, 0, SIMD_2_PI, btVector3(0, 0, 0), false, btScalar(10.0));
}


void debugDrawBullet::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{
	drawSphere(btVector3(PointOnB), 0.3f, color);
}


void debugDrawBullet::reportErrorWarning(const char *warningString)
{
}


void debugDrawBullet::draw3dText(const btVector3 &location, const char *textString)
{
}


void debugDrawBullet::setDebugMode(int debugMode)
{
	mDebugModes = (DebugDrawModes)debugMode;
}


int debugDrawBullet::getDebugMode() const
{
	return mDebugModes;
}
