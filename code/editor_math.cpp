#include "editor_math.h"

#define Assert(expression) if(!expression) {*(int *)0 = 0;}
#define global_variable static
#define local_persist   static

#include "editor_opengl.h"

void
IdentityMatrix4x4(f32 matrix[][4])
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
TranslateMatrix4x4(f32 matrix[][4], vec3 *v)
{
    IdentityMatrix4x4(matrix);
    matrix[3][0] = v->x;
    matrix[3][1] = v->y;
    matrix[3][2] = v->z;
}

void
ScaleMatrix4x4(f32 matrix[][4], f32 scale)
{
    IdentityMatrix4x4(matrix);
    matrix[0][0] = matrix[0][0] * scale;
    matrix[1][1] = matrix[1][1] * scale;
    matrix[2][2] = matrix[2][2] * scale;
}

void
ZeroMatrix4x4(f32 matrix[][4])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix[i][j] = 0;
        }
    }
}

void
MakeOrthographicMatrix(f32 ortho_right, f32 ortho_left, f32 ortho_top,
        f32 ortho_bottom, f32 ortho_far, f32 ortho_near,
        f32 matrix[][4])
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
MultiplyMatrix4x4(f32 m1[][4], f32 m2[][4], f32 r[][4])
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
MakeQuaternion(quat *q, vec3 *v, f32 angle)
{
    /*
     * v: axis
     * q: quat
     * angle: in degrees
     * */
    f32 radians = (THETA_TO_PI(angle));
    q->x = v->x * sinf(radians / 2);
    q->y = v->y * sinf(radians / 2);
    q->z = v->z * sinf(radians / 2);
    q->w = cosf(radians / 2);
}

void
MakeQuaternionToMatrix(quat* q, f32 matrix[][4])
{
    IdentityMatrix4x4(matrix);
    matrix[0][0] = 1 - (2 * ((q->y * q->y) + (q->z * q->z)));
    matrix[0][1] = 2 * ((q->x * q->y) - (q->z * q->w));
    matrix[0][2] = 2 * ((q->x * q->z) + (q->y * q->w));

    matrix[1][0] = 2 * ((q->x * q->y) + (q->z * q->w));
    matrix[1][1] = 1 - (2 * ((q->x * q->x) + (q->z * q->z)));
    matrix[1][2] = 2 * ((q->y * q->z) - (q->x * q->w));

    matrix[2][0] = 2 * ((q->x * q->z) - (q->y * q->w));
    matrix[2][1] = 2 * ((q->y * q->z) + (q->x * q->w));
    matrix[2][2] = 1 - (2 * ((q->x * q->x) + (q->y * q->y)));
}
