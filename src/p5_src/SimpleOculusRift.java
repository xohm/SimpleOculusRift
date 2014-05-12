/* ----------------------------------------------------------------------------
 * SimpleOculusRift
 * ----------------------------------------------------------------------------
 * Copyright (C) 2014 Max Rheiner / Interaction Design Zhdk
 *
 * This file is part of SimpleOculusRift.
 *
 * SimpleOpenNI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version (subject to the "Classpath" exception
 * as provided in the LICENSE.txt file that accompanied this code).
 *
 * SimpleOpenNI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SimpleOpenNI.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------------
 */
 
package SimpleOculusRift;

import processing.core.*;
import java.lang.reflect.Method;
import processing.opengl.*;
import java.net.URL;

public class SimpleOculusRift extends ContextWrapper implements SimpleOculusRiftConstants
{
        static String nativDepLibPath = "";
        static String nativLibPath = "";

	static 
        {   // load the nativ shared lib

            String sysStr = System.getProperty("os.name").toLowerCase();
            String libName = "SimpleOculusRift";
            String archStr = System.getProperty("os.arch").toLowerCase();
            String depLib;

            // check which system + architecture
            if(sysStr.indexOf("win") >= 0)
            {   // windows
                if(archStr.indexOf("86") >= 0)
                {    // 32bit
                    libName += "32.dll";
                    nativLibPath = getLibraryPathWin() + "/SimpleOculusRift/library/";
                    nativDepLibPath = nativLibPath + "win32/";
                }
				else if(archStr.indexOf("64") >= 0)
                {
					libName += "64.dll";
                    nativLibPath = getLibraryPathWin() + "/SimpleOculusRift/library/";
                    nativDepLibPath = nativLibPath + "win64/";
				}

            }
            else if(sysStr.indexOf("nix") >= 0 || sysStr.indexOf("linux") >=  0 )
            {   // unix
                nativLibPath = "/SimpleOculusRift/library/linux";

                if(archStr.indexOf("86") >= 0)
                {   // 32bit
                    libName += "32";
                }
                else if(archStr.indexOf("64") >= 0)
                {
                    libName = "lib" + libName + "64.so";
                    nativLibPath = getLibraryPathLinux() + "/SimpleOculusRift/library/";
                    nativDepLibPath = nativLibPath + "linux64/";
                }

                //System.out.println("nativDepLibPath = " + nativDepLibPath);
            }
            else if(sysStr.indexOf("mac") >= 0)
            {     // mac 
                
                libName = "lib" + libName + ".jnilib";
                nativLibPath = getLibraryPathLinux() + "/SimpleOculusRift/library/";
                nativDepLibPath = nativLibPath + "osx/";
                
                //System.out.println("nativDepLibPath = " + nativDepLibPath);
            }

            try
            {
                //System.out.println("-- " + System.getProperty("user.dir"));
                System.load(nativLibPath + libName);
            }
            catch(UnsatisfiedLinkError e)
            {
                System.out.println("Can't load SimpleOculusRift library (" +  libName  + ") : " + e);
                System.out.println("Verify if you installed SimpleOculusRift correctly.\nhttp://code.google.com/p/simple-openni/wiki/Installation");
            }

        }

    public static String getLibraryPathLinux()
    {
        URL url = SimpleOculusRift.class.getResource("SimpleOculusRift.class");
        if (url != null)
        {
            // Convert URL to string, taking care of spaces represented by the "%20"
            // string.
            String path = url.toString().replace("%20", " ");
            int n0 = path.indexOf('/');

            int n1 = -1;
            n1 = path.indexOf("/SimpleOculusRift/library");
            // exported apps.

            if ((-1 < n0) && (-1 < n1))
                return path.substring(n0, n1);
            else
                return "";
        }
        else
            return "";
     }

    public static String getLibraryPathWin()
    {
        URL url = SimpleOculusRift.class.getResource("SimpleOculusRift.class");
        //System.out.println("url = " + url);
        if (url != null)
        {
            // Convert URL to string, taking care of spaces represented by the "%20"
            // string.
            String path = url.toString().replace("%20", " ");
            int n0 = path.indexOf('/');

            int n1 = -1;
            n1 = path.indexOf("/SimpleOculusRift/library");
            // exported apps.

            // In Windows, path string starts with "jar file/C:/..."
            // so the substring up to the first / is removed.
            n0++;

            if ((-1 < n0) && (-1 < n1))
                return path.substring(n0, n1);
            else
                return "";
        }
        else
            return "";
     }

