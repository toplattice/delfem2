#include <stdio.h>
#include <cstdlib>
#include <cassert>
#include <math.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#if defined(__APPLE__) && defined(__MACH__) // Mac
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#elif defined(__MINGW32__) // probably I'm using Qt and don't want to use GLUT
  #include <GL/glu.h>
#elif defined(WIN32) // windows
  #include <windows.h>
  #include <GL/gl.h>
  #include <GL/glu.h>
#else // linux
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include "delfem2/funcs_gl.h"

static void UnitNormalAreaTri3D(double n[3], double& a, const double v1[3], const double v2[3], const double v3[3])
{
  n[0] = ( v2[1] - v1[1] )*( v3[2] - v1[2] ) - ( v3[1] - v1[1] )*( v2[2] - v1[2] );
  n[1] = ( v2[2] - v1[2] )*( v3[0] - v1[0] ) - ( v3[2] - v1[2] )*( v2[0] - v1[0] );
  n[2] = ( v2[0] - v1[0] )*( v3[1] - v1[1] ) - ( v3[0] - v1[0] )*( v2[1] - v1[1] );
  a = sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2])*0.5;
  const double invlen = 0.5/a;
  n[0]*=invlen;  n[1]*=invlen;  n[2]*=invlen;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void DrawAxis(double s)
{
  GLboolean is_lighting = ::glIsEnabled(GL_LIGHTING);
  ::glDisable(GL_LIGHTING);
  ::glBegin(GL_LINES);
  ::glColor3d(1,0,0);
  ::glVertex3d(0,0,0);
  ::glVertex3d(s,0,0);
  ::glColor3d(0,1,0);
  ::glVertex3d(0,0,0);
  ::glVertex3d(0,s,0);
  ::glColor3d(0,0,1);
  ::glVertex3d(0,0,0);
  ::glVertex3d(0,0,s);
  ::glEnd();
  if( is_lighting ){ ::glEnable(GL_LIGHTING); }
}

void CAxisXYZ::Draw() const{
  glLineWidth(line_width);
  DrawAxis(len);
}


void DrawSphere
(int nla, int nlo)
{
  if( nla <= 1 || nlo <= 2 ){ return; }
  const double pi = 3.1415926535;
  double dla = 2.0*pi/nla;
  double dlo = pi/nlo;
  ::glBegin(GL_QUADS);
  for(int ila=0;ila<nla;ila++){
    for(int ilo=0;ilo<nlo;ilo++){
      double rla0 = (ila+0)*dla;
      double rla1 = (ila+1)*dla;
      double rlo0 = (ilo+0)*dlo;
      double rlo1 = (ilo+1)*dlo;
      double p0[3] = { cos(rla0)*cos(rlo0), cos(rla0)*sin(rlo0), sin(rla0) };
      double p1[3] = { cos(rla0)*cos(rlo1), cos(rla0)*sin(rlo1), sin(rla0) };
      double p2[3] = { cos(rla1)*cos(rlo1), cos(rla1)*sin(rlo1), sin(rla1) };
      double p3[3] = { cos(rla1)*cos(rlo0), cos(rla1)*sin(rlo0), sin(rla1) };
      ::glVertex3dv(p0);
      ::glVertex3dv(p1);
      ::glVertex3dv(p2);
      ::glVertex3dv(p3);
    }
  }
  ::glEnd();
}

void DrawSphereAt(int nla, int nlo, double rad, double x, double y, double z)
{
  ::glTranslated(+x,+y,+z);
  ::glScaled(rad, rad, rad);
  DrawSphere(nla,nlo);
  ::glScaled(1.0/rad, 1.0/rad, 1.0/rad);
  ::glTranslated(-x,-y,-z);
}

void setSomeLighting()
{
  glEnable(GL_LIGHTING);
  //  glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);
//  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  {
    glEnable(GL_LIGHT0);
    GLfloat light0_Kd[]  = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat light0_Specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light0_Pos[4] = {0.25f, 1.0f, +1.25f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_Kd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_Specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_Pos);
  }
  {
    glEnable(GL_LIGHT1);
    GLfloat light1_Kd[]  = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light1_Pos[4] = {0.00f, 0.0f, +1.00f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_Kd);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_Pos);
  }
}

void setSomeLighting2()
{
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
  { // initialize light parameter
    GLfloat light0_Kd[]   = {0.9f, 0.3f, 0.3f, 1.0f};
    GLfloat light0_Pos[4] = {+0.5f, -0.5f, +1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_Kd);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_Pos);
    ////
    GLfloat light1_Kd[]   = {0.3f, 0.3f, 0.9f, 1.0f};
    GLfloat light1_Pos[4] = {-0.5f, +0.5f, +1.0f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_Kd);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_Pos);
  }
}

void setSomeLighting3()
{
  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);
  {
    glEnable(GL_LIGHT0);
    GLfloat light0_Kd[]  = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat light0_Specular[4] = {0.5f, 0.5f, .5f, 1.0f};
    GLfloat light0_Pos[4] = {0.25f, 1.0f, +1.25f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_Kd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_Specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_Pos);
  }
  {
    glEnable(GL_LIGHT1);
    GLfloat light1_Kd[]  = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light1_Pos[4] = {-1.00f, 0.0f, +1.00f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_Kd);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_Pos);
  }
}

static void ShadowMatrix(float m[16], float plane[4], float lpos[3])
{
  float dot = plane[0]*lpos[0] + plane[1]*lpos[1] + plane[2]*lpos[2] + plane[3];
  for(int j=0; j<4;++j){
    for(int i=0; i<4; ++i){
      m[j*4+i] = - plane[j]*lpos[i];
      if (i == j){ m[j*4+i] += dot; }
    }
  }
}

