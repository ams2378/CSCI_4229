/*
 Adil Sadik
 */

#include "CSCIx229.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h> //Used for random
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define PI 3.1416

int mode=1;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=10;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=15.0;   //  Size of world
int    box=1;    //  Draw sky

int axes=1;       //  Display axes

double zoom = .3; //Zoom of the world

unsigned int texture[15];
unsigned int sky[3];
int dth=0;         //  Azimuth of view angle

double Cx = 0;
double Cy = 0;
double Cz = 0;

double Ex;
double Ey;
double Ez;


/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void drawGround(double xSize, double ySize, double zSize, float x,float y,float z, int tex, double th, int rep)
{

   double grassColor[3] = {85.0/255, 107.0/255, 47.0/255};
   double dirtColor[3] = {129.0/255, 69.0/255, 19.0/255};





      //  Save transformation
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);

   glScaled(xSize, ySize, zSize);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(dirtColor[0],dirtColor[1],dirtColor[2]);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1); // Bottom
   glTexCoord2f(0,rep); glVertex3f(+1,-1, 1); // Bottom
   glColor3f(grassColor[0],grassColor[1],grassColor[2]);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1, 1); // Top
   glTexCoord2f(rep,0); glVertex3f(-1,+1, 1); // Top
   //  Back
   glColor3f(dirtColor[0],dirtColor[1],dirtColor[2]);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1); // Bottom
   glTexCoord2f(0,rep); glVertex3f(-1,-1,-1); // Bottom
   glColor3f(grassColor[0],grassColor[1],grassColor[2]);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,-1); // Top
   glTexCoord2f(rep,0); glVertex3f(+1,+1,-1); // Top
   //  Right
   glColor3f(dirtColor[0],dirtColor[1],dirtColor[2]);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1); // Bottom
   glTexCoord2f(0,rep); glVertex3f(+1,-1,-1); // Bottom
   glColor3f(grassColor[0],grassColor[1],grassColor[2]);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1); // Top
   glTexCoord2f(rep,0); glVertex3f(+1,+1,+1); // Top
   //  Left
   glColor3f(dirtColor[0],dirtColor[1],dirtColor[2]);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1); // Bottom
   glTexCoord2f(0,rep); glVertex3f(-1,-1,+1); // Bottom
   glColor3f(grassColor[0],grassColor[1],grassColor[2]);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,+1); // Top
   glTexCoord2f(rep,0); glVertex3f(-1,+1,-1); // Top

   glEnd();

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   //glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[tex]);

   glBegin(GL_QUADS);
   //  Top
   glColor3f(grassColor[0],grassColor[1],grassColor[2]);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1); // Top
   glTexCoord2f(0,rep); glVertex3f(+1,+1,+1); // Top
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1); // Top
   glTexCoord2f(rep,0); glVertex3f(-1,+1,-1); // Top

   glEnd();
   glDisable(GL_TEXTURE_2D);

   glBegin(GL_QUADS);
   //  Bottom
   glColor3f(dirtColor[0],dirtColor[1],dirtColor[2]);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1); // Bottom
   glTexCoord2f(0,rep); glVertex3f(+1,-1,-1); // Bottom
   glTexCoord2f(rep,rep); glVertex3f(+1,-1,+1); // Bottom
   glTexCoord2f(rep,0); glVertex3f(-1,-1,+1); // Bottom
   //  End
   glEnd();

   //  Undo transofrmations
   glPopMatrix();

}


