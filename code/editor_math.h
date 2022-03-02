#ifndef EDITOR_MATH_H
#define EDITOR_MATH_H

#define MATH_IDENTITY_MAT4 {{1.0f, 0.0f, 0.0f, 0.0f},        \
                            {0.0f, 1.0f, 0.0f, 0.0f},        \
                            {0.0f, 0.0f, 1.0f, 0.0f},        \
                            {0.0f, 0.0f, 0.0f, 1.0f}}

#define MATH_ZERO_MAT4 {{0.0f, 0.0f, 0.0f, 0.0f},        \
                        {0.0f, 0.0f, 0.0f, 0.0f},        \
                        {0.0f, 0.0f, 0.0f, 0.0f},        \
                        {0.0f, 0.0f, 0.0f, 0.0f}}

#define PI 3.1415926535897932384626433832795f
#define THETA_TO_PI(angle) ((PI / 180.0f) * angle)

typedef float f32;

struct vec3 {
    f32 x;
    f32 y;
    f32 z;
};

struct quat {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

void IdentityMatrix4x4(f32 matrix[][4]);
void TranslateMatrix4x4(f32 matrix[][4], vec3 *v);
void ScaleMatrix4x4(f32 matrix[][4], f32 scale);
void ZeroMatrix4x4(f32 matrix[][4]);
void MakeOrthographicMatrix(f32 ortho_right, f32 ortho_left,
                            f32 ortho_top, f32 ortho_bottom,
                            f32 ortho_far, f32 ortho_near,
                            f32 matrix[][4]);

void MultiplyMatrix4x4(f32 matrix1[][4], f32 matrix2[][4], f32 result[][4]);
void MakeQuaternion(quat *q, vec3 *v, f32 theta);
void MakeQuaternionToMatrix(quat *q, f32 matrix[][4]);

#endif // EDITOR_MATH_H