void drawFloorShadow(void (*DrawObject)(), float yfloor, float wfloor)
{
  GLboolean is_lighting = ::glIsEnabled(GL_LIGHTING);
  {
    ::glClearStencil(0);
    { // draw floor (stencil 1)
      glEnable(GL_STENCIL_TEST);
      glStencilFunc( GL_ALWAYS, 1, ~0);
      glStencilOp(GL_KEEP,GL_KEEP ,GL_REPLACE);
      { // floor
        ::glDisable(GL_LIGHTING);
        glColor4f(0.6f, 0.6f, 0.5f, 1.0f);
        ::glBegin(GL_QUADS);
        ::glNormal3d(0,1,0);
        ::glVertex3d(-wfloor,yfloor,-wfloor);
        ::glVertex3d(+wfloor,yfloor,-wfloor);
        ::glVertex3d(+wfloor,yfloor,+wfloor);
        ::glVertex3d(-wfloor,yfloor,+wfloor);
        ::glEnd();
      }
    }
    { // draw stensil
      glColorMask(0,0,0,0);
      glDepthMask(0);
      glEnable(GL_STENCIL_TEST);
      glStencilFunc( GL_EQUAL, 1, ~0);
      glStencilOp(GL_KEEP,GL_KEEP ,GL_INCR);
      glPushMatrix();
      {
        float plane[4] = {0,1,0,-yfloor-0.001f};
        float lpos[4] = {0,5,0,1};
        float m_shadow[16]; ShadowMatrix(m_shadow, plane, lpos);
        glMultMatrixf(m_shadow);
      }
      DrawObject();
      glPopMatrix();
      glColorMask(1,1,1,1);
      glDepthMask(1);
    }
    { // draw shadow
      glStencilFunc( GL_EQUAL, 2, ~0 );
      glStencilOp(GL_KEEP, GL_KEEP ,GL_KEEP);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      ::glDisable(GL_DEPTH_TEST);
      ::glDisable(GL_LIGHTING);
      { // draw shadow on floor
        ::glBegin(GL_QUADS);
        glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
        ::glNormal3d(0,1,0);
        ::glVertex3d(-wfloor,yfloor,-wfloor);
        ::glVertex3d(+wfloor,yfloor,-wfloor);
        ::glVertex3d(+wfloor,yfloor,+wfloor);
        ::glVertex3d(-wfloor,yfloor,+wfloor);
        ::glEnd();
      }
      glEnable(GL_DEPTH_TEST);
      glDisable(GL_BLEND);
      glDisable(GL_STENCIL_TEST);
    }
  }
  if( is_lighting ){ ::glEnable(GL_LIGHTING); }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static int Ptn035[]={   /* # */
   2,  12,66, 87,66,
   2,  12,33, 87,33,
   2,  37,91, 37, 8,
   2,  62, 8, 62,91,
  -1
};
static int PtnA[]={   /* A */
   3,   0, 0,  50,100, 100,0,
   2,  25,50,  75, 50,
  -1,
};
static int PtnB[]={   /* B */
  10,   0, 0, 0,100, 75,100, 87,91, 87,58, 75,50, 100,33, 100,8, 87,0, 0,0,
   2,  75,50, 0, 50,
  -1
};
static int PtnC[]={   /* C */
   8,  100,83, 75,100, 25,100, 0,83, 0,16, 25,0, 75,0, 100,16,
  -1,
};
static int PtnD[]={   /* D */
   7,  0,100, 75,100, 100,83, 100,16, 75,0, 0,0, 0,100,
  -1,
};
static int PtnE[]={   /* E */
   4,  100,100,  0,100, 0,0, 100,0,
   2,    0, 50, 87, 50,
  -1,
};
static int PtnF[]={   /* F */
   3,  100,100,  0,100, 0,0,
   2,    0, 50, 75, 50,
  -1,
};
static int PtnG[]={   /* G */
  10,  100,83, 75,100, 25,100, 0,83, 0,16, 25,0, 75,0, 100,16, 100,41, 62,41,
  -1,
};
///////////
static int Ptn3[]={   /* 3 */
  11,  12,83, 37,100, 75,100, 100,83, 100,66, 75,50, 100,33, 100,16, 75,0, 25,0, 0,16,
  -1
};
static int Ptn4[]={   /* 4 */
   3,  37,100, 12,25, 87,25,
   2,  62, 75, 62, 0,
  -1
};
static int Ptn5[]={   /* 5 */
  10,  87,100, 12,100, 12,41, 37,58, 62,58, 87,41, 87,16, 62,0, 37,0, 12,16,
  -1
};
static int Ptn6[]={   /* 6 */
  12,  87,83, 62,100, 25,100, 0,83, 0,16, 25,0, 75,0, 100,16, 100,33, 75,50, 25,50, 0,33,
  -1
};
static int Ptn7[]={   /* 7 */
   5,  12,83, 12,100, 87,100, 50,33, 50,0,
  -1
};
static int Ptn8[]={   /* 8 */
   9,  100,83, 75,100, 25,100,  0,83,  0,66,  25,50,  75,50, 100,66, 100,83,
   8,   25,50,  0, 33,  0, 16, 25, 0, 75, 0, 100,16, 100,33,  75,50,
  -1
};
static int Ptn9[]={   /* 9 */
  12,  0,16, 25,0, 75,0, 100,16, 100,83, 75,100, 25,100, 0,83, 0,58, 25,41, 75,41, 100,58,
  -1
};

// x = ax*[x] + bx
// y = ay*[y] + by
void DrawCharacter
(int* pChr,
 double ax, double bx,
 double ay, double by)
{
  assert(pChr!=0);
  int icur = 0;
  for(;;){
    int np = pChr[icur];
    if( np == -1 ) break;
    ::glBegin(GL_LINE_STRIP);
    for(int ip=0;ip<np;ip++){
      int ix0 = pChr[icur+1+ip*2+0];
      int iy0 = pChr[icur+1+ip*2+1];
      double sx = ix0*ax+bx;
      double sy = iy0*ay+by;
      ::glVertex2d(sx,sy);
    }
    ::glEnd();
    icur += np*2+1;
  }
}

// x = ax*[x] + bx
// y = ay*[y] + by
void DrawCharacter
(char ic,
 double ax, double bx,
 double ay, double by)
{
  int* pChr = 0;
  if( ic == '#'){ pChr = Ptn035; }
  if( ic == 'A'){ pChr = PtnA; }
  if( ic == 'B'){ pChr = PtnB; }
  if( ic == 'C'){ pChr = PtnC; }
  if( ic == 'D'){ pChr = PtnD; }
  if( ic == 'E'){ pChr = PtnE; }
  if( ic == 'F'){ pChr = PtnF; }
  if( ic == 'G'){ pChr = PtnG; }
  if( ic == '3'){ pChr = Ptn3; }
  if( ic == '4'){ pChr = Ptn4; }
  if( ic == '5'){ pChr = Ptn5; }
  if( ic == '6'){ pChr = Ptn6; }
  if( ic == '7'){ pChr = Ptn7; }
  if( ic == '8'){ pChr = Ptn8; }
  if( ic == '9'){ pChr = Ptn9; }  
  assert(pChr!=0);
  int icur = 0;
  for(;;){
    int np = pChr[icur];
    if( np == -1 ) break;
    ::glBegin(GL_LINE_STRIP);
    for(int ip=0;ip<np;ip++){
      int ix0 = pChr[icur+1+ip*2+0];
      int iy0 = pChr[icur+1+ip*2+1];
      double sx = ix0*ax+bx;
      double sy = iy0*ay+by;
      ::glVertex2d(sx,sy);
    }
    ::glEnd();
    icur += np*2+1;
  }
}


void myGlVertex3d
(unsigned int ixyz,
 const std::vector<double>& aXYZ )
{
  ::glVertex3d(aXYZ[ixyz*3+0], aXYZ[ixyz*3+1], aXYZ[ixyz*3+2] );
}

void myGlVertex2d
(unsigned int ixy,
 const std::vector<double>& aXY )
{
  ::glVertex2d(aXY[ixy*2+0], aXY[ixy*2+1] );
}

void myGlNorm3d
(unsigned int ixyz,
 const std::vector<double>& aNorm )
{
  ::glNormal3d(aNorm[ixyz*3+0], aNorm[ixyz*3+1], aNorm[ixyz*3+2] );
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void drawLoop2d
(const std::vector<double>& vec)
{
  ::glBegin(GL_LINES);
  const unsigned int nvec = (int)vec.size()/2;
  for(unsigned int ivec=0;ivec<nvec;ivec++){
    unsigned int jvec = ivec+1; if( jvec >= nvec ){ jvec -= nvec; }
    myGlVertex2d(ivec,vec);
    myGlVertex2d(jvec,vec);
  }
  ::glEnd();
  ////
  ::glBegin(GL_POINTS);
  for(unsigned int ivec=0;ivec<nvec;ivec++){
    myGlVertex2d(ivec,vec);
  }
  ::glEnd();
}

void DrawPoints2D_Vectors
(const double* aXY, int nXY,
 const double* aVal,
 int nstride,
 int noffset,
 double mag)
{
  ::glBegin(GL_LINES);
  for(int ino=0;ino<nXY;ino++){
    const double vx = aVal[ino*nstride+noffset+0]*mag;
    const double vy = aVal[ino*nstride+noffset+1]*mag;
    const double p0[2] = { aXY[ino*2+0],    aXY[ino*2+1]    };
    const double p1[2] = { aXY[ino*2+0]+vx, aXY[ino*2+1]+vy };
    ::glVertex2dv( p0 );
    ::glVertex2dv( p1 );
  }
  ::glEnd();
}

void DrawPoints2D_Points(std::vector<double>& aXY)
{
  const int nxys = (int)aXY.size()/2;
  ::glBegin(GL_POINTS);
  for(int ino=0;ino<nxys;ino++){
    const double p0[2] = { aXY[ino*2+0], aXY[ino*2+1] };
    ::glVertex2dv( p0 );
  }
  ::glEnd();
}

void DrawPoints3D_Points(std::vector<double>& aXYZ)
{
  const int nxyz = (int)aXYZ.size()/3;
  ::glBegin(GL_POINTS);
  for(int ino=0;ino<nxyz;ino++){
    const double p0[3] = { aXYZ[ino*3+0], aXYZ[ino*3+1], aXYZ[ino*3+2]};
    ::glVertex3dv( p0 );
  }
  ::glEnd();
}

void DrawAABB3D_Edge(double cx, double cy, double cz, double wx, double wy, double wz)
{
  const double pxyz[3] = {cx-0.5*wx,cy-0.5*wy,cz-0.5*wz};
  const double pxyZ[3] = {cx-0.5*wx,cy-0.5*wy,cz+0.5*wz};
  const double pxYz[3] = {cx-0.5*wx,cy+0.5*wy,cz-0.5*wz};
  const double pxYZ[3] = {cx-0.5*wx,cy+0.5*wy,cz+0.5*wz};
  const double pXyz[3] = {cx+0.5*wx,cy-0.5*wy,cz-0.5*wz};
  const double pXyZ[3] = {cx+0.5*wx,cy-0.5*wy,cz+0.5*wz};
  const double pXYz[3] = {cx+0.5*wx,cy+0.5*wy,cz-0.5*wz};
  const double pXYZ[3] = {cx+0.5*wx,cy+0.5*wy,cz+0.5*wz};
  ::glBegin(GL_LINES);
  ::glVertex3dv(pxyz); ::glVertex3dv(pxyZ);
  ::glVertex3dv(pxYz); ::glVertex3dv(pxYZ);
  ::glVertex3dv(pXyz); ::glVertex3dv(pXyZ);
  ::glVertex3dv(pXYz); ::glVertex3dv(pXYZ);
  ////
  ::glVertex3dv(pxyz); ::glVertex3dv(pXyz);
  ::glVertex3dv(pxyZ); ::glVertex3dv(pXyZ);
  ::glVertex3dv(pxYz); ::glVertex3dv(pXYz);
  ::glVertex3dv(pxYZ); ::glVertex3dv(pXYZ);
  ////
  ::glVertex3dv(pxyz); ::glVertex3dv(pxYz);
  ::glVertex3dv(pxyZ); ::glVertex3dv(pxYZ);
  ::glVertex3dv(pXyz); ::glVertex3dv(pXYz);
  ::glVertex3dv(pXyZ); ::glVertex3dv(pXYZ);
  ::glEnd();
}

void DrawAABB3D_Edge(const double cw[6])
{
  DrawAABB3D_Edge(cw[0], cw[1], cw[2], cw[3],cw[4], cw[5]);
}

void Draw_AABB3D_MinMaxXYZ_Edge
(double x_min, double x_max,
 double y_min, double y_max,
 double z_min, double z_max)
{
  const double pxyz[3] = {x_min,y_min,z_min};
  const double pxyZ[3] = {x_min,y_min,z_max};
  const double pxYz[3] = {x_min,y_max,z_min};
  const double pxYZ[3] = {x_min,y_max,z_max};
  const double pXyz[3] = {x_max,y_min,z_min};
  const double pXyZ[3] = {x_max,y_min,z_max};
  const double pXYz[3] = {x_max,y_max,z_min};
  const double pXYZ[3] = {x_max,y_max,z_max};
  ::glBegin(GL_LINES);
  ::glVertex3dv(pxyz); ::glVertex3dv(pxyZ);
  ::glVertex3dv(pxYz); ::glVertex3dv(pxYZ);
  ::glVertex3dv(pXyz); ::glVertex3dv(pXyZ);
  ::glVertex3dv(pXYz); ::glVertex3dv(pXYZ);
  ////
  ::glVertex3dv(pxyz); ::glVertex3dv(pXyz);
  ::glVertex3dv(pxyZ); ::glVertex3dv(pXyZ);
  ::glVertex3dv(pxYz); ::glVertex3dv(pXYz);
  ::glVertex3dv(pxYZ); ::glVertex3dv(pXYZ);
  ////
  ::glVertex3dv(pxyz); ::glVertex3dv(pxYz);
  ::glVertex3dv(pxyZ); ::glVertex3dv(pxYZ);
  ::glVertex3dv(pXyz); ::glVertex3dv(pXYz);
  ::glVertex3dv(pXyZ); ::glVertex3dv(pXYZ);
  ::glEnd();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void DrawSingleTri3D_FaceNorm
(const double* aXYZ,
 const int* aIndXYZ,
 const double* pUV)
{
  const int i0 = aIndXYZ[0]; //assert( i0>=0&&i0<(int)aXYZ.size()/3 );
  const int i1 = aIndXYZ[1]; //assert( i1>=0&&i1<(int)aXYZ.size()/3 );
  const int i2 = aIndXYZ[2]; //assert( i2>=0&&i2<(int)aXYZ.size()/3 );
  if( i0 == -1 ){
    assert(i1==-1); assert(i2==-1);
    return;
  }
  const double p0[3] = {aXYZ[i0*3+0], aXYZ[i0*3+1], aXYZ[i0*3+2]};
  const double p1[3] = {aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2]};
  const double p2[3] = {aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2]};
  double un[3], area; UnitNormalAreaTri3D(un,area, p0,p1,p2);
  ::glNormal3dv(un);
  if( pUV != 0 ){ ::glTexCoord2d(pUV[0],pUV[1]); }
  ::glVertex3dv(p0);
  if( pUV != 0 ){ ::glTexCoord2d(pUV[2],pUV[3]); }
  ::glVertex3dv(p1);
  if( pUV != 0 ){ ::glTexCoord2d(pUV[4],pUV[5]); }
  ::glVertex3dv(p2);
}

void DrawSingleQuad3D_FaceNorm
(const double* aXYZ,
 const int* aIndXYZ,
 const double* pUV)
{
  const int i0 = aIndXYZ[0]; //assert( i0 >= 0 && i0 < (int)aXYZ.size()/3 );
  const int i1 = aIndXYZ[1]; //assert( i1 >= 0 && i1 < (int)aXYZ.size()/3 );
  const int i2 = aIndXYZ[2]; //assert( i2 >= 0 && i2 < (int)aXYZ.size()/3 );
  const int i3 = aIndXYZ[3]; //assert( i3 >= 0 && i3 < (int)aXYZ.size()/3 );
  if( i0 == -1 ){
    assert(i1==-1 && i2==-1 && i3 ==-1);
    return;
  }
  const double p0[3] = {aXYZ[i0*3+0], aXYZ[i0*3+1], aXYZ[i0*3+2]};
  const double p1[3] = {aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2]};
  const double p2[3] = {aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2]};
  const double p3[3] = {aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2]};
  {
    double un0[3], area; UnitNormalAreaTri3D(un0,area,  p0, p1, p3);
    if( pUV != 0 ){ ::glTexCoord2d(pUV[0],pUV[1]); }
    ::glNormal3dv(un0);
    ::glVertex3dv(p0);
  }
  {
    double un1[3], area; UnitNormalAreaTri3D(un1,area,  p0, p1, p2);
    if( pUV != 0 ){ ::glTexCoord2d(pUV[2],pUV[3]); }
    ::glNormal3dv(un1);
    ::glVertex3dv(p1);
  }
  {
    double un2[3], area; UnitNormalAreaTri3D(un2,area,  p1, p2, p3);
    if( pUV != 0 ){ ::glTexCoord2d(pUV[4],pUV[5]); }
    ::glNormal3dv(un2);
    ::glVertex3dv(p2);
  }
  {
    double un3[3], area; UnitNormalAreaTri3D(un3,area,  p2, p3, p0);
    if( pUV != 0 ){ ::glTexCoord2d(pUV[6],pUV[7]); }
    ::glNormal3dv(un3);
    ::glVertex3dv(p3);
  }
}

