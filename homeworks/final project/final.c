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

int th=-40;         //  Azimuth of view angle
int ph=10;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
int    en_box=1;    //  Draw sky
double zh=0;      //  Rotation of teapot
double zh2=0;      //  Rotation of teapot
double theta = 0;
double timer= 0;

int axes=0;       //  Display axes
int iter=0;
int iter1=0;
double move_z1=0;
double move_x1=0;

double move_z=0;
double move_x=0;
double zoom = .3; //Zoom of the world

unsigned int texture[15];
unsigned int car_t[10];
unsigned int sky[3];
int dth=0;         //  Azimuth of view angle
int p_mode=0;

int en_fog=1;
int inf=0;

double Cx = 0;
double Cy = 0;
double Cz = 0;
float dis_x=0;

double Ex = 87.35;
double Ey = 23.96;
double Ez = 106.10;

double dim=34.5;   //  Size of world

float white[] = {1,1,1,1};
float black[] = {0,0,0,1};
int first_person_nav=0;


// Light values
int one       =   1;  // Unit value
int distance_z=   10;  // Light distance
int distance_x=   20;  // Light distance
int inc       =  10;  // Ball increment
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =   0;  // Ambient intensity (%)
int diffuse   =  55;  // Diffuse intensity (%)
int specular  =  20;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
float ylight  =   15;  // Elevation of light
int light     =   1;  //  Lighting
int light1     =   0;  //  Lighting
int smooth    =   1;  // Smooth/Flat shading
int stall_light=0;



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
   if (p_mode==0)
      gluPerspective(fov,asp,dim,65*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim*2,+asp*dim*2, -dim*2,+dim*2, -dim*20,+dim*20);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void drawGround(double xSize, double ySize, double zSize, float x,float y,float z, int tex, double th, int rep)
{

   double dirtColor[3] = {129.0/255, 69.0/255, 19.0/255};

   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);

   glColor3f(1,1,1);
   glScaled(xSize, ySize, zSize);

   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   //glColor3f(dirtColor[0],dirtColor[1],dirtColor[2]);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1); // Bottom
   glTexCoord2f(0,rep); glVertex3f(+1,-1, 1); // Bottom
   //glColor3f(grassColor[0],grassColor[1],grassColor[2]);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1, 1); // Top
   glTexCoord2f(rep,0); glVertex3f(-1,+1, 1); // Top
   //  Back
   glNormal3f( 0, 0, -1);
   //glColor3f(dirtColor[0],dirtColor[1],dirtColor[2]);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1); // Bottom
   glTexCoord2f(0,rep); glVertex3f(-1,-1,-1); // Bottom
   //glColor3f(grassColor[0],grassColor[1],grassColor[2]);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,-1); // Top
   glTexCoord2f(rep,0); glVertex3f(+1,+1,-1); // Top
   //  Right
   glNormal3f( +1, 0, 0);
   //glColor3f(dirtColor[0],dirtColor[1],dirtColor[2]);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1); // Bottom
   glTexCoord2f(0,rep); glVertex3f(+1,-1,-1); // Bottom
   //glColor3f(grassColor[0],grassColor[1],grassColor[2]);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1); // Top
   glTexCoord2f(rep,0); glVertex3f(+1,+1,+1); // Top
   //  Left
   glNormal3f( -1, 0, 0);
   //glColor3f(dirtColor[0],dirtColor[1],dirtColor[2]);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1); // Bottom
   glTexCoord2f(0,rep); glVertex3f(-1,-1,+1); // Bottom
   //glColor3f(grassColor[0],grassColor[1],grassColor[2]);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,+1); // Top
   glTexCoord2f(rep,0); glVertex3f(-1,+1,-1); // Top

   glEnd();

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   //glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[tex]);

   glBegin(GL_QUADS);
   //  Top
   glNormal3f( 0, +1, 0);
   //glColor3f(grassColor[0],grassColor[1],grassColor[2]);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1); // Top
   glTexCoord2f(0,rep); glVertex3f(+1,+1,+1); // Top
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1); // Top
   glTexCoord2f(rep,0); glVertex3f(-1,+1,-1); // Top

   glEnd();
   glDisable(GL_TEXTURE_2D);

   glBegin(GL_QUADS);
   //  Bottom
   glNormal3f( 0, -1, 0);
   //glColor3f(dirtColor[0],dirtColor[1],dirtColor[2]);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1); // Bottom
   glTexCoord2f(0,rep); glVertex3f(+1,-1,-1); // Bottom
   glTexCoord2f(rep,rep); glVertex3f(+1,-1,+1); // Bottom
   glTexCoord2f(rep,0); glVertex3f(-1,-1,+1); // Bottom
   //  End
   glEnd();


   //  Bottom
   glNormal3f( 0, -1, 0);
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
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
      //glColor3f(1,1,1);
      glBindTexture(GL_TEXTURE_2D,texture[tex]); //3
   }

   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
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
   glNormal3f( 0, 0, -1);
   glColor3f(1,1,1);
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
   glNormal3f( 1, 0, 0);
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
   glNormal3f( -1, 0, 0);
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
   //glDisable(GL_TEXTURE_2D);


   if (tex!=-1)
      glBindTexture(GL_TEXTURE_2D,texture[7]); //3

   int rep=10;

   glBegin(GL_QUADS);
   //  Top
   glNormal3f( 0, 1, 0);
   glColor3f(1,1,1);
   if (tex!=-1)
      glTexCoord2f(0,0);
   glVertex3f(-a,+b,+c);
   if (tex!=-1)
      glTexCoord2f(rep,0);
   glVertex3f(+a,+b,+c);
   if (tex!=-1)
      glTexCoord2f(rep,rep);
   glVertex3f(+a,+b,-c);
   if (tex!=-1)
      glTexCoord2f(0,rep);
   glVertex3f(-a,+b,-c);
   //  Bottom
   glNormal3f( 0, -1, 0);
   glColor3f(1,1,1);
   glVertex3f(-a,-b,-c);
   glVertex3f(+a,-b,-c);
   glVertex3f(+a,-b,+c);
   glVertex3f(-a,-b,+c);
   //  End
   glEnd();
   glDisable(GL_TEXTURE_2D);
   //  Undo transofrmations
   glPopMatrix();
}


