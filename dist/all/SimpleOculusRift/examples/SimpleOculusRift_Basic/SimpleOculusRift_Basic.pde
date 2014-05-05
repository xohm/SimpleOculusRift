/* --------------------------------------------------------------------------
 * SimpleOculusRift Basic
 * --------------------------------------------------------------------------
 * Processing Wrapper for the Oculus Rift
 * http://github.com/xohm/SimpleOculusRift
 * --------------------------------------------------------------------------
 * prog:  Max Rheiner / Interaction Design / Zhdk / http://iad.zhdk.ch/
 * date:  04/27/2014 (m/d/y)
 * ----------------------------------------------------------------------------
 */ 
 
import SimpleOculusRift.*;

 
SimpleOculusRift   oculusRiftDev;

void setup()
{
  size(1280,800,OPENGL);
  
  println("OPENGL_VERSION: " + PGraphicsOpenGL.OPENGL_VERSION);
  println("GLSL_VERSION: " + PGraphicsOpenGL.GLSL_VERSION);
  
  oculusRiftDev = new SimpleOculusRift(this); 
}

void draw()
{
//    background(100,100,100);
 /*
  PVector orientation = new PVector();
  oculusRiftDev.sensorOrientation(orientation);
  println(orientation);
*/ 
 //  background(110,100,0);
   line(0,0,0,
        0,800*4,0);   
 
   // draw the distortion on the screen
   oculusRiftDev.draw();
   


} 

// SimpleOculusRift call for drawing the scene for each eye
void onDrawScene(int eye)
{  
  stroke(200,200,220);
   
  
  beginShape(LINES);
  vertex(-1600,200,0);
  vertex(1000,0,-1600);
  endShape();
  
 // strokeWeight(.5);
  line(-1600,200,0,1000,20,-1600);

  pushMatrix();
  translate(-1600,200,0);
  sphere(50);
  popMatrix();

  flush();
}

boolean sketchFullScreen() 
{
  //return true;
  return false;
}