void DrawMeshTri3D_FaceNorm
(const double* paXYZ,
 const int* paTri,
 int nTri)
{
  ::glBegin(GL_TRIANGLES);
  for(int itri=0;itri<nTri;++itri){
    DrawSingleTri3D_FaceNorm(paXYZ, paTri+itri*3,0);
  }
  ::glEnd();
}

void DrawMeshTri3D_FaceNorm
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri)
{
  DrawMeshTri3D_FaceNorm(aXYZ.data(), aTri.data(), aTri.size()/3);
}


void DrawMeshTri3D_FaceNorm_TexVtx
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri,
 const std::vector<double>& aTex)
{
  const int nTri = (int)aTri.size()/3;
  /////
  double uv[6];
  ::glBegin(GL_TRIANGLES);
  for(int itri=0;itri<nTri;++itri){
    const int ip0 = aTri[itri*3+0];
    const int ip1 = aTri[itri*3+1];
    const int ip2 = aTri[itri*3+2];
    uv[0] = aTex[ip0*2+0];
    uv[1] = aTex[ip0*2+1];
    uv[2] = aTex[ip1*2+0];
    uv[3] = aTex[ip1*2+1];
    uv[4] = aTex[ip2*2+0];
    uv[5] = aTex[ip2*2+1];
    DrawSingleTri3D_FaceNorm(aXYZ.data(), aTri.data()+itri*3,uv);
  }
  ::glEnd();
}

void DrawMeshTri3D_FaceNorm_TexFace
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri,
 const std::vector<double>& aTex)
{
  const int nTri = (int)aTri.size()/3;
  /////
  ::glBegin(GL_TRIANGLES);
  for(int itri=0;itri<nTri;++itri){
    DrawSingleTri3D_FaceNorm(aXYZ.data(),
                             aTri.data()+itri*3,
                             aTex.data()+itri*6);
  }
  ::glEnd();
}

void DrawMeshElem3D_FaceNorm
(const std::vector<double>& aXYZ,
 const std::vector<int>& aElemInd,
 const std::vector<int>& aElem)
{
  const int nelem = aElemInd.size()-1;
  for(int ielem=0;ielem<nelem;++ielem){
    const int ielemind0 = aElemInd[ielem];
    const int ielemind1 = aElemInd[ielem+1];
    if( ielemind1 - ielemind0 == 3 ){
      ::glBegin(GL_TRIANGLES);
      DrawSingleTri3D_FaceNorm(aXYZ.data(), aElem.data()+ielemind0,0);
      ::glEnd();
    }
    else if(ielemind1-ielemind0 == 4){
      ::glBegin(GL_QUADS);
      DrawSingleQuad3D_FaceNorm(aXYZ.data(),aElem.data()+ielemind0,0);
      ::glEnd();
    }
  }
}

void DrawMeshElem3D_FaceNorm
(const std::vector<double>& aXYZ,
 const std::vector<int>& aElemInd,
 const std::vector<int>& aElem,
 const std::vector<double>& aUV)
{
  const int nelem = aElemInd.size()-1;
  for(int ielem=0;ielem<nelem;++ielem){
    const int ielemind0 = aElemInd[ielem];
    const int ielemind1 = aElemInd[ielem+1];
    if( ielemind1 - ielemind0 == 3 ){
      ::glBegin(GL_TRIANGLES);
      DrawSingleTri3D_FaceNorm(aXYZ.data(),
                               aElem.data()+ielemind0,
                               aUV.data()+ielemind0*2);
      ::glEnd();
    }
    else if(ielemind1-ielemind0 == 4){
      ::glBegin(GL_QUADS);
      DrawSingleQuad3D_FaceNorm(aXYZ.data(),
                                aElem.data()+ielemind0,
                                aUV.data()+ielemind0*2);
      ::glEnd();
    }
  }
}

