// This file is display.cpp.  It includes the skeleton for the display routine

// Basic includes to get this file to work.
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
// OSX systems require different headers
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include "Transform.h"
#include "Geometry.h"

using namespace std ;
#include "variables.h"
#include "inithelpers.h"

// New helper transformation function to transform vector by modelview
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.
// Some of you may want to use the more modern routines in readfile.cpp
// that can also be used.
void transformvec (const GLfloat input[4], GLfloat output[4])
{
  glm::vec4 inputvec(input[0], input[1], input[2], input[3]);
  glm::vec4 outputvec = modelview * inputvec;
  output[0] = outputvec[0];
  output[1] = outputvec[1];
  output[2] = outputvec[2];
  output[3] = outputvec[3];
}

void display()
{
  // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glClearColor(1, 1, 1, 1);

  // glClear(GL_COLOR_BUFFER_BIT);
  // glBegin(GL_POLYGON);
  // glColor3f(1.0, 0.0, 0.0);

  // glVertex2d(0, 0);
  // glColor3f(0.0, 0.0, 0.0);

  // glVertex2d(0, 100);
  // glColor3f(1.0, 0.0, 0.0);

  // glVertex2d(100, 100);
  // glColor3f(0.0, 0.4, 0.0);

  // glVertex2d(100, 0);
  // glEnd();
  // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // 
  glEnable(GL_DEPTH_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  
  // Either use the built-in lookAt function or the lookAt implemented by the user.
  if (useGlu) {
    modelview = glm::lookAt(eye,center,up);
  } else {
    modelview = Transform::lookAt(eye,center,up);
  }
  glUniformMatrix4fv(modelviewPos, 1, GL_FALSE, &modelview[0][0]);

  // Lights are transformed by current modelview matrix.
  // The shader can't do this globally.
  // So we need to do so manually.
  if (numused) {

    glUniform1i(enablelighting,true);
    glUniform1i(numusedcol, numused); //set the variable number of lights used

    for (int i = 0 ; i < numused ; i++ ) {
      transformvec(&lightposn[i * 4],&lightransf[i * 4]);
    }

    
    glUniform4fv(lightpos, numused, lightransf);//change to handle postion of the light position
    glUniform4fv(lightcol, numused, lightcolor);//stores the color of the light

  } else {
    glUniform1i(enablelighting,false);
  }

  // Transformations for objects, involving translation and scaling
  mat4 sc(1.0) , tr(1.0), transf(1.0);
  sc = Transform::scale(sx,sy,1.0);
  tr = Transform::translate(tx,ty,0.0);

  transf = modelview * tr * sc; //multiply the current modelview matrix with the tranlate and the scale matrix.

  // The object draw functions will need to further modify the top of the stack,
  // so assign whatever transformation matrix you intend to work with to modelview
  // rather than use a uniform variable for that.
  modelview = transf; // append the transf matrix to modelView
  
  for (int i = 0 ; i < numobjects ; i++) {
    object* obj = &(objects[i]); // Grabs an object struct.
    
    modelview = transf * obj->transform;
    if (obj->vis == 1) {
      mat4 outlineModelview = modelview * Transform::scale(1.5, 1.5, 1.5);
      glUniformMatrix4fv(modelviewPos, 1, GL_FALSE, &outlineModelview[0][0]);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(1000.0);
      glUniform4f(diffusecol, 0.0f, 0.0f, 0.0f, 1.0f); // Black outline

      if (obj->type == cube) {
        solidCube(obj->size);
      }

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Revert to fill mode
    }
    //load the ambient, diffuse, specular, emission, shininess into the Fragment shader
    glUniform4fv(ambientcol, 1, obj->ambient);
  	glUniform4fv(diffusecol, 1, obj->diffuse);
    glUniform4fv(specularcol, 1, obj->specular);
  	glUniform4fv(emissioncol, 1, obj->emission);
  	glUniform1fv(shininesscol, 1, &obj->shininess);

    glUniformMatrix4fv(modelviewPos, 1, GL_FALSE, &modelview[0][0]);
    
    
    // Actually draw the object
    // We provide the actual drawing functions for you.
    // Remember that obj->type is notation for accessing struct fields
    if (obj->type == cube) {
      solidCube(obj->size);
    }
    else if (obj->type == sphere) {
      const int tessel = 20;
      solidSphere(obj->size, tessel, tessel);
    }
    else if (obj->type == teapot) {
      solidTeapot(obj->size);
    }
    else if (obj->type == point ) {
      glPointSize(100.0); // Set the point size (optional)
      glBegin(GL_POINTS);
      glVertex3f(0, 0, 0);
      glEnd();
      // std::cout << "point created\n" << std::endl;
    }
    
    // if (obj->vis == 1) {
    //   glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //   glStencilMask(0x00);
    //   glDisable(GL_DEPTH_TEST);
    // }
    // }
  }
  // GLuint vao;
  // glGenVertexArrays( 1, &vao );
  // glEnable(GL_DEPTH_TEST);

  

  glBindVertexArray( vao );
  glDrawElements(GL_LINES, lenght, GL_UNSIGNED_INT, NULL);
  glBindVertexArray(vao2);
  glDrawElements(GL_LINES, length2, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(vao3);
  glDrawElements(GL_LINES, length3, GL_UNSIGNED_INT, nullptr);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glBindVertexArray(0);

  // glUniform3f(glGetUniformLocation(shaderProgram, "topColor"), 0.0f, 0.0f, 1.0f); // Blue at the top
  // glUniform3f(glGetUniformLocation(shaderProgram, "bottomColor"), 1.0f, 0.0f, 0.0f); // Red at the bottom
  // glDisable(GL_DEPTH_TEST);
  glutSwapBuffers();
}