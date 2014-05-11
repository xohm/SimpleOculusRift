/* --------------------------------------------------------------------------
 * SimpleOculusRift Hit Test
 * --------------------------------------------------------------------------
 * Processing Wrapper for the Oculus Rift
 * http://github.com/xohm/SimpleOculusRift
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / Zhdk / http://iad.zhdk.ch/
 * date:  04/27/2014 (m/d/y)
 * ----------------------------------------------------------------------------
 * Shows how to get the head orientation
 * ----------------------------------------------------------------------------
 */

import SimpleOculusRift.*;

SimpleOculusRift   oculusRiftDev;

float floorDist = 1.7;

boolean   fullScreen = false;
//boolean   fullScreen = true;

PVector  boxPos = new PVector(-2,  -floorDist + 1, -3);
float    boxSize = 1;
boolean  hit = false;

void setup()
{
  if(fullScreen)
    size(1920, 1200, OPENGL);
  else    
    size(1280, 800, OPENGL);

  oculusRiftDev = new SimpleOculusRift(this,SimpleOculusRift.RenderQuality_Middle); 
  oculusRiftDev.setBknColor(10,13, 2);  // just not total black, to see the barrel distortion

  strokeWeight(.3);
}

void draw()
{
  // check the intersection
  PMatrix3D headOrientationMatrix = oculusRiftDev.headOrientationMatrix();
  headOrientationMatrix.translate(0,0,-10);
  
  PVector eyePos = new PVector(0,0,0);
  PVector dir = headOrientationMatrix.mult(eyePos,null);
  PVector hit1 = new PVector();
  PVector hit2 = new PVector();
  PVector hit1Normal = new PVector();
  PVector hit2Normal = new PVector();
  
  if(boxIntersection(eyePos,dir,
                     boxPos,boxSize,boxSize,boxSize,
                     hit1,hit2,
                     hit1Normal,hit2Normal) > 0)
    hit = true;  
  else
    hit = false;  
                    
  // draw the distortion on the screen
  oculusRiftDev.draw();
} 

// SimpleOculusRift call for drawing the scene for each eye
void onDrawScene(int eye)
{  
  stroke(200, 200, 220);
  fill(100, 100, 220);

  drawGrid(new PVector(0, -floorDist, 0), 10, 10);
      
  // static box
  if(hit)
    fill(200,100,20);
  else
    fill(100,200,20);
    
  pushMatrix();
  translate(boxPos.x,boxPos.y,boxPos.z);
  box(boxSize);
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