void DrawMeshElemPart3D_FaceNorm_TexPoEl
(const std::vector<double>& aXYZ,
 const std::vector<int>& aElemInd,
 const std::vector<int>& aElem,
 const std::vector<int>& aIndElemPart,
 const std::vector<double>& aUV)
{
  const bool isUV = (aUV.size()==aElem.size()*2);
  for(int iie=0;iie<(int)aIndElemPart.size();++iie){
    const int ielem = aIndElemPart[iie];
    const int ielemind0 = aElemInd[ielem];
    const int ielemind1 = aElemInd[ielem+1];
    const double* pUV = isUV ? aUV.data()+ielemind0*2:0;
    if( ielemind1 - ielemind0 == 3 ){
      ::glBegin(GL_TRIANGLES);
      DrawSingleTri3D_FaceNorm(aXYZ.data(),
                               aElem.data()+ielemind0,
                               pUV);
      ::glEnd();
    }
    else if(ielemind1-ielemind0 == 4){
      ::glBegin(GL_QUADS);
      DrawSingleQuad3D_FaceNorm(aXYZ.data(),
                                aElem.data()+ielemind0,
                                pUV);
      ::glEnd();
    }
  }
}

void DrawMeshTri3D_FaceNorm_XYsym
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri)
{
  const int nTri = (int)aTri.size()/3;
  /////
  ::glBegin(GL_TRIANGLES);
  for(int itri=0;itri<nTri;++itri){
    const int i1 = aTri[itri*3+0];
    const int i2 = aTri[itri*3+1];
    const int i3 = aTri[itri*3+2];
    if( i1 == -1 ){
      assert(i2==-1); assert(i3==-1);
      continue;
    }
    assert( i1 >= 0 && i1 < (int)aXYZ.size()/3 );
    assert( i2 >= 0 && i2 < (int)aXYZ.size()/3 );
    assert( i3 >= 0 && i3 < (int)aXYZ.size()/3 );
    double p1[3] = {aXYZ[i1*3+0], aXYZ[i1*3+1], -aXYZ[i1*3+2]};
    double p2[3] = {aXYZ[i2*3+0], aXYZ[i2*3+1], -aXYZ[i2*3+2]};
    double p3[3] = {aXYZ[i3*3+0], aXYZ[i3*3+1], -aXYZ[i3*3+2]};
    double un[3], area;
    UnitNormalAreaTri3D(un,area, p1,p3,p2);
    ::glNormal3dv(un);
    ::glVertex3dv(p1);
    ::glVertex3dv(p3);
    ::glVertex3dv(p2);
  }
  ::glEnd();
}


void DrawMeshTri3DPart_FaceNorm
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri,
 const std::vector<int>& aIndTri)
{
  ::glBegin(GL_TRIANGLES);
  for(int iitri=0;iitri<(int)aIndTri.size();++iitri){
    const int itri = aIndTri[iitri];
    assert( itri>=0&&itri<(int)aTri.size()/3 );
    DrawSingleTri3D_FaceNorm(aXYZ.data(), aTri.data()+itri*3,0);
  }
  ::glEnd();
}

void DrawMeshTri3D_FaceNorm_Flg
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri,
 int iflg,
 const std::vector<int>& aFlgTri)
{
  const int nTri = (int)aTri.size()/3;
//  const int nXYZ = (int)aXYZ.size()/3;
  /////
  ::glBegin(GL_TRIANGLES);
  for(int itri=0;itri<nTri;++itri){
    const int iflg0 = aFlgTri[itri];
    if( iflg0 != iflg ) continue;
    DrawSingleTri3D_FaceNorm(aXYZ.data(), aTri.data()+itri*3,0);
  }
  ::glEnd();
}

void DrawMeshTri3D_FaceNormEdge
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri)
{
  GLboolean is_lighting = glIsEnabled(GL_LIGHTING);
  ////
  const int nTri = (int)aTri.size()/3;
  /////
  ::glBegin(GL_TRIANGLES);
  for (int itri=0; itri<nTri; ++itri){
    DrawSingleTri3D_FaceNorm(aXYZ.data(), aTri.data()+itri*3,0);
  }
  ::glEnd();

  ::glDisable(GL_LIGHTING);
  ::glBegin(GL_LINES);
  ::glColor3d(0, 0, 0);
  for (int itri = 0; itri<nTri; ++itri){
    const int i1 = aTri[itri*3+0];
    const int i2 = aTri[itri*3+1];
    const int i3 = aTri[itri*3+2];
    if( i1 == -1 ){
      assert(i2==-1); assert(i3==-1);
      continue;
    }
    assert(i1>=0&&i1 < (int)aXYZ.size()/3 );
    assert(i2>=0&&i2 < (int)aXYZ.size()/3 );
    assert(i3>=0&&i3 < (int)aXYZ.size()/3 );
    const double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    const double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    const double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    ::glVertex3dv(p1); ::glVertex3dv(p2);
    ::glVertex3dv(p2); ::glVertex3dv(p3);
    ::glVertex3dv(p3); ::glVertex3dv(p1);
  }
  ::glEnd();

  if (is_lighting){ ::glEnable(GL_LIGHTING); }
}

void DrawMeshTri3D_Edge
(const double* aXYZ, int nXYZ,
 const int* aTri, int nTri)
{
  GLboolean is_lighting = glIsEnabled(GL_LIGHTING);
  ////
//  const int nTri = (int)aTri.size()/3;
//  const int nXYZ = (int)aXYZ.size()/3;
  /////
  ::glDisable(GL_LIGHTING);
  ::glBegin(GL_LINES);
  ::glColor3d(0, 0, 0);
  for (int itri = 0; itri<nTri; ++itri){
    const int i1 = aTri[itri*3+0];
    const int i2 = aTri[itri*3+1];
    const int i3 = aTri[itri*3+2];
    if( i1 == -1 ){
      assert(i2==-1); assert(i3==-1);
      continue;
    }
    assert(i1>=0&&i1 < nXYZ);
    assert(i2>=0&&i2 < nXYZ);
    assert(i3>=0&&i3 < nXYZ);
    double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    ::glVertex3dv(p1); ::glVertex3dv(p2);
    ::glVertex3dv(p2); ::glVertex3dv(p3);
    ::glVertex3dv(p3); ::glVertex3dv(p1);
  }
  ::glEnd();
  
  if (is_lighting){ ::glEnable(GL_LIGHTING); }
}

void DrawMeshTri3D_Edge
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri)
{
  DrawMeshTri3D_Edge(aXYZ.data(), aXYZ.size()/3,
                     aTri.data(), aTri.size()/3);
}

void DrawMeshTriMap3D_Edge
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri,
 const std::vector<int>& map)
{
  GLboolean is_lighting = glIsEnabled(GL_LIGHTING);
  ////
  const int nTri = (int)aTri.size()/3;
  const int nXYZ = (int)aXYZ.size()/3;
  /////
  ::glDisable(GL_LIGHTING);
  ::glBegin(GL_LINES);
  ::glColor3d(0, 0, 0);
  for (int itri = 0; itri<nTri; ++itri){
    const int j1 = aTri[itri*3+0];
    const int j2 = aTri[itri*3+1];
    const int j3 = aTri[itri*3+2];
    if( j1 == -1 ){
      assert(j2==-1); assert(j3==-1);
      continue;
    }
    const int i1 = map[j1];
    const int i2 = map[j2];
    const int i3 = map[j3];
    assert(i1>=0 && i1<nXYZ);
    assert(i2>=0 && i2<nXYZ);
    assert(i3>=0 && i3<nXYZ);
    double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    ::glVertex3dv(p1); ::glVertex3dv(p2);
    ::glVertex3dv(p2); ::glVertex3dv(p3);
    ::glVertex3dv(p3); ::glVertex3dv(p1);
  }
  ::glEnd();
  
  if (is_lighting){ ::glEnable(GL_LIGHTING); }
}

void DrawMeshTri3D_FaceNorm
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri,
 const std::vector<double>& aNorm)
{
  unsigned int nTri = (int)aTri.size()/3;
  //  unsigned int nXYZ = (int)aXYZ.size()/3;
  /////
  ::glBegin(GL_TRIANGLES);
  for(unsigned int itri=0;itri<nTri;itri++){
    const unsigned int i1 = aTri[itri*3+0];
    const unsigned int i2 = aTri[itri*3+1];
    const unsigned int i3 = aTri[itri*3+2];
    ::myGlNorm3d(i1,aNorm);  ::myGlVertex3d(i1,aXYZ);
    ::myGlNorm3d(i2,aNorm);  ::myGlVertex3d(i2,aXYZ);
    ::myGlNorm3d(i3,aNorm);  ::myGlVertex3d(i3,aXYZ);
  }
  ::glEnd();
}

