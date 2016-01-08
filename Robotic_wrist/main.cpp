// This program is from the OpenGL Programming Guide.  It shows a robot arm
// that you can rotate by pressing the arrow keys.

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GLUI/GLUI.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

using namespace std;

// Define the joint variables
static int theta1 = 0;
static int theta2 = 0;
static int theta3 = 0;

// Pointers to the windows and some of the controls
int             main_window;
int             counter = 0;
GLUI            *glui;
GLUI_Checkbox   *checkbox;
GLUI_Spinner    *theta1_spinner, *theta2_spinner, *theta3_spinner;
GLUI_Panel      *joint_angles, *rehab_procedures;

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


// Run an idle function to update the GLUI controls based on what is happening in the screen
// This function will allow the angles to be displayed in the spinners as the device runs through
// the rehab procedures
void myGlutIdle(void)
{

    if (glutGetWindow() != main_window)
        glutSetWindow(main_window);
    
    
    glutPostRedisplay();
    
    counter++;
    
    glui->sync_live();
    
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
    // The rotation will occur at a distance away from the wrist at the diameter of the wrist brace
    // the wrist (rotation about the x-axis)
    glRotatef((GLfloat)theta1, 1.0, 0.0, 0.0);
    glTranslatef(1.0, 0.0, 0.0);
    glTranslatef(0.0, 1.0, 0.0);
    wireBox(2.0, 0.4, 1.0);
    
    // Draw the second joint. This joint will allow for ulnar deviation and radial deviation
    // This joint will rotate around the y-axis in the opengl environment
    // This joint will sit at the end of the first joint
    glTranslatef(1.0, 0.0, 0.0);
    glRotatef((GLfloat)theta2, 0.0, 1.0, 0.0);
    glTranslatef(1.0, 0.0, 0.0);
    wireBox(2.0, 0.4, 1.0);
    
    // Draw the third joint, this joint will allow for flexion and extension of the wrist
    // This joint will rotate about the z-axis
    // it will be placed at the end of second joint
    glTranslatef(1.0, 0.0, 0.0);
    glRotatef((GLfloat)theta3, 0.0, 0.0, 1.0);
    glTranslatef(1.0, 0.0, 0.0);
    wireBox(2.0, 0.4, 1.0);
    
    // Draw the hand brace to support the hand
    // Use three wirecube functions
    // Top of the brace
    glTranslatef(0.0, -0.4, 0.0);
    wireBox(2.0, 0.4, 4.0);
    
    // bottom of the hand brace
    glTranslatef(0.0, -2.0, 0.0);
    wireBox(2.0, 0.4, 4.0);
    
    // Side of the hand brace
    glTranslatef(0.0, 1.2, -2.0);
    wireBox(2.0, 2.4, 0.4);

    
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
    gluLookAt(4,2,8, 4,0,0, 0,1,0);
}

// Write a tracking function to move the wrist through the rehabilitation movements smoothly
// This function will take 4 inputs. Two of the inputs will be of type float which and will specify the initial
// and end point of the motion respectively. The third input will be a pointer to the angle to be updated.
// The last input will be the time to complete the motion
// It will create smooth motion by using a cubic polynomial function it will return no values.
void trajectory (int initial_angle, int final_angle, int *theta, int tf){
    
    // Initialize the paramters for the equation
    // Note a1 = 0 since the arm has no initial velocity
    float a0 = initial_angle;
    float a2 = 3/pow(tf,2) * (final_angle - initial_angle);
    float a3 = -2/pow(tf,3) * (final_angle - initial_angle);
    
    // Create a for loop to run the angle through the cubic trajectory
    for (int t = 0; t <= tf; t++ ){
        // Use the cubic trajectory function
        *theta = a0 + a2*pow(t, 2) + a3*pow(t, 3);
        printf("%d\n",theta3);
        //usleep(100);
        // make a call to the display function to display the image on the screen
        display();
        
    }
    
    // Return from the function when the for loop is complete
    return;
    
}

