// g++ 1905099.cpp -o show
//./show
#include <bits/stdc++.h>
#include <fstream>
#include <stack>
#include <cmath>
#include <iostream>
#include <iomanip>
#include "bitmap_image.hpp"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265
#endif

typedef array<array<double, 4>, 4> Matrix4x4;
typedef array<array<double, 3>, 4> Matrix4x3;
typedef array<array<double, 3>, 3> Matrix3x3;

vector<Matrix3x3> trianles;
int counter = 0;

double eyex, eyey, eyez;
double upx, upy, upz;
double lookX, lookY, lookZ;
double fovY, aspectRatio, znear, zfar;

ifstream readFile;
ofstream writeFile1, writeFile2, writeFile3, writeFile4;

int screen_width, screen_height;

struct vec
{
    double x, y, z;
};

struct point
{
    double x, y, z;
};

struct color
{
    int r, g, b;
};

static unsigned long int g_seed = 1;
inline int my_random() // dont understand
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

double normalize(double x, double y, double z)
{
    return 1 / sqrt(x * x + y * y + z * z);
}

vec cross(double x1, double y1, double z1, double x2, double y2, double z2)
{
    return {(z2 * y1) - (z1 * y2),
            (x2 * z1) - (x1 * z2),
            (y2 * x1) - (y1 * x2)};
}

double dot(double x1, double y1, double z1, double x2, double y2, double z2)
{
    return (x1 * x2 + y1 * y2 + z1 * z2);
}

vec rotatePoint(char ch, vec a, double angle)
{
    double n = (a.x, a.y, a.z);

    a.x = a.x * n;
    a.y = a.y * n;
    a.z = a.z * n;

    double theta = (angle * M_PI) / 180;

    vec c_p;
    double d;
    vec v;

    if (ch == 'i')
    {
        v = {1, 0, 0};
    }
    else if (ch == 'j')
    {
        v = {0, 1, 0};
    }
    else if (ch == 'k')
    {
        v = {0, 0, 1};
    }

    c_p = cross(a.x, a.y, a.z, v.x, v.y, v.z);
    d = dot(a.x, a.y, a.z, v.x, v.y, v.z);

    double d_x = d * a.x;
    double d_y = d * a.y;
    double d_z = d * a.z;

    vec c;

    c.x = v.x * cos(theta) + (1 - cos(theta)) * d_x + sin(theta) * c_p.x;
    c.y = v.y * cos(theta) + (1 - cos(theta)) * d_y + sin(theta) * c_p.y;
    c.z = v.z * cos(theta) + (1 - cos(theta)) * d_z + sin(theta) * c_p.z;

    return c;
}

