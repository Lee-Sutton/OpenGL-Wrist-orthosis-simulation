// This program is from the OpenGL Programming Guide.  It shows a robot arm
// that you can rotate by pressing the arrow keys.

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Define the joint variables
static int theta1 = 0;
static int theta2 = 0;
static int theta3 = 0;

// This function will handle input from the keyboard and change the joint variables accordingly
// This will be used for testing purposes, the main input will come from the GLUI user interface
void special(int key, int, int) {
    switch (key) {
        case GLUT_KEY_LEFT: (theta2+= 5) %= 360; break;
        case GLUT_KEY_RIGHT: (theta2 -= 5) %= 360; break;
        case GLUT_KEY_UP: (theta1 += 5) %= 360; break;
        case GLUT_KEY_DOWN: (theta1 -= 5) %= 360; break;
        default: return;
    }
    glutPostRedisplay();
}

// wireBox(w, h, d) makes a wireframe box with width w, height h and
// depth d centered at the origin.  It uses the GLUT wire cube function.
// The calls to glPushMatrix and glPopMatrix are essential here; they enable
// this function to be called from just about anywhere and guarantee that
// the glScalef call does not pollute code that follows a call to myWireBox.
void wireBox(GLdouble width, GLdouble height, GLdouble depth) {
    glPushMatrix();
    glScalef(width, height, depth);
    glutWireCube(1.0);
    glPopMatrix();
}

// Displays the wrist exoskeleton starting with all joint variables = 0
// function is bracketed by glPushMatrix and glPopMatrix
// Used heirarchial modelling to transform the joints around the appropriate frames
void display() {
    
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    // Draw the first joint this joint will allow for pronation and supination of
    // the wrist (rotation about the x-axis)
    glRotatef((GLfloat)theta1, 1.0, 0.0, 0.0);
    glTranslatef(1.0, 0.0, 0.0);
    wireBox(2.0, 0.4, 1.0);
    
    // Draw the second joint. This joint will allow for ulnar deviation and radial deviation
    // This joint will rotate around the y-axis in the opengl environment
    glRotatef((GLfloat)theta2, 0.0, 1.0, 0.0);
    glTranslatef(1.0, 0.0, 0.0);
    wireBox(2.0, 0.4, 1.0);
    
    // Now we are ready to draw the lower arm.  Since the lower arm is attached
    // to the upper arm we put the code here so that all rotations we do are
    // relative to the rotation that we already made above to orient the upper
    // arm.  So, we want to rotate elbow degrees about the z-axis.  But, like
    // before, the anchor point for the rotation is at the end of the box, so
    // we translate <1,0,0> before rotating.  But after rotating we have to
    // position the lower arm at the end of the upper arm, so we have to
    // translate it <1,0,0> again.
    glTranslatef(1.0, 0.0, 0.0);
    glRotatef((GLfloat)theta2, 0.0, 0.0, 1.0);
    glTranslatef(1.0, 0.0, 0.0);
    wireBox(2.0, 0.4, 1.0);
    
    glPopMatrix();
    glFlush();
}

// Handles the reshape event by setting the viewport so that it takes up the
// whole visible region, then sets the projection matrix to something reason-
// able that maintains proper aspect ratio.
void reshape(GLint w, GLint h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, GLfloat(w)/GLfloat(h), 1.0, 20.0);
}

// Perfroms application specific initialization: turn off smooth shading,
// sets the viewing transformation once and for all.  In this application we
// won't be moving the camera at all, so it makes sense to do this.
void init() {
    glShadeModel(GL_FLAT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(1,2,8, 0,0,0, 0,1,0);
}

// Initializes GLUT, the display mode, and main window; registers callbacks;
// does application initialization; enters the main event loop.
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(80, 80);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Robot Arm");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    init();
    glutMainLoop();
}