    /**
    * Creates the SimpleOculusRift context ands inits the modules
    *
    * @param parent
    *          PApplet
    */
    public SimpleOculusRift(PApplet parent,int qualtiy)
    {
    /*
        String curPath = SimpleOculusRift.getcwd();
        SimpleOculusRift.chdir(new String(nativDepLibPath));

        this._parent 	= parent;
        parent.registerDispose(this);

        // setup the callbacks
        setupCallbackFunc();

        // start openni
        this.init();

        SimpleOculusRift.chdir(curPath);

*/
        setDataFolder(getLibraryPathLinux() + "/SimpleOculusRift/library/data/");

        this._parent 	= parent;
        parent.registerDispose(this);

        // setup the callbacks
        setupCallbackFunc();

        // start openni
        this.init(parent.width,parent.height,qualtiy);

    }

    public SimpleOculusRift(PApplet parent)
    {
        setDataFolder(getLibraryPathLinux() + "/SimpleOculusRift/library/data/");

        this._parent 	= parent;
        parent.registerDispose(this);

        // setup the callbacks
        setupCallbackFunc();

        // start openni
        this.init(parent.width,parent.height,RenderQuality_Def);

    }



    protected void setupCallbackFunc()
    {
        _cbObject   = _parent;

        // user callbacks
        setupCB(_parent);
    }

    private void setupCB(Object obj)
    {
        _drawSceneMethod    = getMethodRef(obj,"onDrawScene",new Class[] {int.class});

    }

    protected Method getMethodRef(String methodName,Class[] paraList)
    {
            Method	ret = null;
            try {
                    ret = _parent.getClass().getMethod(methodName,paraList);
            }
            catch (Exception e)
            { // no such method, or an error.. which is fine, just ignore
            }
            return ret;
    }

    public static Method getMethodRef(Object obj,String methodName,Class[] paraList)
    {
            Method	ret = null;
            try {
                    ret = obj.getClass().getMethod(methodName,paraList);
            }
            catch (Exception e)
            { // no such method, or an error.. which is fine, just ignore
            }
            return ret;
    }

    public void dispose()
    {
        close();
    }

    public void finalize()
    {
        close();
    }

    public void drawScene(int eye)
    {

        try {

             _parent.pushMatrix();

             // setup projection matrix
             // setup viewport matrix
             PGraphics3D p3d = (PGraphics3D) _parent.g;

             PMatrix3D proj = p3d.projection;
             PMatrix3D modelview = p3d.modelview;

             //proj = new PMatrix3D();
             getMatrix(eye,
                       proj,
                       modelview) ;

             _drawSceneMethod.invoke(_cbObject, new Object[] { (int)eye });
             _parent.flush();

             _parent.popMatrix();
        }
        catch(Exception e)
        {

        }
    }

    public void sensorOrientation(PVector orientation)
    {
      float[] vec = new float[3];
      super.sensorOrientation(vec);
      orientation.set(vec[0],vec[1],vec[2]);
    }

    public PVector sensorOrientation()
    {
      float[] vec = new float[3];
      super.sensorOrientation(vec);
      return new PVector(vec[0],vec[1],vec[2]);
    }

    public void getMatrix(int eye,
                          PMatrix3D proj,
                          PMatrix3D modelView)
    {
        float[] p = new float[16];
        float[] m = new float[16];

        super.getMatrix(eye,p,m);

        proj.set(p[0],p[1],p[2],p[3],
                 p[4],p[5],p[6],p[7],
                 p[8],p[9],p[10],p[11],
                 p[12],p[13],p[14],p[15]);

        modelView.set(m[0],m[1],m[2],m[3],
                          m[4],m[5],m[6],m[7],
                          m[8],m[9],m[10],m[11],
                          m[12],m[13],m[14],m[15]);
    }

    public PMatrix3D headOrientationMatrix()
    {
        float[] vec = new float[3];
        super.sensorOrientation(vec);

        PMatrix3D mat = new PMatrix3D();
        mat.rotateY(vec[0]);
        mat.rotateX(vec[1]);
        mat.rotateZ(vec[2]);

        return mat;
    }

    protected PApplet           _parent;
    protected static boolean    _initFlag;

    protected Object            _cbObject;
    protected Method 		_drawSceneMethod;
}