Matrix4x3 matrixProduct(const Matrix4x4 &A, const Matrix4x3 &B)
{
    Matrix4x3 result{};
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result[i][j] = 0;
            for (int k = 0; k < 4; ++k)
            {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}

Matrix4x4 matrixProduct2(const Matrix4x4 &A, const Matrix4x4 &B)
{
    Matrix4x4 result{};
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result[i][j] = 0;
            for (int k = 0; k < 4; ++k)
            {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return result;
}

void stage1()
{
    string command;
    stack<Matrix4x4> S;
    Matrix4x4 M = {
        {{1, 0, 0, 0},
         {0, 1, 0, 0},
         {0, 0, 1, 0},
         {0, 0, 0, 1}} // Identity matrix
    };

    while (true)
    {
        readFile >> command;
        // cout << command << endl;
        if (command == "triangle")
        {
            // cout << "indeed triangle" << endl;
            counter++;
            Matrix4x3 P;

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (j == 3)
                        P[j][i] = 1;
                    else
                        readFile >> P[j][i];
                }
            }

            P = matrixProduct(M, P);
            writeFile1 << "";

            Matrix3x3 t;

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    t[j][i] = P[j][i] / P[3][i];
                    writeFile1 << fixed << setprecision(7) << t[j][i] << " ";
                }
                writeFile1 << endl;
            }

            trianles.push_back(t);
            writeFile1 << endl;
        }
        else if (command == "translate")
        {
            // cout << "indeed translate" << endl;
            Matrix4x4 T;

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (j == 3 && i != 3)
                        readFile >> T[i][j];
                    else
                        T[i][j] = (i == j);
                }
            }

            M = matrixProduct2(M, T);
        }
        else if (command == "scale")
        {
            // cout << "indeed tscale" << endl;
            Matrix4x4 T;

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (i == 3 && j == 3)
                        T[i][j] = 1;
                    else if (i == j)
                        readFile >> T[i][j];
                    else
                        T[i][j] = 0;
                }
            }

            M = matrixProduct2(M, T);
        }
        else if (command == "rotate")
        {
            // cout << "indeed rotate" << endl;
            vec a;
            double angle;

            readFile >> angle >> a.x >> a.y >> a.z;
            vec c1 = rotatePoint('i', a, angle);
            vec c2 = rotatePoint('j', a, angle);
            vec c3 = rotatePoint('k', a, angle);

            Matrix4x4 T = {
                {{c1.x, c2.x, c3.x, 0},
                 {c1.y, c2.y, c3.y, 0},
                 {c1.z, c2.z, c3.z, 0},
                 {0, 0, 0, 1}}};

            M = matrixProduct2(M, T);
        }
        else if (command == "push")
        {
            // cout << "indeed push" << endl;
            S.push(M);
        }
        else if (command == "pop")
        {
            // cout << "indeed pop" << endl;
            M = S.top();
            S.pop();
        }
        else if (command == "end")
        {
            // cout << "indeed end" << endl;
            break;
        }
    }
}
void stage2(const Matrix4x4 &V)
{
    for (int k = 0; k < counter; k++)
    {
        Matrix4x3 t;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (i == 3)
                    t[i][j] = 1;
                else
                    t[i][j] = trianles[k][i][j];
            }
        }

        t = matrixProduct(V, t);
        writeFile2 << "";

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {

                trianles[k][j][i] = t[j][i] / t[3][i];

                writeFile2 << fixed << setprecision(7) << trianles[k][j][i] << " ";
            }
            writeFile2 << endl;
        }

        writeFile2 << endl;
    }
}

void stage3(double fovy, double aspectRatio, double znear, double zfar)
{
    // //cout << fixed << setprecision(6) << fovY << " " << aspectRatio << " " << znear << " " << zfar << endl;

    double fovx, t, r;
    fovx = fovy * aspectRatio;
    double theta;

    theta = (fovy * M_PI) / (2.0 * 180);
    t = znear * tan(theta);

    theta = (fovx * M_PI) / (2.0 * 180);
    r = znear * tan(theta);

    Matrix4x4 P = {
        {{znear / r, 0, 0, 0},
         {0, znear / t, 0, 0},
         {0, 0, -(zfar + znear) / (zfar - znear), -(2.0 * zfar * znear) / (zfar - znear)},
         {0, 0, -1.0, 0}}};

    for (int k = 0; k < counter; k++)
    {
        Matrix4x3 t;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (i == 3)
                    t[i][j] = 1;
                else
                    t[i][j] = trianles[k][i][j];
            }
        }

        t = matrixProduct(P, t);
        writeFile3 << "";

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                // //cout << t[j][i] << " ";

                trianles[k][j][i] = t[j][i] / t[3][i];
                writeFile3 << fixed << setprecision(7) << trianles[k][j][i] << " ";
            }
            writeFile3 << endl;
            // //cout << endl;
        }

        writeFile3 << endl;
        // //cout << endl;
    }
}

