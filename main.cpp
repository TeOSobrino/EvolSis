#include <ga.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <cmath>

#define MAX_ITERATIONS 100
#define PREDICTIVE_HORIZON 100
#define STATE_DIMENSION 12
#define CONTROL_DIMENSION 4
#define CONTROL_HORIZON 10 
// Global Initial State
Eigen::Matrix<gene_t, CONTROL_DIMENSION, MAX_ITERATIONS> u_control;
Eigen::Matrix<gene_t, STATE_DIMENSION, MAX_ITERATIONS + 1> x_state;
int iteration;

Eigen::Matrix<gene_t, CONTROL_DIMENSION, CONTROL_DIMENSION> R;
Eigen::Matrix<gene_t, STATE_DIMENSION, STATE_DIMENSION> Q;
Eigen::Matrix<gene_t, STATE_DIMENSION, CONTROL_DIMENSION> B;
Eigen::Matrix<gene_t, STATE_DIMENSION, STATE_DIMENSION> A;

struct Quaternion {
    double w, x, y, z;
};

struct RotationMatrix {
    double m[3][3];
};

struct EulerAngles {
    double roll, pitch, yaw;
};

Quaternion RodriguesToQuaternion(double rx, double ry, double rz) {
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

RotationMatrix QuaternionToRotationMatrix(const Quaternion& q) {
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

EulerAngles RotationMatrixToEulerAngles(const RotationMatrix& R) {
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

gene_t cost_functional2(individual& ind)
{
    // Convert the C-style array to an Eigen column vector
    Eigen::Map<Eigen::Matrix<gene_t, CONTROL_DIMENSION, CONTROL_HORIZON>> u_map(ind.genes);
    Eigen::Matrix<gene_t, CONTROL_DIMENSION, Eigen::Dynamic> u_best(CONTROL_DIMENSION, PREDICTIVE_HORIZON);
    Eigen::Matrix<gene_t, STATE_DIMENSION, PREDICTIVE_HORIZON + 1> x_horiz;
    
    u_best.setZero();
    u_best.leftCols(CONTROL_HORIZON) = u_map;

    x_horiz.col(0) = x_state.col(iteration); 
    gene_t sum = 0.0;

    for(int i = 0; i < PREDICTIVE_HORIZON; i++) {
        x_horiz.col(i + 1) = A * x_horiz.col(i) + B * u_best.col(i);
        sum += (x_horiz.col(i).transpose() * Q * x_horiz.col(i) + u_best.col(i).transpose() * R * u_best.col(i)).norm(); 
    }    
    ind.fitness = 100000 / sum; 

   // Possibility to add constraints
   // TO DO...

    return ind.fitness;
}

gene_t hill_fnt(individual& ind)
{
    gene_t val = ind.genes[0];
    return (val > 10 ? 20 - val : val);
}

// obs: para usar a parabóla verifique se MAX^2 <= RAND_MAX -> (MAX << 2^16)
gene_t parabola(individual& ind)
{
    gene_t val = ind.genes[0];
    gene_t y = -val * val + 12 * val + 8;
    ind.fitness = y;

    return y;
}

gene_t senoid(individual& ind)
{

    gene_t x = ind.genes[0];
    gene_t y = (2 * cos(0.039 * x) + 5 * sin(0.05 * x) + 0.5 * cos(0.01 * x) +
                10 * sin(0.07 * x) + 5 * sin(0.1 * x) + 5 * sin(0.035 * x)) *
                   10 +
               500;
    ind.fitness = y;

    return y;
}

gene_t multi_dim(individual& ind)
{
    gene_t x = ind.genes[0];
    gene_t y = ind.genes[1];

    gene_t z = 5 - 2 * (x * x + 5 * y * y);

    ind.fitness = z;
    return z;
}

gene_t wtf(individual& ind)
{
    gene_t x = ind.genes[0];
    gene_t y = ind.genes[1];
    gene_t z = ind.genes[2];

    gene_t k = -5 * x * x - 10 * y * y - 5 * z * z + 10 + 4 * x * y -
               5 * x * z + 10 * x + 4 * z - 5 * x * x * x * x -
               3 * x * x * x * x * z * z * z * z;
    ind.fitness = k;
    
    return k;
}

int main(int argc, char *argv[])
{
    if( argc < 2 ) {
        std::cout << "Insuficient number of arguments!\n";
    }
    std::cout << argv[1] << std::endl;
    // eval_ptr evalued_fnt = &wtf;
    eval_ptr evalued_fnt = &cost_functional2;
    
    A << 1.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0245250, 0.0000000, 0.0500000, 0.0000000, 0.0000000, 0.0000000, 0.0002044, 0.0000000,
    0.0000000, 1.0000000, 0.0000000, -0.0245250, 0.0000000, 0.0000000, 0.0000000, 0.0500000, 0.0000000, -0.0002044, 0.0000000, 0.0000000,
    0.0000000, 0.0000000, 1.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0500000, 0.0000000, 0.0000000, 0.0000000,
    0.0000000, 0.0000000, 0.0000000, 1.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0250000, 0.0000000, 0.0000000,
    0.0000000, 0.0000000, 0.0000000, 0.0000000, 1.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0250000, 0.0000000,
    0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 1.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0250000,
    0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.9810000, 0.0000000, 1.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0122625, 0.0000000,
    0.0000000, 0.0000000, 0.0000000, -0.9810000, 0.0000000, 0.0000000, 0.0000000, 1.0000000, 0.0000000, -0.0122625, 0.0000000, 0.0000000,
    0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 1.0000000, 0.0000000, 0.0000000, 0.0000000,
    0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 1.0000000, 0.0000000, 0.0000000,
    0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 1.0000000, 0.0000000,
    0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 1.0000000;


    B <<
     -0.0007069, 0.0007773, 0.0007091, -0.0007795,
    0.0007034, 0.0007747, -0.0007042, -0.0007739,
    0.0052554, 0.0052554, 0.0052554, 0.0052554,
    -0.1720966, -0.1895213, 0.1722891, 0.1893288,
    -0.1729419, 0.1901740, 0.1734809, -0.1907131,
    0.0123423, -0.0045148, -0.0174024, 0.0095748,
    -0.0565520, 0.0621869, 0.0567283, -0.0623632,
    0.0562756, 0.0619735, -0.0563386, -0.0619105,
    0.2102143, 0.2102143, 0.2102143, 0.2102143,
    -13.7677303, -15.1617018, 13.7831318, 15.1463003,
    -13.8353509, 15.2139209, 13.8784751, -15.2570451,
    0.9873856, -0.3611820, -1.3921880, 0.7659845;


   Q.diagonal() << 100.0000000, 100.0000000, 100.0000000, 4.0000000, 4.0000000, 400.0000000, 4.0000000, 4.0000000, 4.0000000, 2.0408163, 2.0408163, 4.0000000;
   //Q.diagonal() << 10.0000000, 10.0000000, 10.0000000, 1000.0000000, 1000.0000000, 1000.0000000, 100.0000000, 100.0000000, 100.0000000, 10000.0000000, 10000.0000000, 10000.0000000;

    R.diagonal() << 4.0000000, 4.0000000, 4.0000000, 4.0000000;
   //R.diagonal() << 1.0000000, 1.0000000, 1.0000000, 1.0000000;



    x_state.col((iteration = 0)) << 0, 1, 0, 0.2, 0, 0, 0.1, 0, 0, 0, 0, 0;
    //x_state.col((iteration = 0)) << 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0;

    individual* best_s = interface("b", "c", evalued_fnt);
    for (int i = 0; i < MAX_ITERATIONS; i++) { 
        for(int j = 0; j < CONTROL_DIMENSION; j++) {
            if(i < CONTROL_HORIZON) {
                u_control(j, i) = best_s->genes[i*(CONTROL_DIMENSION) + j];
            } else {
                u_control(j, i) = 0;
            }
        }
        x_state.col(iteration + 1) = A * x_state.col(iteration) + B * u_control.col(iteration);
        std::cout << "next x: " << x_state.col(iteration + 1) << std::endl; 
        //printf("best fit: %lf3f, best_sol = ", evalued_fnt(*best_s));
        //individual_print(*best_s);         
        iteration++;
    }
    free(best_s);
    //std::cout << A << std::endl << B << std::endl << u_control << std::endl;
   /*for(iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        // Call the AG with the current state
        //individual* best_s = interface("b", "c", evalued_fnt);
        std::cout << "ITERATION " << iteration << " FROM " << MAX_ITERATIONS << std::endl; 
        // Updates the best control signal
        for (int i = 0; i < CONTROL_DIMENSION; i++) {
           // std::cout << "OHAIOU" << std::endl;
            //std::cout << "EUU " << best_s->genes[i] << std::endl;
            //std::cout << "i = " << i << "j = " << iteration << std::endl;
            u_control(i, iteration) = best_s->genes[i];
        }
        
        // Calculates the next state with the optimal control signal
        x_state.col(iteration + 1) = A * x_state.col(iteration) + B * u_control.col(iteration);
        std::cout << "next x: " << x_state.col(iteration + 1) << std::endl; 
        //printf("best fit: %lf3f, best_sol = ", evalued_fnt(*best_s));
        //individual_print(*best_s);
        
        free(best_s);
    }*/
    std::cout << std::endl << x_state << std::endl;
    std::cout << std::endl << u_control << std::endl;

     // Write state data to a CSV file
    std::ofstream stateFile("state.csv");
    //stateFile << "x,y,z,phi,theta,psi,dx,dy,dz,dphi,dtheta,dpsi\n"; // Header
    stateFile << "x,y,z,yaw,pitch,roll,dx,dy,dz,dyaw,dpitch,droll\n"; // Header 
    for(int i = 0; i < MAX_ITERATIONS ; i++) {
        
    // Convert Rodrigues vector to quaternion
    Quaternion q = RodriguesToQuaternion(x_state.col(i).row(3).norm(), x_state.col(i).row(4).norm(), x_state.col(i).row(5).norm());
    Quaternion dq = RodriguesToQuaternion(x_state.col(i).row(9).norm(), x_state.col(i).row(10).norm(), x_state.col(i).row(11).norm());

    // Convert quaternion to rotation matrix
    RotationMatrix R = QuaternionToRotationMatrix(q);
    RotationMatrix dR = QuaternionToRotationMatrix(dq);

    // Convert rotation matrix to Euler angles (roll, pitch, yaw)
    EulerAngles eulerAngles = RotationMatrixToEulerAngles(R);
    EulerAngles deulerAngles = RotationMatrixToEulerAngles(dR);

    double roll_deg = eulerAngles.roll * 180.0 / M_PI;
    double pitch_deg = eulerAngles.pitch * 180.0 / M_PI;
    double yaw_deg = eulerAngles.yaw * 180.0 / M_PI;
    
    double droll_deg = deulerAngles.roll * 180.0 / M_PI;
    double dpitch_deg = deulerAngles.pitch * 180.0 / M_PI;
    double dyaw_deg = deulerAngles.yaw * 180.0 / M_PI;



        stateFile << x_state.col(i).row(0) << "," << x_state.col(i).row(1) << "," << x_state.col(i).row(2) << ",";
        //stateFile << x_state.col(i).row(3) << "," << x_state.col(i).row(4) << "," << x_state.col(i).row(5) << ",";
        stateFile << yaw_deg << "," << pitch_deg << "," << roll_deg << ",";
        stateFile << x_state.col(i).row(6) << "," << x_state.col(i).row(7) << "," << x_state.col(i).row(8) << ",";
        //stateFile << x_state.col(i).row(9) << "," << x_state.col(i).row(10) << "," << x_state.col(i).row(11) << "\n";
        stateFile << dyaw_deg << "," << dpitch_deg << "," << droll_deg << "\n";

    
    std::cout << "Itr " << i + 1 << ", Roll: " << roll_deg << ", Pitch: " << pitch_deg << ", Yaw: " << yaw_deg << std::endl;
    }
    stateFile.close();
 
    // Write control data to a CSV file
    std::ofstream controlFile("control.csv");
    controlFile << "u1,u2,u3,u4\n"; // Header
    for(int i = 0; i < MAX_ITERATIONS ; i++) {
        controlFile << u_control.col(i).row(0) << "," << u_control.col(i).row(1) << "," << u_control.col(i).row(2) << "," << u_control.col(i).row(3) << "\n";
    }
    controlFile.close();
    //individual* best_s = interface("b", "c", evalued_fnt);

    //printf("best fit: %lf3f, best_sol = ", evalued_fnt(*best_s));
    //individual_print(*best_s);
    //printf("\n");

    //free(best_s);

    /*std::string s = std::string("python3 ") + std::string(argv[1]);
    s += std::string(" ");
    s += std::to_string(ISLAND_NUM);
    if(system(s.c_str()))
        std::cout << "error printing" << "\n";*/
    
    return 0;
}
