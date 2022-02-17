#ifndef EDITOR_H
#define EDITOR_H

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

void IdentityMatrix4x4(float matrix[][4]);
void ScaleMatrix4x4(float matrix[][4]);
void ZeroMatrix4x4(float matrix[][4]);
void MakeOrthographicMatrix(float ortho_right, float ortho_left,
                            float ortho_top, float ortho_bottom,
                            float ortho_far, float ortho_near,
                            float matrix[][4]);

void MultiplyMatrix4x4(float matrix1[][4], float matrix2[][4], float result[][4]);
void MakeQuaternion(float *quat, float angle);
void MakeQuaternionToMatrix(float *quat, float matrix[][4]);

#endif // EDITOR_H
