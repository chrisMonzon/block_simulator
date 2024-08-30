/******************************************************************************/
/* Program skeleton provided by Ravi Ramamoorthi  */
/* Grid rendering and cube manipulation via user input coded by Christopher Monzon */
/******************************************************************************/

// You shouldn't have to edit this file at all!

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
#include "shaders.h"
#include "Transform.h"
#include <FreeImage.h>
#include "Geometry.h"

using namespace std; 

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "inithelpers.h" // prototypes for readfile.cpp  
void display(void);  // prototype for display function.  

// Reshapes the window
void reshape(int width, int height){
  w = width;
  h = height;

  glViewport(0, 0, w, h);

  float aspect = (float) w / (float) h, zNear = 0.1, zFar = 99.0 ;
  // float aspect = (float) 750 / (float) h, zNear = 0.1, zFar = 99.0 ;
  // I am changing the projection matrix to fit with the new window aspect ratio
  if (useGlu) projection = glm::perspective(glm::radians(fovy),aspect,zNear,zFar) ; 
  else {
	  projection = Transform::perspective(fovy,aspect,zNear,zFar) ;
  }
  // Now send the updated projection matrix to the shader
  glUniformMatrix4fv(projectionPos, 1, GL_FALSE, &projection[0][0]);
}

void printHelp() {
  std::cout << "ABOUT:\nThis is a building block simulation program.\nBuild your wildest dreams cube by cube, moving, resizing, rotating, and coloring every cube to your liking!"
    
    // << "press 'g' to switch between using glm::lookAt and glm::Perspective or your own LookAt.\n"       
    << "CREDIT:\nProgram skeleton provided by Ravi Ramamoorthi as part of UCSD's online CSE167X course."
    << "Grid rendering and cube manipulation via user input coded by Christopher Monzon.\n"
    << "\n\nSETTINGS:\n"
    << "press 'r' to reset the camera angle.\n"
    << "press 'v' 't' 's' to do view [default], translate, scale the entire scene.\n"
    << "press '1' to set your cube to translation mode, \n'2' for scaling, and \n'3' for rotation.\n"
    << "press 'n' to temprorarily turn off the wireframe indicating the selected cube.\n"
    << "\nCONTROLS:\n"
    << "press '+' or '-' to change the amount of rotation that\noccurs with each arrow press.\n" 
    << "press 'w/a/s/d/q/z' for special cube operations: translate, scale, or rotate.\n"
    << "press 'p' to spawn a new cube.\n"
    << "press 'o' to clone your cube, spawning it in the same place the original.\n"
    << "press 'e' to change the current cube being controlled.\n"
    << "press 'f' to change the color of the current cube"
    << "press 'c' to delete the selected cubes.\n"
    << "press 'x' to delete all cubes.\n"
    << "press 'esc' to quit.\n"
    << "\npress 'h' to print this message again.\n"  ;      
}