/*
 *  Draw a building
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void building(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, float a, float b, float c, int tex)
{

   if (tex != -1){
      //  Enable textures
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
      //glColor3f(1,1,1);
      glBindTexture(GL_TEXTURE_2D,texture[3]);
   }

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(1,1,1);
   if (tex!=-1)
      glTexCoord2f(0,0);
   glVertex3f(-a,-b, c);
   if (tex!=-1)
      glTexCoord2f(1,0);
   glVertex3f(+a,-b, c);
   if (tex!=-1)
      glTexCoord2f(1,1);
   glVertex3f(+a,+b, c);
   if (tex!=-1)
      glTexCoord2f(0,1);
   glVertex3f(-a,+b, c);

   //  Back
   glColor3f(0,0,1);
   if (tex!=-1)
      glTexCoord2f(0,0);
   glVertex3f(+a,-b,-c);
   if (tex!=-1)
      glTexCoord2f(1,0);
   glVertex3f(-a,-b,-c);
   if (tex!=-1)
      glTexCoord2f(1,1);
   glVertex3f(-a,+b,-c);
   if (tex!=-1)
      glTexCoord2f(0,1);
   glVertex3f(+a,+b,-c);
   //  Right
   glColor3f(1,1,1);
   if (tex!=-1)
      glTexCoord2f(0,0);
   glVertex3f(+a,-b,+c);
   if (tex!=-1)
      glTexCoord2f(1,0);
   glVertex3f(+a,-b,-c);
   if (tex!=-1)
      glTexCoord2f(1,1);
   glVertex3f(+a,+b,-c);
   if (tex!=-1)
      glTexCoord2f(0,1);
   glVertex3f(+a,+b,+c);
   //  Left
   glColor3f(1,1,1);
   if (tex!=-1)
      glTexCoord2f(0,0);
   glVertex3f(-a,-b,-c);
   if (tex!=-1)
      glTexCoord2f(1,0);
   glVertex3f(-a,-b,+c);
   if (tex!=-1)
      glTexCoord2f(1,1);
   glVertex3f(-a,+b,+c);
   if (tex!=-1)
      glTexCoord2f(0,1);
   glVertex3f(-a,+b,-c);
   glEnd();
   glDisable(GL_TEXTURE_2D);


   glBegin(GL_QUADS);
   //  Top
   glColor3f(0,1,1);
   glVertex3f(-a,+b,+c);
   glVertex3f(+a,+b,+c);
   glVertex3f(+a,+b,-c);
   glVertex3f(-a,+b,-c);
   //  Bottom
   glColor3f(1,0,1);
   glVertex3f(-a,-b,-c);
   glVertex3f(+a,-b,-c);
   glVertex3f(+a,-b,+c);
   glVertex3f(-a,-b,+c);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}


void parking_g(double x,double y,double z,
          double dx,double dy,double dz,
          double th)
{

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //  Right
   //glColor3f(0,0,0);

   double sx=.4;
   double sy=1;
   double sz=.4;

   double ty=2;

   for (int i=0; i<10; i=i+ty) {
      building(0,0+i,0, sx,sy,sz, 0, 1,1,1, 4);
      building(8,0+i,0, sx,sy,sz, 0, 1,1,1, 4);
      building(16,0+i,0, sx,sy,sz, 0, 1,1,1, 4);

      building(0,0+i,8, sx,sy,sz, 0, 1,1,1, 4);
      building(8,0+i,8, sx,sy,sz, 0, 1,1,1, 4);
      building(16,0+i,8, sx,sy,sz, 0, 1,1,1, 4);

      building(0,0+i,16, sx,sy,sz, 0, 1,1,1, 4);
      building(8,0+i,16, sx,sy,sz, 0, 1,1,1, 4);
      building(16,0+i,16, sx,sy,sz, 0, 1,1,1, 4);
   }

   sx=.4;
   sy=.4;
   sz=1;


   building(8,-1,8, 10,.2,10, 0, 1,1,1, 4);
   building(8,4,8, 10,.2,10, 0, 1,1,1, 4);
   building(8,8,8, 10,.2,10, 0, 1,1,1, 4);

   glEnd();

   glPopMatrix();
}


/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, float a, float b, float c, int tex)
{

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   //glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[12]);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-a,-b, c);
   glTexCoord2f(1,0); glVertex3f(+a,-b, c);
   glTexCoord2f(1,1); glVertex3f(+a,+b, c);
   glTexCoord2f(0,1); glVertex3f(-a,+b, c);
   glEnd();
   glDisable(GL_TEXTURE_2D);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   //glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[2]);

   glBegin(GL_QUADS);
   //  Right
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(+a,-b,+c);
   glTexCoord2f(1,0); glVertex3f(+a,-b,-c);
   glTexCoord2f(1,1); glVertex3f(+a,+b,-c);
   glTexCoord2f(0,1); glVertex3f(+a,+b,+c);
   //  Left
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-a,-b,-c);
   glTexCoord2f(1,0); glVertex3f(-a,-b,+c);
   glTexCoord2f(1,1); glVertex3f(-a,+b,+c);
   glTexCoord2f(0,1); glVertex3f(-a,+b,-c);

   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(+a,-b,-c);
   glTexCoord2f(1,0); glVertex3f(-a,-b,-c);
   glTexCoord2f(1,1); glVertex3f(-a,+b,-c);
   glTexCoord2f(0,1); glVertex3f(+a,+b,-c);

   glEnd();
   glDisable(GL_TEXTURE_2D);

   glBegin(GL_QUADS);
   //  Top
   glColor3f(0,1,1);
   glVertex3f(-a,+b,+c);
   glVertex3f(+a,+b,+c);
   glVertex3f(+a,+b,-c);
   glVertex3f(-a,+b,-c);
   //  Bottom
   glColor3f(1,0,1);
   glVertex3f(-a,-b,-c);
   glVertex3f(+a,-b,-c);
   glVertex3f(+a,-b,+c);
   glVertex3f(-a,-b,+c);

   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}


static void triangle(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{


   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D,texture[1]);

   int rep=4;

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_TRIANGLES);
   //  Front
   glColor3f(1,0,0);
   glVertex3f(0,+5, +6);
   glVertex3f(-5,0,+6);
   glVertex3f(+5,0,+6);


   //  Back
   glColor3f(0,0,1);
   glColor3f(1,0,0);
   glVertex3f(0,+5,-6);
   glVertex3f(-5,0,-6);
   glVertex3f(+5,0,-6);
   //  End
   glEnd();

   glBegin(GL_QUADS);
   //  Right
   glColor3f(1,1,0);
   glTexCoord2f(0,0); glVertex3f(+0,+5,+6);
   glTexCoord2f(0,rep); glVertex3f(+0,+5,-6);
   glTexCoord2f(rep,rep); glVertex3f(+5,+0,-6);
   glTexCoord2f(rep,0); glVertex3f(+5,+0,+6);
   // Left
   glColor3f(1,1,0);
   glTexCoord2f(0,0); glVertex3f(+0,+5,+6);
   glTexCoord2f(0,rep); glVertex3f(+0,+5,-6);
   glTexCoord2f(rep,rep); glVertex3f(-5,+0,-6);
   glTexCoord2f(rep,0); glVertex3f(-5,+0,+6);
   // Bottom
   glColor3f(1,0,1);
   glTexCoord2f(0,0); glVertex3f(+5,+0,-6);
   glTexCoord2f(0,rep); glVertex3f(+5,+0,+6);
   glTexCoord2f(rep,rep); glVertex3f(-5,+0,+6);
   glTexCoord2f(rep,0); glVertex3f(-5,+0,-6);
   //  End
   glEnd();
   glDisable(GL_TEXTURE_2D);

   //  Undo transofrmations
   glPopMatrix();
}


void road(double x,double y,double z,
          double dx,double dy,double dz,
          double th, double l, double w, double d)
{

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   glBegin(GL_QUADS);
   //  Right
   glColor3f(0,0,0);
   glVertex3f(+0,+d,+0);
   glVertex3f(+0,+d,+w);
   glVertex3f(+l,+d,+w);
   glVertex3f(+l,+d,+0);
   //  End
   glEnd();

   glPopMatrix();
}

void highrise_building(double x,double y,double z,
            double dx,double dy,double dz,
            double th)
{

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   double sx=2;
   double sy=4;
   double sz=2;

   double ty=4;

   building(-8,ty+0,0, sx,sy,sz, 0, 1,1,1, 4);
   building(-8,ty+2,0, sx,sy,sz, 0, 1,1,1, 4);
   building(-8,ty+4,0, sx,sy,sz, 0, 1,1,1, 4);

   building(2,ty+0,0, sx,sy,sz, 0, 1,1,1, 4);
   building(2,ty+2,0, sx,sy,sz, 0, 1,1,1, 4);
   building(2,ty+4,0, sx,sy,sz, 0, 1,1,1, 4);

   building(12,ty+0,0, sx,sy,sz, 0, 1,1,1, 4);
   building(12,ty+2,0, sx,sy,sz, 0, 1,1,1, 4);
   building(12,ty+4,0, sx,sy,sz, 0, 1,1,1, 4);


   building(-8,ty+0,10, sx,sy,sz, 180, 1,1,1, 4);
   building(-8,ty+2,10, sx,sy,sz, 180, 1,1,1, 4);
   building(-8,ty+4,10, sx,sy,sz, 180, 1,1,1, 4);

   building(2,ty+0,10, sx,sy,sz, 180, 1,1,1, 4);
   building(2,ty+2,10, sx,sy,sz, 180, 1,1,1, 4);
   building(2,ty+4,10, sx,sy,sz, 180, 1,1,1, 4);

   building(12,ty+0,10, sx,sy,sz, 180, 1,1,1, 4);
   building(12,ty+2,10, sx,sy,sz, 180, 1,1,1, 4);
   building(12,ty+4,10, sx,sy,sz, 180, 1,1,1, 4);

   glTranslated(-2,0,-.5);

   drawGround(4,.2,3,   17,0,12, 8, 0, 4);
   drawGround(4,.2,3,   17,0,6, 8, 0, 4);
   drawGround(4,.2,3,   17,0,0, 8, 0, 4);

   drawGround(4,.2,3,   9,0,12, 8, 0, 4);
   drawGround(4,.2,3,   9,0,6, 8, 0, 4);
   drawGround(4,.2,3,   9,0,0, 8, 0, 4);

   drawGround(4,.2,3,   1,0,12, 8, 0, 4);
   drawGround(4,.2,3,   1,0,6, 8, 0, 4);
   drawGround(4,.2,3,   1,0,0, 8, 0, 4);

   drawGround(4,.2,3,   -7,0,12, 8, 0, 4);
   drawGround(4,.2,3,   -7,0,6, 8, 0, 4);
   drawGround(4,.2,3,   -7,0,0, 8, 0, 4);

   glPopMatrix();
}


void home(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   triangle(0,3,-1, 1,1,1, 0);
   cube(0,0,0, .8,.8,.8, 0, 5, 5, 8, 2);

   glPopMatrix();
}


void home_full(double x,double y,double z,
               double dx,double dy,double dz,
               double th)
{

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   triangle(0,3.5,0, 1,.8,1.1, 0);
   cube(0,0,0, .8,.8,.8, 0, 5, 5, 8, 2);

   drawGround(10, .2, 12, 0,-4,.3, 4, 0, 1);
   drawGround(10, .2, 1.3, 0,-4,13.5, 7, 0, 10);
   glPolygonOffset(1,1);
   drawGround(10, .2, 2.3, 0,-4,17, 6, 0, 1); // road

   glPopMatrix();

}

void home_block(double x,double y,double z,
               double dx,double dy,double dz,
               double th)
{

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);


   double dis=38.5;

   home_full(0,4,0, 1,1,1, 0);
   home_full(20,4,0, 1,1,1, 0);
   home_full(40,4,0, 1,1,1, 0);
   home_full(60,4,0, 1,1,1, 0);

   home_full(0,4,dis, 1,1,1, 180);
   home_full(20,4,dis, 1,1,1, 180);
   home_full(40,4,dis, 1,1,1, 180);
   home_full(60,4,dis, 1,1,1, 180);

   glPopMatrix();

}

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
   //glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glTexCoord2f(Sin(th)*Cos(ph) , Sin(ph)); glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

/*
 *  Draw a sphere (version 1)
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere1(double x,double y,double z, double dx,double dy,double dz, double r)
{
   const int d=5;
   int th,ph;


   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   glScaled(r,r,r);

   //glEnable(GL_TEXTURE_2D);
   //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   //glBindTexture(GL_TEXTURE_2D,texture[10]);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,-90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,d-90);
   }
   glEnd();

   //  Latitude bands
   for (ph=d-90;ph<=90-2*d;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   Vertex(0,90);
   for (th=0;th<=360;th+=d)
   {
      Vertex(th,90-d);
   }
   glEnd();
   //glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}


static void blimp_cabin(double x,double y,double z, double dx,double dy,double dz, double r)
{
   //const int d=5;
   //int th,ph;

   //glEnable(GL_TEXTURE_2D);
   //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   //glBindTexture(GL_TEXTURE_2D,texture[10]);

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   glScaled(r,r,r);

   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 *  Adapted from Example 7 given on Moodle
 *  Draw a cylinder
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the z axis
 *     given top & bottom color
 *     given side color
 */

