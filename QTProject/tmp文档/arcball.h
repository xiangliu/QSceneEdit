#ifndef ARCBALL_H
#define ARCBALL_H

/* Arcball, written by Bradley Smith, March 24, 2006
* arcball.h is free to use and modify for any purpose, with no
* restrictions of copyright or license.
*
* Using the arcball:
*   Call arcball_setzoom after setting up the projection matrix.
*
*     The arcball, by default, will act as if a sphere with the given
*     radius, centred on the origin, can be directly manipulated with
*     the mouse. Clicking on a point should drag that point to rest under
*     the current mouse position. eye is the position of the eye relative
*     to the origin. up is unused.
*
*     Alternatively, pass the value: (-radius/|eye|)
*     This puts the arcball in a mode where the distance the mouse moves
*     is equivalent to rotation along the axes. This acts much like a
*     trackball. (It is for this mode that the up vector is required,
*     which must be a unit vector.)
*
*     You should call arcball_setzoom after use of gluLookAt.
*     gluLookAt(eye.x,eye.y,eye.z, ?,?,?, up.x,up.y,up.z);
*     The arcball derives its transformation information from the
*     openGL projection and viewport matrices. (modelview is ignored)
*
*     If looking at a point different from the origin, the arcball will still
*     act as if it centred at (0,0,0). (You can use this to translate
*     the arcball to some other part of the screen.)
*
*   Call arcball_start with a mouse position, and the arcball will
*     be ready to manipulate. (Call on mouse button down.)
*   Call arcball_move with a mouse position, and the arcball will
*     find the rotation necessary to move the start mouse position to
*     the current mouse position on the sphere. (Call on mouse move.)
*   Call arcball_rotate after resetting the modelview matrix in your
*     drawing code. It will call glRotate with its current rotation.
*   Call arcball_reset if you wish to reset the arcball rotation.
*/
#include <windows.h>
#include <gl/glut.h>
#include <cmath> // for sqrt
#include "trimesh/Vec.h"


class arcball
{
public:
	arcball(void);
	~arcball(void);

public:
	GLfloat *ab_quat;
	GLfloat *ab_last;
	GLfloat *ab_next;

private:
	// the distance from the origin to the eye
	GLfloat ab_zoom;
	GLfloat ab_zoom2; //zoom2指的是zoom的平方

	// the radius of the arcball
	//为啥要有两个radius
	GLfloat ab_sphere;
	GLfloat ab_sphere2; //sphere2是sphere的平方

	// the distance from the origin of the plane that intersects
	// the edge of the visible sphere (tangent（正切的、相切的） to a ray from the eye)
	GLfloat ab_edge;

	vec ab_start;//初始位置
	vec ab_curr;//暂时位置
	vec ab_eye;//眼睛位置
	vec ab_eyedir;//?????????????，看函数arcball_setzoom里的初始化意思是1化的eye vec
	vec ab_up;
	vec center;

	GLdouble *ab_glp;  // Projection Matrix
	GLdouble *ab_glm; // Model Matrix
	int *ab_glv;   // viewport

public:
	void arcball_setzoom(float radius, vec eye, vec up);
	void arcball_multiMatrix();
	void arcball_reset();
	void arcball_rotStart(int mx, int my);
	void arcball_rotMove(int mx, int my);
	void arcball_tranStart(int mx, int my);
	void arcball_tranMove(int mx, int my);
	void arcball_trans(float dx, float dy, float dz);
	void arcball_scale(float sx,float sy,float sz);
	void arcball_setCenter(vec center);
	vec sphere_coords(GLdouble mx, GLdouble my);
	vec edge_coords(vec m);


public:
	static void quatidentity(GLfloat* q);
	static void quatidentity(GLdouble* q);
	static void quaternion(GLfloat* q, GLfloat x, GLfloat y, GLfloat z, GLfloat w,vec center);
	static void quatscale(GLfloat* dest, GLfloat x,GLfloat y,GLfloat z);
	static void quattrans(GLfloat* dest, GLfloat x, GLfloat y, GLfloat z);
	static void quatcopy(GLfloat* dst, GLfloat* src);
	static void quatnext(GLfloat* dest, GLfloat* left, GLfloat* right);
};

#endif
