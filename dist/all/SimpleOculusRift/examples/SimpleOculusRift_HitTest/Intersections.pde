
PVector getPlaneNormal(PVector p1, PVector p2, PVector p3)
{
  PVector u = PVector.sub(p2, p1);
  PVector v = PVector.sub(p3, p1);

  u.normalize();
  v.normalize();

  PVector ret = PVector.cross(u, v, null);
  ret.normalize();
  return ret;
}


// -----------------------------------------------------------------------


// this code is based on the paper from Tomas Möller and Ben Trumbore
// 'Fast, minimum storage ray-triangle intersection.'
// http://www.graphics.cornell.edu/pubs/1997/MT97.html

boolean triangleIntersection(PVector p,
                             PVector d,
                             PVector v0,
                             PVector v1,
                             PVector v2,
                             PVector hit)
{
    float a,f,u,v;
    PVector e1 = PVector.sub(v1,v0);
    PVector e2 = PVector.sub(v2,v0);

    PVector h = d.cross(e2);
    a = e1.dot(h);

    if (a > -0.00001f && a < 0.00001f)
        return false;

    f = 1.0f / a;
    PVector s = PVector.sub(p,v0);
    u = f * s.dot(h);

    if (u < 0.0f || u > 1.0f)
        return false;

    PVector q = s.cross(e1);
    v = f * d.dot(q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    float t = f * e2.dot(q);

    if (t > 0.00001f) // ray intersection
    {
        hit.set(PVector.add(p , PVector.mult(d,t)));
        return true;
    }
    else
        return false;

}

boolean quadIntersection(PVector p,
                         PVector d,
                         PVector p1,
                         PVector p2,
                         PVector p3,
                         PVector p4,
                         PVector hit)
{

    if( triangleIntersection(p,d,
                             p1,p2,p3,
                             hit))
        return true;
    else if( triangleIntersection(p,d,
                                  p3,p4,p1,
                                  hit))
        return true;

    return false;
}

int boxIntersection(PMatrix3D xform,
                    PVector p,
                    PVector dir,
                    PVector boxCenter,
                    float boxWidth,
                    float boxHeigth,
                    float boxDepth,
                    PVector hit1,PVector hit2,
                    PVector hit1Normal,PVector hit2Normal)
{
    float x2 = boxWidth *.5f;
    float y2 = boxHeigth *.5f;
    float z2 = boxDepth *.5f;

    PVector p1 = new PVector(-x2,y2,-z2);
    PVector p2 = new PVector(x2,y2,-z2);
    PVector p3 = new PVector(x2,y2,z2);
    PVector p4 = new PVector(-x2,y2,z2);

    PVector p5 = new PVector(-x2,-y2,-z2);
    PVector p6 = new PVector(x2,-y2,-z2);
    PVector p7 = new PVector(x2,-y2,z2);
    PVector p8 = new PVector(-x2,-y2,z2);


    p1 = xform.mult(PVector.add(p1, boxCenter),null);
    p2 = xform.mult(PVector.add(p2, boxCenter),null);
    p3 = xform.mult(PVector.add(p3, boxCenter),null);
    p4 = xform.mult(PVector.add(p4, boxCenter),null);
    p5 = xform.mult(PVector.add(p5, boxCenter),null);
    p6 = xform.mult(PVector.add(p6, boxCenter),null);
    p7 = xform.mult(PVector.add(p7, boxCenter),null);
    p8 = xform.mult(PVector.add(p8, boxCenter),null);

    PVector[] hit = new PVector[2];
    hit[0] = new PVector();
    hit[1] = new PVector();
    
    PVector[] hitNormal = new PVector[2];
    hitNormal[0] = new PVector();
    hitNormal[1] = new PVector();
    
    int hitCount = 0;

    // check top
    if(quadIntersection(p,
                        dir,
                        p1,p2,p3,p4,
                        hit[hitCount]))
    {
        hitNormal[hitCount] = getPlaneNormal(p1,p2,p3);
        hitCount++;
    }

    // check bottom
    if(quadIntersection(p,
                        dir,
                        p5,p8,p7,p6,
                        hit[hitCount]))
    {
        hitNormal[hitCount] = getPlaneNormal(p5,p8,p7);
        hitCount++;
    }

    // check front
    if(hitCount < 2 && quadIntersection(p,
                                        dir,
                                        p4,p3,p7,p8,
                                        hit[hitCount]))
    {
        hitNormal[hitCount] = getPlaneNormal(p4,p3,p7);
        hitCount++;
    }

    // check back
    if(hitCount < 2 && quadIntersection(p,
                                        dir,
                                        p1,p5,p6,p2,
                                        hit[hitCount]))
    {
        hitNormal[hitCount] = getPlaneNormal(p1,p5,p6);
        hitCount++;
    }

    // check left
    if(hitCount < 2 && quadIntersection(p,
                                        dir,
                                        p1,p4,p8,p5,
                                        hit[hitCount]))
    {
        hitNormal[hitCount] = getPlaneNormal(p1,p4,p8);
        hitCount++;
    }

    // check right
    if(hitCount < 2 && quadIntersection(p,
                                        dir,
                                        p2,p6,p7,p3,
                                        hit[hitCount]))
    {
        hitNormal[hitCount] = getPlaneNormal(p2,p6,p7);
        hitCount++;
    }

    if(hitCount > 0)
    {
        if(hitCount > 1)
        {
            float dist1 = PVector.sub(p,hit[0]).mag();
            float dist2 = PVector.sub(p,hit[1]).mag();
            
            if(dist1 < dist2)
            {
                hit1.set(hit[0]);
                hit2.set(hit[1]);

                hit1Normal.set(hitNormal[0]);
                hit2Normal.set(hitNormal[1]);
            }
            else
            {
                hit1.set(hit[1]);
                hit2.set(hit[0]);

                hit1Normal.set(hitNormal[1]);
                hit2Normal.set(hitNormal[0]);
            }

        }
        else
        {
            hit1.set(hit[0]);
            hit1Normal.set(hitNormal[0]);
        }
    }

    return hitCount;
}

int boxIntersection(PMatrix3D xform,
                    PVector p,
                    PVector dir,
                    PVector boxCenter,
                    float boxWidth,
                    float boxHeigth,
                    float boxDepth,
                    PVector hit1,PVector hit2)
{
    float x2 = boxWidth *.5f;
    float y2 = boxHeigth *.5f;
    float z2 = boxDepth *.5f;

    PVector p1 = new PVector(-x2,y2,-z2);
    PVector p2 = new PVector(x2,y2,-z2);
    PVector p3 = new PVector(x2,y2,z2);
    PVector p4 = new PVector(-x2,y2,z2);

    PVector p5 = new PVector(-x2,-y2,-z2);
    PVector p6 = new PVector(x2,-y2,-z2);
    PVector p7 = new PVector(x2,-y2,z2);
    PVector p8 = new PVector(-x2,-y2,z2);


    p1 = xform.mult(PVector.add(p1, boxCenter),null);
    p2 = xform.mult(PVector.add(p2, boxCenter),null);
    p3 = xform.mult(PVector.add(p3, boxCenter),null);
    p4 = xform.mult(PVector.add(p4, boxCenter),null);
    p5 = xform.mult(PVector.add(p5, boxCenter),null);
    p6 = xform.mult(PVector.add(p6, boxCenter),null);
    p7 = xform.mult(PVector.add(p7, boxCenter),null);
    p8 = xform.mult(PVector.add(p8, boxCenter),null);

    PVector[] hit = new PVector[2];
    hit[0] = new PVector();
    hit[1] = new PVector();
    
    int hitCount = 0;

    // check top
    if(quadIntersection(p,
                        dir,
                        p1,p2,p3,p4,
                        hit[hitCount]))
    {
        hitCount++;
    }

    // check bottom
    if(quadIntersection(p,
                        dir,
                        p5,p8,p7,p6,
                        hit[hitCount]))
    {
        hitCount++;
    }

    // check front
    if(hitCount < 2 && quadIntersection(p,
                                        dir,
                                        p4,p3,p7,p8,
                                        hit[hitCount]))
    {
        hitCount++;
    }

    // check back
    if(hitCount < 2 && quadIntersection(p,
                                        dir,
                                        p1,p5,p6,p2,
                                        hit[hitCount]))
    {
        hitCount++;
    }

    // check left
    if(hitCount < 2 && quadIntersection(p,
                                        dir,
                                        p1,p4,p8,p5,
                                        hit[hitCount]))
    {
        hitCount++;
    }

    // check right
    if(hitCount < 2 && quadIntersection(p,
                                        dir,
                                        p2,p6,p7,p3,
                                        hit[hitCount]))
    {
         hitCount++;
    }

    if(hitCount > 0)
    {
        if(hitCount > 1)
        {
            float dist1 = PVector.sub(p,hit[0]).mag();
            float dist2 = PVector.sub(p,hit[1]).mag();
            
            if(dist1 < dist2)
            {
                hit1.set(hit[0]);
                hit2.set(hit[1]);
            }
            else
            {
                hit1.set(hit[1]);
                hit2.set(hit[0]);
            }

        }
        else
        {
            hit1.set(hit[0]);
        }
    }

    return hitCount;
}

int boxIntersection(PVector p,
                    PVector dir,
                    PVector boxCenter,
                    float boxWidth,
                    float boxHeigth,
                    float boxDepth,
                    PVector hit1,PVector hit2,
                    PVector hit1Normal,PVector hit2Normal)
{   
    return boxIntersection(new PMatrix3D(),
                           p,
                           dir,
                           boxCenter,
                           boxWidth,
                           boxHeigth,
                           boxDepth,
                           hit1,hit2,
                           hit1Normal,hit2Normal);
}

