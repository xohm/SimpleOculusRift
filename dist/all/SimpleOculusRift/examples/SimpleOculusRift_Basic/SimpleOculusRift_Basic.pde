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
  // grid
PVector  planeP1 = new PVector(0,0,0);
PVector  planeP2 = new PVector(1,0,0);
PVector  planeP3 = new PVector(0,1,0);

bool fullScreen = true;
void setup()
{
  if(fullScreen)
    size(1920,1200,OPENGL);
  else    
    size(1280,800,OPENGL);
  
  println("OPENGL_VERSION: " + PGraphicsOpenGL.OPENGL_VERSION);
  println("GLSL_VERSION: " + PGraphicsOpenGL.GLSL_VERSION);
  
  oculusRiftDev = new SimpleOculusRift(this); 
  oculusRiftDev.setBknColor(.4,0,0);
  
  stroke(200,200,220);
  fill(100,100,220);
  
  strokeWeight(.3);

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

float anim = 0;

// SimpleOculusRift call for drawing the scene for each eye
void onDrawScene(int eye)
{  
   
  beginShape(LINES);
  vertex(-1600,200,0);
  vertex(1000,0,-1600);
  endShape();


  pushMatrix();
  translate(-1000,-1500,0);
  for(int i=0;i<20;i++)
  {
    line(0,0,-3000,
         0,0,3000);
    translate(100,0,0);
  }
  popMatrix();
  
  pushMatrix();
  translate(0,-1500,-1000);
  for(int i=0;i<20;i++)
  {
    line(-3000,0,0,
         3000,0,0);
    translate(0,0,100);
  }
  popMatrix();
   
 // strokeWeight(.5);
  line(-1600,200,0,1000,20,-1600);

  pushMatrix();
  translate(-1600,200,0);
  sphere(300);
  popMatrix();
   
  pushMatrix();
  translate(0,0,-1250);
  rotateY(anim);
  anim+=.008;
  box(200);
  popMatrix();

  pushMatrix();
  translate(400,0,-1250);
  box(200);
  popMatrix();

  pushMatrix();
  translate(-400,0,-1250);
   box(200);
  popMatrix();

  flush();
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

    void drawPlane(PVector p1, PVector p2, PVector p3,
                                 int len, int repeat)
    {
        repeat--;

        // p1 is the center
        PVector u = PVector.sub(p2, p1);
        u.normalize();
        PVector v = PVector.sub(p3, p1);
        v.normalize();
        PVector dirUp = u.cross(v);
        dirUp.normalize();

        // rectangular
        PVector dirV = u.cross(dirUp);
        dirV.normalize();

        PVector stepsU = PVector.mult(u, (float)len / (float)repeat);
        PVector stepsV = PVector.mult(dirV, (float)len / (float)repeat);

        PVector posU1 = PVector.add(PVector.mult(stepsU, -0.5f * repeat), PVector.mult(stepsV, -0.5f * repeat));
        PVector posU2 = PVector.add(PVector.mult(stepsU, -0.5f * repeat), PVector.mult(stepsV, 0.5f * repeat));

        PVector posV1 = PVector.add(PVector.mult(stepsU, -0.5f * repeat), PVector.mult(stepsV, -0.5f * repeat));
        PVector posV2 = PVector.add(PVector.mult(stepsU, 0.5f * repeat), PVector.mult(stepsV, -0.5f * repeat));

                posU1.add(p1);
                posU2.add(p1);
                posV1.add(p1);
                posV2.add(p1);

        // horz
        for (int i=0;i<repeat+1;i++)
        {
            line(posU1.x, posU1.y, posU1.z,
                   posU2.x, posU2.y, posU2.z);
            line(posV1.x, posV1.y, posV1.z,
                   posV2.x, posV2.y, posV2.z);

            posU1.add(stepsU);
            posU2.add(stepsU);

            posV1.add(stepsV);
            posV2.add(stepsV);
        }
    }
