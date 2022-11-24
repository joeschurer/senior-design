//render a csv of a 3D point cloud in OpenGL
//File contains x,y,z coordinates of points
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <GL/glut.h>
#include <GL/gl.h>

//compile with g++ -O3 -Wall -o render render.cpp -lfreeglut
//g++ -O0 -g3 -Wall -c -fmessage-length=0 -o render.o render.cpp
//g++ -o render.exe render.o -lglu32 -lopengl32 -lfreeglut

using namespace std;

//global variables
vector<float> x;
vector<float> y;
vector<float> z;
int numPoints = 0;

//function to read in the csv file
void readCSV(string filename)
{
    ifstream file(filename);
    string line;
    while(getline(file, line))
    {
        int pos = line.find(",");
        string xStr = line.substr(0, pos);
        line = line.substr(pos+1);
        pos = line.find(",");
        string yStr = line.substr(0, pos);
        line = line.substr(pos+1);
        string zStr = line;
        x.push_back(stof(xStr));
        y.push_back(stof(yStr));
        z.push_back(stof(zStr));
        numPoints++;
    }
}
//Camera starts at 0,0,0 and looks at 0,0,-1 but can be moved with wasd
//points are rendered at their x,y,z coordinates as white dots on a black background
//the camera can be rotated with the mouse and zoomed in and out with the mouse wheel
//after the camera is moved the scene can be reset with the r key
//the scene can be exited with the escape key

//function to draw the points
void drawPoints()
{
    glPointSize(1);
    glBegin(GL_POINTS);
    for(int i = 0; i < numPoints; i++)
    {
        glVertex3f(x[i], y[i], z[i]);
    }
    glEnd();
}

//function to handle keypresses
void handleKeypress(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);
        case 'w':
            glTranslatef(0, 0, 0.1);
            break;
        case 's':
            glTranslatef(0, 0, -0.1);
            break;
        case 'a':
            glTranslatef(-0.1, 0, 0);
            break;
        case 'd':
            glTranslatef(0.1, 0, 0);
            break;
        case 'r':
            glLoadIdentity();
            gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
            break;
    }
    drawPoints();
    glutSwapBuffers();
    //glutPostRedisplay();
}



//function to draw the axes
void drawAxes()
{
    glLineWidth(2);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();
}

//function to draw the scene
void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
    //drawAxes();
    drawPoints();
    glutSwapBuffers();
}

//function to handle mouse movement
void handleMouse(int x, int y)
{
    static int lastX = -1;
    static int lastY = -1;
    if(lastX != -1)
    {
        int deltaX = x - lastX;
        int deltaY = y - lastY;
        glRotatef(deltaX, 0, 1, 0);
        glRotatef(deltaY, 1, 0, 0);
    }
    lastX = x;
    lastY = y;
    drawPoints();
    glutSwapBuffers();
    //glutPostRedisplay();
}

//function to handle mouse wheel
void handleMouseWheel(int button, int dir, int x, int y)
{
    if(dir > 0)
    {
        glTranslatef(0, 0, 0.1);
    }
    else
    {
        glTranslatef(0, 0, -0.1);
    }
    drawPoints();
    glutSwapBuffers();
    //glutPostRedisplay();
}

//function to initialize the scene
void initRendering()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 1);
}

//function to handle window resizing
void handleResize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (double)w / (double)h, 1, 200);
}

//redraw the scene with new camera position


//main function
int main(int argc, char** argv)
{
    readCSV("test2.csv");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Render");
    initRendering();
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(handleMouseWheel);
    glutMotionFunc(handleMouse);
    glutReshapeFunc(handleResize);
    glutMainLoop();
    return 0;
}
