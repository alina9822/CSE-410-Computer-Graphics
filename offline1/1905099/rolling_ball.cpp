// g++ rolling_ball.cpp -o show -lglut -lGLU -lGL
//./show
#include <bits/stdc++.h>
using namespace std;

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265
#endif

#include <iostream>
#include <iomanip>
#include <cmath>

int counter = 0;

GLfloat eyex = 4, eyey = 4, eyez = 4;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 1, upy = 0, upz = 0;
int k = 0;

const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;

int animate;

struct point
{
    GLfloat x, y, z;
};

void draw_BlackArea()
{
    glBegin(GL_QUADS);
    glColor3f(0, 0, 0);
    glVertex3f(0.0f, -0.3, 0.0f);
    glVertex3f(1.50f, -0.3, 0.0f);
    glVertex3f(1.5f, -0.3, -1.5f);
    glVertex3f(0.0f, -0.3, -1.5f);
    glEnd();
}
void draw_whiteArea()
{
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, -0.3, 0.0f);
    glVertex3f(1.50f, -0.3, 0.0f);
    glVertex3f(1.5f, -0.3, -1.5f);
    glVertex3f(0.0f, -0.3, -1.5f);
    glEnd();
}

GLuint _displayListId_blackArea;
GLuint _displayListId_whiteArea;

void initRendering()
{
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    _displayListId_blackArea = glGenLists(1);
    glNewList(_displayListId_blackArea, GL_COMPILE);
    draw_BlackArea();
    glEndList();

    _displayListId_whiteArea = glGenLists(2);
    glNewList(_displayListId_whiteArea, GL_COMPILE);
    draw_whiteArea();
    glEndList();
}

void drawScene()
{
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glRotatef(i * 90, 0, 1, 0);
        glBegin(GL_QUADS);
        glColor3f(1, 0, 0);
        glVertex3f(4.5f, 0.3f, -4.5f);
        glVertex3f(-4.5f, 0.3f, -4.5f);
        glVertex3f(-4.5f, -0.3f, -4.5f);
        glVertex3f(4.5f, -0.3f, -4.5f);
        glEnd();
        glPopMatrix();
    }

    glTranslatef(-45 * 1.5, 0.0, 20 * 1.5);

    for (float j = 0.0; j > (-90 * 1.5); j -= 1.5)
    {
        k++;
        for (float i = 0.0; i < (45 * 3.0); i += 3.0)
        {
            if (k % 2 == 0)
            {
                glPushMatrix();
                glTranslatef(i, 0.0, j);
                glCallList(_displayListId_blackArea);
                glPopMatrix();
            }
            else
            {
                glPushMatrix();
                glTranslatef(i + 1.5, 0.0, j);
                glCallList(_displayListId_blackArea);
                glPopMatrix();
            }
        }
    }
    for (float j = 0.0; j > (-90 * 1.5); j -= 1.5)
    {
        k++;
        for (float i = 0.0; i < (45 * 3.0); i += 3.0)
        {
            if (k % 2 != 0)
            {
                glPushMatrix();
                glTranslatef(i, 0.0, j);
                glCallList(_displayListId_whiteArea);
                glPopMatrix();
            }
            else
            {
                glPushMatrix();
                glTranslatef(i + 1.5, 0.0, j);
                glCallList(_displayListId_whiteArea);
                glPopMatrix();
            }
        }
    }
}

point pos, l, r, u;
float rotationAng = 45.0;

void asxes()
{
    // opent gl funtion
    glColor3f(0, 1.0, 1.0);
    glLineWidth(2);
    glBegin(GL_LINES);
    {
        glVertex3f(100, 0, 0);
        glVertex3f(-100, 0, 0);

        // glVertex3f(0, 100, 0);
        // glVertex3f(0, -100, 0);

        glVertex3f(0, 0, 100);
        glVertex3f(0, 0, -100);
    }
    glEnd();
    glLineWidth(1);
}

struct point points[8 + 1][8 + 1]; /// I go all the points in this
float b_angle = 45;
float b_x1, b_z1, b_x2, b_z2;
float g_x1, g_z1, g_x2, g_z2;

