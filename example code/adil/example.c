/*
 *  Hello World Triangle Version 5
 */
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
double th=0;  //  Rotation angle
double zh=0;  //  Spin angle

float points[50000][3];

/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;


int iter = 0;
int iterInc = 10;

/*
 *  Main
 */
static void lorenz (void)
{
   int i;
   /*  Coordinates  */
   double x = 1;
   double y = 1;
   double z = 1;

   points[0][0] += 1; 
   points[0][1] += 1; 
   points[0][2] += 1; 
   /*  Time step  */
   float dt = 0.001;

   printf("%5d %8.3f %8.3f %8.3f\n",0,x,y,z);
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (i=0;i<50000;i++)
   {
      float dx = s*(y-x);
      float dy = x*(r-z)-y;
      float dz = x*y - b*z;
      points[i][0] += dt*dx;
      points[i][1] += dt*dy;
      points[i][2] += dt*dz;
      //printf("%5d %8.3f %8.3f %8.3f\n",i+1,points[i][0],points[i][1],points[i][2]);
      //printf("%5d %8.3f %8.3f %8.3f\n",i+1,x,y,z);
   }
}


/*
 * Convenience function for text
 */
#define LEN 8192  //  Maximum amount of text
void Print(const char* format , ...)
{
   char    buf[LEN]; // Text storage
   char*   ch=buf;   // Text pointer
   //  Create text to be display
   va_list args;
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display text string
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
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
 * This function is called by GLUT to display the scene
 */

void display()
{

   //  Clear screen and Z-buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //glPushMatrix();

   //	glRotatef(25, 1.0, 0.0, 0.0);
   //	glRotatef(50, 0.0, 1.0, 0.0);
   //	glRotatef(35, 0.0, 0.0, 1.0);

   //  Reset transformations
   //glLoadIdentity();
   //  RGB triangle
   glBegin(GL_LINE_STRIP);
   //glBegin(GL_POINTS);
   int i=0;
   for (i=0;i<50000;i=i+60)
   {
      //glColor3fv(0.0,0.0,1.0); glVertex3fv(points[i]);
      glVertex3fv(points[i]);
      //printf("%5d %8.3f %8.3f %8.3f\n",i+1,points[i][0],points[i][1],points[i][2]);
   }

/*	while( i < iter && i < 50000 ) {
		glColor3fv(points[i]);
		glVertex3fv(points[i++]);
	}
	glEnd();
	
	if( iter < 50000 ) {
		if( iter + iterInc > 50000 ) iter = 50000;
		else iter+=iterInc;
	}
*/
   //glVertex2f(0.0,0.0);
   //glVertex2f(0.5,-0.5);
   //glVertex2f(-0.5,-0.3);

   //  Sanity check
   ErrCheck("display");
   //  Flush and swap buffer
   glFlush();
   glutSwapBuffers();
   //glPopMatrix();
}


/*
 * This function is called by GLUT when special keys are pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow - increase rotation by 5 degree
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow - decrease rotation by 5 degree
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Request display update
   glutPostRedisplay();
}

/*
 * This function is called by GLUT when the window is resized
 */
/*void reshape(int width,int height)
{
   //  Calculate width to height ratio
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set viewport as entire window
   glViewport(0,0, width,height);
   //  Select projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Set projection to identity
   glLoadIdentity();
   //  Orthogonal projection:  unit cube adjusted for aspect ratio
   glOrtho(-w2h,+w2h, -1.0,+1.0, -1.0,+1.0);
   //  Select model view matrix
   glMatrixMode(GL_MODELVIEW);
   //  Set model view to identity
   glLoadIdentity();
}*/


static void reshape(int width, int height) 
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-5.0, 5.0, -h*2, h*2, 1.0, 300.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -60.0);
}


/*
 * This function is called by GLUT when idle
 */
void idle()
{
   //  Get elapsed (wall) time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   //  Calculate spin angle 90 degrees/second
   zh = fmod(90*t,360);
   //  Request display update
   glutPostRedisplay();
}

/*
 * GLUT based Hello World
 */
int main(int argc,char* argv[])
{


   lorenz();

   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered true color window with Z-buffer
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   //glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

   glutInitWindowPosition(50, 50);
   glutInitWindowSize(500, 500);

   //  Create window
   glutCreateWindow("Adil Sadik- Assignment 1- Lorenz Attractor");
   //  Register display, reshape, idle and key callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutIdleFunc(idle);
   glutSpecialFunc(special);
   //  Enable Z-buffer depth test
   //glEnable(GL_DEPTH_TEST);
   //  Pass control to GLUT for events
   glutMainLoop();
   //  Return to OS
   return 0;
}

