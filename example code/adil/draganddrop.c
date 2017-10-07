// clickdrag.cpp
// Glenn G. Chappell
// 18 Sep 2012
//
// For CS 381 Fall 2012
// Click-and-Drag Demo

// OpenGL/GLUT includes - DO THESE FIRST
//#include <cstdlib>       // Do this before GL/GLUT includes
using std::exit;
#ifndef __APPLE__
# include <GL/glut.h>    // GLUT stuff, includes OpenGL headers as well
#else
# include <GLUT/glut.h>  // Apple puts glut.h in a different place
#endif

// Other includes
#include "lib381/bitmapprinter.h"
                         // For class BitmapPrinter
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;


// class Rect
// Rectangle that can draw itself, be repositioned, do extent testing
class Rect {
public:

    // Ctor - takes x,y of center, half of side lengths
    // all in drawing coordinates
    Rect(double theXpos, double theYpos,
         double theXhalfside, double theYhalfside)
        :xhalfside_(theXhalfside), yhalfside_(theYhalfside)
    { setXpos(theXpos); setYpos(theYpos); }

    // Compiler-generated copy ctor, copy =, dctor used

    // setXpos
    // Set x position of center to given value
    void setXpos(double theXpos)
    { xpos_ = theXpos; }

    // setYpos
    // Set y position of center to given value
    void setYpos(double theYpos)
    { ypos_ = theYpos; }

    // getXpos
    // Return x position of center
    double getXpos() const
    { return xpos_; }

    // getYpoS
    // Return y position of center
    double getYpos() const
    { return ypos_; }

    // draw
    // Draws rectangle in OpenGL
    // Assumes matrix mode is GL_MODELVIEW
    void draw() const
    {
        glPushMatrix();
        glTranslated(xpos_, ypos_, 0.);
        glScaled(xhalfside_, yhalfside_, 1.);
        drawSquare();
        glPopMatrix();
    }

    // inExtent
    // Given x, y pos of pt (drawing coords)
    // returns true of pt lies inside the rectangle
    bool inExtent(double x, double y) const
    {
    return x >= xpos_ - xhalfside_
        && x <= xpos_ + xhalfside_
        && y >= ypos_ - yhalfside_
        && y <= ypos_ + yhalfside_;
    }

private:

    // drawSquare (copied from mouse.cpp)
    // Draws a filled square, using current GL states,
    //  in the x,y plane, centered at the origin, aligned w/ x & y axes,
    //  with side 2.
    void drawSquare() const
    {
        glBegin(GL_QUADS);
            glVertex2d(-1., -1.);
            glVertex2d( 1., -1.);
            glVertex2d( 1.,  1.);
            glVertex2d(-1.,  1.);
        glEnd();
    }

private:

    double xpos_, ypos_;            // x, y pos of center (object coords)
    double xhalfside_, yhalfside_;  // half side lengths (object coords)
    // Note: Object coordinates are the coordinates we draw in. In
    //  particular, the coordinates passed to the glVertex* command are
    //  in object coordinates.

};  // End class Rect


// Global variables
// Keyboard
const int ESCKEY = 27;         // ASCII value of Escape

// Window/viewport
const int startwinsize = 600;  // Start window width & height (pixels)
int winw = 1, winh = 1;        // Window width, height (pixels)
                               //  (Initialize to avoid spurious errors)
double cam_xmin, cam_xmax, cam_ymin, cam_ymax;
                               // Viewport bdaries in world (camunits)

// Objects
Rect myrect(0.0, 0.0, 0.3, 0.2);

// Mouse
double cam_mousex, cam_mousey; // Mouse x, y pos (camunits)
                               //  Above initialized to 0., 0.
                               //  then set at first mouse event
bool dragging;                 // True if currently dragging object


// myDisplay
// The GLUT display function
void myDisplay()
{
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw objects

    // Draw the rectangle
    glColor3d(dragging ? 1.0 : 0.0, 0.3, 0.3);  // Change color on drag
    glLoadIdentity();
    myrect.draw();

    // Draw documentation
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);  // Set up simple ortho projection
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0., double(winw), 0., double(winh));
    glColor3d(0., 0., 0.);        // Black text
    BitmapPrinter p(20., winh-20., 20.);
    p.print("Click and drag the object");
    p.print("Space    Recenter");
    p.print("Esc      Quit");
    glPopMatrix();                // Restore prev projection
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}


