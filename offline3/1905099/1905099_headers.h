#include <bits/stdc++.h>
#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif

#define epsilon 0.0000001

using namespace std;

struct Color
{
    double r, g, b;

    // Color c2(1.0, 0.5, 0.25);
    Color(double r, double g, double b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    // Color c1;
    Color()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }

    // clip
    void clip()
    {
        if (this->r > 1)
            this->r = 1;
        if (this->r < 0)
            this->r = 0;
        if (this->g > 1)
            this->g = 1;
        if (this->g < 0)
            this->g = 0;
        if (this->b > 1)
            this->b = 1;
        if (this->b < 0)
            this->b = 0;
    }
};

struct Point
{
    double x, y, z, n;

    Point(double x, double y, double z, double n)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->n = n;
    }

    Point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->n = 0.1;
    }
    Point()
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
        this->n = 1.0;
    }

    Point(const Point &p) : x(p.x), y(p.y), z(p.z), n(p.n) {}

    Point operator+(Point p)
    {
        return Point(x + p.x, y + p.y, z + p.z);
    }

    Point operator-(Point p)
    {
        return Point(x - p.x, y - p.y, z - p.z);
    }

    Point operator*(double c)
    {
        return Point(x * c, y * c, z * c);
    }

    Point operator/(double c)
    {
        return Point(x / c, y / c, z / c);
    }

    // dot product
    double operator*(Point &p)
    {
        return (x * p.x + y * p.y + z * p.z);
    }

    // cross product
    Point operator^(Point &p)
    {
        return Point(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
    }

    void scaleDown()
    {
        x /= n;
        y /= n;
        z /= n;
        n = 1.0;
    }

    double length()
    {
        return sqrt(x * x + y * y + z * z);
    }

    void normalize()
    {
        double l = length();
        x /= l;
        y /= l;
        z /= l;
    }
};

struct Coefficients
{
    double ambient, diffuse, specular, reflection;
    int shininess;
    // Coefficients cof(0.1, 0.5, 0.5, 0.5, 10);
    Coefficients(double ambient, double diffuse, double specular, double reflection, int shininess)
    {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->reflection = reflection;
        this->shininess = shininess;
    }
    // Coefficients cof;
    Coefficients()
    {
        this->ambient = 0.1;
        this->diffuse = 0.5;
        this->specular = 0.5;
        this->reflection = 0.5;
        this->shininess = 10;
    }
};

class Ray
{
public:
    Point start, dir;
    Ray()
    {
        start = Point(0, 0, 0);
        dir = Point(0, 0, 0);
    }
    Ray(Point start, Point dir)
    {
        this->start = start;
        dir.normalize();
        this->dir = dir;
    }
};

class PointLight
{
public:
    Point position;
    Color color;
    PointLight()
    {
        position = Point(0, 0, 0);
        color = Color(1, 1, 1);
    }
    PointLight(Point position, Color color)
    {
        this->position = position;
        this->color = color;
    }
    void draw()
    {
        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        glColor3f(color.r, color.g, color.b);
        glutSolidSphere(0.5, 10, 10);
        glPopMatrix();
    }
};

class SpotLight
{
};

class Object
{
public:
    Color color;
    Coefficients coefficients;
    Object()
    {
        color = Color(0, 0, 0);
        coefficients = Coefficients();
    }
    Object(Color color, Coefficients coefficients)
    {
        this->color = color;
        this->coefficients = coefficients;
    }
    // virtual void setCoefficients(Coefficients coefficients) = 0;
    virtual void draw() = 0;
    virtual double intersect(Ray *ray, Color *color, int level)
    {
        return -1.0;
    }

    void ComputeCoefficients(Point intersecting_point, Ray lightray, Ray ray_normal, Ray *r, PointLight *pointLight, Color intersectpoint_color, Color *color)
    {
        double lambert_value = max(0.0, ray_normal.dir * lightray.dir);
        color->r += pointLight->color.r * intersectpoint_color.r * coefficients.diffuse * lambert_value;
        color->g += pointLight->color.g * intersectpoint_color.g * coefficients.diffuse * lambert_value;
        color->b += pointLight->color.b * intersectpoint_color.b * coefficients.diffuse * lambert_value;

        Point dir = ray_normal.dir * 2.0 * (ray_normal.dir * lightray.dir) - lightray.dir;

        dir.normalize();
        Ray reflected_ray(intersecting_point + (dir * epsilon), dir);
        Point V = r->dir * -1.0;
        double phone_value = max(0.0, reflected_ray.dir * V);
        color->r += pointLight->color.r * intersectpoint_color.r * coefficients.specular * pow(phone_value, coefficients.shininess);
        color->g += pointLight->color.g * intersectpoint_color.g * coefficients.specular * pow(phone_value, coefficients.shininess);
        color->b += pointLight->color.b * intersectpoint_color.b * coefficients.specular * pow(phone_value, coefficients.shininess);
        cout<<color->r<<" "<<color->g<<" "<<color->b<<endl; 
    }
};