float normalize(float x, float y, float z)
{
    return 1 / sqrt(x * x + y * y + z * z);
}
point cross(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return {(z1 * y2) - (z2 * y1),
            (x1 * z2) - (x2 * z1),
            (y1 * x2) - (y2 * x1)};
}

float dot(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return (x1 * x2 + y1 * y2 + z1 * z2);
}
float a_x, a_z;
float rotaionAng = 0;
point rotatePoint(float x, float y, float z, float radius)
{
    float f_x = b_x2 - b_x1;
    float f_z = b_z2 - b_z1;

    float n = (f_x, 0, f_z);

    a_x = -f_z * n;
    a_z = f_x * n;

    float v_x = x - b_x1;
    float v_y = y - 0;
    float v_z = z - b_z1;

    float theta = 0.1 / radius;
    rotaionAng += theta;

    point c_p = cross(a_x, 0, a_z, v_x, v_y, v_z);
    float d = dot(a_x, 0, a_z, v_x, v_y, v_z);

    float d_x = d * a_x;
    float d_z = d * a_z;

    point p;

    p.x = v_x * cos(theta) + (1 - cos(theta)) * d_x + sin(theta) * c_p.x;
    p.y = v_y * cos(theta) + (1 - cos(theta)) * 0 + sin(theta) * c_p.y;
    p.z = v_z * cos(theta) + (1 - cos(theta)) * d_z + sin(theta) * c_p.z;

    p.x = p.x + b_x1;
    p.z = p.z + b_z1;

    return p;
}

void changePoints(double radius, int stacks, int slices, bool forward)
{
    float m_x, m_z;
    m_x = 0.15 * cos(b_angle * M_PI / 180.0);
    m_z = 0.15 * sin(b_angle * M_PI / 180.0);

    point a = cross(g_x2 - g_x1, 2.0, g_z2 - g_z1, b_x2 - b_x1, 0, b_z2 - b_z1);
    float n = normalize(a.x, a.y, a.z);
    a.x *= n;
    a.z *= n;
    a.y *= n;

    // float f_x = b_x2 - b_x1;
    // float f_z = b_z2 - b_z1;

    // float n = (f_x, 0, f_z);

    // a_x = -f_z * n;
    // a_z = f_x * n;

    a_x = a.x;
    a_z = a.z;

    float theta = 8 / radius;

    if (forward)
    {

        b_x1 += m_x;
        b_z1 += m_z;
        b_x2 += m_x;
        b_z2 += m_z;
        g_x1 += m_x;
        g_z1 += m_z;
        g_x2 += m_x;
        g_z2 += m_z;
        rotaionAng -= theta;
        // for (int j = 0; j <= stacks; j++)
        // {
        //     for (int i = 0; i < slices + 1; i++)
        //     {
        //         points[j][i].x += m_x;
        //         // points[j][i].y += 0;
        //         points[j][i].z += m_z;
        //     }
        // }
        // for (int j = 0; j <= stacks; j++)
        // {
        //     for (int i = 0; i < slices + 1; i++)
        //     {
        //         point p = rotatePoint(points[j][i].x, points[j][i].y, points[j][i].z, radius);
        //         points[j][i].x = p.x;
        //         points[j][i].y = p.y;
        //         points[j][i].z = p.z;
        //     }
        // }
    }
    else
    {

        b_x1 -= m_x;
        b_z1 -= m_z;
        b_x2 -= m_x;
        b_z2 -= m_z;
        g_x1 -= m_x;
        g_z1 -= m_z;
        g_x2 -= m_x;
        g_z2 -= m_z;
        rotaionAng += theta;
        // for (int j = 0; j <= stacks; j++)
        // {
        //     for (int i = 0; i < slices + 1; i++)
        //     {
        //         points[j][i].x -= m_x;
        //         // points[j][i].y += 0;
        //         points[j][i].z -= m_z;
        //     }
        // }
        // for (int j = 0; j <= stacks; j++)
        // {
        //     for (int i = 0; i < slices + 1; i++)
        //     {
        //         point p = rotatePoint(points[j][i].x, points[j][i].y, points[j][i].z, radius);
        //         points[j][i].x = p.x;
        //         points[j][i].y = p.y;
        //         points[j][i].z = p.z;
        //     }
        // }
    }
    if (b_z1 - radius <= -4.5)
    {
        float rz = (b_z2 - b_z1) * -1;
        b_z2 = rz + b_z1;
        b_angle = -1 * b_angle;
    }
    else if (b_x1 - radius <= -4.5)
    {
        float rx = (b_x2 - b_x1) * -1;
        b_x2 = rx + b_x1;
        b_angle = 180 - b_angle;
    }
    else if (b_x1 + radius >= 4.5)
    {
        float rx = (b_x2 - b_x1) * -1;
        b_x2 = rx + b_x1;
        b_angle = 180 - b_angle;
    }
    else if (b_z1 + radius >= 4.5)
    {
        float rz = (b_z2 - b_z1) * -1;
        b_z2 = rz + b_z1;
        b_angle = -1 * b_angle;
    }
}
void genPoints(double radius, int stacks, int slices)
{
    b_x1 = b_z1 = g_x1 = g_z1 = 0;
    b_x2 = cos(b_angle * M_PI / 180.0);
    b_z2 = sin(b_angle * M_PI / 180.0);
    g_x2 = 0;
    g_z2 = 0;

    for (int j = 0; j <= stacks; j++)
    {
        double phi = -M_PI / 2.0 + j * M_PI / stacks;
        double r = radius * cos(phi);
        double h = radius * sin(phi);
        for (int i = 0; i < slices + 1; i++)
        {
            double theta = i * 2.0 * M_PI / slices;
            points[j][i].x = r * cos(theta);
            points[j][i].y = h;
            points[j][i].z = r * sin(theta);
        }
    }
}
void drawSphere(double radius, int stacks, int slices)
{

    float r = 1;
    float g = 0;
    float b = 0;

    glBegin(GL_QUADS);
    for (int j = 0; j < stacks; j++)
    {
        if (j == (floor(stacks) / 2.0))
        {
            // cout << "whats wrong" << endl;
            r = 1 - r;
            g = 1 - g;
        }
        for (int i = 0; i < slices; i++)
        {
            r = 1 - r;
            g = 1 - g;

            GLfloat c = (2 + cos((i + j) * 2.0 * M_PI / slices)) / 3;
            glColor3f(r, g, b);
            glVertex3f(points[j][i].x, points[j][i].y, points[j][i].z);
            glVertex3f(points[j][i + 1].x, points[j][i + 1].y, points[j][i + 1].z);

            glVertex3f(points[j + 1][i + 1].x, points[j + 1][i + 1].y, points[j + 1][i + 1].z);
            glVertex3f(points[j + 1][i].x, points[j + 1][i].y, points[j + 1][i].z);
        }
    }
    glEnd();
}

