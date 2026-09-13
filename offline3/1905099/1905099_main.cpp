#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif
#include "1905099_headers.h"
#include "bitmap_image.hpp"

#ifndef M_PI
#define M_PI 3.14159265
#endif

#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

ifstream readfile;

Point pos, l, r, u;
float rotationAng = 45.0;

int noOfObjects;
int noOfPointLights;
Object *objects[100];
PointLight *pointLights[100];

int screen;
int windowSize = 800;
double fovy = 60;

int imgCnt = 0;

#define CAMERA_CHANGE 0.5
#define OBJECT_ROTATION 0.1

void init()
{
    printf("Do your initialization here\n");
    glClearColor(0.0f, 0.0f, 0.0f,
                 1.0f); // Set background color to black and opaque

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, 1, 1, 500);
    //    glOrtho(-10,10,-10,10,-10,10);

    //   initialize camera position
    pos.x = 100;
    pos.y = -10;
    pos.z = 10;
    l.x = -1;
    l.y = 0;
    l.z = 0;
    u.x = 0;
    u.y = 0;
    u.z = 1;
    r.x = 0;
    r.y = 1;
    r.z = 0;
}

void axes()
{
    glLineWidth(3);
    glBegin(GL_LINES);
    {
        glColor3f(1.0f, 0.0f, 0.0f); // Red - x axis
        glVertex3f(-100, 0, 0);
        glVertex3f(100, 0, 0);

        glColor3f(0.0f, 1.0f, 0.0f); // Green - y axis
        glVertex3f(0, -100, 0);
        glVertex3f(0, 100, 0);

        glColor3f(0.0f, 0.0f, 1.0f); // Blue - z axis
        glVertex3f(0, 0, -100);
        glVertex3f(0, 0, 100);
    }
    glEnd();
    glLineWidth(1);
}

void display()
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(pos.x, pos.y, pos.z,
              pos.x + l.x, pos.y + l.y, pos.z + l.z,
              u.x, u.y, u.z);
    glRotatef(rotationAng, 0, 0, 1);

    axes();

    Object *floor = new Floor(1000, 20);
    floor->draw();

    for (int i = 0; i < noOfObjects; i++)
    {
        objects[i]->draw();
    }

    for (int i = 0; i < noOfPointLights; i++)
    {
        pointLights[i]->draw();
    }

    glutSwapBuffers();
}

void idle()
{
    glutPostRedisplay(); // Post a re-paint request to activate display(),
                         // variable set koire then call koro display
}

void capture()
{
    bitmap_image out(screen, screen);

    out.set_all_channels(0, 0, 0);

    double plane_distance = ((double)windowSize / 2.0) / tan((fovy / 2.0) * (M_PI / 180.0));

    Point topleft = pos + l * plane_distance - r * (windowSize / 2.0) + u * (windowSize / 2.0);

    double du = (double)windowSize / screen;
    double dv = (double)windowSize / screen;
    cout << du << " " << dv << endl;

    topleft = topleft + r * (du / 2.0) - u * (dv / 2.0);

    cout << topleft.x << " " << topleft.y << " " << topleft.z << endl;

    for (int i = 0; i < screen; i++)
    {
        for (int j = 0; j < screen; j++)
        {
            Point cur = topleft + r * (i * du) - u * (j * dv);
            Point direction = cur - pos;
            // cout<<direction.x<<" "<<direction.y<<" "<<direction.z<<endl;
            Ray ray(pos, direction);

            int nearest = 99999999;
            double t, tMin = 99999999;

// cout<<"noOfObjects: "<<noOfObjects<<endl;
            for (int i = 0; i < noOfObjects; i++)
            {
                Color *color = new Color(0, 0, 0);
                t = objects[i]->intersect(&ray, color, 0);
                if (t > 0 && t < tMin)
                {
                    tMin = t;
                    nearest = i;
                }
            }

            if (nearest != 99999999)
            {
                Color *color = new Color(0, 0, 0);
                tMin = objects[nearest]->intersect(&ray, color, 1);
                // cout << color->r << " " << color->g << " " << color->b << endl;
                color->clip();
                out.set_pixel(i, j, round(color->r * 255), round(color->g * 255), round(color->b * 255));
            }
        }
    }

    out.save_image("Output_1" + to_string(++imgCnt) + ".bmp");
    cout << "image captured\n";
}

void keyboardListener(unsigned char key, int x, int y)
{
    float v = 0.1;
    double rate = 0.1;
    switch (key)
    {
    // Control eye (location of the eye)
    // control eyex
    case '0':
        capture();
        // bitmap_image_count++;
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

void loadData()
{
    readfile.open("input.txt");
    readfile >> screen;
    cout << screen << endl;
    string obType;
    Color color;
    Coefficients cof;
    int shine;

    readfile >> noOfObjects;

    for (int i = 0; i < noOfObjects; i++)
    {
        readfile >> obType;
        if (obType == "sphere")
        {
            double radius;
            Point center;
            readfile >> center.x >> center.y >> center.z;
            readfile >> radius >> color.r >> color.g >> color.b;
            cout << center.x << " " << center.y << " " << center.z << " " << radius << " " << color.r << " " << color.g << " " << color.b << endl;
            readfile >> cof.ambient >> cof.diffuse >> cof.specular >> cof.reflection;
            readfile >> cof.shininess;
            objects[i] = new Sphere(center, radius, color, cof);
        }
    }

    readfile >> noOfPointLights;
    cout << noOfPointLights << endl;
    for (int i = 0; i < noOfPointLights; i++)
    {
        Point p;
        Color c;
        readfile >> p.x >> p.y >> p.z >> c.r >> c.g >> c.b;
        cout << p.x << " " << p.y << " " << p.z << " " << c.r << " " << c.g << " " << c.b << endl;
        pointLights[i] = new PointLight(p, c);
    }

    readfile.close();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    loadData();
    glutInitWindowSize(windowSize, windowSize); // Set the window's initial width & height
    glutInitWindowPosition(750,
                           250); // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("1905099_raytracing");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);

    glutIdleFunc(idle);
    init();

    glutMainLoop();
    return 0;
}