/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void box(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, float a, float b, float c, int tex)
{

   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,sky[0]);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, -1);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-a,-b, c);
   glTexCoord2f(1,0); glVertex3f(+a,-b, c);
   glTexCoord2f(1,1); glVertex3f(+a,+b, c);
   glTexCoord2f(0,1); glVertex3f(-a,+b, c);

   // back
   glNormal3f( 0, 0, +1);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(+a,-b,-c);
   glTexCoord2f(1,0); glVertex3f(-a,-b,-c);
   glTexCoord2f(1,1); glVertex3f(-a,+b,-c);
   glTexCoord2f(0,1); glVertex3f(+a,+b,-c);

   //  Right
   glNormal3f( -1, 0, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(+a,-b,+c);
   glTexCoord2f(1,0); glVertex3f(+a,-b,-c);
   glTexCoord2f(1,1); glVertex3f(+a,+b,-c);
   glTexCoord2f(0,1); glVertex3f(+a,+b,+c);

   //  Left
   glNormal3f( +1, 0, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-a,-b,-c);
   glTexCoord2f(1,0); glVertex3f(-a,-b,+c);
   glTexCoord2f(1,1); glVertex3f(-a,+b,+c);
   glTexCoord2f(0,1); glVertex3f(-a,+b,-c);

   glEnd();
   glDisable(GL_TEXTURE_2D);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,sky[1]);
   glBegin(GL_QUADS);
   //  Top
   glNormal3f( 0, -1, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-a,+b,+c);
   glTexCoord2f(1,0); glVertex3f(+a,+b,+c);
   glTexCoord2f(1,1); glVertex3f(+a,+b,-c);
   glTexCoord2f(0,1); glVertex3f(-a,+b,-c);

   glEnd();
   glDisable(GL_TEXTURE_2D);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,sky[2]);

   glBegin(GL_QUADS);
   //  Bottom
   glNormal3f( 0, 1, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-a,-b,-c);
   glTexCoord2f(40,0); glVertex3f(+a,-b,-c);
   glTexCoord2f(40,40); glVertex3f(+a,-b,+c);
   glTexCoord2f(0,40);  glVertex3f(-a,-b,+c);
   //  End
   glEnd();

   glDisable(GL_TEXTURE_2D);
   //  Undo transofrmations
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

   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);


   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
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
   glNormal3f( 0, 0, 1);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-a,-b, c);
   glTexCoord2f(1,0); glVertex3f(+a,-b, c);
   glTexCoord2f(1,1); glVertex3f(+a,+b, c);
   glTexCoord2f(0,1); glVertex3f(-a,+b, c);
   glEnd();
   glDisable(GL_TEXTURE_2D);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   //glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[2]);

   glBegin(GL_QUADS);
   //  Right
   glNormal3f( 1, 0, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(+a,-b,+c);
   glTexCoord2f(1,0); glVertex3f(+a,-b,-c);
   glTexCoord2f(1,1); glVertex3f(+a,+b,-c);
   glTexCoord2f(0,1); glVertex3f(+a,+b,+c);
   //  Left
   glNormal3f( -1, 0, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-a,-b,-c);
   glTexCoord2f(1,0); glVertex3f(-a,-b,+c);
   glTexCoord2f(1,1); glVertex3f(-a,+b,+c);
   glTexCoord2f(0,1); glVertex3f(-a,+b,-c);

   // bottom
   glNormal3f( -1, 0, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(+a,-b,-c);
   glTexCoord2f(1,0); glVertex3f(-a,-b,-c);
   glTexCoord2f(1,1); glVertex3f(-a,+b,-c);
   glTexCoord2f(0,1); glVertex3f(+a,+b,-c);

   glEnd();
   glDisable(GL_TEXTURE_2D);

   glBegin(GL_QUADS);
   //  Top
   glNormal3f( 0, 1, 0);
   //glColor3f(0,1,1);
   glColor3f(1,1,1);
   glVertex3f(-a,+b,+c);
   glVertex3f(+a,+b,+c);
   glVertex3f(+a,+b,-c);
   glVertex3f(-a,+b,-c);
   //  Bottom
   glNormal3f( 0, -1, 0);
   //glColor3f(1,0,1);
   glColor3f(1,1,1);
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

   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[1]);

   int rep=1;

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_TRIANGLES);
   //  Front
   glNormal3f( 0, 0, 1);
   //glColor3f(1,0,0);
   glColor3f(1,1,1);
   glVertex3f(0,+5, +6);
   glVertex3f(-5,0,+6);
   glVertex3f(+5,0,+6);


   //  Back
   glNormal3f( 0, 0, -1);
   //glColor3f(0,0,1);
   //glColor3f(1,0,0);
   glColor3f(1,1,1);
   glVertex3f(0,+5,-6);
   glVertex3f(-5,0,-6);
   glVertex3f(+5,0,-6);
   //  End
   glEnd();

   glBegin(GL_QUADS);
   //  Right
   glNormal3f( 1, 0, 0);
   //glColor3f(1,1,0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(+0,+5,+6);
   glTexCoord2f(0,rep); glVertex3f(+0,+5,-6);
   glTexCoord2f(rep,rep); glVertex3f(+5,+0,-6);
   glTexCoord2f(rep,0); glVertex3f(+5,+0,+6);
   // Left
   glNormal3f(-1, 0, 0);
   //glColor3f(1,1,0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(+0,+5,+6);
   glTexCoord2f(0,rep); glVertex3f(+0,+5,-6);
   glTexCoord2f(rep,rep); glVertex3f(-5,+0,-6);
   glTexCoord2f(rep,0); glVertex3f(-5,+0,+6);
   // Bottom
   glNormal3f(0, -1, 0);
   //glColor3f(1,0,1);
   glColor3f(1,1,1);
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
   //glColor3f(0,0,0);
   glColor3f(1,1,1);
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

   building(-8,ty+0,0, sx,sy,sz, 0, 1,1,1, 3);
   building(-8,ty+2,0, sx,sy,sz, 0, 1,1,1, 3);
   building(-8,ty+4,0, sx,sy,sz, 0, 1,1,1, 3);

   building(2,ty+0,0, sx,sy,sz, 0, 1,1,1, 3);
   building(2,ty+2,0, sx,sy,sz, 0, 1,1,1, 3);
   building(2,ty+4,0, sx,sy,sz, 0, 1,1,1, 3);

   building(12,ty+0,0, sx,sy,sz, 0, 1,1,1, 3);
   building(12,ty+2,0, sx,sy,sz, 0, 1,1,1, 3);
   building(12,ty+4,0, sx,sy,sz, 0, 1,1,1, 3);

   building(-8,ty+0,10, sx,sy,sz, 180, 1,1,1, 3);
   building(-8,ty+2,10, sx,sy,sz, 180, 1,1,1, 3);
   building(-8,ty+4,10, sx,sy,sz, 180, 1,1,1, 3);

   building(2,ty+0,10, sx,sy,sz, 180, 1,1,1, 3);
   building(2,ty+2,10, sx,sy,sz, 180, 1,1,1, 3);
   building(2,ty+4,10, sx,sy,sz, 180, 1,1,1, 3);

   building(12,ty+0,10, sx,sy,sz, 180, 1,1,1, 3);
   building(12,ty+2,10, sx,sy,sz, 180, 1,1,1, 3);
   building(12,ty+4,10, sx,sy,sz, 180, 1,1,1, 3);

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
   drawGround(10, .2, 1.3, 0,-4,13.5, 7, 0, 5);
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

