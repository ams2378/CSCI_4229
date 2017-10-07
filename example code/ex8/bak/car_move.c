//---+----4----+----3----+----2----+----1----+---||---+----1----+----2----+----3----+----4----+---\\
//---+----4----+----3----+----2----+----1----+---||---+----1----+----2----+----3----+----4----+---\\

#include <stdio.h>
#include <stdlib.h>
//#include <gl.h>
//#include <glu.h>
//#include <glut.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

float _angle = 0.0f,   // For rotating the car
        xpos = 0.0f,   // For moving the car in the X axis
        ypos = 0.0f;   // For moving the car in the Y axis

float cameraheight = -20.0f;   // For zoom or unzoom the camera

//---+----4----+----3----+----2----+----1----+---||---+----1----+----2----+----3----+----4----+---\\
//--------------------------------------   handleKeypress   -----------------------------------------

void handleKeypress (unsigned char key, int x, int y)
{
    switch (key) {
       case '1':  _angle += 5.0f;        break;
       case '2':  _angle -= 5.0f;        break;
       case '3':  cameraheight -= 5.0f;  break;
       case '4':  cameraheight += 5.0f;  break;
       case '5':  xpos += 1.0f;          break;
       case '6':  xpos -= 1.0f;          break;
       case '7':  ypos += 1.0f;          break;
       case '8':  ypos -= 1.0f;          break;

       case 'r':  xpos         =   0.0f;
		  ypos         =   0.0f;
		  _angle       =   0.0f;
		  cameraheight = -20.0f; break;

       case  27:  exit(0);               break;   // esc
    }

    glutPostRedisplay ();
}

//---+----4----+----3----+----2----+----1----+---||---+----1----+----2----+----3----+----4----+---\\
//---------------------------------------   initRendering   ----------------------------------------

void initRendering (void)
{
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_COLOR_MATERIAL);
    glClearColor (0.2f, 0.2f, 0.2f, 1.0f);
}

//---+----4----+----3----+----2----+----1----+---||---+----1----+----2----+----3----+----4----+---\\
//---------------------------------------   handleResize   -----------------------------------------

void handleResize (int w, int h)
{
    glViewport     (0, 0, w, h);
    glMatrixMode   (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (45.0, (double)w / (double)h, 0.01, 500.0);
}

//---+----4----+----3----+----2----+----1----+---||---+----1----+----2----+----3----+----4----+---\\
//-----------------------------------------   Draw_Car   -------------------------------------------

void Draw_Car (void)
{
    glBegin (GL_QUADS);                      // Ferrari
       glColor3f  ( 1.0f ,  0.0f, 0.0f);     // Red Ferrari
       glVertex3f (-2.25f,  1.0f, 0.0f);     // Meters (4,5m long per 2,25m wide)
       glVertex3f ( 2.25f,  1.0f, 0.0f);
       glVertex3f ( 2.25f, -1.0f, 0.0f);
       glVertex3f (-2.25f, -1.0f, 0.0f);
    glEnd();
}

//---+----4----+----3----+----2----+----1----+---||---+----1----+----2----+----3----+----4----+---\\
//----------------------------------------   drawScene   -------------------------------------------

/*
void drawScene (void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

//  glRotatef    (-_angle, 0.0f, 1.0f, 0.0f);
    glRotatef    (-_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef ( 0.0f, 0.0f, cameraheight);

    glPushMatrix();
       glTranslatef (xpos, ypos, 0.0f);
       Draw_Car ();
    glPopMatrix();

    glutSwapBuffers();
}
*/

void drawScene (void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    glTranslatef ( 0.0f, 0.0f, cameraheight);

//  Rotation moved into Push-Pop block to make it only apply to car.
//  Try code below.  Then try putting glRotate after glTranslate.
//  Neither way works. Something more sophisticated has to be done.

    glPushMatrix();
       glTranslatef (xpos, ypos, 0.0f);
       glRotatef    (-_angle, 0.0f, 0.0f, 1.0f);
       Draw_Car ();
    glPopMatrix();

    glutSwapBuffers();
}


//---+----4----+----3----+----2----+----1----+---||---+----1----+----2----+----3----+----4----+---\\
//------------------------------------------   update   --------------------------------------------

void update (int value)
{
    _angle += 0.1f;

    if (_angle > 360 ) _angle -= 360;

    glutPostRedisplay ();
    glutTimerFunc (16, update, 0);
}

//---+----4----+----3----+----2----+----1----+---||---+----1----+----2----+----3----+----4----+---\\
//-------------------------------------------   main   ---------------------------------------------

int main (int argc, char* argv[])
{
    glutInit (&argc, argv);
    glutInitDisplayMode     (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize      (640, 480);
    glutInitWindowPosition  (400, 200);

    glutCreateWindow ("Test");
    initRendering ();

    glutDisplayFunc  (drawScene);
    glutKeyboardFunc (handleKeypress);
    glutReshapeFunc  (handleResize);
//  glutTimerFunc    (16, update, 0);

    glutMainLoop ();
    return 0;
}

//---+----4----+----3----+----2----+----1----+---||---+----1----+----2----+----3----+----4----+---\\