void keyboard(unsigned char key, int x, int y) {
  object* obj = &(objects[currz]);
  mat4 newObjTrans;
  object nobj;
  object objs[maxobjects];
  float moveAmt = 0.002 / .03 / 5; 
  bool p_pressed = 0;
  bool f_pressed = 0;
  // mat4 eTrans;
  glPolygonOffset(1,1);
  switch(key) {
    case 'h':
      printHelp();
      break;
    case '+':
      amount++;
      std::cout << "amount set to " << amount << "\n" ;
      break;
    case '-':
      amount--;
      std::cout << "amount set to " << amount << "\n" ; 
      break;
    case 27:  
      exit(0) ;
      break ;
    case '1':
      std::cout << "Cube operation set to: translate" << std::endl;
      mode = trans;
      break;
    case '2':
      std::cout << "Cube operation set to: scale" << std::endl;
      mode = resize;
      break;
    case '3':
      std::cout << "Cube operation set to: rotate" << std::endl;
      mode = rot;
      break;
    case 'r':
      std::cout << "Camera settings reset" << std::endl;
      eye = eyeinit ; 
      up = upinit ; 
      amount = amountinit ;
      transop = view ;
      sx = 1.0 ; 
      sy = 1.0 ;
      tx = 0;
      ty = 0 - amount * .1; 
      if (visObjects != 0) {
        offx = 0;
        offy = 0;
        offz = 0;
      }
      break;
    case 'x':
      if (visObjects != 0) {
        std::cout << "All objects cleared" << std::endl;
        visObjects = 0;
        for (int i = 0; i < maxobjects; ++i) {
          objects[i].size = 0;
          objects[i].vis = 0;
        }
        nobj.size = 0;
        nobj.type = sphere;
        objects[1] = nobj;
        currz = 0;
        offx = 0;
        offy = 0;
        offz = 0;
      } else {
        std::cout << "ERROR: There are no objects to clear" << std::endl;
      }
      break ;  
    case 'c':
      std::cout << "Current object cleared" << std::endl;
      // std::cout << "numobj = " << numobjects << std::endl;
      // std::cout << "currz = " << numobjects << std::endl;
      if (numobjects > 1 && visObjects != 0) {
        if (currz == 0 && visObjects ==  1) {
          // visObjects--;
          // numobjects--;
          visObjects = 0;
          for (int i = 0; i < maxobjects; ++i) {
            objects[i].size = 0;
            objects[i].vis = 0;
          }
          nobj.size = 0;
          nobj.type = sphere;
          objects[1] = nobj;
          break;
        } else {
          visObjects--;
          numobjects--;
          
          for (int i = currz; i < numobjects; ++i) {
            // objects[i] = objects[i + 1];
              // if (objects[i].type != sphere && objects[i + 1].type != sphere ) {
                objects[i].type = objects[i + 1].type; 
                objects[i].size = objects[i + 1].size; 
                for (int j = 0; j < 4; ++j) {
                  objects[i].ambient[j] = objects[i + 1].ambient[j]; 
                  objects[i].diffuse[j] = objects[i + 1].diffuse[j]; 
                  objects[i].specular[j] = objects[i + 1].specular[j]; 
                  objects[i].emission[j] = objects[i + 1].emission[j];
                }
                objects[i].shininess = objects[i + 1].shininess; 
                objects[i].transform = objects[i + 1].transform; 
            // }
            objects[currz].vis = 1;
          }
          
        }
          if (objects[currz].type == sphere) {
            currz--;
            obj = &(objects[currz]);
            obj->vis = 1;
          }
        // std::cout << "numobj = " << numobjects << std::endl;
        // std::cout << "currz = " << numobjects << std::endl;
        // std::cout << "type = " << objects[currz].type << std::endl;
      } else {
        std::cout << "ERROR: There are no objects to clear" << std::endl;
      }
      break ;  
    case 'v': 
      transop = view ;
      std::cout << "Scene operation is set to View\n" ; 
      break ; 
    case 't':
      transop = translate ; 
      std::cout << "Scene operation is set to Translate\n" ; 
      break ; 
    case 'n':
      std::cout << "Selected cube wireframe turned off" << std::endl;
      for (int i = 0 ; i < numobjects ; i++) {
          object* objj = &(objects[i]); // Grabs an object struct.
          objj->vis = 0;
      }
      break;
    case 'w':
      if (visObjects != 0) {
        // std::cout << "mode = " << mode << std::endl;
        if (mode == trans) {
          // * mat4(Transform::rotate(2.5, vec3(0, 1, 0)))
          newObjTrans = obj->transform  * Transform::translate(0, moveAmt + offy, 0);
          obj->transform = newObjTrans ;
          // std::cout << "Moved " << currz<< "st cube forward "<< std::endl; 
        } else if (mode == resize) {
          newObjTrans = obj->transform * Transform::scale(1, 1.04, 1);
          obj->transform = newObjTrans ;
          offy -= (0.04 * moveAmt) / 50;
          // std::cout << "Upscaled cube width" << currz << "\n" ; 
        } else {
          newObjTrans = obj->transform * mat4(Transform::rotate(5, vec3(0, 0, -1)));
          obj->transform = newObjTrans ;
          // std::cout << "Rotate " << currz<< "st cube right "<< std::endl;
        }
      }
      break ;
    case 'a':
      if (visObjects != 0) { 
        if (mode == trans) {
          // * mat4(Transform::rotate(2.5, vec3(-1, 0, 0)))
          newObjTrans = obj->transform  * Transform::translate(-1 * (moveAmt + offx), 0, 0);
          obj->transform = newObjTrans ;
          // std::cout << "Moved " << currz<< "st cube left " << std::endl; 
        } else if (mode == resize) {
          newObjTrans = obj->transform * Transform::scale(0.96, 1, 1);
          obj->transform = newObjTrans ;
          offx -= (0.04 * moveAmt) / 50;
          // std::cout << "Downscaled cube length" << currz << "\n" ; 
        } else {
          newObjTrans = obj->transform * mat4(Transform::rotate(5, vec3(0, 1, 0)));
          obj->transform = newObjTrans ;
          // std::cout << "Rotate " << currz<< "st cube right "<< std::endl;
        }
      }
      break ;
    case 's':
      if (visObjects != 0) {
        if (mode == trans) {
          // * mat4(Transform::rotate(2.5, vec3(0, -1, 0)))
          newObjTrans = obj->transform * Transform::translate(0, -1 * (moveAmt+ offy), 0);
          obj->transform = newObjTrans ;
          // std::cout << "Moved " << currz<< "st cube backward " << std::endl; 
        } else if (mode == resize) {
          newObjTrans = obj->transform * Transform::scale(1, 0.96, 1);
          obj->transform = newObjTrans ;
          offy += (0.04 * moveAmt) / 50;
          // std::cout << "Downscaled cube width " << currz << "\n" ; 
        } else {
          newObjTrans = obj->transform * mat4(Transform::rotate(5, vec3(0, 0, 1)));
          obj->transform = newObjTrans ;
          // std::cout << "Rotate " << currz<< "st cube right "<< std::endl;
        }
      }
      break ;
    case 'd':
      if (visObjects != 0) {
        if (mode == trans) {
          // * mat4(Transform::rotate(2.5, vec3(1, 0, 0)))
          newObjTrans = obj->transform * Transform::translate(moveAmt + offx, 0, 0);
          obj->transform = newObjTrans ;
          // std::cout << "Moved " << currz<< "st cube right " << std::endl; 
        } else if (mode == resize) {
          newObjTrans = obj->transform * Transform::scale(1.04, 1, 1);
          obj->transform = newObjTrans ;
          offx += (0.04 * moveAmt);
          // std::cout << "Upscaled cube length" << currz << "\n" ; 
        } else {
          newObjTrans = obj->transform * mat4(Transform::rotate(5, vec3(0, -1, 0)));
          obj->transform = newObjTrans ;
          // std::cout << "Rotate " << currz<< "st cube right "<< std::endl;
        }
      }
      break ; 
    case 'q':
      if (visObjects != 0) {
        if (mode == trans) { 
          newObjTrans = obj->transform * Transform::translate(0, 0, moveAmt + offz);
          obj->transform = newObjTrans ;
          // std::cout << "Moved " << currz<< "st cube up " << std::endl; 
        } else if (mode == resize) {
          newObjTrans = obj->transform * Transform::scale(1, 1, 1.04);
          obj->transform = newObjTrans ;
          offz -= (0.04 * moveAmt) / 50;
          // std::cout << "Upscaled cube height" << currz << "\n" ; 
        } else {
          newObjTrans = obj->transform * mat4(Transform::rotate(5, vec3(1, 0, 0)));
          obj->transform = newObjTrans ;
          // std::cout << "Rotate " << currz<< "st cube right "<< std::endl;
        }
      }
      break ; 
    case 'z':
      if (visObjects != 0) {
        if (mode == trans) {
          newObjTrans = obj->transform * Transform::translate(0, 0, -(moveAmt + offz));
          obj->transform = newObjTrans ;
          // std::cout << "Moved cube down " << currz << "\n" ; 
        } else if (mode == resize) {
          newObjTrans = obj->transform * Transform::scale(1, 1, 0.96);
          obj->transform = newObjTrans ;
          offz += (0.04 * moveAmt) / 50;
          // std::cout << "Downscaled cube height" << currz << "\n" ; 
        } else {
          newObjTrans = obj->transform * mat4(Transform::rotate(5, vec3(-1, 0, 0)));
          obj->transform = newObjTrans ;
          // std::cout << "Rotate " << currz<< "st cube right "<< std::endl;
        }
      }
      break ; 
    case 'o':
      if (visObjects != 0) {
        o_pressed = 1;
      } else {
        break;
      }
    case 'p': 
      if (numobjects < maxobjects) {
        std::cout << "Cube created" << std::endl;
        for (int i = 0 ; i < numobjects ; i++) {
          object* objj = &(objects[i]); // Grabs an object struct.
          objj->vis = 0;
        }
        visObjects++;
        for (int i = 0; i < 4; ++i) {
          diffuse[i] = colors[currColor][i];
        }
        nobj.size = 0.002 / .03;
        for (int i = 0; i < 4; i++) {
          (nobj.ambient)[i] = ambient[i]; (nobj.diffuse)[i] = diffuse[i];
          (nobj.specular)[i] = specular[i]; (nobj.emission)[i] = emission[i];
        }
        nobj.shininess = 75; 
        currz++;
        if (o_pressed == 0) {
          nobj.transform = mat4(1.0);
        }
        nobj.vis = 1;
        nobj.p = 1;
        nobj.type = cube;
        objects[visObjects - 1] = nobj;
        nobj.size = 0;

        nobj.type = sphere;
        objects[numobjects] = nobj;
        ++numobjects;
        // if (e_pressed == 0) {
          currz = currz % (visObjects);
          currz = visObjects - 1;
        // }
        obj = &(objects[currz]); // fka 367
        // std::cout << "numobj = " << numobjects << std::endl;
        // std::cout << "currz =  " << currz << std::endl;
        p_pressed = 1;
      } else {
        std::cout << "Object limit reached" << std::endl;
      }
      break ;
    case 'f':
      currColor = (currColor + 1) % 11;
      for (int i = 0; i < 4; ++i) {
        diffuse[i] = colors[currColor][i];
      }
      nobj.size = 0.002 / .03;
      // Set the object's light properties
      for (int i = 0; i < 4; i++) {
        (nobj.ambient)[i] = ambient[i]; (nobj.diffuse)[i] = diffuse[i];
        (nobj.specular)[i] = specular[i]; (nobj.emission)[i] = emission[i];
      }
      nobj.shininess = 75;
      nobj.vis = 1;
      nobj.transform = objects[currz].transform;
      nobj.type = cube;
      nobj.f = 1;
      // std::cout << "currz = " << currz << std::endl;
      objects[currz] = nobj;
      f_pressed = 1;
      break;
    case 'e':
      currz++;
      obj->vis = 0;
      currz = currz % (visObjects);
      obj = &(objects[currz]);
      obj->vis = 1;
      std::cout << "e currz = " << currz << std::endl;
      e_pressed = 1;
      eTrans = objects[currz].transform;
      // for (int i = 0; i < 4; i++) {
      //     for (int j = 0; j < 4; j++) {
      //       // eTrans[i][j] = obj->transform[i][j];
      //       // std::cout << eTrans[i][j] << " ";
      //     }
      //     std::cout << std::endl;
      //   }
      break ; 
  }
  if (visObjects != 0) {
    if (objects[currz].type == sphere) {
      int temp = numobjects -1 ;
      while (objects[temp].type != cube) {
        currz--;
        temp--;
      }
      objects[currz].vis = 1;
    }
    if (o_pressed && obj->p == 1 && currz != 0) {
      if (e_pressed == 0) {
        obj->transform = objects[currz - 1 + f_pressed].transform;
        // std::cout << "obj idx cloned = " << (currz - 1 + f_pressed) << std::endl;
      } else {
        obj->transform = eTrans;
        // for (int i = 0; i < 4; i++) {
        //   for (int j = 0; j < 4; j++) {
        //     std::cout << eTrans[i][j] << " ";
        //   }
        //   std::cout << std::endl;
        // }
        // std::cout << "obj cloned " << std::endl;
      }
      std::cout << "Cube cloned" << std::endl;
      obj->p = 0; 
      e_pressed = 0;
      o_pressed = 0;
    } 
  }
  glutPostRedisplay();
}

