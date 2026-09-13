// g++ magic_cube.cpp -o show -lglut -lGLU -lGL
//./show
#include <bits/stdc++.h>
using namespace std;

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif
/*******************************/

#ifndef M_PI
#define M_PI 3.14159265
#endif
float shrink = 0;
float t = .01;
float tri_h = 2;
float cyl_h = sqrt(2) * tri_h;
float cyl_r = 0;

struct point
{
    GLfloat x, y, z;
};

void drawCylinder(int nmax, int nmin, float height, float radius)
{
    double MAX = height / nmax;
    double MIN = 2.0 * M_PI / nmin;
    int i, j;
    float r = 1;
    float g = 0;
    float b = 1;

    for (i = 0; i < nmax; ++i)
    {
        GLfloat z0 = 0.5 * height - i * MAX;
        GLfloat z1 = z0 - MAX;
        // r=1-r;
        // g=1-g;

        glBegin(GL_TRIANGLE_STRIP);
        for (j = 0; j <= nmin; ++j)
        {
            glColor3f(1, 1, 0);
            double a = j * MIN;
            GLfloat x = radius * cos(a);
            GLfloat y = radius * sin(a);
            glNormal3f(x / radius, y / radius, 0.0);
            glTexCoord2f(j / (GLfloat)nmin, i / (GLfloat)nmax);
            glVertex3f(x, y, z0);

            glNormal3f(x / radius, y / radius, 0.0);
            glTexCoord2f(j / (GLfloat)nmin, (i + 1) / (GLfloat)nmax);
            glVertex3f(x, y, z1);
        }

        glEnd();
    }
}

void drawAllCylinders()
{
    cyl_h = sqrt(2) * tri_h;
    if (shrink > 0.1)
        cyl_r = sqrt(2) * shrink + .01;
    else
        cyl_r = sqrt(2) * shrink;

    float a = (sqrt(2) - cyl_r) * sin(M_PI / 4.0);
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glRotatef(i * 90, 0, 1, 0);
        glTranslatef(a + (cyl_r - shrink) - .005, 0, a + (cyl_r - shrink) - .005);
        glRotatef(-45, 0, 1, 0);
        drawCylinder(10, 36, cyl_h, cyl_r);
        glPopMatrix();
    }

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glRotatef(i * 90, 1, 0, 0);
        glTranslatef(0, a + (cyl_r - shrink) - .005, a + (cyl_r - shrink) - .005);
        glRotatef(45, 1, 0, 0);
        drawCylinder(10, 36, cyl_h, cyl_r);
        glPopMatrix();
    }

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glRotatef(-i * 90, 0, 0, 1);
        glTranslatef(-a - (cyl_r - shrink) + .005, a + (cyl_r - shrink) - .005, 0);
        glRotatef(45, 0, 0, 1);
        glRotatef(90, 0, 1, 0);
        drawCylinder(10, 36, cyl_h, cyl_r);
        glPopMatrix();
    }
}

/*********************/

/*********************************/
/// from the link sir provided
float computeScaleForLength(const float v[3], float length)
{
    // and normalize the vector then re-scale to new radius
    return length / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}
vector<vector<point>> getUnitPositiveX(unsigned int pointsPerRow)
{
    const float DEG2RAD = acos(-1.0f) / 180.0f;
    vector<vector<point>> p;

    std::vector<float> vertices;
    float n1[3]; // normal of longitudinal plane rotating along Y-axis
    float n2[3]; // normal of latitudinal plane rotating along Z-axis
    float v[3];  // direction vector intersecting 2 planes, n1 x n2
    float a1;    // longitudinal angle along y-axis
    float a2;    // latitudinal angle
    float scale;

    // rotate latitudinal plane from 45 to -45 degrees along Z-axis
    for (unsigned int i = 0; i < pointsPerRow; ++i)
    {
        // normal for latitudinal plane
        a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;
        vector<point> row_p;

        // rotate longitudinal plane from -45 to 45 along Y-axis
        for (unsigned int j = 0; j < pointsPerRow; ++j)
        {
            // normal for longitudinal plane
            a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            scale = computeScaleForLength(v, 2 * shrink);
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;

            point single_p;
            single_p.x = v[0];
            single_p.y = v[1];
            single_p.z = v[2];

            row_p.push_back(single_p);

            vertices.push_back(v[0]);
            vertices.push_back(v[1]);
            vertices.push_back(v[2]);

            // DEBUG
            // std::cout << "vertex: (" << v[0] << ", " << v[1] << ", " << v[2] << "), "
            //          << sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) << std::endl;
        }
        p.push_back(row_p);
    }

    return p;
}

