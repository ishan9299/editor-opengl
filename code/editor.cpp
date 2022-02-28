#include "editor.h"

#define Assert(expression) if(!expression) {*(int *)0 = 0;}
#define global_variable static
#define local_persist   static

#include "editor_opengl.h"

void
IdentityMatrix4x4(float matrix[][4])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) {
                matrix[i][j] = 1;
            } else {
                matrix[i][j] = 0;
            }
        }
    }
}

void
TranslateMatrix4x4(float matrix[][4], float *v)
{
    IdentityMatrix4x4(matrix);
    matrix[3][0] = v[0];
    matrix[3][1] = v[1];
    matrix[3][2] = v[2];
}

void
ScaleMatrix4x4(float matrix[][4], float scale)
{
    IdentityMatrix4x4(matrix);
    matrix[0][0] = matrix[0][0] * scale;
    matrix[1][1] = matrix[1][1] * scale;
    matrix[2][2] = matrix[2][2] * scale;
}

void
ZeroMatrix4x4(float matrix[][4])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[i][j] = 0;
        }
    }
}

void
MakeOrthographicMatrix(float ortho_right, float ortho_left, float ortho_top,
        float ortho_bottom, float ortho_far, float ortho_near,
        float matrix[][4])
{
    IdentityMatrix4x4(matrix);
    matrix[0][0] = 2.0f / (ortho_right - ortho_left);
    matrix[0][3] = -((ortho_right + ortho_left) / (ortho_right - ortho_left));
    matrix[1][1] = 2.0f / (ortho_top - ortho_bottom);
    matrix[1][3] = -((ortho_top + ortho_bottom) / (ortho_top - ortho_bottom));
    matrix[2][2] = -((2.0f) / (ortho_far - ortho_near));
    matrix[2][3] = -((ortho_far + ortho_near) / (ortho_far - ortho_near));
    matrix[3][3] = 1.0f;
}

void
MultiplyMatrix4x4(float m1[][4], float m2[][4], float r[][4])
{
    r[0][0] = (m1[0][0] * m2[0][0]) + (m1[0][1] * m2[1][0]) + (m1[0][2] * m2[2][0]) + (m1[0][3] * m2[3][0]);
    r[0][1] = (m1[0][0] * m2[0][1]) + (m1[0][1] * m2[1][1]) + (m1[0][2] * m2[2][1]) + (m1[0][3] * m2[3][1]);
    r[0][2] = (m1[0][0] * m2[0][2]) + (m1[0][1] * m2[1][2]) + (m1[0][2] * m2[2][2]) + (m1[0][3] * m2[3][2]);
    r[0][3] = (m1[0][0] * m2[0][3]) + (m1[0][1] * m2[1][3]) + (m1[0][2] * m2[2][3]) + (m1[0][3] * m2[3][3]);

    r[1][0] = (m1[1][0] * m2[0][0]) + (m1[1][1] * m2[1][0]) + (m1[1][2] * m2[2][0]) + (m1[1][3] * m2[3][0]);
    r[1][1] = (m1[1][0] * m2[0][1]) + (m1[1][1] * m2[1][1]) + (m1[1][2] * m2[2][1]) + (m1[1][3] * m2[3][1]);
    r[1][2] = (m1[1][0] * m2[0][2]) + (m1[1][1] * m2[1][2]) + (m1[1][2] * m2[2][2]) + (m1[1][3] * m2[3][2]);
    r[1][3] = (m1[1][0] * m2[0][3]) + (m1[1][1] * m2[1][3]) + (m1[1][2] * m2[2][3]) + (m1[1][3] * m2[3][3]);

    r[2][0] = (m1[2][0] * m2[0][0]) + (m1[2][1] * m2[1][0]) + (m1[2][2] * m2[2][0]) + (m1[2][3] * m2[3][0]);
    r[2][1] = (m1[2][0] * m2[0][1]) + (m1[2][1] * m2[1][1]) + (m1[2][2] * m2[2][1]) + (m1[2][3] * m2[3][1]);
    r[2][2] = (m1[2][0] * m2[0][2]) + (m1[2][1] * m2[1][2]) + (m1[2][2] * m2[2][2]) + (m1[2][3] * m2[3][2]);
    r[2][3] = (m1[2][0] * m2[0][3]) + (m1[2][1] * m2[1][3]) + (m1[2][2] * m2[2][3]) + (m1[2][3] * m2[3][3]);

    r[3][0] = (m1[3][0] * m2[0][0]) + (m1[3][1] * m2[1][0]) + (m1[3][2] * m2[2][0]) + (m1[3][3] * m2[3][0]);
    r[3][1] = (m1[3][0] * m2[0][1]) + (m1[3][1] * m2[1][1]) + (m1[3][2] * m2[2][1]) + (m1[3][3] * m2[3][1]);
    r[3][2] = (m1[3][0] * m2[0][2]) + (m1[3][1] * m2[1][2]) + (m1[3][2] * m2[2][2]) + (m1[3][3] * m2[3][2]);
    r[3][3] = (m1[3][0] * m2[0][3]) + (m1[3][1] * m2[1][3]) + (m1[3][2] * m2[2][3]) + (m1[3][3] * m2[3][3]);
}

void
MakeQuaternion(float *quat, float x, float y, float z, float angle)
{
    float radians = (THETA_TO_PI(angle));
    quat[0] = x * sinf(radians / 2);
    quat[1] = y * sinf(radians / 2);
    quat[2] = z * sinf(radians / 2);
    quat[3] = cosf(radians / 2);
}

void
MakeQuaternionToMatrix(float *quat, float matrix[][4])
{
    IdentityMatrix4x4(matrix);
    matrix[0][0] = 1 - (2 * ((quat[1] * quat[1]) + (quat[2] * quat[2])));
    matrix[0][1] = 2 * ((quat[0] * quat[1]) - (quat[2] * quat[3]));
    matrix[0][2] = 2 * ((quat[0] * quat[2]) + (quat[1] * quat[3]));

    matrix[1][0] = 2 * ((quat[0] * quat[1]) + (quat[2] * quat[3]));
    matrix[1][1] = 1 - (2 * ((quat[0] * quat[0]) + (quat[2] * quat[2])));
    matrix[1][2] = 2 * ((quat[1] * quat[2]) - (quat[0] * quat[3]));

    matrix[2][0] = 2 * ((quat[0] * quat[2]) - (quat[1] * quat[3]));
    matrix[2][1] = 2 * ((quat[1] * quat[2]) + (quat[0] * quat[3]));
    matrix[2][2] = 1 - (2 * ((quat[0] * quat[0]) + (quat[1] * quat[1])));
}