void DrawMeshTri3D_FaceNorm
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTriVtx,
 const std::vector<double>& aNorm,
 const std::vector<int>& aTriNrm)
{
  const int nTri = (int)aTriVtx.size()/3;
  assert( (int)aTriNrm.size() == nTri*3 );
  //  unsigned int nXYZ = (int)aXYZ.size()/3;
  /////
  ::glBegin(GL_TRIANGLES);
  for(int itri=0;itri<nTri;itri++){
    const int iv1 = aTriVtx[itri*3+0];
    const int iv2 = aTriVtx[itri*3+1];
    const int iv3 = aTriVtx[itri*3+2];
    const int in1 = aTriNrm[itri*3+0];
    const int in2 = aTriNrm[itri*3+1];
    const int in3 = aTriNrm[itri*3+2];
    bool bn1=in1>=0&&in1*3<aNorm.size();
    bool bn2=in2>=0&&in2*3<aNorm.size();
    bool bn3=in3>=0&&in3*3<aNorm.size();
    bool bv1=iv1>=0&&iv1*3<aXYZ.size();
    bool bv2=iv2>=0&&iv2*3<aXYZ.size();
    bool bv3=iv3>=0&&iv3*3<aXYZ.size();
    bool bn123 = bn1 && bn2 && bn3;
    bool bv123 = bv1 && bv2 && bv3;
    if( bn123 && bv123 ){
      ::myGlNorm3d(in1, aNorm); ::myGlVertex3d(iv1, aXYZ);
      ::myGlNorm3d(in2, aNorm); ::myGlVertex3d(iv2, aXYZ);
      ::myGlNorm3d(in3, aNorm); ::myGlVertex3d(iv3, aXYZ);
    }
    else if( bv123 ){
      ::myGlVertex3d(iv1, aXYZ);
      ::myGlVertex3d(iv2, aXYZ);
      ::myGlVertex3d(iv3, aXYZ);
    }
  }
  ::glEnd();
}

void DrawMeshTri3D_FaceEdge
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTri)
{
  const int nTri = (int)aTri.size()/3;
  /////
  ::glBegin(GL_TRIANGLES);
  for(int itri=0;itri<nTri;itri++){
    const int i1 = aTri[itri*3+0];
    const int i2 = aTri[itri*3+1];
    const int i3 = aTri[itri*3+2];
    ::myGlVertex3d(i1,aXYZ);
    ::myGlVertex3d(i2,aXYZ);
    ::myGlVertex3d(i3,aXYZ);
  }
  ::glEnd();
  ////////
  ::glColor3d(0,0,0);
  ::glBegin(GL_LINES);
  for(int itri=0;itri<nTri;itri++){
    const unsigned int i1 = aTri[itri*3+0];
    const unsigned int i2 = aTri[itri*3+1];
    const unsigned int i3 = aTri[itri*3+2];
    ::myGlVertex3d(i1,aXYZ);
    ::myGlVertex3d(i2,aXYZ);
    ::myGlVertex3d(i2,aXYZ);
    ::myGlVertex3d(i3,aXYZ);
    ::myGlVertex3d(i3,aXYZ);
    ::myGlVertex3d(i1,aXYZ);
  }
  ::glEnd();
}

/////////////////////////////////////

void DrawMeshTri2D_Face
(const std::vector<int>& aTri,
 const std::vector<double>& aXY)
{
  const int ntri = (int)aTri.size()/3;
  //  const int nxys = (int)aXY.size()/2;
  ::glBegin(GL_TRIANGLES);
  for(int itri=0;itri<ntri;itri++){
    //      double color[3];
    const int i0 = aTri[itri*3+0];
    const int i1 = aTri[itri*3+1];
    const int i2 = aTri[itri*3+2];
    const double p0[2] = { aXY[i0*2+0], aXY[i0*2+1] };
    const double p1[2] = { aXY[i1*2+0], aXY[i1*2+1] };
    const double p2[2] = { aXY[i2*2+0], aXY[i2*2+1] };
    ::glVertex2dv( p0 );
    ::glVertex2dv( p1 );
    ::glVertex2dv( p2 );
  }
  ::glEnd();
}

void DrawMeshTri2D_FaceDisp2D
(const double* aXY, int nXY,
 const int* aTri, int nTri,
 const double* aDisp, int nstride)
{
  //  const int nxys = (int)aXY.size()/2;
  ::glColor3d(1,1,1);
  ::glBegin(GL_TRIANGLES);
  for(int itri=0;itri<nTri;itri++){
    //      double color[3];
    const int i0 = aTri[itri*3+0];
    const int i1 = aTri[itri*3+1];
    const int i2 = aTri[itri*3+2];
    const double p0[2] = { aXY[i0*2+0]+aDisp[i0*nstride+0], aXY[i0*2+1]+aDisp[i0*nstride+1] };
    const double p1[2] = { aXY[i1*2+0]+aDisp[i1*nstride+0], aXY[i1*2+1]+aDisp[i1*nstride+1] };
    const double p2[2] = { aXY[i2*2+0]+aDisp[i2*nstride+0], aXY[i2*2+1]+aDisp[i2*nstride+1] };
    ::glVertex2dv( p0 );
    ::glVertex2dv( p1 );
    ::glVertex2dv( p2 );
  }
  ::glEnd();
  ////////////////
  ::glDisable(GL_LIGHTING);
  ::glColor3d(0,0,0);
  ::glBegin(GL_LINES);
  for(int itri=0;itri<nTri;itri++){
    const int i0 = aTri[itri*3+0];
    const int i1 = aTri[itri*3+1];
    const int i2 = aTri[itri*3+2];
    const double p0[2] = { aXY[i0*2+0]+aDisp[i0*nstride+0], aXY[i0*2+1]+aDisp[i0*nstride+1] };
    const double p1[2] = { aXY[i1*2+0]+aDisp[i1*nstride+0], aXY[i1*2+1]+aDisp[i1*nstride+1] };
    const double p2[2] = { aXY[i2*2+0]+aDisp[i2*nstride+0], aXY[i2*2+1]+aDisp[i2*nstride+1] };
    ::glVertex2dv( p0 ); ::glVertex2dv( p1 );
    ::glVertex2dv( p1 ); ::glVertex2dv( p2 );
    ::glVertex2dv( p2 ); ::glVertex2dv( p0 );
  }
  ::glEnd();
}


void DrawMeshTri2D_Edge
(const double* aXY, int nXY,
 const int* aTri, int nTri)
{
  //  const unsigned int nxys = (int)aXY.size()/2;
  ////////////////
  ::glColor3d(0,0,0);
  ::glBegin(GL_LINES);
  for(int itri=0;itri<nTri;itri++){
    const unsigned int ino0 = aTri[itri*3+0];
    const unsigned int ino1 = aTri[itri*3+1];
    const unsigned int ino2 = aTri[itri*3+2];
    ::glVertex2d( aXY[ino0*2+0], aXY[ino0*2+1] );
    ::glVertex2d( aXY[ino1*2+0], aXY[ino1*2+1] );
    ::glVertex2d( aXY[ino1*2+0], aXY[ino1*2+1] );
    ::glVertex2d( aXY[ino2*2+0], aXY[ino2*2+1] );
    ::glVertex2d( aXY[ino2*2+0], aXY[ino2*2+1] );
    ::glVertex2d( aXY[ino0*2+0], aXY[ino0*2+1] );
  }
  ::glEnd();
}

void DrawMeshTri2D_Edge
(const std::vector<int>& aTri,
 const std::vector<double>& aXY)
{
  DrawMeshTri2D_Edge(aXY.data(), aXY.size()/2,
                     aTri.data(), aTri.size()/3);
}



////////////////////////////////////////////////////////////////////////////////
/// Quad

void DrawMeshQuad3D_Edge
(const double* aXYZ, int nXYZ,
 const int* aQuad, int nQuad)
{
  GLboolean is_lighting = glIsEnabled(GL_LIGHTING);
  ////
  ::glDisable(GL_LIGHTING);
  ::glBegin(GL_LINES);
  ::glColor3d(0, 0, 0);
  for (int iq = 0; iq<nQuad; ++iq){
    const int i1 = aQuad[iq*4+0];
    const int i2 = aQuad[iq*4+1];
    const int i3 = aQuad[iq*4+2];
    const int i4 = aQuad[iq*4+3];
    if( i1 == -1 ){
      assert(i2==-1); assert(i3==-1);
      continue;
    }
    assert(i1>=0&&i1<nXYZ);
    assert(i2>=0&&i2<nXYZ);
    assert(i3>=0&&i3<nXYZ);
    assert(i4>=0&&i4<nXYZ);
    double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    double p4[3] = { aXYZ[i4*3+0], aXYZ[i4*3+1], aXYZ[i4*3+2] };
    ::glVertex3dv(p1); ::glVertex3dv(p2);
    ::glVertex3dv(p2); ::glVertex3dv(p3);
    ::glVertex3dv(p3); ::glVertex3dv(p4);
    ::glVertex3dv(p4); ::glVertex3dv(p1);
  }
  ::glEnd();
  if (is_lighting){ ::glEnable(GL_LIGHTING); }
}