void drawOneSphere()
{

    vector<vector<point>> p = getUnitPositiveX(12);
    glPushMatrix();
    // if(shrink>.2)
    // glTranslatef(-.02,0,0);

    for (int i = 0; i < 12 - 1; i++)
    {
        for (int j = 0; j < 12 - 1; j++)
        {
            glPushMatrix();
            glTranslatef(2 - 2 * shrink, 0, 0);

            glBegin(GL_QUADS);
            {
                int k = i * 3;
                glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
                glVertex3f(p[i][j + 1].x, p[i][j + 1].y, p[i][j + 1].z);
                glVertex3f(p[i + 1][j + 1].x, p[i + 1][j + 1].y, p[i + 1][j + 1].z);
                glVertex3f(p[i + 1][j].x, p[i + 1][j].y, p[i + 1][j].z);
            }
            glEnd();
            cout << p[i][j].x << " " << p[i][j].z << " " << p[i][j].z << endl;
            glPopMatrix();
        }
    }
    glPopMatrix();
}
void drawCubeSphere()
{
    glColor3f(0, 1, 0);
    drawOneSphere();
    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    drawOneSphere();
    glPopMatrix();
    glColor3f(0, 0, 1);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    drawOneSphere();
    glPopMatrix();
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    drawOneSphere();
    glPopMatrix();
    glColor3f(1, 0, 0);
    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    drawOneSphere();
    glPopMatrix();
    glPushMatrix();
    glRotatef(-90, 0, 0, 1);
    drawOneSphere();
    glPopMatrix();
}
/****************************************************/
int counter = 0;

point pos, l, r, u;
float rotationAng = 45.0;

void asxes()
{
    // opent gl funtion
    glColor3f(0.5, 0.5, 0);
    glBegin(GL_LINES);
    {
        glVertex3f(100, 0, 0);
        glVertex3f(-100, 0, 0);

        glVertex3f(0, -100, 0);
        glVertex3f(0, 100, 0);

        glVertex3f(0, 0, 100);
        glVertex3f(0, 0, -100);
    }
    glEnd();
}

void triangle(double a, bool ref)
{
    tri_h = a - (2 * shrink);
    glBegin(GL_TRIANGLES);
    {
        if (ref)
            glVertex3f(a - shrink, -shrink, shrink);
        else
            glVertex3f(a - shrink, shrink, shrink);

        if (ref)
            glVertex3f(shrink, -a + shrink, shrink);
        else
            glVertex3f(shrink, a - shrink, shrink);
        if (ref)
            glVertex3f(shrink, -shrink, a - shrink);
        else
            glVertex3f(shrink, shrink, a - shrink);
    }
    glEnd();
}

void drawOctahedral()
{
    float r = 1;
    float g = 0;
    float b = 1;
    bool ref = 0;

    for (int i = 0; i < 8; i++)
    {
        r = 1 - r;
        g = 1 - g;
        if (i == 4)
        {
            ref = 1;
            r = 1 - r;
            g = 1 - g;
        }

        glColor3f(r, g, b);
        glPushMatrix();
        glRotatef(i * 90, 0, 1, 0);
        if (shrink > 0.1)
        {
            if (ref)
                glTranslatef(t, -t, t);
            else
                glTranslatef(t, t, t);
        }
        triangle(2, ref);
        glPopMatrix();
    }
}

int animate;

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
            
    glRotatef(rotationAng, 0, 0, 1);

    asxes();
    drawCubeSphere();
    drawOctahedral();
    drawAllCylinders();
    glFlush(); // This flush really important
    // buffer is ready sent to the monitor
}

void init()
{
    animate = 1;
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION); // matrix mode ta hobe Gl projection mode
    // glLoadIdentity();            //ager j matrix ta oita fele deoa shudhu matro identity matrix ta\
    load kora \
    so projection matrix ta ekhon shudhumatro identity matrix
    gluPerspective(80, 1, 1, 100.0);
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
        ++counter;
    }
    glutPostRedisplay();
    glutTimerFunc(1000, Timer, 0);
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y)
{
    float v = 0.1;
    double rate = 0.1;
    switch (key)
    {

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
    case ',':
    {
        if (shrink < 1)
        {
            shrink += v;
            t += .01;
            // r += v;
            // h -= (2 * v);
        }
        cout << shrink << endl;
        break;
    }
    case '.':
        if (shrink > 0.1)
        {
            if (shrink == .2)
                t = 0;
            else
                t -= .01;
            shrink -= v;

            // r -= v;
            // h += (2 * v);
        }
        break;
    case 27:     // ESC key
        exit(0); // Exit window
        break;
    }
    glutPostRedisplay();
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
    u.y = 0;
    u.z = 1;
    r.x = 0;
    r.y = 1;
    r.z = 0;
    printf("Hello world");
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 00);

    glutInitWindowSize(700, 700);
    glutCreateWindow("OpoenGL demo");

    init();

    glutDisplayFunc(display); // called on repaint request
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    // glutIdleFunc(idle);
    glutTimerFunc(5000, Timer, 0);

    glutMainLoop();

    return 0;
}
