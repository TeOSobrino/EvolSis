#include <rodriguez.hpp>

auto RodriguesToQuaternion(double rx, double ry, double rz) -> Quaternion {
    double r_norm = std::sqrt(rx * rx + ry * ry + rz * rz);
    // Handle zero rotation separately to avoid division by zero
    if (r_norm < 1e-6) {
        return {1.0, 0.0, 0.0, 0.0};
    }
    double theta = 2 * std::atan(r_norm);

    double sin_half_theta = std::sin(theta / 2.0);
    double cos_half_theta = std::cos(theta / 2.0);

    double ux = rx / r_norm;
    double uy = ry / r_norm;
    double uz = rz / r_norm;

    Quaternion q;
    q.w = cos_half_theta;
    q.x = ux * sin_half_theta;
    q.y = uy * sin_half_theta;
    q.z = uz * sin_half_theta;

    return q;
}

auto QuaternionToRotationMatrix(const Quaternion& q) -> RotationMatrix {
    RotationMatrix R;
    R.m[0][0] = 1 - 2 * q.y * q.y - 2 * q.z * q.z;
    R.m[0][1] = 2 * q.x * q.y - 2 * q.z * q.w;
    R.m[0][2] = 2 * q.x * q.z + 2 * q.y * q.w;
    R.m[1][0] = 2 * q.x * q.y + 2 * q.z * q.w;
    R.m[1][1] = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
    R.m[1][2] = 2 * q.y * q.z - 2 * q.x * q.w;
    R.m[2][0] = 2 * q.x * q.z - 2 * q.y * q.w;
    R.m[2][1] = 2 * q.y * q.z + 2 * q.x * q.w;
    R.m[2][2] = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
    
    return R;
}

auto RotationMatrixToEulerAngles(const RotationMatrix& R) -> EulerAngles {
    EulerAngles angles;

    // Assuming the order of rotations is around z (yaw), y (pitch), and x (roll)
    double sy = std::sqrt(R.m[0][0] * R.m[0][0] + R.m[1][0] * R.m[1][0]);

    bool singular = sy < 1e-6; // If cos(y) (sy) is close to zero, use singularities
    if (!singular) {
        angles.roll = std::atan2(R.m[2][1], R.m[2][2]);
        angles.pitch = std::atan2(-R.m[2][0], sy);
        angles.yaw = std::atan2(R.m[1][0], R.m[0][0]);
    } else {
        angles.roll = std::atan2(-R.m[1][2], R.m[1][1]);
        angles.pitch = std::atan2(-R.m[2][0], sy);
        angles.yaw = 0;
    }
    
    return angles;
}

auto convert_rodriguez_params2euler_angles(double rx, double ry, double rz) -> EulerAngles {
    // Convert Rodrigues vector to quaternion
    Quaternion q = RodriguesToQuaternion(rx, ry, rz);
    // Convert quaternion to rotation matrix
    RotationMatrix R = QuaternionToRotationMatrix(q);
    // Convert rotation matrix to Euler angles (yaw, pitch, roll)
    return RotationMatrixToEulerAngles(R); 
}