void drawArrows()
{
    // b_x2 = cos(b_angle * M_PI / 180.0);
    // b_z2 = sin(b_angle * M_PI / 180.0);
    // cout << a << endl;
    glLineWidth(3);
    glBegin(GL_LINES);
    {
        glColor3f(0, 0, 1);

        glVertex3f(b_x1, 0, b_z1);
        glVertex3f(b_x2, 0, b_z2);

        // float b_ax, b_az;
        // if (b_x2 > b_x1)
        //     b_ax = b_x2 - 0.3;
        // else
        //     b_ax = b_x2 + 0.3;

        // if (b_z2 > b_z1)
        //     b_az = b_z2 - 0.3;
        // else
        //     b_az = b_z2 + 0.3;
        // glPushMatrix();
        // glRotatef(170,b_x2-b_x1,0,b_z2-b_z1);
        // glVertex3f(b_ax, 0, b_az);
        // glVertex3f(b_x2, 0, b_z2);
        // glPushMatrix();

        glColor3f(0, 1, 0.8);
        glVertex3f(g_x1, 0, g_z1);
        glVertex3f(g_x2, 2, g_z2);
    }
    glEnd();
}
void display()
{
    glEnable(GL_DEPTH_TEST);
    // printf("wer are in display function counter : %d\n", ++counter);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Reset the color of all color to pixel
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // gluLookAt(10, 10, 10, 0, 0, 0, 0, 1, 0);
     gluLookAt(pos.x, pos.y, pos.z,
              pos.x + l.x, pos.y + l.y, pos.z + l.z,
              u.x, u.y, u.z);
                 glRotatef(rotationAng, 0, 1, 0);

    asxes();
    // drawMaze();
    // draw_BlackArea();
    // draw_whiteArea();
    glPushMatrix();
    // glTranslatef()
    glTranslatef(b_x1, 0, b_z1);
    glRotatef(rotaionAng, a_x, 0, a_z);
    drawSphere(0.3, 8, 8);

    glPopMatrix();
    drawArrows();
    initRendering();
    drawScene();

    // glFlush(); // This flush really important
    // buffer is ready sent to the monitor
    glutSwapBuffers();
}

