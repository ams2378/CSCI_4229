/* CSCI 4229 Summer 2016
 * Assignment 2: Projection
 * Adil Sadik
 * sadik.adil@gmail.com
 */

#include "CSCIx229.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle

int dth=0;         //  Azimuth of view angle
//double zh=0;      //  Rotation of teapot
int axes=1;       //  Display axes
int p_mode=0;
double dim= 8;
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio

unsigned int texture[4];
int stall_light=0;
double rep=1;  //  Repetition

int ntex=1;    //  Texture

double Ex;
double Ey;
double Ez;

double Cx = 0;
double Cy = 0;
double Cz = 0;

int first_person_nav=0;

// Light values
int one       =   1;  // Unit value
int distance_z=   10;  // Light distance
int distance_x=   3;  // Light distance
int inc       =  10;  // Ball increment
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =   0;  // Ambient intensity (%)
int diffuse   =  55;  // Diffuse intensity (%)
int specular  =  20;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   4;  // Elevation of light
int light     =   3;  //  Lighting
int smooth    =   1;  // Smooth/Flat shading

//  Cosine and Sine in degrees
//#define Cos(x) (cos((x)*3.1415927/180))
//#define Sin(x) (sin((x)*3.1415927/180))

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
      gluPerspective(fov,asp,(dim*2)/4,4*2*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim*2,+asp*dim*2, -dim*2,+dim*2, -dim*20,+dim*20);

   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}


/*
 *  Draw the base of the ship
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     length and width
 *     color RGB
 */
static void base(double x,double y,double z,
                 double dx,double dy,double dz,
                 float dl, float length,
                 float width, float depth, float r, float g, float b, int tex)
{


   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   //glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[tex]);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);

   //  Cube
   glBegin(GL_QUADS);
   //  Top
   glColor3f(r,g,b);
   glNormal3f( 0, +1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-length,+width,+depth);
   glTexCoord2f(rep,0.0); glVertex3f(+length,+width,+depth);
   glTexCoord2f(rep,rep); glVertex3f(+length,+width,-depth);
   glTexCoord2f(0.0,rep); glVertex3f(-length,+width,-depth);
   //  Bottom
   glNormal3f( 0, -1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-length-(-dl),-width,+depth);
   glTexCoord2f(rep,0.0); glVertex3f(+length-dl,-width,+depth);
   glTexCoord2f(rep,rep); glVertex3f(+length-dl,-width,-depth);
   glTexCoord2f(0.0,rep); glVertex3f(-length-(-dl),-width,-depth);
   //  Front
   glColor3f(r,g,b);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-length,+width,+depth);
   glTexCoord2f(rep,0.0); glVertex3f(+length,+width,+depth);
   glTexCoord2f(rep,rep); glVertex3f(+length-dl,-width,+depth);
   glTexCoord2f(0.0,rep); glVertex3f(-length-(-dl),-width,+depth);
   //  Back
   glColor3f(r,g,b);
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0.0,0.0); glVertex3f(-length,+width,-depth);
   glTexCoord2f(rep,0.0); glVertex3f(+length,+width,-depth);
   glTexCoord2f(rep,rep); glVertex3f(+length-dl,-width,-depth);
   glTexCoord2f(0.0,rep); glVertex3f(-length-(-dl),-width,-depth);
   //  Right
   glColor3f(r,g,b);
   glNormal3f( +1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(+length,+width,+depth);
   glTexCoord2f(rep,0.0); glVertex3f(+length,+width,-depth);
   glTexCoord2f(rep,rep); glVertex3f(+length-dl,-width,-depth);
   glTexCoord2f(0.0,rep); glVertex3f(+length-dl,-width,+depth);
   //  Left
   glColor3f(r,g,b);
   glNormal3f( -1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-length,+width,+depth);
   glTexCoord2f(rep,0.0); glVertex3f(-length,+width,-depth);
   glTexCoord2f(rep,rep); glVertex3f(-length-(-dl),-width,-depth);
   glTexCoord2f(0.0,rep); glVertex3f(-length-(-dl),-width,+depth);

   //  End
   glEnd();

   //  Switch off textures so it doesn't apply to the rest
   glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}


/*
 *  Draw the funnels of the ship
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 *     r- radius
 *     height
*/
static void funnel(double x,double y,double z,
                 double dx,double dy,double dz,
                 double r, double height)
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

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[1]);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(-90,1,0,0);
   glScaled(dx,dy,dz);

   glColor3f(1,0,0);

   // Cylinder
   glColor3f(1,1,1);

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
         glTexCoord2f(Cos(i),j); glVertex3f(r*Cos(i), r*Sin(i), j);
         glNormal3f(Cos(i), Sin(i), 0);
         glTexCoord2f(Cos(i),j+1); glVertex3f(r*Cos(i), r*Sin(i), j+1);
      }
      glEnd();
   }

   //  Switch off textures so it doesn't apply to the rest
   glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}

