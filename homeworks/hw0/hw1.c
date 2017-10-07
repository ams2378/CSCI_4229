/* CSCI 4229 Summer 2016
 * Assignment 1: Lorenz attractor
 * Adil Sadik
 * sadik.adil@gmail.com
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//  Globals
int th=0;       // Azimuth of view angle
int ph=0;       // Elevation of view angle
double dim=2;   // Dimension of orthogonal box

float points[50000][3];
float rotx = 0.0, roty = 0.0, rotz = 0.0;
double idle_c;
int animation=0;
int display_axis=0;

/*  Lorenz Parameters  */
float s  = 10;
float b  = 2.6666;
float r  = 28;

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
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
 *  Lorenz generator
 */
static void lorenz (void)
{
   int i;
   /*  Coordinates  */
   float x = 1;
   float y = 1;
   float z = 1;

   points[0][0] = 1;
   points[0][1] = 1;
   points[0][2] = 1;

   /*  Time step  */
   float dt = 0.001;

   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (i=0;i<50000;i++)
   {
      float dx = s*(y-x);
      float dy = x*(r-z)-y;
      float dz = x*y - b*z;

      x += dt*dx;
      y += dt*dy;
      z += dt*dz;

      points[i+1][0] = x;
      points[i+1][1] = y;
      points[i+1][2] = z;

   }
}

/*
 *  Function to print any errors encountered
 */
void ErrCheck(char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

/*
 *  Display the scene
 */
void display()
{

   // generate vertices
   lorenz();

   //  Clear screen and Z-buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //  Reset previous transforms
   glLoadIdentity();

   //  Set view angle
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);

   // rotate around axis
   glRotatef(rotx, 1.0, 0.0, 0.0);
   glRotatef(roty, 0.0, 1.0, 0.0);
   glRotatef(rotz, 0.0, 0.0, 1.0);

   glBegin(GL_LINE_STRIP);
   int i=0;

   if (animation==1){
      while (i < idle_c && i<50000){
         glColor3fv(points[i]);
         glVertex3fv(points[i]);
         i=i+1;
      }
      if (idle_c==50000)
         idle_c = 0;
   } else {
      while (i<50000){
         glColor3fv(points[i]);
         glVertex3fv(points[i]);
         i=i+1;
      }
   }
   glEnd();

   if (display_axis==1) {
      //  Draw axes in white
      glColor3f(1,1,1);
      glBegin(GL_LINES);
      glVertex3d(0,0,0);
      glVertex3d(40,0,0);
      glVertex3d(0,0,0);
      glVertex3d(0,40,0);
      glVertex3d(0,0,0);
      glVertex3d(0,0,40);
      glEnd();
      //  Label axes
      glRasterPos3d(40,0,0);
      Print("X");
      glRasterPos3d(0,40,0);
      Print("Y");
      glRasterPos3d(0,0,40);
      Print("Z");
   }

   //  Display parameters
   glColor3f(1,1,1);
   glWindowPos2i(5,5);
   Print("Lorenz params: (s, b, r) : (%f, %f, %f)",s,b,r);

   //  Sanity check
   ErrCheck("display");
   //  Flush and swap buffer
   glFlush();
   glutSwapBuffers();
}


/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27) {
      exit(0);
   //  Reset view angle
   } else if (ch == '0') {
      s  = 10;
      b  = 2.6666;
      r  = 28;
      th = ph = 0;
      animation=0;
   } else if (ch == 's') {
      s += 1;
   } else if (ch == 'S') {
      s -= 1;
   } else if (ch == 'b') {
      b += 1;
   } else if (ch == 'B') {
      b -= 1;
   } else if (ch == 'r') {
      r += 1;
   } else if (ch == 'R') {
      r -= 1;
   } else if (ch == 'x') {
      rotx += 10.0;
   } else if (ch == 'y') {
      roty += 10.0;
   } else if (ch == 'z') {
      rotz += 10.0;
   } else if (ch == 'a') {
      animation = 1;
   } else if (ch == 'A') {
      animation = 0;
   } else if (ch == 'd') {
      display_axis = 1;
   } else if (ch == 'D') {
      display_axis = 0;
   }

   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
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
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 * This function is called by GLUT when idle
 */
void idle()
{
   if (animation==1) {
      idle_c = idle_c + 100;
   }
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   glOrtho(-dim*w2h*30, +dim*w2h*30, -dim*30, +dim*30, -dim*30, +dim*55);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{

   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(500,500);
   //  Create the window
   glutCreateWindow("Assignment 1: Adil Sadik");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}

