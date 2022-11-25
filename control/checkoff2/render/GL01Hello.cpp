

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
    gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
    //drawAxes();
    drawPoints();
    glutSwapBuffers();
}

//function to handle keyboard input
void handleKeypress(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);
    }
}

//function to handle window resizing
void handleResize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (double)w / (double)h, 1, 200);
}

//allow for mouse input to rotate the scene
void mouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        glRotatef(1, 0, 1, 0);
    }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        glRotatef(1, 1, 0, 0);
    }
    glutPostRedisplay();
}

//allows for mouse input to zoom in and out
void mouseWheel(int button, int dir, int x, int y)
{
    if(dir > 0)
    {
        glTranslatef(0, 0, 0.1);
    }
    else
    {
        glTranslatef(0, 0, -0.1);
    }
    glutPostRedisplay();
}


//main function
int main(int argc, char** argv)
{
    //read in the csv file
    readCSV("test.csv");

    //initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);

    //create the window
    glutCreateWindow("3D Point Cloud");
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouse);
    //glutMouseWheelFunc(mouseWheel);

    //initialize rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //start the main loop
    glutMainLoop();
    return 0;
}