void init()
{
    animate = 0;
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION); // matrix mode ta hobe Gl projection mode
    glLoadIdentity();            //ager j matrix ta oita fele deoa shudhu matro identity matrix ta\
    load kora \
    so projection matrix ta ekhon shudhumatro identity matrix
    gluPerspective(80, 1, 1, 100);
}

void idle()
{
    printf("currently have no job");
    glutPostRedisplay(); // nijera jodi repaint request korte chai
    /// idle thakle bar bar repiant request ta call korte thakbe
}

void Timer(int value)
{
    // printf("inside timer %d\n", ++counter);
    if (animate)
    {
        changePoints(0.3, 8, 8, true);
    }
    glutPostRedisplay();
    glutTimerFunc(50, Timer, 0);
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y)
{
    float v = 0.1;
    double rate = 0.1;
    switch (key)
    {
    // Control eye (location of the eye)
    // control eyex
    case ' ':
        animate = 1 - animate;
        break;
    case '1':
        r.x = r.x * cos(rate) + l.x * sin(rate);
        r.y = r.y * cos(rate) + l.y * sin(rate);
        r.z = r.z * cos(rate) + l.z * sin(rate);

        l.x = l.x * cos(rate) - r.x * sin(rate);
        l.y = l.y * cos(rate) - r.y * sin(rate);
        l.z = l.z * cos(rate) - r.z * sin(rate);
        break;
    case '2':
        r.x = r.x * cos(-rate) + l.x * sin(-rate);
        r.y = r.y * cos(-rate) + l.y * sin(-rate);
        r.z = r.z * cos(-rate) + l.z * sin(-rate);

        l.x = l.x * cos(-rate) - r.x * sin(-rate);
        l.y = l.y * cos(-rate) - r.y * sin(-rate);
        l.z = l.z * cos(-rate) - r.z * sin(-rate);
        break;
    case '3':
        l.x = l.x * cos(rate) + u.x * sin(rate);
        l.y = l.y * cos(rate) + u.y * sin(rate);
        l.z = l.z * cos(rate) + u.z * sin(rate);

        u.x = u.x * cos(rate) - l.x * sin(rate);
        u.y = u.y * cos(rate) - l.y * sin(rate);
        u.z = u.z * cos(rate) - l.z * sin(rate);
        break;
    case '4':
        l.x = l.x * cos(-rate) + u.x * sin(-rate);
        l.y = l.y * cos(-rate) + u.y * sin(-rate);
        l.z = l.z * cos(-rate) + u.z * sin(-rate);

        u.x = u.x * cos(-rate) - l.x * sin(-rate);
        u.y = u.y * cos(-rate) - l.y * sin(-rate);
        u.z = u.z * cos(-rate) - l.z * sin(-rate);
        break;
    case '5':
        u.x = u.x * cos(rate) + r.x * sin(rate);
        u.y = u.y * cos(rate) + r.y * sin(rate);
        u.z = u.z * cos(rate) + r.z * sin(rate);

        r.x = r.x * cos(rate) - u.x * sin(rate);
        r.y = r.y * cos(rate) - u.y * sin(rate);
        r.z = r.z * cos(rate) - u.z * sin(rate);
        break;
    case '6':
        u.x = u.x * cos(-rate) + r.x * sin(-rate);
        u.y = u.y * cos(-rate) + r.y * sin(-rate);
        u.z = u.z * cos(-rate) + r.z * sin(-rate);

        r.x = r.x * cos(-rate) - u.x * sin(-rate);
        r.y = r.y * cos(-rate) - u.y * sin(-rate);
        r.z = r.z * cos(-rate) - u.z * sin(-rate);
        break;

    case 'd':
        rotationAng += 5.0;
        break;
    case 'a':
        rotationAng -= 5.0;
        break;
    case 'w':
        pos.z += v;
        break;
    case 's':
        pos.z -= v;
        break;
    case 'l':
    {
        b_angle += 10;
        float ang = (10.0 * M_PI / 180.0);

        float n = normalize(g_x2 - g_x1, 2, g_z2 - g_z1);
        point p = cross((g_x2 - g_x1) * n, 2.0 * n, (g_z2 - g_z1) * n, b_x2 - b_x1, 0, b_z2 - b_z1);
        // cout << p.x << " " << p.z;

        float rx = (b_x2 - b_x1) * cos(ang) + p.x * sin(ang);
        float rz = (b_z2 - b_z1) * cos(ang) + p.z * sin(ang);
        b_x2 = rx + b_x1;
        b_z2 = rz + b_z1;

        // float rx = (b_x2 - b_x1) * cos(ang) - (b_z2 - b_z1) * sin(ang);
        // float rz = (b_z2 - b_z1) * cos(ang) + (b_x2 - b_x1) * sin(ang);
        // b_x2 = rx + b_x1;
        // b_z2 = rz + b_z1;

        break;
    }
    case 'j':
    {
        b_angle -= 10;
        float ang = -(10.0 * M_PI / 180.0);

        float n = normalize(g_x2 - g_x1, 2, g_z2 - g_z1);
        point p = cross((g_x2 - g_x1) * n, 2.0 * n, (g_z2 - g_z1) * n, b_x2 - b_x1, 0, b_z2 - b_z1);

        // cout << p.x << " " << p.z;

        float rx = (b_x2 - b_x1) * cos(ang) + p.x * sin(ang);
        float rz = (b_z2 - b_z1) * cos(ang) + p.z * sin(ang);
        b_x2 = rx + b_x1;
        b_z2 = rz + b_z1;

        // float rx = (b_x2 - b_x1) * cos(ang) - (b_z2 - b_z1) * sin(ang);
        // float rz = (b_z2 - b_z1) * cos(ang) + (b_x2 - b_x1) * sin(ang);
        // b_x2 = rx + b_x1;
        // b_z2 = rz + b_z1;

        break;
    }
    case 'i':
    {
        changePoints(0.3, 8, 8, true);
        break;
    }
    case 'k':
    {
        changePoints(0.3, 8, 8, false);
        break;
    }

    case 27:     // ESC key
        exit(0); // Exit window
        break;
    }
    glutPostRedisplay(); // Post a paint request to activate display()
}

