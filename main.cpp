#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <vector>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "./include/stb_image.h"


GLuint idsTextura[3];

float baseRotate = 0.0f, armRotate = 0.0f, forearmRotate = 0.0f, clawRotate = 0.0f;

float armSizeHeight = 1.0f;
float clawAngle = 45.0f;
float armBaseRadiusSize = 0.165f;
float baseBaseRadiusSize = 0.23f;
bool objIsGrabbed = false;
float forearmBaseRadiusSize = 0.12f;
float forearmSizeHeight = 1.2f;
float resolution = 35.0f;
float baseHandX = 0.0f;
float baseHandY = 0.1f;
float baseHandZ = 0.0f;
float sphereR = 1.0f;
float sphereG = 0.0f;
float sphereB = 0.0f;

GLfloat mat_ambient[] = {0.8f, 0.5f, 0.2f, 1.0f};
GLfloat mat_diffuse[] = {0.8f, 0.5f, 0.2f, 1.0f};
GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat mat_shininess[] = {50.0f};


float indicadorX = 0.05f, indicadorY = 0.35f, indicadorZ = 0.1f;
float medioX = 0.0f, medioY = 0.35f, medioZ = 0.0f;
float anelarX = -0.05f, anelarY = 0.35f, anelarZ = -0.1f;
float mindinhoX = -0.1f, mindinhoY = 0.35f, mindinhoZ = -0.15f;
float polegarX = 0.0f, polegarY = 0.2f, polegarZ = 0.2f;

// Physics parameters
float posObjX = 0.8f;
float posObjY = 3.0;
float posObjZ =  0.8f;       // Initial position (Y-coordinate)
float objRadius = 0.3f;
float vel = 0.0f;         // Initial velocity
double acc = -9.8 *40.0;  // Gravity (m/s²)
const float timeStep = 0.016f; // Fixed time step (for ~60 FPS)

// Estrutura para representar um ponto 3D
struct Point3D {
    float x, y, z;
};

// Variável global para armazenar os pontos da esfera
std::vector<Point3D> spherePoints;

// Timing
clock_t prevTime = clock();

// Global variables for camera control
float cameraPosX = 2.0f, cameraPosY = 2.0f, cameraPosZ = 5.0f;
float cameraTargetX = 0.0f, cameraTargetY = 0.0f, cameraTargetZ = 0.0f;
float cameraSpeed = 0.5f; // Speed of camera movement

void carregarTextura(const char* nomeArquivo, int indice)
{
 int largura, altura, canais;

 unsigned char *dados = stbi_load(nomeArquivo, &largura, &altura, &canais, 0);

 if (!dados)
  exit(1);

 // gerar textura
 glGenTextures(1, &idsTextura[indice]);
 glBindTexture(GL_TEXTURE_2D, idsTextura[indice]);

 // configurar parametros da textura
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

 glTexImage2D(GL_TEXTURE_2D, 0, (canais == 4) ? GL_RGBA : GL_RGB, 
               largura, altura, 0, (canais == 4) ? GL_RGBA : GL_RGB,
               GL_UNSIGNED_BYTE, dados);

 // liberar a memoria da imagem
 stbi_image_free(dados);              
}


void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    carregarTextura("textura.jpg", 0);
    carregarTextura("metal-texture.png", 1);
    carregarTextura("circuits.png", 2);
    GLfloat lightPos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}


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
    case 5:
        posObjY = 3.0f;
        break;
    case 6:
        acc = -9.8*40;
        posObjY = 3.0f;
        break;
    case 7:
        acc = -5*40;
        posObjY = 3.0f;
        break;
    case 8:
        acc = -20*40;
        posObjY = 3.0f;
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
    

    int gravityMenu = glutCreateMenu(menu);
    glutAddMenuEntry("9.8", 6);
    glutAddMenuEntry("5", 7);
    glutAddMenuEntry("20", 8);

    glutCreateMenu(menu);
    glutAddMenuEntry("Reset Articulations", 1);
    glutAddMenuEntry("Reset Sphere", 5);
    glutAddSubMenu("Gravity", gravityMenu);
    glutAddSubMenu("Light Settings", lightMenu);
    glutAddMenuEntry("Exit", 0);

    glutAttachMenu(GLUT_RIGHT_BUTTON); // Activate menu on right-click
}

void drawObject()
{
    // Ativa o uso de texturas
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idsTextura[1]); // Associa a textura

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Use MODULATE para combinar com a iluminação

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para não interferir na textura

    // Cria um objeto quadrático para renderizar a esfera
    GLUquadric *quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);  // Ativa a textura
    gluQuadricNormals(quad, GLU_SMOOTH); // Normais suaves para iluminação

    glPushMatrix();
    glTranslatef(posObjX, posObjY, posObjZ); // Move para a posição desejada
    gluSphere(quad, objRadius, 50, 50);      // Desenha a esfera com textura
    glPopMatrix();

    gluDeleteQuadric(quad); // Libera memória do objeto quadrático

    // Desativa o uso de texturas para outros objetos, se necessário
    glDisable(GL_TEXTURE_2D);
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
    GLUquadric *quad = gluNewQuadric();               // Cria o objeto quadrático
    gluQuadricTexture(quad, GL_TRUE);                 // Habilita a textura no quadrático
    gluQuadricNormals(quad, GLU_SMOOTH);              // Normais suaves para iluminação

    glPushMatrix();
    glRotatef(angle, x, y, z);                        // Rotação no ângulo e eixo especificados
    gluCylinder(quad, baseRadius, topRadius, height, 20, 20); // Desenha o cilindro
    glPopMatrix();

    gluDeleteQuadric(quad);                           // Deleta o objeto quadrático
}

