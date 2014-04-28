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
import javax.media.opengl.*;
import processing.opengl.PGraphicsOpenGL; 
 
SimpleOculusRift   oculusRiftDev;
GL2 gl; 
PGraphicsOpenGL pgl;
PVector orientation = new PVector();

void setup()
{
  size(1280,800,OPENGL);
 // size(1680,1100,OPENGL);
  pgl = (PGraphicsOpenGL) g;  // g may change
  gl = ((PJOGL) pgl.beginPGL()).gl.getGL2();
  
  oculusRiftDev = new SimpleOculusRift(this);
}

void draw()
{
 background(0);


 /*
  oculusRiftDev.sensorOrientation(orientation);
  println(orientation);
*/ 
// pgl.beginPGL();
  oculusRiftDev.draw();
 //   pgl.endPGL();
} 

float ani =0;
void onDrawScene(int eye)
{ 
//  resetShader();

 // pgl.pushFramebuffer();
  /*
   pgl.beginPGL();
   
   pgl.endPGL();
 */
 // pgl.beginPGL();
//  background(220,0,0);
 //  stroke(100);
 
   gl.glBegin(GL.GL_LINE_STRIP);
   gl.glVertex3f(-1600,200,0);
   gl.glVertex3f(1000,0,-1600);
   gl.glEnd();
   
   
 loadMatrix(eye); 
   
   /*
   pgl.beginDraw();
   pgl.endDraw();
   */

  stroke(200,100,0);
  
  beginShape(LINES);
  vertex(-1600,200,0);
  vertex(1000,0,-1600);
  endShape();
  
    line(-1600,200,0,1000,20,-1600);

  /*
  line(0,0,0,1280,800,-1000+ani);
  line(-1000,0,0,
       1000,0,-1000);
 //  ani+=.1;
  */
  pushMatrix();
  translate(-1600,200,0);
  sphere(50);
  popMatrix();
  //println("draw eye: " + eye);
//  pgl.endPGL();

 // pgl.popFramebuffer();

  flush();

}

void loadMatrix(int eye) 
{
  
  PGraphics3D p3d = (PGraphics3D) g;
  
  PMatrix3D proj = p3d.projection; 
  PMatrix3D modelview = p3d.modelview;
 
  oculusRiftDev.getMatrix(eye,
                          proj,
                          modelview) ;
}