void DrawMeshQuad3D_Edge
(const std::vector<double>& aXYZ,
 const std::vector<int>& aQuad)
{
  DrawMeshQuad3D_Edge(aXYZ.data(), aXYZ.size()/3,
                      aQuad.data(), aQuad.size()/4);
}


void DrawMeshQuad3D_FaceNorm
(const double* aXYZ,
 const int* aQuad, const int nQuad)
{
  ::glBegin(GL_QUADS);
  for(unsigned int iq=0;iq<nQuad;++iq){
    DrawSingleQuad3D_FaceNorm(aXYZ, aQuad+iq*4, 0);
  }
  ::glEnd();
}

void DrawMeshQuad3D_FaceNorm
(const std::vector<double>& aXYZ,
 const std::vector<int>& aQuad)
{
  DrawMeshQuad3D_FaceNorm(aXYZ.data(),aQuad.data(),aQuad.size()/4);
}


void DrawMeshQuad2D_Edge
(const double* aXY, int nXY,
 const int* aQuad, int nQuad)
{
  GLboolean is_lighting = glIsEnabled(GL_LIGHTING);
  /////
  ::glDisable(GL_LIGHTING);
  ::glBegin(GL_LINES);
  ::glColor3d(0, 0, 0);
  for (int iq = 0; iq<nQuad; ++iq){
    const int i1 = aQuad[iq*4+0];
    const int i2 = aQuad[iq*4+1];
    const int i3 = aQuad[iq*4+2];
    const int i4 = aQuad[iq*4+3];
    if( i1 == -1 ){
      assert(i2==-1); assert(i3==-1);
      continue;
    }
    assert(i1>=0&&i1<nXY);
    assert(i2>=0&&i2<nXY);
    assert(i3>=0&&i3<nXY);
    assert(i4>=0&&i4<nXY);
    double p1[2] = { aXY[i1*2+0], aXY[i1*2+1] };
    double p2[2] = { aXY[i2*2+0], aXY[i2*2+1] };
    double p3[2] = { aXY[i3*2+0], aXY[i3*2+1] };
    double p4[2] = { aXY[i4*2+0], aXY[i4*2+1] };
    ::glVertex2dv(p1); ::glVertex2dv(p2);
    ::glVertex2dv(p2); ::glVertex2dv(p3);
    ::glVertex2dv(p3); ::glVertex2dv(p4);
    ::glVertex2dv(p4); ::glVertex2dv(p1);
  }
  ::glEnd();
  if (is_lighting){ ::glEnable(GL_LIGHTING); }
}

void DrawMeshQuad2D_Edge
(const std::vector<double>& aXY,
 const std::vector<int>& aQuad)
{
  DrawMeshQuad2D_Edge(aXY.data(), aXY.size()/2,
                      aQuad.data(), aQuad.size()/4);
}

///////////////////////////////////////////

void DrawMeshTet3DSurface_FaceNorm
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTet,
 const std::vector<int>& aTetFace)
{
  const unsigned int noelTetFace[4][3] = {
    { 1, 2, 3 },
    { 0, 3, 2 },
    { 0, 1, 3 },
    { 0, 2, 1 } };
  //  const int nTri = (int)aTri.size()/3;
  // const int nXYZ = (int)aXYZ.size()/3;
  /////
  ::glBegin(GL_TRIANGLES);
  for(int itf=0;itf<(int)aTetFace.size()/2;++itf){
    int itet = aTetFace[itf*2+0];
    int iface = aTetFace[itf*2+1];
    const int i1 = aTet[itet*4+noelTetFace[iface][0]];
    const int i2 = aTet[itet*4+noelTetFace[iface][1]];
    const int i3 = aTet[itet*4+noelTetFace[iface][2]];
    if( i1 == -1 ){
      assert(i2==-1); assert(i3==-1);
      continue;
    }
    assert( i1 >= 0 && i1 < (int)aXYZ.size()/3 );
    assert( i2 >= 0 && i2 < (int)aXYZ.size()/3 );
    assert( i3 >= 0 && i3 < (int)aXYZ.size()/3 );
    double p1[3] = {aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2]};
    double p2[3] = {aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2]};
    double p3[3] = {aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2]};
    double un[3], area;
    UnitNormalAreaTri3D(un,area, p1,p2,p3);
    ::glNormal3dv(un);
    ::myGlVertex3d(i1,aXYZ);
    ::myGlVertex3d(i2,aXYZ);
    ::myGlVertex3d(i3,aXYZ);
  }
  ::glEnd();
}

void DrawMeshTet3DSurface_Edge
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTet,
 const std::vector<int>& aTetFace)
{
  const unsigned int noelTetFace[4][3] = {
    { 1, 2, 3 },
    { 0, 3, 2 },
    { 0, 1, 3 },
    { 0, 2, 1 } };
  
  GLboolean is_lighting = glIsEnabled(GL_LIGHTING);
  ////
  const int nXYZ = (int)aXYZ.size()/3;
  /////
  ::glDisable(GL_LIGHTING);
  ::glBegin(GL_LINES);
  ::glColor3d(0, 0, 0);
  for (int itf=0; itf<(int)aTetFace.size()/2;++itf){
    int itet = aTetFace[itf*2+0];
    int iface = aTetFace[itf*2+1];
    const int i1 = aTet[itet*4+noelTetFace[iface][0]];
    const int i2 = aTet[itet*4+noelTetFace[iface][1]];
    const int i3 = aTet[itet*4+noelTetFace[iface][2]];
    if( i1 == -1 ){
      assert(i2==-1); assert(i3==-1);
      continue;
    }
    assert(i1>=0&&i1 < nXYZ);
    assert(i2>=0&&i2 < nXYZ);
    assert(i3>=0&&i3 < nXYZ);
    double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    ::glVertex3dv(p1); ::glVertex3dv(p2);
    ::glVertex3dv(p2); ::glVertex3dv(p3);
    ::glVertex3dv(p3); ::glVertex3dv(p1);
  }
  ::glEnd();
  
  if (is_lighting){ ::glEnable(GL_LIGHTING); }
}

void DrawMeshTet3D_Edge
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTet)
{
  for (int itet = 0; itet<(int)aTet.size()/4; itet++){
    const int i0 = aTet[itet*4+0];
    const int i1 = aTet[itet*4+1];
    const int i2 = aTet[itet*4+2];
    const int i3 = aTet[itet*4+3];
    const double p0[3] = { aXYZ[i0*3+0], aXYZ[i0*3+1], aXYZ[i0*3+2] };
    const double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    const double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    const double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    //::glColor3d(0, 0, 0);
    ::glBegin(GL_LINES);
    ::glVertex3dv(p0); ::glVertex3dv(p1);
    ::glVertex3dv(p0); ::glVertex3dv(p2);
    ::glVertex3dv(p0); ::glVertex3dv(p3);
    ::glVertex3dv(p1); ::glVertex3dv(p2);
    ::glVertex3dv(p1); ::glVertex3dv(p3);
    ::glVertex3dv(p2); ::glVertex3dv(p3);
    ::glEnd();
  }
}

void DrawMeshTet3D_EdgeDisp
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTet,
 const std::vector<double>& aDisp)
{
  for (int itet = 0; itet<(int)aTet.size()/4; itet++){
    const int i0 = aTet[itet*4+0];
    const int i1 = aTet[itet*4+1];
    const int i2 = aTet[itet*4+2];
    const int i3 = aTet[itet*4+3];
    const double p0[3] = { aXYZ[i0*3+0], aXYZ[i0*3+1], aXYZ[i0*3+2] };
    const double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    const double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    const double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    double q0[3] = { p0[0]+aDisp[i0*3+0], p0[1]+aDisp[i0*3+1], p0[2]+aDisp[i0*3+2] };
    double q1[3] = { p1[0]+aDisp[i1*3+0], p1[1]+aDisp[i1*3+1], p1[2]+aDisp[i1*3+2] };
    double q2[3] = { p2[0]+aDisp[i2*3+0], p2[1]+aDisp[i2*3+1], p2[2]+aDisp[i2*3+2] };
    double q3[3] = { p3[0]+aDisp[i3*3+0], p3[1]+aDisp[i3*3+1], p3[2]+aDisp[i3*3+2] };
    ::glBegin(GL_LINES);
    ::glVertex3dv(q0); ::glVertex3dv(q1);
    ::glVertex3dv(q0); ::glVertex3dv(q2);
    ::glVertex3dv(q0); ::glVertex3dv(q3);
    ::glVertex3dv(q1); ::glVertex3dv(q2);
    ::glVertex3dv(q1); ::glVertex3dv(q3);
    ::glVertex3dv(q2); ::glVertex3dv(q3);
    ::glEnd();
  }
}