static void cylinder(double x,double y,double z,
                 double dx,double dy,double dz,
                 double r, double height, int tex, double th)
{

   int i=0;
   int j=0;
   int d=5;


   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT,GL_EMISSION,black);

   if (tex!=-1) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D,texture[tex]);
   }

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,1,0,0);
   //glRotated(90,1,0,0);
   glScaled(dx,dy,dz);

   glColor3f(1,0,0);

   // Cylinder
   glColor3f(1,1,1);



   /* Top of Cylinder */
   glBegin(GL_TRIANGLE_FAN);
      glVertex3d(0.0, 0.0, height);

      //for(i = 0.0; i < 2*PI*r*4; i+=.125) {
      for(i = 0.0; i <= 360; i+=10) {
         glVertex3d(r * cos(i), r * sin(i), height);
      }
   glEnd();

   /* Bottom of Cylinder */
   glBegin(GL_TRIANGLE_FAN);
      glVertex3d(0.0, 0, 0.0);

      //for(i = 0.0; i < 2*PI*r*4; i+=.125) {
      for(i = 0.0; i <= 360; i+=10) {
         glVertex3d(r * cos(i), r * sin(i), 0);
      }
   glEnd();


   for (j=0;j<=height;j+=1)
   {
      glBegin(GL_TRIANGLE_STRIP);

      //glTexCoord2f(0.0,0.0); glVertex3f(0, 0, 0);

      if (j>=(height-(height/5))) {
         glColor3f(1,0,0);
      }
      for (i=0;i<=360;i+=d)
      {
         glNormal3f(Cos(i), Sin(i), 0);
         if (tex!=-1)
            glTexCoord2f(Cos(i),j);
         glVertex3f(r*Cos(i), r*Sin(i), j);
         glNormal3f(Cos(i), Sin(i), 0);
         if (tex!=-1)
            glTexCoord2f(Cos(i),j+1);
         glVertex3f(r*Cos(i), r*Sin(i), j+1);
      }
      glEnd();
   }

   if (tex!=-1)
      //  Switch off textures so it doesn't apply to the rest
      glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}