// myIdle
// The GLUT idle function
void myIdle()
{
    // Print OpenGL errors, if there are any (for debugging)
    static int error_count = 0;
    if (GLenum err = glGetError())
    {
        ++error_count;
        cerr << "OpenGL ERROR " << error_count << ": "
             << gluErrorString(err) << endl;
    }
}


// myKeyboard
// The GLUT keyboard function
void myKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ESCKEY:  // Esc: quit
        exit(0);
        break;
    case ' ':     // Space: recenter object
        if (!dragging)
        {
            myrect.setXpos(0.);
            myrect.setYpos(0.);
            glutPostRedisplay();
        }
        break;
    }
}


// myReshape
// The GLUT reshape function
void myReshape(int w, int h)
{
    // Set viewport & save window dimensions in globals
    glViewport(0, 0, w, h);
    winw = w;
    winh = h;

    // Set up projection
    // Save max/min x/y coords in globals
    // Projection is orthographic. Aspect ratio is correct,
    // and region -1..1 in x & y always just fits in viewport.
    if (w > h)
    {
        cam_xmin = -double(w)/h;
        cam_xmax = double(w)/h;
        cam_ymin = -1.;
        cam_ymax = 1.;
    }
    else
    {
        cam_xmin = -1.;
        cam_xmax = 1.;
        cam_ymin = -double(h)/w;
        cam_ymax = double(h)/w;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(cam_xmin, cam_xmax, cam_ymin, cam_ymax);

    glMatrixMode(GL_MODELVIEW);  // Always go back to model/view mode
}


// saveMousePos
// Given mouse position in GLUT format, compute corresponding camera
// coords and save in globals cam_mousex, cam_mousey.
// Uses globals winw, winh, cam_xmin, cam_xmax, cam_ymin, cam_ymax.
void saveMousePos(int x, int y)
{
    double t;  // Lirping parameter

    t = double(x)/winw;
    cam_mousex = cam_xmin + t*(cam_xmax-cam_xmin);

    t = double(y)/winh;
    cam_mousey = cam_ymax + t*(cam_ymin-cam_ymax);

    glutPostRedisplay();
}


// myMouse
// The GLUT mouse function
void myMouse(int button, int state, int x, int y)
{
    // Compute mouse pos in camera coords & save in globals
    saveMousePos(x, y);
    // NOTE: Need to do above before calling inExtent below

    // Deal with dragging if left button press/release
    if (button == GLUT_LEFT_BUTTON)
    {
        // Set dragging depending on whether button down in object
        dragging = (state == GLUT_DOWN)
                && myrect.inExtent(cam_mousex, cam_mousey);
        glutPostRedisplay();
    }
}


// myMotion
// The GLUT motion function
void myMotion(int x, int y)
{
    double old_mousex = cam_mousex;
    double old_mousey = cam_mousey;
    // Compute mouse pos in camera coords & save in globals
    saveMousePos(x, y);

    if (dragging)
    {
        myrect.setXpos(myrect.getXpos() + cam_mousex - old_mousex);
        myrect.setYpos(myrect.getYpos() + cam_mousey - old_mousey);
        glutPostRedisplay();
    }
}


// init
// Initialize GL states & global data
// Called by main after window creation
void init()
{
    // Mouse
    cam_mousex = 0.;
    cam_mousey = 0.;
    dragging = false;

    // OpenGL Stuff
    // (Nothing here)
}


int main(int argc, char ** argv)
{
    // Initialize OpenGL/GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // Make a window
    glutInitWindowSize(startwinsize, startwinsize);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("CS 381 - Click-and-Drag Demo");

    // Initialize GL states & register GLUT callbacks
    init();
    glutDisplayFunc(myDisplay);
    glutIdleFunc(myIdle);
    glutKeyboardFunc(myKeyboard);
    glutReshapeFunc(myReshape);
    glutMouseFunc(myMouse);
    glutMotionFunc(myMotion);

    // Do something
    glutMainLoop();

    return 0;
}