void cylinder(double x,double y,double z,
              double dx,double dy,double dz,
              double th, char lcol, char scol, double r, double h) {
   double i;
   double radius = r, height = h;

   glPushMatrix();
   //  Transform cube
   glTranslated(x,y,z);
   glRotated(th,1,0,0);
   glScaled(dx,dy,dz);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D,texture[13]);

   /* Tube of Cylinder */
   glBegin(GL_QUAD_STRIP);
      for(i = 0.0; i <= 2.1*PI; i+=.05){
         glTexCoord2f(1/(radius * cos(i)),1/(radius * sin(i))); glVertex3d(radius * cos(i), height, radius * sin(i));
         glTexCoord2f(1/(radius * cos(i)),1/(radius * sin(i))); glVertex3d(radius * cos(i), -height, radius * sin(i));
      }

   glDisable(GL_TEXTURE_2D);
   glEnd();

   /* Top of Cylinder */
   glBegin(GL_TRIANGLE_FAN);
      glVertex3d(0.0, height, 0.0);

      for(i = 0.0; i < 2*PI*radius*4; i+=.125) {
         glVertex3d(radius * cos(i), height, radius * sin(i));
      }
   glEnd();

   /* Bottom of Cylinder */
   glBegin(GL_TRIANGLE_FAN);
      glVertex3d(0.0, -height, 0.0);

      for(i = 0.0; i < 2*PI*radius*4; i+=.125) {
         glVertex3d(radius * cos(i), -height, radius * sin(i));
      }
   glEnd();

   glPopMatrix();
}

