#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <algorithm>

float baseRotate = 0.0f, armRotate = 0.0f, forearmRotate = 0.0f, clawRotate = 0.0f;

float armSizeHeight = 1.0f;

float armBaseRadiusSize = 0.165f;
float baseBaseRadiusSize = 0.23f;

float forearmBaseRadiusSize = 0.12f;
float forearmSizeHeight = 1.2f;

// Physics parameters
float posObjX = 0.8f;
float posObjY = 0.9f;
float posObjZ =  0.8f;       // Initial position (Y-coordinate)
float vel = 0.0f;         // Initial velocity
const float acc = -9.8 *50.0f;  // Gravity (m/s²)
const float timeStep = 0.016f; // Fixed time step (for ~60 FPS)

// Timing
clock_t prevTime = clock();

void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    GLfloat lightPos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void drawObject()
{
    // Draw the object at the current position
    glPushMatrix();
    glTranslatef(posObjX, posObjY, posObjZ);  // Move to the current position
    glColor3f(1.0f, 0.0f, 0.0f);   // Red color
    glutSolidSphere(0.05, 32, 32); // Draw a sphere
    glPopMatrix();
}

void drawAxes()
{
    glLineWidth(2.0); // Aumenta a espessura das linhas
    glBegin(GL_LINES);

    // Eixo X (Vermelho)
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(-2.0, 0.0, 0.0);
    glVertex3f(2.0, 0.0, 0.0);

    // Eixo Y (Verde)
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, -2.0, 0.0);
    glVertex3f(0.0, 2.0, 0.0);

    // Eixo Z (Azul)
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, -2.0);
    glVertex3f(0.0, 0.0, 2.0);

    glEnd();
}

void drawCylinder(float baseRadius, float topRadius, float height, float angle, float x, float y, float z)
{
    GLUquadric *quad = gluNewQuadric();
    glPushMatrix();
    glRotatef(angle, x, y, z); // Rotação no ângulo e eixo especificados
    gluCylinder(quad, baseRadius, topRadius, height, 20, 20);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

void drawBase()
{
    glColor3f(0.5f, 0.35f, 0.05f);
    // Desenhar a esfera
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f);
    glutSolidSphere(baseBaseRadiusSize, baseBaseRadiusSize *100, baseBaseRadiusSize*100); // Raio = 0.2, subdivisões longitude/latitude = 20
    glPopMatrix();
}

void drawArmArticulation()
{
    glColor3f(0.5f, 0.35f, 0.05f);
    // Desenhar a esfera
    glPushMatrix();
    glTranslatef(0, armSizeHeight, 0.0f);
    glutSolidSphere(armBaseRadiusSize, armBaseRadiusSize *100, armBaseRadiusSize *100); // Raio = 0.2, subdivisões longitude/latitude = 20
    glPopMatrix();
}

void drawHandForearmArticulation()
{
    glColor3f(0.5f, 0.35f, 0.05f);
    // Desenhar a esfera
    glPushMatrix();
    glTranslatef(0, forearmSizeHeight, 0.0f);
    glutSolidSphere(forearmBaseRadiusSize, forearmBaseRadiusSize *100, forearmBaseRadiusSize *100); // Raio = 0.2, subdivisões longitude/latitude = 20
    glPopMatrix();
}


void drawArm()
{
    glColor3f(0.8f, 0.5f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f);
    drawCylinder(armBaseRadiusSize, baseBaseRadiusSize, armSizeHeight,90.f,0.1f,0.0f,0.0f);
    glPopMatrix();
}

void drawForearm()
{
    glColor3f(0.8f, 0.5f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, forearmSizeHeight, 0.0f);
    drawCylinder(forearmBaseRadiusSize, armBaseRadiusSize, forearmSizeHeight,90.f,0.1f,0.0f,0.0f);
    glPopMatrix();
}

void drawClaw()
{
    glColor3f(0.4f, 0.3f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f);
    glScalef(0.14f, 0.5f, 0.3f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void update(int value) {
    // Calculate time since the last frame
    clock_t currentTime = clock();
    float dt = (float)(currentTime - prevTime) / CLOCKS_PER_SEC;
    prevTime = currentTime;

    // Cap dt for consistency in low frame rates
    dt = std::min(dt, timeStep);

    // Update velocity and position
    vel += acc * dt;
    posObjY += vel * dt;

    // Clamp position to ground level
    if (posObjY < 0.0f) { // Assume -0.9 is the ground level
        posObjY = 0.0f;
        vel = 0.0f; // Stop velocity on hitting ground
    }

    // Redisplay
    glutPostRedisplay();
    
    // Continue updating
    glutTimerFunc(16, update, 0); // 16 ms for ~60 FPS
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(2.0, 2.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    drawObject();

    drawAxes();
    // Base rotation
    glPushMatrix();
    glRotatef(baseRotate, 0.0, 1.0, 0.0);
    drawBase();

    // Arm rotation
    glTranslatef(0.0, 0.1, 0.0);
    glRotatef(armRotate, 0.0, 0.0, 1.0);
    drawArm();

    drawArmArticulation();

    // Forearm rotation
    glTranslatef(0.0, 1.0, 0.0);
    glRotatef(forearmRotate, 0.0, 0.0, 1.0);
    drawForearm();

    drawHandForearmArticulation();

    // Claw rotation
    glTranslatef(0.0, forearmSizeHeight+0.1, 0.0);
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
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}