extern int noOfObjects;
extern int noOfPointLights;
extern Object *objects[100];
extern PointLight *pointLights[100];

class Sphere : public Object
{
public:
    Point center;
    double radius;

    struct Point points[50][50];
    int stacks, slices;

    Sphere(Point center, double radius, Color color, Coefficients coefficients)
    {
        this->center = center;
        this->radius = radius;
        this->color = color;
        this->coefficients = coefficients;

        stacks = slices = 40;

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

    void draw()
    {
        glPushMatrix();
        glTranslatef(center.x, center.y, center.z);
        glBegin(GL_QUADS);
        for (int j = 0; j < stacks; j++)
        {

            for (int i = 0; i < slices; i++)
            {
                GLfloat c = (2 + cos((i + j) * 2.0 * M_PI / slices)) / 3;
                glColor3f(color.r, color.g, color.b);
                glVertex3f(points[j][i].x, points[j][i].y, points[j][i].z);
                glVertex3f(points[j][i + 1].x, points[j][i + 1].y, points[j][i + 1].z);

                glVertex3f(points[j + 1][i + 1].x, points[j + 1][i + 1].y, points[j + 1][i + 1].z);
                glVertex3f(points[j + 1][i].x, points[j + 1][i].y, points[j + 1][i].z);
            }
        }
        glEnd();
        glPopMatrix();
    }

    // void setCoefficients(Coefficients coefficients)
    // {
    //     this->coefficients = coefficients;
    // }

    double intersect(Ray *r, Color *color, int level)
    {
        r->start = r->start - center;

        double ld, d, l;
        Point temp = r->start * -1.0;
        ld = r->dir * temp;

        // cout<<ld<<endl;

        if (ld < 0)
        {
            return -1;
        }

        d = sqrt(r->start * r->start - ld * ld);
        if (d * d > radius * radius)
        {
            return -1;
        }

        l = sqrt(radius * radius - d * d);

        // cout<<l<<endl;
        double t1 = ld - l;
        double t2 = ld + l;

        double tmin;

        if (level == 0)
        {
            tmin = min(t1, t2);
            if (tmin < 0)
            {
                return -1;
            }
            else
            {
                return tmin;
            }
        }

        r->start = r->start + center;
        Point intersecting_point = r->start + r->dir * tmin;

        Color intersectColor = this->color;
        color->r = intersectColor.r * coefficients.ambient;
        color->g = intersectColor.g * coefficients.ambient;
        color->b = intersectColor.b * coefficients.ambient;

        Point normal = intersecting_point - center;
        Ray normalRay(intersecting_point, normal);
        // cout << noOfPointLights << endl;
        for (int i = 0; i < noOfPointLights; i++)
        {
            Point dir = pointLights[i]->position - intersecting_point;
            Ray lightray(pointLights[i]->position, dir);
            // cout << lightray.start.x << " " << lightray.start.y << " " << lightray.start.z << endl;

            double tMinimmum = 99999999, t;
            for (int j = 0; j < noOfObjects; j++)
            {
                Color *tempcolor = new Color(0, 0, 0);
                double t = objects[j]->intersect(&lightray, tempcolor, 0);
                if (t > 0 && t < tMinimmum)
                {
                    tMinimmum = t;
                }
            }
            Point minObjIntersect = lightray.start + lightray.dir * tMinimmum;
            // cout << min_obj_intersecting_point.x << " " << min_obj_intersecting_point.y << " " << min_obj_intersecting_point.z << endl;

            if ((minObjIntersect - lightray.start).length() < (lightray.start - intersecting_point).length())
            {
                cout<<"continued"<<endl;
                continue;
            }
            ComputeCoefficients(intersecting_point, lightray, normalRay, r, pointLights[i], intersectColor, color);
        }

        return tmin;
    }
};

class Floor : public Object
{
private:
    int floorWidth;
    int tileWidth;

public:
    Floor(int floorWidth, int tileWidth)
    {
        this->floorWidth = floorWidth;
        this->tileWidth = tileWidth;
    }
    void drawCheckerBox(double a, int color = 0)
    {
        glBegin(GL_QUADS);
        {
            if (color == 0)
            {
                glColor3f(0.0f, 0.0f, 0.0f); // Black
            }
            else
            {
                glColor3f(1.0f, 1.0f, 1.0f); // White
            }
            glVertex3f(0, 0, 0);
            glVertex3f(0, a, 0);
            glVertex3f(a, a, 0);
            glVertex3f(a, 0, 0);
        }
        glEnd();
    }
    void draw()
    {
        int t = floorWidth / tileWidth;
        for (int i = -t; i < t; i++)
        {
            for (int j = -t; j < t; j++)
            {
                glPushMatrix();
                glTranslatef(i * tileWidth, j * tileWidth, 0);
                drawCheckerBox(tileWidth, (i + j) % 2);
                glPopMatrix();
            }
        }
    }
    //    void setCoefficients(Coefficients coefficients)
    //     {
    //         this->coefficients = coefficients;
    //     }
};