static void leaf(double x,double y,double z, double dx,double dy,double dz)
{

   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[11]);

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);

   int r=1;

   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
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
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
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

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   glRotatef(90,1,0,0);

   double height=1.5;

   leaves(0,0,-.2, 1,1,1);
   leaves(0,0,0, 1,1,1);
   cylinder(0,0,0, 1,1,1, .15, height, 13, 0);

   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

static void forest(double x,double y,double z, double dx,double dy,double dz)
{
   //const int d=5;
   //int th,ph;

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[11]);

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   glRotatef(0,1,0,0);

   for (int i=0; i<10; i++){
      for (int j=0; j<10; j++){
         tree(j*3,1,i*3, 1,2,1);
         drawGround(3,.1,3, j*3,-3,i*3, 4, 0, 1);
      }
   }

   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 *  GLUT calls this toutine when there is nothing else to do
 */
void idle()
{
   double t1 = glutGet(GLUT_ELAPSED_TIME)/10000.0;
   zh = fmod(90*t1,360);

   double t2 = glutGet(GLUT_ELAPSED_TIME)/10000.0;
   zh2 = fmod(90*t2,360);
   glutPostRedisplay();
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (int ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}


 static void carmodel(double x,double y,double z, double dx,double dy,double dz, double th)
{

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   glRotatef(th,0,1,0);


   glBegin(GL_QUADS);
   // rear side
   //  Front
   glNormal3f( 0, 0, -1);
   glColor3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(1,0, 0);
   glTexCoord2f(1,0); glVertex3f(1,1, 0);
   glTexCoord2f(1,1); glVertex3f(3,2, 0);
   glTexCoord2f(0,1); glVertex3f(3,0, 0);


   glNormal3f( 0, 0, 1);
   glColor3f(0,1,0);
   glTexCoord2f(0,0); glVertex3f(1,0, 2);
   glTexCoord2f(1,0); glVertex3f(1,1, 2);
   glTexCoord2f(1,1); glVertex3f(3,2, 2);
   glTexCoord2f(0,1); glVertex3f(3,0, 2);


   glNormal3f( -1, 0, 0);
   glColor3f(0,0,1);
   glTexCoord2f(0,0); glVertex3f(1,0, 0);
   glTexCoord2f(1,0); glVertex3f(1,1, 0);
   glTexCoord2f(1,1); glVertex3f(1,1, 2);
   glTexCoord2f(0,1); glVertex3f(1,0, 2);


   glNormal3f( 1, 0, 0);
   glColor3f(1,0,1);
   glTexCoord2f(0,0); glVertex3f(3,0, 0);
   glTexCoord2f(1,0); glVertex3f(3,2, 0);
   glTexCoord2f(1,1); glVertex3f(3,2, 2);
   glTexCoord2f(0,1); glVertex3f(3,0, 2);


   glNormal3f( 0, -1, 0);
   glColor3f(1,1,0);
   glTexCoord2f(0,0); glVertex3f(1,0, 0);
   glTexCoord2f(1,0); glVertex3f(3,0, 0);
   glTexCoord2f(1,1); glVertex3f(3,0, 2);
   glTexCoord2f(0,1); glVertex3f(1,0, 2);


   glNormal3f( 0, 1, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(1,1, 0);
   glTexCoord2f(1,0); glVertex3f(3,2, 0);
   glTexCoord2f(1,1); glVertex3f(3,2, 2);
   glTexCoord2f(0,1); glVertex3f(1,1, 2);

   // middle

   glNormal3f( -1, 0, 0);
   glColor3f(1,0,1);
   glTexCoord2f(0,0); glVertex3f(3,0, 0);
   glTexCoord2f(1,0); glVertex3f(3,2, 0);
   glTexCoord2f(1,1); glVertex3f(3,2, 2);
   glTexCoord2f(0,1); glVertex3f(3,0, 2);

   glNormal3f( 1, 0, 0);
   glColor3f(1,0,1);
   glTexCoord2f(0,0); glVertex3f(7,0, 0);
   glTexCoord2f(1,0); glVertex3f(7,2, 0);
   glTexCoord2f(1,1); glVertex3f(7,2, 2);
   glTexCoord2f(0,1); glVertex3f(7,0, 2);

   glNormal3f( 0, 0, -1);
   glColor3f(1,1,0);
   glTexCoord2f(0,0); glVertex3f(3,0, 0);
   glTexCoord2f(1,0); glVertex3f(3,2, 0);
   glTexCoord2f(1,1); glVertex3f(7,2, 0);
   glTexCoord2f(0,1); glVertex3f(7,0, 0);

   glNormal3f( 0, 0, 1);
   glColor3f(1,1,0);
   glTexCoord2f(0,0); glVertex3f(3,0, 2);
   glTexCoord2f(1,0); glVertex3f(3,2, 2);
   glTexCoord2f(1,1); glVertex3f(7,2, 2);
   glTexCoord2f(0,1); glVertex3f(7,0, 2);


   glNormal3f( 0, 1, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(3,2, 0);
   glTexCoord2f(1,0); glVertex3f(3,2, 2);
   glTexCoord2f(1,1); glVertex3f(7,2, 2);
   glTexCoord2f(0,1); glVertex3f(7,2, 0);

   glNormal3f( 0, -1, 0);
   glTexCoord2f(0,0); glVertex3f(3,0, 0);
   glTexCoord2f(1,0); glVertex3f(3,0, 2);
   glTexCoord2f(1,1); glVertex3f(7,2, 2);
   glTexCoord2f(0,1); glVertex3f(7,2, 0);

   // front

   glNormal3f( -1, 0, 0);
   glColor3f(1,0,1);
   glTexCoord2f(0,0); glVertex3f(7,0, 0);
   glTexCoord2f(1,0); glVertex3f(7,2, 0);
   glTexCoord2f(1,1); glVertex3f(7,2, 2);
   glTexCoord2f(0,1); glVertex3f(7,0, 2);

   glNormal3f( 1, 0, 0);
   glColor3f(1,0,1);
   glTexCoord2f(0,0); glVertex3f(11,0, 0);
   glTexCoord2f(1,0); glVertex3f(11,1, 0);
   glTexCoord2f(1,1); glVertex3f(11,1, 2);
   glTexCoord2f(0,1); glVertex3f(11,0, 2);

   glNormal3f( 0, 0, -1);
   glColor3f(1,1,0);
   glTexCoord2f(0,0); glVertex3f(7,0, 0);
   glTexCoord2f(1,0); glVertex3f(11,0, 0);
   glTexCoord2f(1,1); glVertex3f(11,1, 0);
   glTexCoord2f(0,1); glVertex3f(7,2, 0);

   glNormal3f( 0, 0,1);
   glColor3f(1,1,0);
   glTexCoord2f(0,0); glVertex3f(7,0, 2);
   glTexCoord2f(1,0); glVertex3f(11,0, 2);
   glTexCoord2f(1,1); glVertex3f(11,1, 2);
   glTexCoord2f(0,1); glVertex3f(7,2, 2);

   glNormal3f( 0, 1, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(7,2, 0);
   glTexCoord2f(1,0); glVertex3f(11,1, 0);
   glTexCoord2f(1,1); glVertex3f(11,1, 2);
   glTexCoord2f(0,1); glVertex3f(7,2, 2);


   glNormal3f( 0, -1, 0);
   glColor3f(1,1,0);
   glTexCoord2f(0,0); glVertex3f(7,0, 0);
   glTexCoord2f(1,0); glVertex3f(11,0, 0);
   glTexCoord2f(1,1); glVertex3f(11,0, 2);
   glTexCoord2f(0,1); glVertex3f(7,0, 2);


   // back window
   glNormal3f( -1, 0, 0);
   glColor3f(1,0,0);
   glTexCoord2f(0,0); glVertex3f(3,2, 0);
   glTexCoord2f(1,0); glVertex3f(3.5,3, 0);
   glTexCoord2f(1,1); glVertex3f(3.5,3, 2);
   glTexCoord2f(0,1); glVertex3f(3,2, 2);


   glNormal3f( 1, 0, 0);
   glColor3f(0,1,0);
   glTexCoord2f(0,0); glVertex3f(6.5,3, 0);
   glTexCoord2f(1,0); glVertex3f(7,2, 0);
   glTexCoord2f(1,1); glVertex3f(7,2, 2);
   glTexCoord2f(0,1); glVertex3f(6.5,3, 2);


   glNormal3f( 0, 1, 0);
   glColor3f(0,0,1);
   glTexCoord2f(0,0); glVertex3f(3.5,3, 0);
   glTexCoord2f(1,0); glVertex3f(3.5,3, 2);
   glTexCoord2f(1,1); glVertex3f(6.5,3, 2);
   glTexCoord2f(0,1); glVertex3f(6.5,3, 0);


   glNormal3f( 0, 0, -1);
   glColor3f(1,1,0);
   glTexCoord2f(0,0); glVertex3f(3,2, 0);
   glTexCoord2f(1,0); glVertex3f(3.5,3, 0);
   glTexCoord2f(1,1); glVertex3f(6.5,3, 0);
   glTexCoord2f(0,1); glVertex3f(7,2, 0);


   glNormal3f( 0, 0, 1);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(3,2, 2);
   glTexCoord2f(1,0); glVertex3f(3.5,3, 2);
   glTexCoord2f(1,1); glVertex3f(6.5,3, 2);
   glTexCoord2f(0,1); glVertex3f(7,2, 2);



   glEnd();

   cylinder(3,0,-.2, 1,1,1, .8, .1, -1, 0);
   cylinder(7,0,-.2, 1,1,1, .8, .1, -1, 0);
   cylinder(3,0,2.2, 1,1,1, .8, .1, -1, 180);
   cylinder(7,0,2.2, 1,1,1, .8, .1, -1, 180);

   glPopMatrix();

}

void lamp(double x,double y,double z,
          double dx,double dy,double dz,
          double th)
{
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   double height0 = 1.5;
   double height1 = .5;

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light1)
   {
        glEnable(GL_LIGHT1);
        //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient , 1.0}; //1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse , 1.0}; //1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular, 1.0}; //1.0};
        //  Light position
        //float Position[]  = {distance_x*Cos(zh),ylight,distance_z*Sin(zh),1.0};
        float Position[]  = {0+50,-.15,.7+65}; // FIXME multiply with tx matrix
        //float Position[]  = {0,-.15,.7};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0],Position[1],Position[2] , .12);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        //glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT1);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT1,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT1,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT1,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT1,GL_POSITION,Position);

        float Direction[] = {x,-y,z,1};
        float sco=180;    //  Spot cutoff angle
        float Exp=0;      //  Spot exponent

        //  Set spotlight parameters
        glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,Direction);
        glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,sco);
        glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,Exp);

        //glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION , 0.0);
        //glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION   , 0.0);
        //glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION, 0.0);


        if (axes) {
           glWindowPos2i(5,75);
           Print("Position: X=%f Y=%f Z=%f",Position[0],Position[1],Position[2]);
           glWindowPos2i(5,75);
           Print("Position: X=%f Y=%f Z=%f",Position[0],Position[1],Position[2]);
        }
   }
   else
     glDisable(GL_LIGHT1);

   cylinder(0,0,0, 1,1,1, .05, height0, 13, 90);
   cylinder(0,0,-.2, 1,1,1, .05, height1, 13, 0);
   //drawGround(1,.1,1,   0,0,0, 8, 0, 4);
   //ball(0,-.15,.7, .12);

  // cylinder(0,0,.6, 1,1,1, .1, height2, 13, 90);


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
      building(0,0+i,0, sx,sy,sz, 0, 1,1,1, 2);
      building(8,0+i,0, sx,sy,sz, 0, 1,1,1, 2);
      building(16,0+i,0, sx,sy,sz, 0, 1,1,1, 2);

      building(0,0+i,8, sx,sy,sz, 0, 1,1,1, 2);
      building(8,0+i,8, sx,sy,sz, 0, 1,1,1, 2);
      building(16,0+i,8, sx,sy,sz, 0, 1,1,1, 2);

      building(0,0+i,16, sx,sy,sz, 0, 1,1,1, 2);
      building(8,0+i,16, sx,sy,sz, 0, 1,1,1, 2);
      building(16,0+i,16, sx,sy,sz, 0, 1,1,1, 2);
   }

   sx=.4;
   sy=.4;
   sz=1;

   building(8,-1,8, 10,.2,10, 0, 1,1,1, 7);
   carmodel(12, 0,8.9, 1,1,1, 90);
   carmodel(9, 0,8.9, 1,1,1, 90);
   carmodel(4, 0,8.9, 1,1,1, 90);
   building(8,4,8, 10,.2,10, 0, 1,1,1, 7);
   carmodel(12, 5,8.9, 1,1,1, 90);
   carmodel(9, 5,8.9, 1,1,1, 90);
   carmodel(4, 5,8.9, 1,1,1, 90);
   building(8,8,8, 10,.2,10, 0, 1,1,1, 7);
   carmodel(12, 9,8.9, 1,1,1, 90);
   carmodel(9, 9,8.9, 1,1,1, 90);
   carmodel(4, 9,8.9, 1,1,1, 90);

   carmodel(13,9,18.9, 1,1,.8, 90);
   carmodel(10,9,18.9, 1,1,.8, 90);
   carmodel(5,9,18.9, 1,1,.8, 90);

   glEnd();

   glPopMatrix();
}

