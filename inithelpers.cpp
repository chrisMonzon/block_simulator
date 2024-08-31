#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include "Transform.h"

using namespace std;
#include "variables.h"
#include "inithelpers.h"
#include "Geometry.h"
#include "shaders.h"

void init() {
  // Initialize shaders
  currz = 0;
  visObjects = 0;
  numobjects = 0;
  currColor = 3;
  offx = 0;
  offy = 0;
  offz = 0;

  w = 750; h = 500;
  eyeinit = vec3(0, -2, 2);
  center = vec3(0, 0, 0);
  upinit = Transform::upvector(vec3(0, 1, 1), eyeinit);
  fovy = 30;
  colors[0] = vec4(1.5, 1.5, 1.5, 1);   // white
  colors[1] = vec4(.8, 0.8, .8, 1);   // silver
  colors[2] = vec4(.35, .35, .35, 1); // black
  colors[3] = vec4(1.6, .55, .55, 1); // red
  colors[4] = vec4(1.4, 0.9, 0.3, 1); // orange
  colors[5] = vec4(1.3, 1.3, 0, 1); // yellow
  colors[6] = vec4(.25, 1, .25, 1); // green
  colors[7] = vec4(.25, .25, 1, 1); // blue
  colors[8] = vec4(.25, 1, 1, 1); // teal
  colors[9] = vec4(1, .25, 1, 1); // purple
  colors[10] = vec4(2.5, 1.08, 1.4, 1); // pink

  GLfloat lightss[3][8] = {{0.55, 0.55, 0.1, 0, .45, .45, .75, .75},
                      {-0.55, 0.55, 0.1, 0, .45, .45, .75, .75},
                      {0, -0.6, 0.1, 0, .45, .45, .75, .75}};
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 8; i++) {
      lightposn[i + 4 * numused] = lightss[numused][i];
      lightcolor[i + 4 * numused] = lightss[numused][i + 4];
    }
    ++numused;
  }
  
  vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
  fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
  shaderprogram = initprogram(vertexshader, fragmentshader) ; 
  // Get locations of all uniform variables.
  enablelighting = glGetUniformLocation(shaderprogram,"enablelighting") ;
  lightpos = glGetUniformLocation(shaderprogram,"lightposn") ;       
  lightcol = glGetUniformLocation(shaderprogram,"lightcolor") ;       
  numusedcol = glGetUniformLocation(shaderprogram,"numused") ;       
  ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;       
  diffusecol = glGetUniformLocation(shaderprogram,"diffuse") ;       
  specularcol = glGetUniformLocation(shaderprogram,"specular") ;       
  emissioncol = glGetUniformLocation(shaderprogram,"emission") ;       
  shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;    
  projectionPos = glGetUniformLocation(shaderprogram, "projection");
  modelviewPos = glGetUniformLocation(shaderprogram, "modelview");
  // Initialize geometric shapes
  initBufferObjects();
  initTeapot(); initCube(); initSphere();

  // grid starts here
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> vertices2;
  std::vector<glm::vec3> vertices3;
  std::vector<glm::uvec4> indices;
  std::vector<glm::uvec4> indices2;
  std::vector<glm::uvec4> indices3;

  int slices = 20;

  for(int j=0; j<=slices; ++j) {
    for(int i=0; i<=slices; ++i) {

      float x = (float)i/(float)slices * 4 / 3 - 0.002 / .03;
      float y = (float)j/(float)slices * 4  / 3;// - 4.5;
      float z = 0;
      float z2 = 0;

      float zamt= .035;
      float y1 = 0.035;
      vertices.push_back(glm::vec3(x -.49 - .08, y + y1, z - zamt));
      vertices2.push_back(glm::vec3(x - .49 - .08, z2 + 1 + y1 + .01 / .03, y - zamt));
      vertices3.push_back(glm::vec3(-1*z-0.49+.92 + .01 / .03 - 0.002 / .03, -1 * x + 1 + y1 + .01 / .03 - 0.002 / .03, y - zamt));
    }
  }

  for(int j=0; j<slices; ++j) {
    for(int i=0; i<slices; ++i) {

      int row1 =  j    * (slices+1);
      int row2 = (j+1) * (slices+1);

      indices.push_back(glm::uvec4(row1+i, row1+i+1, row1+i+1, row2+i+1));
      indices.push_back(glm::uvec4(row2+i+1, row2+i, row2+i, row1+i));
      indices2.push_back(glm::uvec4(row1+i, row1+i+1, row1+i+1, row2+i+1));
      indices2.push_back(glm::uvec4(row2+i+1, row2+i, row2+i, row1+i));
      indices3.push_back(glm::uvec4(row1+i, row1+i+1, row1+i+1, row2+i+1));
      indices3.push_back(glm::uvec4(row2+i+1, row2+i, row2+i, row1+i));

    }
  }
  
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &(vertices[0]), GL_STATIC_DRAW );
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

  GLuint ibo;
  glGenBuffers( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(glm::uvec4), &(indices[0]), GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  lenght = (GLuint)indices.size()*4;

  GLuint vbo2, ibo2;

  glGenVertexArrays(1, &vao2);
  glBindVertexArray(vao2);

  glGenBuffers(1, &vbo2);
  glBindBuffer(GL_ARRAY_BUFFER, vbo2);
  glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &(vertices2[0]), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glGenBuffers(1, &ibo2);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo2);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(glm::uvec4), &(indices2[0]), GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  length2 = (GLuint)indices2.size() * 4;

  GLuint vbo3, ibo3;

  glGenVertexArrays(1, &vao3);
  glBindVertexArray(vao3);

  glGenBuffers(1, &vbo3);
  glBindBuffer(GL_ARRAY_BUFFER, vbo3);
  glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(glm::vec3), &(vertices3[0]), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glGenBuffers(1, &ibo3);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo3);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices3.size() * sizeof(glm::uvec4), &(indices3[0]), GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  length3 = (GLuint)indices3.size() * 4;

}

void initializeScene()
{
    stack <mat4> transfstack;
    transfstack.push(mat4(1.0));  // identity

    int i;
    transfstack.push(transfstack.top());

    if (numobjects == maxobjects) { // No more objects
      cerr << "Reached Maximum Number of Objects " << numobjects << " Will ignore further objects\n";
    } else {
      object * obj = &(objects[numobjects]);
      obj->size = 0;

      // Set the object's light properties
      for (i = 0; i < 4; i++) {
        (obj->ambient)[i] = ambient[i];
        (obj->diffuse)[i] = diffuse[i];
        (obj->specular)[i] = specular[i];
        (obj->emission)[i] = emission[i];
      }
      obj->shininess = shininess;
      obj->transform = mat4(1);
      obj->type = sphere;
      ++numobjects;
    }

    if (transfstack.size() <= 1) {
      cerr << "Stack has no elements.  Cannot Pop\n";
    } else {
      transfstack.pop();
    }
      
    eye = eyeinit;
    up = upinit;
    amount = amountinit;
    sx = sy = 1.0;  // keyboard controlled scales in x and y
    tx = 0;
    ty = -amountinit * .1;  // keyboard controllled translation in x and y
    useGlu = false; // don't use the glu perspective/lookat fns

    glEnable(GL_DEPTH_TEST);
}