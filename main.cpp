#include <GL/glut.h>

float baseRotate = 0.0f, armRotate = 0.0f, forearmRotate = 0.0f, clawRotate = 0.0f;

void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    GLfloat lightPos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void drawBase()
{
    glColor3f(0.5f, 0.35f, 0.05f);

    // Desenhar o topo do cilindro como um disco
    glPushMatrix();
    glutSolidCone(0.2, 0.2, 20, 1);
    glPopMatrix();

    // Desenhar o corpo do cilindro como um cone com zero de altura
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f);
    glRotatef(180, 1.0, 0.0, 0.0); // Rotacionar o cone para ficar de cabeça para baixo
    glutSolidCone(0.2, 0.1, 20, 20);
    glPopMatrix();
}

void drawArm()
{
    glColor3f(0.8f, 0.5f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glScalef(0.2f, 1.0f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawForearm()
{
    glColor3f(0.6f, 0.4f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glScalef(0.15f, 1.0f, 0.15f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawClaw()
{
    glColor3f(0.4f, 0.3f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.25f, 0.0f);
    glScalef(0.1f, 0.5f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(2.0, 2.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Base rotation
    glPushMatrix();
    glRotatef(baseRotate, 0.0, 1.0, 0.0);
    drawBase();

    // Arm rotation
    glTranslatef(0.0, 0.1, 0.0);
    glRotatef(armRotate, 0.0, 0.0, 1.0);
    drawArm();

    // Forearm rotation
    glTranslatef(0.0, 1.0, 0.0);
    glRotatef(forearmRotate, 0.0, 0.0, 1.0);
    drawForearm();

    // Claw rotation
    glTranslatef(0.0, 1.0, 0.0);
    glRotatef(clawRotate, 0.0, 0.0, 1.0);
    drawClaw();

    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
        baseRotate += 5.0f;
        break;
    case 'a':
        baseRotate -= 5.0f;
        break;
    case 'w':
        armRotate += 5.0f;
        break;
    case 's':
        armRotate -= 5.0f;
        break;
    case 'e':
        forearmRotate += 5.0f;
        break;
    case 'd':
        forearmRotate -= 5.0f;
        break;
    case 'r':
        clawRotate += 5.0f;
        break;
    case 'f':
        clawRotate -= 5.0f;
        break;
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Robotic Arm");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}