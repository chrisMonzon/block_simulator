// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.
#include<iostream>
#include "Transform.h"
using namespace std;
#include <glm/gtc/reciprocal.hpp>

// // Helper rotation function.  Please implement this.
mat3 Transform::rotate(const float degrees, const vec3& axis)
{
  float angle = glm::radians(degrees);
	float cosAngle = cos(angle);
	float sinAngle = sin(angle);
	vec3 axis2 = glm::normalize(axis);
	mat3 ident =  mat3(1, 0, 0, 
										0, 1, 0,
										0, 0, 1);
	// identity matrix of cos0
	mat3 identAngle = mat3(cosAngle, 0, 0,
												0, cosAngle, 0,
												0, 0, cosAngle);
	// identity matrix of sin0
	mat3 identAngle2 = mat3(sinAngle, 0, 0,
												0, sinAngle, 0,
												0, 0, sinAngle);
	// I_3x3 * cos0
	mat3 term1 = identAngle * ident;
	// 1 - cos0
	mat3 negIdentAngle = mat3(1 - cosAngle, 0, 0,
														0, 1 - cosAngle, 0,
														0, 0, 1 - cosAngle);
	// (aa^T)(1-cos0)
	mat3 term2 = negIdentAngle * glm::transpose(mat3(axis2.x * axis2.x, axis2.x * axis2.y, axis2.x * axis2.z,
																									axis2.x * axis2.y, axis2.y * axis2.y, axis2.y * axis2.z,
																									axis2.x * axis2.z, axis2.y * axis2.z, axis2.z * axis2.z));
	mat3 term3 = identAngle2 * glm::transpose(mat3(0, -1 * axis2.z, axis2.y,
																									axis2.z, 0, -1 * axis2.x,
																								-1 * axis2.y, axis2.x, 0));
	mat3 final = term1 + term2 + term3;
  return final;
}


void Transform::left(float degrees, vec3& eye, vec3& up)
{
  // YOUR CODE FOR HW2 HERE
  eye = Transform::rotate(degrees, up) * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up)
{
  // YOUR CODE FOR HW2 HERE
  float angle = -1 * degrees;
	vec3 cross = glm::cross(eye, up);
	cross = glm::normalize(cross);
	eye = Transform::rotate(degrees, cross) * eye;
	up = Transform::rotate(degrees, cross) * up;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up)
{
  mat4 transVec = glm::transpose(mat4(1, 0, 0, -1 * eye.x,
																			0, 1, 0, -1 * eye.y,
																			0, 0, 1, -1 * eye.z,
																			0, 0 ,0, 1));
	vec3 w = glm::normalize(eye);
	vec3 u = glm::normalize(glm::cross(up, w));
	vec3 v = glm::cross(w, u);
	mat4 rotvec = glm::transpose(mat4(u.x, u.y, u.z, 0,
																		v.x, v.y, v.z, 0,
																		w.x, w.y, w.z, 0,
																		0, 0, 0, 1));
	return rotvec * transVec;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
  mat4 ret;
  // YOUR CODE FOR HW2 HERE
  // New, to implement the perspective transform as well.  
  float theta = (float)glm::radians(fovy/2);
  float distance = (float)(1/glm::tan(theta));
  float A = -1 * (float)(zFar + zNear) / (float)(zFar - zNear);
  float B = -2 * (float)(zFar * zNear) / (float)(zFar - zNear);
  mat4 perspective = glm::transpose(mat4((float)(distance/aspect), 0, 0, 0,
                          0, distance, 0, 0,
                          0, 0, A, B,
                          0, 0, -1, 0));
  return perspective;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz)
{
  mat4 ret = glm::transpose(mat4(sx, 0, 0, 0,
																0, sy, 0, 0,
																0, 0, sz, 0,
																0, 0, 0, 1));
  // YOUR CODE FOR HW2 HERE
  // Implement scaling 
  return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz)
{
  mat4 ret = glm::transpose(mat4(1, 0, 0, tx,
                  0, 1, 0, ty,
                  0, 0, 1, tz,
                  0, 0, 0, 1));
  // YOUR CODE FOR HW2 HERE
  // Implement translation 
  return ret;
}

// To normalize the up direction and construct a coordinate frame.
// As discussed in the lecture.  May be relevant to create a properly
// orthogonal and normalized up.
// This function is provided as a helper, in case you want to use it.
// Using this function (in readfile.cpp or display.cpp) is optional.

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec)
{
  vec3 x = glm::cross(up,zvec);
  vec3 y = glm::cross(zvec,x);
  vec3 ret = glm::normalize(y);
  return ret;
}


Transform::Transform()
{

}

Transform::~Transform()
{

}