void specialKey(int key, int x, int y) {
  switch(key) {
    case 100: //left
      if (transop == view) Transform::left(amount, eye,  up);
      else if (transop == translate) tx -= amount * 0.01 ; 
      break;
    case 101: //up
      if (transop == view) Transform::up(amount,  eye,  up);
      else if (transop == translate) ty += amount * 0.01 ; 
      break;
    case 102: //right
      if (transop == view) Transform::left(-amount, eye,  up);
      else if (transop == translate) tx += amount * 0.01 ; 
      break;
    case 103: //down
      if (transop == view) Transform::up(-amount,  eye,  up);
      else if (transop == translate) ty -= amount * 0.01 ; 
      break;
  }
  glutPostRedisplay();
}

int main(int argc, char* argv[]) {
  FreeImage_Initialise();
  glutInit(&argc, argv);
// OSX systems require an extra window init flag
#ifdef __APPLE__
  glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#else
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#endif
  glutInitWindowSize(750, 500);
  glutCreateWindow("HW2: Scene Viewer");

#ifndef __APPLE__ // GLew not needed on OSX systems
  GLenum err = glewInit() ; 
  if (GLEW_OK != err) { 
    std::cerr << "Error: " << glewGetString(err) << std::endl; 
  } 
#endif

  init();
  initializeScene() ; 
  glutDisplayFunc(display);
  glutSpecialFunc(specialKey);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);
  glutReshapeWindow(w, h);

  printHelp();
  glutMainLoop();
  FreeImage_DeInitialise();
  destroyBufferObjects();
  return 0;
}
