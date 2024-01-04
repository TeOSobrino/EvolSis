#pragma once
#include <cmath>

struct Quaternion {
    double w, x, y, z;
};

struct RotationMatrix {
    double m[3][3];
};

struct EulerAngles {
    double yaw, pitch, roll;
};

auto RodriguesToQuaternion(double rx, double ry, double rz) -> Quaternion;

auto QuaternionToRotationMatrix(const Quaternion& q) -> RotationMatrix;

auto RotationMatrixToEulerAngles(const RotationMatrix& R) -> EulerAngles;

auto convert_rodriguez_params2euler_angles(double rx, double ry, double rz) -> EulerAngles;