/*
 * static void base(double x,double y,double z,
 *                double dx,double dy,double dz,
 *                float dl, float length,
 *                float width, float depth, float r, float g, float b)
 * */

static void ship(double x,double y,double z,
                 double dx,double dy,double dz, double th)
{

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   // Draw the ship on origin
   base(0,0,0 , 0.5,0.5,0.5, 5, 10, 4, 30, 1,1,1, 0);
   base(0,3,0 , 0.5,0.5,0.5, 0, 10, 2, 30, 1,1,1, 2);
   base(0,4.5,0 , 0.4,0.4,0.4, 0, 10, 2, 20, 1,1,1, 3);
   base(0,5.5,0 , 0.3,0.3,0.3, 0, 10, 2, 20, 1,1,1, 2);


   //base(0,1.5,0 , 0.5,0.5,0.5 , 0, 10, 1, 30, 1,0,1, 2);
   //base(0,2,0 , 0.4,0.4,0.4 , 0, 10, 2, 20, 0,1,0, 0);
   //base(0,3,0 , 0.3,0.3,0.3 , 0, 10, 2, 20, 1,0,0, 2);

   //funnel(0,6,-4,.1,.1,.1,6,50);
   //funnel(0,6,0,.1,.1,.1,6,50);
   //funnel(0,6,4,.1,.1,.1,6,50);

   funnel(0,3,-4,.1,.1,.1,6,80);
   funnel(0,3,0,.1,.1,.1,6,80);
   funnel(0,3,4,.1,.1,.1,6,80);

   glEnd();
   //  Undo transformations
   glPopMatrix();
}


/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
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
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=10; //1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   //  Decide what to draw

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

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance_x*Cos(zh),ylight,distance_z*Sin(zh),1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0],Position[1],Position[2] , 0.3);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
        glWindowPos2i(5,75);
        Print("Position: X=%f Y=%f Z=%f",Position[0],Position[1],Position[2]);
   }
   else
     glDisable(GL_LIGHTING);

   if (light)
   {
      glWindowPos2i(5,45);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shinyvec[0]);
   }

   glEnable(GL_TEXTURE_2D);
   //glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , mode?GL_REPLACE:GL_MODULATE);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D,texture[0]);

   ship(0,0,-10 , 0.3,0.3,0.3, 0);
   ship(8,0,3 , 0.3,0.3,0.3, 0);
   ship(-8,0,6 , 0.3,0.3,0.3, 0);

   glDisable(GL_TEXTURE_2D);

   //  White
   glColor3f(1,1,1);
   //  Draw axes
   if (axes)
   {
      glBegin(GL_LINES);
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
   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,5);
   //  Print the text string
   glWindowPos2i(5,25);
   Print("Angle=%d,%d ",th,ph);
   if (p_mode==0) {
      Print("Mode = Prespective ");
      if (first_person_nav)
         Print("First person view enabled");
      else
         Print("First person view disabled");
   } else
      Print("Mode = Ortho ");

   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
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
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;

   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this toutine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/2000.0;
   zh = fmod(90*t,360);
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
   else if (ch == 'q' || ch == 'Q')
      axes = 1-axes;
   //  Switch projection mode
   else if (ch == 'p') {
      p_mode = 1;  // ortho
      first_person_nav = 0;
   } else if (ch == 'P')
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
      first_person_nav = 1;
      Ex = Ey = 0;
      Ez = 30;
   }
   else if (ch == 'F')
      first_person_nav = 0;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
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
   // increase distance in X
   else if (ch=='t' && distance_x>0)
      distance_x -= 1;
   else if (ch=='T' && distance_x>=0)
      distance_x += 1;
   // increase distance in Z
   else if (ch=='y' && distance_z>0)
      distance_z -= 1;
   else if (ch=='Y' && distance_z>=0)
      distance_z += 1;
   //  Toggle light movement
   else if (ch == '9')
      stall_light = 1-stall_light;

   //  Translate shininess power to value (-1 => 0)
   shinyvec[0] = shininess<0 ? 0 : pow(2.0,shininess);
   //  Animate if requested
   glutIdleFunc(stall_light?idle:NULL);

   //  Keep angles to +/-360 degrees
   dth %= 360;

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
   // Set projection
   Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Assignment 2: Adil Sadik");
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Load textures
   texture[0] = LoadTexBMP("metal.bmp");
   texture[1] = LoadTexBMP("funnel.bmp");
   texture[2] = LoadTexBMP("boat2.bmp");
   texture[3] = LoadTexBMP("boat0.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}