void stage4(string path, double screen_height, double screen_width)
{
    double left_limit = -1, right_limit = 1;
    double top_limit = 1, bottom_limit = -1;
    double z_min = -2, z_max = 2;

    double dx = (right_limit - left_limit) / screen_width;
    double dy = (top_limit - bottom_limit) / screen_height;
    double Top_Y = top_limit - (dy / 2.0);
    double Left_X = left_limit + (dx / 2.0);

    // cout << dy << " " << dx << endl;
    // cout << Top_Y << " " << Left_X << endl;

    vector<vector<double>> z_buffer(screen_height, vector<double>(screen_width));

    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            z_buffer[i][j] = z_max;
        }
    }

    bitmap_image out(screen_width, screen_height);
    out.set_all_channels(0, 0, 0); // background color black

    readFile.open(path + "/my_stage3.txt");

    for (int k = 0; k < counter; k++)
    {
        point A = {trianles[k][0][0], trianles[k][1][0], trianles[k][2][0]};
        point B = {trianles[k][0][1], trianles[k][1][1], trianles[k][2][1]};
        point C = {trianles[k][0][2], trianles[k][1][2], trianles[k][2][2]};

        // point A, B, C;
        readFile >> A.x >> A.y >> A.z;
        readFile >> B.x >> B.y >> B.z;
        readFile >> C.x >> C.y >> C.z;

        // sorting A<B<C
        if (A.y > B.y)
            swap(A, B);
        if (A.y > C.y)
            swap(A, C);
        if (B.y > C.y)
            swap(B, C);

        // cout << fixed << setprecision(6) << A.x << " " << A.y << " " << A.z << endl;
        // cout << fixed << setprecision(6) << B.x << " " << B.y << " " << B.z << endl;
        // cout << fixed << setprecision(6) << C.x << " " << C.y << " " << C.z << endl;

        // cout << C.y << " " << A.y << endl;
        color c = {my_random(), my_random(), my_random()};

        double top_scanline;
        double bottom_scanline;

        bottom_scanline = B.y;
        top_scanline = C.y;

        bottom_scanline = max(bottom_scanline, bottom_limit);
        top_scanline = min(top_scanline, top_limit);

        for (double ys = bottom_scanline; ys <= top_scanline; ys += dy)
        {

            double xa = 0, xb = -1;
            double za = 0, zb = -1;

            if (B.y != C.y and A.y != C.y)
            {

                xa = B.x + (ys - B.y) * (C.x - B.x) / (C.y - B.y);
                xb = C.x + (ys - C.y) * (A.x - C.x) / (A.y - C.y);

                za = B.z + (ys - B.y) * (C.z - B.z) / (C.y - B.y);
                zb = C.z + (ys - C.y) * (A.z - C.z) / (A.y - C.y);

                if (xa > xb)
                {
                    swap(xa, xb);
                    swap(za, zb);
                }
            }
            xa = max(xa, left_limit);
            xb = min(xb, right_limit);

            for (double xp = xa; xp <= xb; xp += dx)
            {
                if (xb == xa)
                    continue;

                int i = (Top_Y - ys) / dy;
                int j = (xp - Left_X) / dx;

                double z = za + (zb - za) * (xp - xa) / (xb - xa);
                if (z < z_buffer[i][j] and z > z_min)
                {
                    z_buffer[i][j] = z;
                    out.set_pixel(j, i, c.r, c.g, c.b);
                }
            }
        }

        top_scanline = A.y;
        bottom_scanline = B.y;
        // cout << bottom_scanline << " " << top_scanline << endl;
        top_scanline = min(top_scanline, top_limit);
        bottom_scanline = max(bottom_scanline, bottom_limit);
        // cout << bottom_scanline << " " << top_scanline << endl;
        for (double ys = bottom_scanline; ys <= top_scanline; ys += dy)
        {
            double xa = 0, xb = -1;
            double za = 0, zb = -1;

            if (B.y != A.y and A.y != C.y)
            {
                xa = B.x + (ys - B.y) * (A.x - B.x) / (A.y - B.y);
                xb = C.x + (ys - C.y) * (A.x - C.x) / (A.y - C.y);

                za = B.z + (ys - B.y) * (A.z - B.z) / (A.y - B.y);
                zb = C.z + (ys - C.y) * (A.z - C.z) / (A.y - C.y);

                if (xa > xb)
                {
                    swap(xa, xb);
                    swap(za, zb);
                }
            }

            xa = max(xa, left_limit);
            xb = min(xb, right_limit);

            for (double xp = xa; xp <= xb; xp += dx)
            {
                if (xb == xa)
                    continue;

                int i = (Top_Y - ys) / dy;
                int j = (xp - Left_X) / dx;

                double z = za + (zb - za) * (xp - xa) / (xb - xa);
                if (z < z_buffer[i][j] and z > z_min)
                {
                    z_buffer[i][j] = z;
                    out.set_pixel(j, i, c.r, c.g, c.b);
                }
            }
        }
    }
    writeFile4 << "";

    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            if (z_buffer[i][j] < z_max)
            {
                writeFile4 << fixed << setprecision(6) << z_buffer[i][j] << "\t";
            }
        }
        writeFile4 << endl;
    }

    out.save_image(path + "/out.bmp");

    readFile.close();

    z_buffer.clear();
    z_buffer.shrink_to_fit();
}