void DrawMeshTet3D_FaceNormal
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTet)
{
  for (int itet = 0; itet<(int)aTet.size()/4; itet++){
    const int i0 = aTet[itet*4+0];
    const int i1 = aTet[itet*4+1];
    const int i2 = aTet[itet*4+2];
    const int i3 = aTet[itet*4+3];
    const double p0[3] = { aXYZ[i0*3+0], aXYZ[i0*3+1], aXYZ[i0*3+2] };
    const double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    const double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    const double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    double un0[3], a0; UnitNormalAreaTri3D(un0,a0, p1,p2,p3);
    double un1[3], a1; UnitNormalAreaTri3D(un1,a1, p2,p0,p3);
    double un2[3], a2; UnitNormalAreaTri3D(un2,a2, p3,p0,p1);
    double un3[3], a3; UnitNormalAreaTri3D(un3,a3, p0,p2,p1);
    //    ::glColor3d(0, 0, 0);
    ::glBegin(GL_TRIANGLES);
    ::glNormal3dv(un0); ::glVertex3dv(p1); ::glVertex3dv(p2); ::glVertex3dv(p3);
    ::glNormal3dv(un1); ::glVertex3dv(p2); ::glVertex3dv(p3); ::glVertex3dv(p0);
    ::glNormal3dv(un2); ::glVertex3dv(p3); ::glVertex3dv(p0); ::glVertex3dv(p1);
    ::glNormal3dv(un3); ::glVertex3dv(p0); ::glVertex3dv(p1); ::glVertex3dv(p2);
    ::glEnd();
  }
}

void DrawHex3D_FaceNorm
(const std::vector<double>& aXYZ,
 const std::vector<int>& aHex)
{
  const int noelElemFace_Hex[8][4] = { // this is corresponds to VTK_VOXEL
    { 0, 4, 7, 3 }, // -x
    { 1, 2, 6, 5 }, // +x
    { 0, 1, 5, 4 }, // -y
    { 3, 7, 6, 2 }, // +y
    { 0, 3, 2, 1 }, // -z
    { 4, 5, 6, 7 }  // +z
  };
  ::glBegin(GL_TRIANGLES);
  for (int ihex = 0; ihex<(int)aHex.size()/8; ihex++){
    const int i0 = aHex[ihex*8+0];
    const int i1 = aHex[ihex*8+1];
    const int i2 = aHex[ihex*8+2];
    const int i3 = aHex[ihex*8+3];
    const int i4 = aHex[ihex*8+4];
    const int i5 = aHex[ihex*8+5];
    const int i6 = aHex[ihex*8+6];
    const int i7 = aHex[ihex*8+7];
    const double p0[3] = { aXYZ[i0*3+0], aXYZ[i0*3+1], aXYZ[i0*3+2] };
    const double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    const double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    const double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    const double p4[3] = { aXYZ[i4*3+0], aXYZ[i4*3+1], aXYZ[i4*3+2] };
    const double p5[3] = { aXYZ[i5*3+0], aXYZ[i5*3+1], aXYZ[i5*3+2] };
    const double p6[3] = { aXYZ[i6*3+0], aXYZ[i6*3+1], aXYZ[i6*3+2] };
    const double p7[3] = { aXYZ[i7*3+0], aXYZ[i7*3+1], aXYZ[i7*3+2] };
    const double* aP[8] = {p0,p1,p2,p3,p4,p5,p6,p7};
    for(int iface=0;iface<6;++iface){
      const double* q0 = aP[ noelElemFace_Hex[iface][0] ];
      const double* q1 = aP[ noelElemFace_Hex[iface][1] ];
      const double* q2 = aP[ noelElemFace_Hex[iface][2] ];
      const double* q3 = aP[ noelElemFace_Hex[iface][3] ];
      double un0[3], a0; UnitNormalAreaTri3D(un0,a0, q0,q1,q2);
      ::glNormal3dv(un0); ::glVertex3dv(q0); ::glVertex3dv(q1); ::glVertex3dv(q2);
      double un1[3], a1; UnitNormalAreaTri3D(un1,a1, q0,q2,q3);
      ::glNormal3dv(un1); ::glVertex3dv(q0); ::glVertex3dv(q2); ::glVertex3dv(q3);
    }
  }
  ::glEnd();
}

void DrawHex3D_FaceNormDirp
(const std::vector<double>& aXYZ,
 const std::vector<int>& aHex,
 const std::vector<double>& aDisp)
{
  const int noelElemFace_Hex[8][4] = { // this is corresponds to VTK_VOXEL
    { 0, 4, 7, 3 }, // -x
    { 1, 2, 6, 5 }, // +x
    { 0, 1, 5, 4 }, // -y
    { 3, 7, 6, 2 }, // +y
    { 0, 3, 2, 1 }, // -z
    { 4, 5, 6, 7 }  // +z
  };
  ::glBegin(GL_TRIANGLES);
  for (int ihex = 0; ihex<(int)aHex.size()/8; ihex++){
    const int i0 = aHex[ihex*8+0];
    const int i1 = aHex[ihex*8+1];
    const int i2 = aHex[ihex*8+2];
    const int i3 = aHex[ihex*8+3];
    const int i4 = aHex[ihex*8+4];
    const int i5 = aHex[ihex*8+5];
    const int i6 = aHex[ihex*8+6];
    const int i7 = aHex[ihex*8+7];
    const double p0[3] = { aXYZ[i0*3+0], aXYZ[i0*3+1], aXYZ[i0*3+2] };
    const double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    const double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    const double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    const double p4[3] = { aXYZ[i4*3+0], aXYZ[i4*3+1], aXYZ[i4*3+2] };
    const double p5[3] = { aXYZ[i5*3+0], aXYZ[i5*3+1], aXYZ[i5*3+2] };
    const double p6[3] = { aXYZ[i6*3+0], aXYZ[i6*3+1], aXYZ[i6*3+2] };
    const double p7[3] = { aXYZ[i7*3+0], aXYZ[i7*3+1], aXYZ[i7*3+2] };
    ////
    const double r0[3] = { p0[0]+aDisp[i0*3+0], p0[1]+aDisp[i0*3+1], p0[2]+aDisp[i0*3+2] };
    const double r1[3] = { p1[0]+aDisp[i1*3+0], p1[1]+aDisp[i1*3+1], p1[2]+aDisp[i1*3+2] };
    const double r2[3] = { p2[0]+aDisp[i2*3+0], p2[1]+aDisp[i2*3+1], p2[2]+aDisp[i2*3+2] };
    const double r3[3] = { p3[0]+aDisp[i3*3+0], p3[1]+aDisp[i3*3+1], p3[2]+aDisp[i3*3+2] };
    const double r4[3] = { p4[0]+aDisp[i4*3+0], p4[1]+aDisp[i4*3+1], p4[2]+aDisp[i4*3+2] };
    const double r5[3] = { p5[0]+aDisp[i5*3+0], p5[1]+aDisp[i5*3+1], p5[2]+aDisp[i5*3+2] };
    const double r6[3] = { p6[0]+aDisp[i6*3+0], p6[1]+aDisp[i6*3+1], p6[2]+aDisp[i6*3+2] };
    const double r7[3] = { p7[0]+aDisp[i7*3+0], p7[1]+aDisp[i7*3+1], p7[2]+aDisp[i7*3+2] };
    const double* aR[8] = {r0,r1,r2,r3,r4,r5,r6,r7};
    for(int iface=0;iface<6;++iface){
      const double* q0 = aR[ noelElemFace_Hex[iface][0] ];
      const double* q1 = aR[ noelElemFace_Hex[iface][1] ];
      const double* q2 = aR[ noelElemFace_Hex[iface][2] ];
      const double* q3 = aR[ noelElemFace_Hex[iface][3] ];
      double un0[3], a0; UnitNormalAreaTri3D(un0,a0, q0,q1,q2);
      ::glNormal3dv(un0); ::glVertex3dv(q0); ::glVertex3dv(q1); ::glVertex3dv(q2);
      double un1[3], a1; UnitNormalAreaTri3D(un1,a1, q0,q2,q3);
      ::glNormal3dv(un1); ::glVertex3dv(q0); ::glVertex3dv(q2); ::glVertex3dv(q3);
    }
  }
  ::glEnd();
}

