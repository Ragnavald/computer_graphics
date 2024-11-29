#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <vector>

float baseRotate = 0.0f, armRotate = 0.0f, forearmRotate = 0.0f, clawRotate = 0.0f;

float armSizeHeight = 1.0f;

float armBaseRadiusSize = 0.165f;
float baseBaseRadiusSize = 0.23f;

float forearmBaseRadiusSize = 0.12f;
float forearmSizeHeight = 1.2f;
float resolution = 35.0f;

// Physics parameters
float posObjX = 0.8f;
float posObjY = 0.9f;
float posObjZ =  0.8f;       // Initial position (Y-coordinate)
float objRadius = 0.3f;
float vel = 0.0f;         // Initial velocity
const float acc = -9.8 *50.0f;  // Gravity (m/s²)
const float timeStep = 0.016f; // Fixed time step (for ~60 FPS)

// Timing
clock_t prevTime = clock();

void resetArticulations() {
    baseRotate = 0.0f;
    armRotate = 0.0f;
    forearmRotate = 0.0f;
    clawRotate = 0.0f;
    glutPostRedisplay();
}

void adjustLight(float x, float y, float z) {
    GLfloat lightPos[] = {x, y, z, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glutPostRedisplay();
}

void menu(int option) {
    switch (option) {
    case 1:
        resetArticulations();
        break;
    case 2:
        adjustLight(1.0f, 1.0f, 1.0f); // Default position
        break;
    case 3:
        adjustLight(2.0f, 2.0f, 2.0f); // Brighter position
        break;
    case 4:
        adjustLight(0.5f, 0.5f, 0.5f); // Dim position
        break;
    case 0:
        exit(0); // Exit the program
    }
}

void createMenu() {
    int lightMenu = glutCreateMenu(menu);
    glutAddMenuEntry("Default Light", 2);
    glutAddMenuEntry("Brighter Light", 3);
    glutAddMenuEntry("Dim Light", 4);

    glutCreateMenu(menu);
    glutAddMenuEntry("Reset Articulations", 1);
    glutAddSubMenu("Light Settings", lightMenu);
    glutAddMenuEntry("Exit", 0);

    glutAttachMenu(GLUT_RIGHT_BUTTON); // Activate menu on right-click
}


void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    GLfloat lightPos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

// Estrutura para representar um ponto 3D
struct Point3D {
    float x, y, z;
};

// Variável global para armazenar os pontos da esfera
std::vector<Point3D> spherePoints;

// Função para gerar pontos da superfície da esfera
void generateSpherePoints(float centerX, float centerY, float centerZ, float radius, int resolution) {
    spherePoints.clear(); // Limpa qualquer dado anterior
    for (int i = 0; i <= resolution; ++i) { // Ângulo polar (θ)
        float theta = M_PI * i / resolution; // De 0 a π

        for (int j = 0; j <= resolution; ++j) { // Ângulo azimutal (φ)
            float phi = 2 * M_PI * j / resolution; // De 0 a 2π

            // Calcula as coordenadas do ponto
            float x = centerX + radius * sin(theta) * cos(phi);
            float y = centerY + radius * sin(theta) * sin(phi);
            float z = centerZ + radius * cos(theta);

            // Adiciona o ponto ao vetor global
            spherePoints.push_back({x, y, z});
        }
    }
}

void drawObject()
{
    // Draw the object at the current position
    glPushMatrix();
    glTranslatef(posObjX, posObjY, posObjZ);  // Move to the current position
    glColor3f(1.0f, 0.0f, 0.0f);   // Red color
    glutSolidSphere(objRadius, objRadius*100, objRadius*100); // Draw a sphere
    glPopMatrix();
}

void drawSpherePoints() {
    glPushMatrix(); // Salva a matriz atual

    // Aplica a translação para mover a esfera para a posição correta
    glTranslatef(posObjX-0.89f, posObjY-1.0f, posObjZ-1.0f);

    glBegin(GL_POINTS); // Inicia a renderização de pontos
    glColor3f(0.0f, 1.0f, 0.0f); // Cor verde para os pontos
    for (const auto& point : spherePoints) {
        glVertex3f(point.x, point.y, point.z); // Desenha cada ponto
    }
    glEnd();

    glPopMatrix(); // Restaura a matriz original
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
// Global variables for camera control
float cameraPosX = 2.0f, cameraPosY = 2.0f, cameraPosZ = 5.0f;
float cameraTargetX = 0.0f, cameraTargetY = 0.0f, cameraTargetZ = 0.0f;
float cameraSpeed = 0.5f; // Speed of camera movement

void specialKeys(int key, int x, int y) {
    // Calculate direction vector
    float directionX = cameraTargetX - cameraPosX;
    float directionY = cameraTargetY - cameraPosY;
    float directionZ = cameraTargetZ - cameraPosZ;

    // Normalize direction vector
    float length = sqrt(directionX * directionX + directionY * directionY + directionZ * directionZ);
    directionX /= length;
    directionY /= length;
    directionZ /= length;

    // Handle arrow keys
    switch (key) {
    case GLUT_KEY_UP: // Move forward
        cameraPosX += directionX * cameraSpeed;
        cameraPosY += directionY * cameraSpeed;
        cameraPosZ += directionZ * cameraSpeed;
        break;
    case GLUT_KEY_DOWN: // Move backward
        cameraPosX -= directionX * cameraSpeed;
        cameraPosY -= directionY * cameraSpeed;
        cameraPosZ -= directionZ * cameraSpeed;
        break;
    case GLUT_KEY_LEFT: // Rotate left
        cameraPosX -= directionZ * cameraSpeed; // Perpendicular to the forward direction
        cameraPosZ += directionX * cameraSpeed;
        break;
    case GLUT_KEY_RIGHT: // Rotate right
        cameraPosX += directionZ * cameraSpeed; // Perpendicular to the forward direction
        cameraPosZ -= directionX * cameraSpeed;
        break;
    }
    glutPostRedisplay();
}

void resetCamera() {
    cameraPosX = 2.0f;
    cameraPosY = 2.0f;
    cameraPosZ = 5.0f;
    cameraTargetX = 0.0f;
    cameraTargetY = 0.0f;
    cameraTargetZ = 0.0f;
    glutPostRedisplay(); // Redesenha a cena
}

// Função de callback do mouse
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Reseta a câmera ao clicar com o botão esquerdo do mouse
        resetCamera();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set camera
    gluLookAt(
        cameraPosX, cameraPosY, cameraPosZ, // Camera position
        cameraTargetX, cameraTargetY, cameraTargetZ, // Camera target
        0.0, 1.0, 0.0 // Up vector
    );

    drawObject();
    drawSpherePoints();
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
    glTranslatef(0.0, forearmSizeHeight + 0.1, 0.0);
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

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Robotic Arm with Camera Control");
    init();
    generateSpherePoints(posObjX, posObjY, posObjZ, objRadius, resolution);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys); // Registrar o handler das teclas especiais
    glutMouseFunc(mouse); // Registrar o handler do mouse
    glutTimerFunc(16, update, 0);
    createMenu();
    glutMainLoop();
    return 0;
}