void printTriangles()
{
    for (int k = 0; k < counter; k++)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                cout << fixed << setprecision(7) << trianles[k][j][i] << " ";
            }
            cout << endl;
        }

        cout << endl;
    }
}

int main(int argc, char **argv)
{
    string path, st;
    cin >> st;

    path = "IOs/";
    path += st;

    readFile.open(path + "/scene.txt");

    writeFile1.open(path + "/stage1.txt");
    writeFile2.open(path + "/stage2.txt");
    writeFile3.open(path + "/stage3.txt");
    writeFile4.open(path + "/z_buffer.txt");

    vec l, r, u;

    readFile >> eyex >> eyey >> eyez;
    // cout << fixed << setprecision(6) << eyex << " " << eyey << " " << eyez << endl;
    readFile >> lookX >> lookY >> lookZ;
    // cout << fixed << setprecision(6) << lookX << " " << lookY << " " << lookZ << endl;
    readFile >> upx >> upy >> upz;
    // cout << fixed << setprecision(6) << upx << " " << upy << " " << upz << endl;
    readFile >> fovY >> aspectRatio >> znear >> zfar;
    // cout << fixed << setprecision(6) << fovY << " " << aspectRatio << " " << znear << " " << zfar << endl;

    l = {lookX - eyex, lookY - eyey, lookZ - eyez};
    // cout << l.x << " " << l.y << " " << l.z << endl;
    double n;
    n = normalize(l.x, l.y, l.z);
    l = {l.x * n, l.y * n, l.z * n};
    // cout << l.x << " " << l.y << " " << l.z << endl;

    // cout << upx << upy << upz << endl;

    r = cross(l.x, l.y, l.z, upx, upy, upz);
    // cout << r.x << " " << r.y << " " << r.z << endl;
    n = normalize(r.x, r.y, r.z);
    r = {r.x * n, r.y * n, r.z * n};
    // cout << r.x << " " << r.y << " " << r.z << endl;

    u = cross(r.x, r.y, r.z, l.x, l.y, l.z);

    Matrix4x4 T = {
        {{1, 0, 0, -eyex},
         {0, 1, 0, -eyey},
         {0, 0, 1, -eyez},
         {0, 0, 0, 1}}};

    Matrix4x4 R = {
        {{r.x, r.y, r.z, 0},
         {u.x, u.y, u.z, 0},
         {-l.x, -l.y, -l.z, 0},
         {0, 0, 0, 1}}};

    Matrix4x4 V = matrixProduct2(R, T);

    stage1();
    // printTriangles();
    stage2(V);
    // printTriangles();
    stage3(fovY, aspectRatio, znear, zfar);
    // printTriangles();

    cout << counter << endl;

    readFile.close();

    readFile.open(path + "/config.txt");

    readFile >> screen_width >> screen_height;

    readFile.close();
    stage4(path, screen_height, screen_width);

    writeFile4.close();
    writeFile1.close();
    writeFile2.close();
    writeFile3.close();

    return 0;
}