static void movecar(double x,double y,double z, double dx,double dy,double dz, float a)
{

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   glRotatef    (a, 0.0f, 1.0f, 0.0f);


   carmodel(0,-4.3,0, .5,.8,.8, 0);
   //drawGround(.2,1,.2, 0,-4,0, 6, 0, 1);

   glPopMatrix();
}


static void world(double x,double y,double z, double dx,double dy,double dz)
{

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   glRotatef(0,1,0,0);

   home_block(0,0,0, .6,.6,.6, 0);
   home_block(0,0,36, .6,.6,.6, 360);
   drawGround(37,.2,2.3, 44,0,29, 6, 90, 1); // road
   highrise_building(49.8,0,14, 1.1,1,1, 90);
   drawGround(12,.2,2.3, 58,0,30.5, 6, 0, 1); // road
   parking_g(47.5,.5,34, .6,.6,.6, 0);
   //sphere1(0,40,0, .6,.2,.3, 10);
   //forest(47.8,3,46, .3,1,.3);
   forest(47.8,3,48.9, .5,1,.5);

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

   if (p_mode==0)
   {
      if (first_person_nav==1) {
         Cx = +2*dim*Sin(dth);
         Cy = Ey;
         Cz = -2*dim*Cos(dth);

         gluLookAt(Ex,Ey,Ez , Cx+Ex, Cy, Cz+Ez , 0,Cos(ph),0);

      }
      else {
         Ex = -4*dim*Sin(th)*Cos(ph);
         Ey = +4*dim        *Sin(ph);
         Ez = +4*dim*Cos(th)*Cos(ph);

         gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
      }

   }
   //  Orthogonal - set world orientation
   else
   {
      //  Set view angle
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }


   if(en_fog==1)
   {
     GLfloat fogcolour[4]={1.0,1.0,1.0,1.0};

     glFogfv(GL_FOG_COLOR,fogcolour);              /* Define the fog colour */
     glFogf(GL_FOG_DENSITY,.015);                   /* How dense */
     glFogi(GL_FOG_MODE,GL_EXP);                   /* exponential decay */
     glFogf(GL_FOG_START,dim);                   /* Where wwe start fogging */
     glFogf(GL_FOG_END,40*dim);                       /* end */
     glHint(GL_FOG_HINT, GL_FASTEST);              /* compute per vertex */
     glEnable(GL_FOG);/* ENABLE */

   } else {
	 glDisable(GL_FOG);
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
        //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        double rad=40;
        double base=20;

        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient , 1.0}; //1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse , 1.0}; //1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular, 1.0}; //1.0};
        //  Light position
        float Position[]  = {base+rad*Cos(theta),rad*Sin(theta), 40.0, 1-inf};
        if (theta<190)
           theta = theta + .2;
        else if (timer<20) {
           timer = timer+.1;
        } else {
           theta = 0;
           timer = 0;
        }

        if (theta>180) {
          ambient = 0;
          specular = 0;
          diffuse = 0;
        } else {
          ambient = 0;
          specular = 65;
          diffuse = 20;
        }

        glColor3f(1,1-inf,1-inf);
        ball(Position[0],Position[1],Position[2] , 3);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        //glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);

        if (axes) {
           glWindowPos2i(5,75);
           Print("Position: X=%f Y=%f Z=%f",Position[0],Position[1],Position[2]);
           glWindowPos2i(5,85);
           Print("th ph %d %d ", th, ph);
        }
   }
   else
     glDisable(GL_LIGHT0);



   if (light==1 && axes == 1)
   {
      glWindowPos2i(5,45);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shinyvec[0]);
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


   if (en_box)
       box(35,25,50, 1,1,1, 0, 60,25,60, 0);


   glTranslated(-10,0,20);
   world(0,0,0, 1,1,1);

   lamp (46,6,50, 3,3,3, -90);

   double stop_1=44;
   double start_z=9.5;

   if (move_x<=stop_1) {
      movecar(move_x,5,start_z, 1,1,1, 0);
      move_x=move_x+.2;
   } else if (iter<5) {
      if (move_x>=(stop_1+iter*2) && move_x <=(stop_1+(iter*2)+2)) {
         movecar(stop_1,5,start_z, 1,1,1, -18*iter);
         move_x=move_x+.2;
      } else
         iter++;
   } else if (move_z<=55){
      movecar(stop_1,5,start_z+move_z, 1,1,1, -18*iter);
      move_z=move_z+.2;
   } else {
      move_x=move_z=iter=0;

   }

   // Add some traffic
   double stop_11=44;
   double start_z1=45;

   if (move_x1<=stop_11) {
      movecar(move_x1,5,start_z1, 1,1,1, 0);
      move_x1=move_x1+.2;
   } else if (iter1<5) {
      if (move_x1>=(stop_11+iter1*2) && move_x1 <=(stop_11+(iter1*2)+2)) {
         movecar(stop_11,5,start_z1, 1,1,1, 18*iter1);
         move_x1=move_x1+.2;
      } else
         iter1++;
   } else if (move_z1<=55){
      movecar(stop_11,5,start_z1-move_z1, 1,1,1, 18*iter1);
      move_z1=move_z1+.2;
   } else {
      move_x1=move_z1=iter1=0;
   }


   //  Display parameters
   glWindowPos2i(5,5);
   //Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal");
   glPopMatrix();

   if (axes) {
   if (p_mode==0) {
      Print("Mode = Prespective ");
      if (first_person_nav)
         Print("First person view enabled");
      else
         Print("First person view disabled");
   } else
      Print("Mode = Ortho ");

   Print("Ex Ey Ez %f %f %f  ", Ex, Ey, Ez);
   Print("dim %f ", dim);
   }
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
      th -= 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th += 5;
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

   double step=.2;
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0') {
      dth = first_person_nav =0;
      p_mode = 0;
      Ex = 72.3;
      Ey = 22.22;
      Ez = 103.25;
      dim=33;
      th=-40;
      ph=10;
   //  Toggle axes
   } else if (ch == 'q' || ch == 'Q')
      axes = 1-axes;
   //  Switch projection mode
   else if (ch == 'P')
      p_mode = 0; // perspective
      //  PageUp key - increase dim
   else if (ch == 'z')
      dim += 0.5;
   //  PageDown key - decrease dim
   else if (ch == 'Z' && dim>1)
      dim -= 0.5;
   //  Change field of view angle
   else if (ch == '-' && fov>1)
      fov--;
   else if (ch == '+' && fov<179)
      fov++;
   else if (ch == 'w') {
      Ex = Ex + Cx*step;
      Ez = Ez + Cz*step;
   }
   else if (ch == 's') {
      Ex = Ex - Cx*step;
      Ez = Ez - Cz*step;
   }
   else if (ch == 'a')
      dth -= 5;
   else if (ch == 'd')
      dth += 5;
   else if (ch == 'f') {
      first_person_nav = 1;
      Ex = -13;
      Ey = 2;
      Ez = 31.28;
      dim=3.5;
   }
   else if (ch == 'F')
      first_person_nav = 0;
   else if (ch == 'x')
      en_box = 0;
   else if (ch == 'X')
      en_box = 1;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.5;
   else if (ch==']')
      ylight += 0.5;
   //  Ambient level
   else if (ch=='b' && ambient>0)
      ambient -= 5;
   else if (ch=='B' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='u' && diffuse>0)
      diffuse -= 5;
   else if (ch=='U' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='c' && specular>0)
      specular -= 5;
   else if (ch=='C' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
   else if (ch == 'l')
      light = 1;
   else if (ch == 'L')
      light = 0;
   else if (ch == 'g')
      en_fog = 1;
   else if (ch == 'G')
      en_fog = 0;
   else if (ch == 'i' || ch == 'I')
      inf = 1-inf;

   //  Keep angles to +/-360 degrees
   dth %= 360;
   //  Translate shininess power to value (-1 => 0)
   shinyvec[0] = shininess<0 ? 0 : pow(2.0,shininess);
   //  Animate if requested
   //glutIdleFunc(stall_light?idle:NULL);

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
   glutInitWindowSize(2400,1600);
   glutCreateWindow("City- Adil Sadik");
   //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   //  Set callbacks
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
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

   sky[0] = LoadTexBMP("sky3.bmp");
   sky[1] = LoadTexBMP("sky4.bmp");
   sky[2] = LoadTexBMP("soil.bmp");

   //car_t[0] = LoadTexBMP("sidemirror.bmp");
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