void DrawHex3D_Edge
(const std::vector<double>& aXYZ,
 const std::vector<int>& aHex)
{
  const int noelEdge_Hex[12][2] = {
    {0,1},{3,2},{4,5},{7,6},
    {0,3},{1,2},{4,7},{5,6},
    {0,4},{1,5},{3,7},{2,6} };
  ::glBegin(GL_LINES);
  for (int ihex = 0; ihex<(int)aHex.size()/8; ihex++){
    const int i0 = aHex[ihex*8+0];
    const int i1 = aHex[ihex*8+1];
    const int i2 = aHex[ihex*8+2];
    const int i3 = aHex[ihex*8+3];
    const int i4 = aHex[ihex*8+4];
    const int i5 = aHex[ihex*8+5];
    const int i6 = aHex[ihex*8+6];
    const int i7 = aHex[ihex*8+7];
    const double p0[3] = { aXYZ[i0*3+0], aXYZ[i0*3+1], aXYZ[i0*3+2] };
    const double p1[3] = { aXYZ[i1*3+0], aXYZ[i1*3+1], aXYZ[i1*3+2] };
    const double p2[3] = { aXYZ[i2*3+0], aXYZ[i2*3+1], aXYZ[i2*3+2] };
    const double p3[3] = { aXYZ[i3*3+0], aXYZ[i3*3+1], aXYZ[i3*3+2] };
    const double p4[3] = { aXYZ[i4*3+0], aXYZ[i4*3+1], aXYZ[i4*3+2] };
    const double p5[3] = { aXYZ[i5*3+0], aXYZ[i5*3+1], aXYZ[i5*3+2] };
    const double p6[3] = { aXYZ[i6*3+0], aXYZ[i6*3+1], aXYZ[i6*3+2] };
    const double p7[3] = { aXYZ[i7*3+0], aXYZ[i7*3+1], aXYZ[i7*3+2] };
    const double* aP[8] = {p0,p1,p2,p3,p4,p5,p6,p7};
    for(int iedge=0;iedge<12;++iedge){
      const double* q0 = aP[ noelEdge_Hex[iedge][0] ];
      const double* q1 = aP[ noelEdge_Hex[iedge][1] ];
      ::glVertex3dv(q0);
      ::glVertex3dv(q1);
    }
  }
  ::glEnd();
}

void DrawMeshTet3D_FaceNormDisp
(const std::vector<double>& aXYZ,
 const std::vector<int>& aTet,
 const std::vector<double>& aDisp)
{
  for (int itet = 0; itet<(int)aTet.size()/4; itet++){
    const int i0 = aTet[itet*4+0];
    const int i1 = aTet[itet*4+1];
    const int i2 = aTet[itet*4+2];
    const int i3 = aTet[itet*4+3];
    const double p0[3] = { aXYZ[i0*3+0]+aDisp[i0*3+0], aXYZ[i0*3+1]+aDisp[i0*3+1], aXYZ[i0*3+2]+aDisp[i0*3+2] };
    const double p1[3] = { aXYZ[i1*3+0]+aDisp[i1*3+0], aXYZ[i1*3+1]+aDisp[i1*3+1], aXYZ[i1*3+2]+aDisp[i1*3+2] };
    const double p2[3] = { aXYZ[i2*3+0]+aDisp[i2*3+0], aXYZ[i2*3+1]+aDisp[i2*3+1], aXYZ[i2*3+2]+aDisp[i2*3+2] };
    const double p3[3] = { aXYZ[i3*3+0]+aDisp[i3*3+0], aXYZ[i3*3+1]+aDisp[i3*3+1], aXYZ[i3*3+2]+aDisp[i3*3+2] };
    double un0[3], a0; UnitNormalAreaTri3D(un0,a0, p1,p2,p3);
    double un1[3], a1; UnitNormalAreaTri3D(un1,a1, p2,p0,p3);
    double un2[3], a2; UnitNormalAreaTri3D(un2,a2, p3,p0,p1);
    double un3[3], a3; UnitNormalAreaTri3D(un3,a3, p0,p2,p1);
    //    ::glColor3d(0, 0, 0);
    ::glBegin(GL_TRIANGLES);
    ::glNormal3dv(un0); ::glVertex3dv(p1); ::glVertex3dv(p2); ::glVertex3dv(p3);
    ::glNormal3dv(un1); ::glVertex3dv(p2); ::glVertex3dv(p3); ::glVertex3dv(p0);
    ::glNormal3dv(un2); ::glVertex3dv(p3); ::glVertex3dv(p0); ::glVertex3dv(p1);
    ::glNormal3dv(un3); ::glVertex3dv(p0); ::glVertex3dv(p1); ::glVertex3dv(p2);
    ::glEnd();
  }
}






///////////////////////////////////



void Draw_SurfaceMeshEdge
(unsigned int nXYZ, const double* paXYZ,
 unsigned int nTri, const unsigned int* paTri)
{
  ::glEnableClientState(GL_VERTEX_ARRAY);
  ::glVertexPointer(3 , GL_DOUBLE , 0 , paXYZ);
  ::glBegin(GL_LINES);
  for(unsigned int itri=0;itri<nTri;itri++){
    const unsigned int i1 = paTri[itri*3+0];
    const unsigned int i2 = paTri[itri*3+1];
    const unsigned int i3 = paTri[itri*3+2];
    glArrayElement(i1);
    glArrayElement(i2);
    glArrayElement(i2);
    glArrayElement(i3);
    glArrayElement(i3);
    glArrayElement(i1);
  }
  ::glEnd();
  ::glDisableClientState(GL_VERTEX_ARRAY);
  return;
}

void Draw_SurfaceMeshFace
(unsigned int nXYZ, const double* paXYZ,
 unsigned int nTri, const unsigned int* paTri)
{
  ::glEnableClientState(GL_VERTEX_ARRAY);
  ::glVertexPointer(3 , GL_DOUBLE , 0 , paXYZ);
  ::glDrawElements(GL_TRIANGLES , nTri*3 , GL_UNSIGNED_INT , paTri);
  ::glDisableClientState(GL_VERTEX_ARRAY);
  return;
  /*
  /////
  ::glColor3d(1,1,1);
  ::glBegin(GL_TRIANGLES);
  for(unsigned int itri=0;itri<nTri;itri++){
    const unsigned int i1 = paTri[itri*3+0];
    const unsigned int i2 = paTri[itri*3+1];
    const unsigned int i3 = paTri[itri*3+2];
    ::glVertex3dv(paXYZ+i1*3);
    ::glVertex3dv(paXYZ+i2*3);
    ::glVertex3dv(paXYZ+i3*3);
  }
  ::glEnd();
  ::glColor3d(0,0,0);
  ::glBegin(GL_LINES);
  for(unsigned int itri=0;itri<nTri;itri++){
    const unsigned int i1 = paTri[itri*3+0];
    const unsigned int i2 = paTri[itri*3+1];
    const unsigned int i3 = paTri[itri*3+2];
    ::glVertex3dv(paXYZ+i1*3);
    ::glVertex3dv(paXYZ+i2*3);
    ::glVertex3dv(paXYZ+i2*3);
    ::glVertex3dv(paXYZ+i3*3);
    ::glVertex3dv(paXYZ+i3*3);
    ::glVertex3dv(paXYZ+i1*3);
  }
  ::glEnd();
  */
}


void DrawBox_MinMaxXYZ
(double x_min, double x_max,
 double y_min, double y_max,
 double z_min, double z_max)
{// show bounding box
  ::glBegin(GL_LINES);
  ::glVertex3d(x_max,y_min,z_max); ::glVertex3d(x_min,y_min,z_max);
  ::glVertex3d(x_max,y_min,z_min); ::glVertex3d(x_min,y_min,z_min);
  ::glVertex3d(x_max,y_max,z_max); ::glVertex3d(x_min,y_max,z_max);
  ::glVertex3d(x_max,y_max,z_min); ::glVertex3d(x_min,y_max,z_min);
  ::glVertex3d(x_max,y_min,z_max); ::glVertex3d(x_max,y_max,z_max);
  ::glVertex3d(x_min,y_min,z_max); ::glVertex3d(x_min,y_max,z_max);
  ::glVertex3d(x_max,y_min,z_min); ::glVertex3d(x_max,y_max,z_min);
  ::glVertex3d(x_min,y_min,z_min); ::glVertex3d(x_min,y_max,z_min);
  ::glVertex3d(x_max,y_min,z_min); ::glVertex3d(x_max,y_min,z_max);
  ::glVertex3d(x_min,y_min,z_min); ::glVertex3d(x_min,y_min,z_max);
  ::glVertex3d(x_max,y_max,z_min); ::glVertex3d(x_max,y_max,z_max);
  ::glVertex3d(x_min,y_max,z_min); ::glVertex3d(x_min,y_max,z_max);
  ::glEnd();
}

void DrawBox_MinMaxXYZ
(double aabbMinMaxXYZ[6])
{// show bounding box
  DrawBox_MinMaxXYZ(aabbMinMaxXYZ[0], aabbMinMaxXYZ[1],
                    aabbMinMaxXYZ[2], aabbMinMaxXYZ[3],
                    aabbMinMaxXYZ[4], aabbMinMaxXYZ[5]);
}




void showdepth()
{
  GLint view[4]; glGetIntegerv(GL_VIEWPORT, view); // get viewport size
  GLubyte* buffer = (GLubyte *)malloc(view[2] * view[3]); // get buffer with view port size
  if (!buffer) { return; }
  
  glFinish(); // wait for finishing display
  
  // read depth buffer
  glReadPixels(view[0], view[1], view[2], view[3],
               GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, buffer);
  // write to color buffer
  glDrawPixels(view[2], view[3], GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
  
  free(buffer);
}

