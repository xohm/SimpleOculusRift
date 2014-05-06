/* --------------------------------------------------------------------------
 * SimpleOculusRift Basic
 * --------------------------------------------------------------------------
 * Processing Wrapper for the Oculus Rift
 * http://github.com/xohm/SimpleOculusRift
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / Zhdk / http://iad.zhdk.ch/
 * date:  04/27/2014 (m/d/y)
 * ----------------------------------------------------------------------------
 * 1 unit should be 1m in the scene
 * ----------------------------------------------------------------------------
 */

import SimpleOculusRift.*;

SimpleOculusRift   oculusRiftDev;

float   animRot = 0;
float   animLin1 = 5;
boolean animDirFlag1 = true;
float   animLin2 = 2;
boolean animDirFlag2 = true;

float floorDist = 1.7;


boolean   fullScreen = false;
//boolean   fullScreen = true;

void setup()
{
  if(fullScreen)
    size(1920, 1200, OPENGL);
  else    
    size(1280, 800, OPENGL);

  println("OPENGL_VERSION: " + PGraphicsOpenGL.OPENGL_VERSION);
  println("GLSL_VERSION: " + PGraphicsOpenGL.GLSL_VERSION);

  oculusRiftDev = new SimpleOculusRift(this); 
  oculusRiftDev.setBknColor(10,13, 2);  // just not total black, to see the barr el distortion

  strokeWeight(.3);
}

void draw()
{
  /*
  // get the data of head tracking sensor
  PVector orientation = new PVector();
  oculusRiftDev.sensorOrientation(orientation);
  println(orientation);   
  */

  // draw the distortion on the screen
  oculusRiftDev.draw();
} 

// SimpleOculusRift call for drawing the scene for each eye
void onDrawScene(int eye)
{  
  /*
  //shows which eye is currently rendered
  if(eye == SimpleOculusRift.StereoEye_Left)
    println("draw left eye");
  else
    println("draw right eye");
  */
  
  stroke(200, 200, 220);
  fill(100, 100, 220);

  drawGrid(new PVector(0, -floorDist, 0), 10, 10);

  // sphere
  pushMatrix();
  translate(-3, -floorDist + animLin1 - .5, 0);
  sphere(1);
  popMatrix();
  animLin1 += .005 * (animDirFlag1 ? -1:1);
  if(animLin1 < 3)
    animDirFlag1 = !animDirFlag1;
  else if(animLin1 > 8)
    animDirFlag1 = !animDirFlag1;
    
  // anim box
  fill(100,20,100);
  pushMatrix();
  translate(0, 0, -3);
  rotateY(animRot);
  animRot+=.008;
  box(1);
  popMatrix();

  // anim box
  fill(100,20,20);
  pushMatrix();
  translate(animLin2, -floorDist + .5, -1.5);
  box(1);
  popMatrix();
  
  animLin2 += .01 * (animDirFlag2 ? -1:1);
  if(animLin2 < -1.0)
    animDirFlag2 = !animDirFlag2;
  else if(animLin2 > 3)
    animDirFlag2 = !animDirFlag2;
      

  // static box
  fill(100,200,20);
  pushMatrix();
  translate(-3,  -floorDist + 1, 0);
  box(2);
  popMatrix();
}

boolean sketchFullScreen() 
{
  return fullScreen;
}     

void keyPressed()
{
  println("reset head orientation");
  oculusRiftDev.resetOrientation();
}

void drawGrid(PVector center, float length, int repeat)
{
  pushMatrix();

  translate(center.x, center.y, center.z);
  float pos;

  for (int x=0; x < repeat+1;x++)
  {
    pos = -length *.5 + x * length / repeat;

    line(-length*.5, 0, pos, 
    length*.5, 0, pos);

    line(pos, 0, -length*.5, 
    pos, 0, length*.5);
  }


  popMatrix();
}

