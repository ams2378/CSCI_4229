/* CSCI 4229 Summer 2016
 * Assignment 2: Projection
 * Adil Sadik
 * sadik.adil@gmail.com
 */
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
double zh=0;      //  Rotation of teapot
int axes=1;       //  Display axes
int mode=0;       //  What to display
int p_mode=1;
double dim= 10;
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio

double Ex;
double Ey;
double Ez;

double Cx = 0;
double Cy = 0;
double Cz = 0;

int first_person_nav=0;

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

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
                 float width, float depth, float r, float g, float b)
{

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);


   //  Top
   glColor3f(r,g,b);
   glVertex3f(-length,+width,+depth);
   glVertex3f(+length,+width,+depth);
   glVertex3f(+length,+width,-depth);
   glVertex3f(-length,+width,-depth);
   //  Bottom
   glVertex3f(-length-(-dl),-width,+depth);
   glVertex3f(+length-dl,-width,+depth);
   glVertex3f(+length-dl,-width,-depth);
   glVertex3f(-length-(-dl),-width,-depth);
   //  Front
   glColor3f(r,g,b);
   glVertex3f(-length,+width,+depth);
   glVertex3f(+length,+width,+depth);
   glVertex3f(+length-dl,-width,+depth);
   glVertex3f(-length-(-dl),-width,+depth);
   //  Back
   glColor3f(r,g,b);
   glVertex3f(-length,+width,-depth);
   glVertex3f(+length,+width,-depth);
   glVertex3f(+length-dl,-width,-depth);
   glVertex3f(-length-(-dl),-width,-depth);
   //  Right
   glColor3f(r,g,b);
   glVertex3f(+length,+width,+depth);
   glVertex3f(+length,+width,-depth);
   glVertex3f(+length-dl,-width,-depth);
   glVertex3f(+length-dl,-width,+depth);
   //  Left
   glColor3f(r,g,b);
   glVertex3f(-length,+width,+depth);
   glVertex3f(-length,+width,-depth);
   glVertex3f(-length-(-dl),-width,-depth);
   glVertex3f(-length-(-dl),-width,+depth);

   //  End
   glEnd();
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
      glBegin(GL_QUAD_STRIP);
      if (j>=(height-(height/5))) {
         glColor3f(1,0,0);
      }
      for (i=0;i<=360;i+=d)
      {
         glVertex3f(r*Cos(i), r*Sin(i), j);
         glVertex3f(r*Cos(i), r*Sin(i), j+1);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}


/*
 *  Draw the ship
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
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
   base(0,0,0 , 0.5,0.5,0.5 , 5, 10, 2, 30, 1,1,1);
   base(0,1.5,0 , 0.5,0.5,0.5 , 0, 10, 1, 30, 1,0,1);
   base(0,2,0 , 0.4,0.4,0.4 , 0, 10, 2, 20, 0,1,0);
   base(0,3,0 , 0.3,0.3,0.3 , 0, 10, 2, 20, 1,0,0);
   funnel(0,3,-4,.1,.1,.1,6,40);
   funnel(0,3,0,.1,.1,.1,6,40);
   funnel(0,3,4,.1,.1,.1,6,40);

   glEnd();
   //  Undo transformations
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

   switch (mode)
   {
      //  Draw ships
      case 0:
         ship(0,0,8 , 0.4,0.4,0.4, 0);
         ship(0,0,-13 , 0.3,0.3,0.3, 0);
         ship(8,0,0 , 0.3,0.3,0.3, 0);
         ship(-8,0,0 , 0.3,0.3,0.3, 0);
         break;
      case 1:
         ship(0,0,5 , 0.6,0.6,0.6, 0);

         break;
   }
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
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = dth = first_person_nav = p_mode = 1;
   //  Toggle axes
   else if (ch == 'q' || ch == 'Q')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'm')
      mode = 0;
   else if (ch == 'M')
      mode = 1;
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
 *  GLUT calls this toutine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360);
   glutPostRedisplay();
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
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}