// Flexion Extension function to take the wrist through flexion extension rehab
void flexion_rehab(void){
    // Write a function here to take the wrist through rehab with slow movement
    // Need a tracking function
    // Print something on the screen for testing purposes
    printf("flexion/extension rehab procedures activated...\n");
}

// Supination Extension function to take the wrist through supination rehab
void supination_rehab(void){
    // Write a function here to take the wrist through rehab with slow movement
    // Need a tracking function
    // Print something on the screen for testing purposes
    printf("Supination/Pronation rehab procedures activated...\n");
}

// Radial Extension function to take the wrist through radial extension rehab
void radial_rehab(void){
    // Write a function here to take the wrist through rehab with slow movement
    // Need a tracking function
    // Print something on the screen for testing purposes
    printf("Radial/Ulnar deviation rehab procedures activated...\n");
    int * theta3_pointer;
    theta3_pointer = &theta3;
    
    // Make calls to the trajectory function to run through the rehab procedures smoothly
    // If the wrist is not currently at the zero position, take it there
    if (theta3 != 0){
        trajectory(theta3, 0, theta3_pointer, 100); // Take you from the current position to +0
    }
    
    // After returning to the zero position, run through the rehab
    trajectory(theta3, 60, theta3_pointer, 100); // Take you from the current position to +60
    trajectory(theta3, -60, theta3_pointer, 100); // Take you from the current position to -60
    trajectory(theta3, 0, theta3_pointer, 100); // Take you from the current position to
    return;
}

// Initializes GLUT, the display mode, and main window; registers callbacks;
// does application initialization; enters the main event loop.
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(80, 80);
    glutInitWindowSize(800, 600);
    main_window = glutCreateWindow("Robot Arm");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    init();
    
    // Use the GLUI interface to set up a GUI to control the wrist rehabiliation device
    // Create the main glui window
    glui = GLUI_Master.create_glui( "GLUI", 0, 400, 50 );
    
    // Add an invisible panel to hold the controls
    GLUI_Panel *panel1 = new GLUI_Panel(glui, "", GLUI_PANEL_NONE );
    
    // Create a panel to contain the joint angles
    joint_angles = new GLUI_Panel( panel1, "Joint Angles" );
    
    // Add spinners for the three joint variables
    // Spinner for theta 1
    theta1_spinner = new GLUI_Spinner( joint_angles, "Scale:", &theta1);
    theta1_spinner->set_float_limits( -90, 90 );
    theta1_spinner->set_alignment( GLUI_ALIGN_RIGHT );
    
    // Spinner for theta 2
    theta2_spinner = new GLUI_Spinner( joint_angles, "Scale:", &theta2);
    theta2_spinner->set_float_limits( -40, 40 );
    theta2_spinner->set_alignment( GLUI_ALIGN_RIGHT );
    
    // Spinner for theta 3
    theta3_spinner = new GLUI_Spinner( joint_angles, "Scale:", &theta3);
    theta3_spinner->set_float_limits( -60, 60 );
    theta3_spinner->set_alignment( GLUI_ALIGN_RIGHT );
    
    // Create a panel to contain the rehabilitation procedures
    rehab_procedures = new GLUI_Panel( panel1, "Rehabiliation Procedures" );
    
    // Add buttons to simulate the wrist rehabilitation procedures
    // First button for the flexion/extension rehab
    new GLUI_Button( rehab_procedures, "Flexion/Extension", 0,(GLUI_Update_CB) flexion_rehab);
    
    // Second button for the supination and pronation
    new GLUI_Button( rehab_procedures, "Supination/Pronation", 0,(GLUI_Update_CB) supination_rehab);
    
    // Third button is for radial, ulnar deviation
    new GLUI_Button( rehab_procedures, "Radial/Ulnar", 0,(GLUI_Update_CB) radial_rehab);

    // Add a quit button
    new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );
    
    
    // Link the controls to the main graphics window
    glui->set_main_gfx_window( main_window );
    
    // Make a call to the GLUI idle function here so that the spinners update as the program runs
    GLUI_Master.set_glutIdleFunc(myGlutIdle);
    
    // Enter the glut main loop
    glutMainLoop();
}