void specialKeyListener(int key, int x, int y)
{
    double v = 0.25;

    double s;
    switch (key)
    {
    case GLUT_KEY_LEFT:
        pos.x -= r.x;
        pos.y -= r.y;
        pos.z -= r.z;
        break;
    case GLUT_KEY_RIGHT:
        pos.x += r.x;
        pos.y += r.y;
        pos.z += r.z;
        break;
    case GLUT_KEY_UP:
        pos.x += l.x;
        pos.y += l.y;
        pos.z += l.z;
        break;
    case GLUT_KEY_DOWN:
        pos.x -= l.x;
        pos.y -= l.y;
        pos.z -= l.z;
        break;
    case GLUT_KEY_PAGE_UP:
        pos.x += u.x;
        pos.y += u.y;
        pos.z += u.z;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x -= u.x;
        pos.y -= u.y;
        pos.z -= u.z;
        break;

    default:
        return;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    pos.x = 6;
    pos.y = 1;
    pos.z = 2;

    l.x = -1;
    l.y = 0;
    l.z = 0;
    u.x = 0;
    u.y = 1;
    u.z = 0;
    r.x = 0;
    r.y = 1;
    r.z = 0;
    printf("Hello world");
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 00);

    glutInitWindowSize(650, 650);
    glutCreateWindow("OpoenGL demo");

    genPoints(0.3, 8, 8);

    init();

    glutDisplayFunc(display); // called on repaint request
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    // glutIdleFunc(idle);
    glutTimerFunc(5, Timer, 0);

    glutMainLoop();

    return 0;
}