void drawBase()
{
    // Ativa o uso de texturas
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idsTextura[2]); // Associa a textura

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Combina textura com iluminação

    // Configuração de materiais
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para não interferir na textura

    // Criação do objeto quadrático para a esfera
    GLUquadric *quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);   // Ativa a textura para a esfera
    gluQuadricNormals(quad, GLU_SMOOTH); // Normais suaves para iluminação

    // Desenho da esfera com textura
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f);     // Move para a posição desejada
    gluSphere(quad, baseBaseRadiusSize, 50, 50); // Esfera com raio e subdivisões
    glPopMatrix();

    gluDeleteQuadric(quad); // Libera memória do objeto quadrático

    // Desativa o uso de texturas para outros objetos
    glDisable(GL_TEXTURE_2D);
}


void drawArmArticulation()
{
        // Ativa o uso de texturas
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idsTextura[2]); // Associa a textura

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Combina textura com iluminação

    // Configuração de materiais
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para não interferir na textura

    // Criação do objeto quadrático para a esfera
    GLUquadric *quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);   // Ativa a textura para a esfera
    gluQuadricNormals(quad, GLU_SMOOTH); // Normais suaves para iluminação

    // Desenho da esfera com textura
    glPushMatrix();
    glTranslatef(0, armSizeHeight, 0.0f);
    gluSphere(quad, armBaseRadiusSize, 50, 50); // Esfera com raio e subdivisões
    glPopMatrix();

    gluDeleteQuadric(quad); // Libera memória do objeto quadrático

    // Desativa o uso de texturas para outros objetos
    glDisable(GL_TEXTURE_2D);
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
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idsTextura[0]); // Associa a textura

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Use MODULATE para combinar com a iluminação


    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glColor3f(1.0f, 1.0f, 1.0f); // Defina a cor como branca para não interferir na textura

    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f);
    drawCylinder(armBaseRadiusSize, baseBaseRadiusSize, armSizeHeight, 90.f, 0.1f, 0.0f, 0.0f);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}


void drawForearm()
{

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idsTextura[0]); // Associa a textura

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Use MODULATE para combinar com a iluminação


    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glColor3f(1.0f, 1.0f, 1.0f); // Defina a cor como branca para não interferir na textura

    glPushMatrix();
    glTranslatef(0.0f, forearmSizeHeight, 0.0f);
    drawCylinder(forearmBaseRadiusSize, armBaseRadiusSize, forearmSizeHeight,90.f,0.1f,0.0f,0.0f);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void drawTexturedCube()
{
    glBegin(GL_QUADS);

    // Face frontal
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f,  0.5f);

    // Face traseira
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f, -0.5f);

    // Face superior
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,  0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f,  0.5f);

    // Face inferior
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f,  0.5f);

    // Face direita
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f, -0.5f,  0.5f);

    // Face esquerda
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f,  0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f,  0.5f);

    glEnd();
}


void drawClaw(float clawAngle)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idsTextura[2]); // Associa a textura

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Combina textura com iluminação

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para não interferir na textura

    // Base da garra (palma da mão)
    glPushMatrix();
    glTranslatef(baseHandX, baseHandY, baseHandZ); // Posição da base
    glScalef(0.2f, 0.5f, 0.3f); // Escala da base
    drawTexturedCube(); // Desenha o cubo com textura
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // Função para desenhar um dedo
    auto drawFinger = [](float posX, float posY, float posZ, float angle, float length, float width) {
        glPushMatrix();
        glTranslatef(posX, posY, posZ); // Posição inicial do dedo
        glRotatef(angle, 0.0f, 0.0f, 1.0f); // Rotação do dedo
        glTranslatef(length / 2, 0.0f, 0.0f); // Ajuste para rotação
        glScalef(length, width, width); // Escala do dedo
        drawTexturedCube(); // Desenha o dedo como cubo texturizado
        glPopMatrix();
    };

    // Dedos superiores
    drawFinger(indicadorX, indicadorY, indicadorZ, clawAngle, 0.3f, 0.05f); // Indicador
    drawFinger(medioX, medioY, medioZ, clawAngle, 0.3f, 0.05f);           // Médio
    drawFinger(anelarX, anelarY, anelarZ, clawAngle, 0.3f, 0.05f);        // Anelar
    drawFinger(mindinhoX, mindinhoY, mindinhoZ, clawAngle, 0.25f, 0.05f); // Mindinho   
    drawFinger(polegarX, polegarY, polegarZ, -clawAngle, 0.25f, 0.05f);   // Polegar
}


void update(int value) {
    // Calculate time since the last frame
    clock_t currentTime = clock();
    float dt = (float)(currentTime - prevTime) / CLOCKS_PER_SEC;
    prevTime = currentTime;

    // Cap dt for consistency in low frame rates
    dt = std::min(dt, timeStep);


    // Clamp position to ground level
    if (posObjY < 0.0f) {
        posObjY = 0.0f;
        vel = 0.0f; // Stop velocity on hitting ground
    }else{
        vel += acc * dt;
        posObjY += vel * dt;
    }

    // Redisplay
    glutPostRedisplay();
    
    // Continue updating
    glutTimerFunc(16, update, 0); // 16 ms for ~60 FPS
}

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
    drawClaw(clawAngle);
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
    case 't': // Abrir os dedos
        clawAngle += 5.0f;
        if (clawAngle > 45.0f) clawAngle = 45.0f; // Limite máximo de abertura
        break;
    case 'g': // Fechar os dedos
        clawAngle -= 5.0f;
        if (clawAngle < -30.0f) clawAngle = -30.0f; // Limite mínimo de fechamento
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