static void leaf(double x,double y,double z, double dx,double dy,double dz)
{
   //const int d=5;
   //int th,ph;

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D,texture[11]);

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);

   int r=1;

   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(0,0,0);
   //glRotatef(45,0,1,0);
   glTexCoord2f(r,0); glVertex3f(-.25,-1,1);
   glTexCoord2f(r,r); glVertex3f(0,-1.5,1);
   glTexCoord2f(0,r); glVertex3f(.25,-1,1);
   glEnd();

   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}


static void leaves(double x,double y,double z, double dx,double dy,double dz)
{
   //const int d=5;
   //int th,ph;

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D,texture[11]);

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);

   leaf(0,0,0, 1,1,1);
   for (int i=0; i<=13; i++) {
      glRotatef(26.56,0,0,1);
      leaf(0,0,0, 1,1,1);
   }

   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}


static void tree(double x,double y,double z, double dx,double dy,double dz)
{
   //const int d=5;
   //int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   glRotatef(90,1,0,0);

   double height=1.5;

   leaves(0,0,-.2, 1,1,1);
   leaves(0,0,0, 1,1,1);
   cylinder(0,0,height, 1,1,1, 90,0,0, .15, height);

   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

static void forest(double x,double y,double z, double dx,double dy,double dz)
{
   //const int d=5;
   //int th,ph;

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D,texture[11]);

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   glRotatef(0,1,0,0);

   for (int i=0; i<20; i++){
      for (int j=0; j<20; j++){
         tree(j*3,0,i*3, 1,1,1);
         drawGround(3,.1,3, j*3,-3,i*3, 4, 0, 1);
      }
   }

   //drawGround(37,0,40, 10,-3,30, 6, 0, 1);

   double height=1.5;

   leaves(0,0,-.2, 1,1,1);
   leaves(0,0,0, 1,1,1);
   cylinder(0,0,height, 1,1,1, 90,0,0, .15, height);

   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{

   srand(109323499); // Always use the same random seed so that when it rotates it uses the same thing

   //int i,j,k;
   const double len=15;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   glPushMatrix();
   //  Perspective - set eye position
   if (mode==1)
   {
      Ex = (-1*dim*Sin(th)*Cos(ph));
      Ey = (+1*dim        *Sin(ph));
      Ez = (+1*dim*Cos(th)*Cos(ph));
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);

      //gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);

   } else if (mode==2) {

      Cx = +2*dim*Sin(dth);
      Cy = Ey;
      Cz = -2*dim*Cos(dth);

      gluLookAt(Ex,Ey,Ez , Cx+Ex, Cy, Cz+Ez , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   glScaled(zoom, zoom, zoom);
   //drawScene();
   if (axes)
   {
      glBegin(GL_LINES);
      glColor3f(0,0,0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

   if (axes)
   {
      glBegin(GL_LINES);
      glColor3f(0,0,0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

   //double base_y=4.5;

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D,sky[2]);

   float D=dim*3;
   int r=40;
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-D,0,+D);
   glTexCoord2f(0,r); glVertex3f(+D,0,+D);
   glTexCoord2f(r,r); glVertex3f(+D,0,-D);
   glTexCoord2f(r,0); glVertex3f(-D,0,-D);
   glEnd();

   glDisable(GL_TEXTURE_2D);


   glTranslated(-30,0,-30);


   //double dis=38.5;

   home_block(0,0,0, .6,.6,.6, 0);
   home_block(0,0,36, .6,.6,.6, 360);
   drawGround(37,.2,2.3, 44,0,29, 6, 90, 1); // road
   highrise_building(49.8,0,14, 1.1,1,1, 90);
   drawGround(12,.2,2.3, 58,0,30.5, 6, 0, 1); // road
   parking_g(47.5,.5,34, .6,.6,.6, 0);
   //sphere1(0,40,0, .6,.2,.3, 10);
   forest(47.8,3,46, .3,1,.3);

   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
   glPopMatrix();



   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
   {
      th = 0;
      ph = 5;
      mode=0;
   }
   //  Switch display mode
   else if (ch == 'm' || ch == 'M')
      mode = mode+1;
      if (mode==3)
          mode=0;
   //  Change field of view angle
   else if (ch == '[' && ch>1)
      fov--;
   else if (ch == ']' && ch<179)
      fov++;
   // Changes the zoom
   else if ((ch == '-' || ch == '_') && ch>1)
      zoom *=.9;
   else if ((ch == '+' || ch == '=') && ch<179)
      zoom /=.9;
   else if (ch == 'q' || ch == 'Q')
      axes = 1-axes;
   //  Toggle skybox
   else if (ch == 'b' || ch == 'B')
      box = 1-box;
   //  Switch projection mode
   else if (ch == 'w') {
      Ex = Ex + Cx*.02;
      Ez = Ez + Cz*.02;
   }
   else if (ch == 's') {
      Ex = Ex - Cx*.02;
      Ez = Ez - Cz*.02;
   }
   else if (ch == 'a')
      dth -= 5;
   else if (ch == 'd')
      dth += 5;
   else if (ch == 'f') {
      Ex = 0;
      Ey = .7;
      Ez = 0;
   }

   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   //
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowPosition(100, 100);
   glutInitWindowSize(800,600);
   glutCreateWindow("Forrest -- Jacob Haynes");
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   texture[1] = LoadTexBMP("roof.bmp");
   texture[2] = LoadTexBMP("wall.bmp");
   texture[3] = LoadTexBMP("building.bmp");
   texture[4] = LoadTexBMP("grass.bmp");
   //texture[5] = LoadTexBMP("parking.bmp");
   texture[6] = LoadTexBMP("road.bmp");
   texture[7] = LoadTexBMP("pavement.bmp");
   texture[8] = LoadTexBMP("floor.bmp");
   //texture[9] = LoadTexBMP("sky2.bmp");
   //texture[10] = LoadTexBMP("blimp.bmp");
   texture[11] = LoadTexBMP("leaf.bmp");
   texture[12] = LoadTexBMP("frontdoor.bmp");
   texture[13] = LoadTexBMP("tree.bmp");

   //sky[0] = LoadTexBMP("sky3.bmp");
   //sky[1] = LoadTexBMP("sky5.bmp");
   sky[2] = LoadTexBMP("soil.bmp");
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
