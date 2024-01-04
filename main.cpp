#include <ga.hpp>
#include "cost_function.hpp"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <fstream> 

Quadrotor* Quadrotor::instance = NULL; 

int main(int argc, char *argv[])
{
    // eval_ptr evalued_fnt = &wtf;
    eval_ptr evalued_fnt = &cost_functional;
     
   Q.diagonal() << 100.0000000, 100.0000000, 100.0000000, 4.0000000, 4.0000000, 400.0000000, 4.0000000, 4.0000000, 4.0000000, 2.0408163, 2.0408163, 4.0000000;
   //Q.diagonal() << 10.0000000, 10.0000000, 10.0000000, 1000.0000000, 1000.0000000, 1000.0000000, 100.0000000, 100.0000000, 100.0000000, 10000.0000000, 10000.0000000, 10000.0000000;

    R.diagonal() << 4.0000000, 4.0000000, 4.0000000, 4.0000000;
   //R.diagonal() << 1.0000000, 1.0000000, 1.0000000, 1.0000000;
 
    Quadrotor::initialize((Quadrotor::x_t() << 0, 1, 0, 0.2, 0, 0, 0.1, 0, 0, 0, 0, 0).finished());
    
    Quadrotor* drone = Quadrotor::get_instance();
    individual* best_s = interface("b", "c", evalued_fnt);
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        Quadrotor::u_t new_control_signal;
        for(int j = 0; j < CONTROL_DIMENSION; j++) {
            if(i < CONTROL_HORIZON) {
                //u_control(j, i) = best_s->genes[i*(CONTROL_DIMENSION) + j];
                new_control_signal.row(j) << best_s->genes[i*(CONTROL_DIMENSION) + j];
            } else {
                //u_control(j, i) = 0;
                new_control_signal.row(j) << 0;
            }
        } 
        drone->update_record(new_control_signal);
        std::cout << "next x: " << drone->get_curr_state() << std::endl;  
    }
    free(best_s);

    Quadrotor::x_matrix_t x_state = drone->get_x_trajectory();
    Quadrotor::u_matrix_t u_control = drone->get_u_history();

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
    std::ofstream stateFile("build/state.csv");
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
    std::ofstream controlFile("build/control.csv");
    controlFile << "u1,u2,u3,u4\n"; // Header
    for(int i = 0; i < MAX_ITERATIONS ; i++) {
        controlFile << u_control.col(i).row(0) << "," << u_control.col(i).row(1) << "," << u_control.col(i).row(2) << "," << u_control.col(i).row(3) << "\n";
    }
    controlFile.close();
    
    Quadrotor::free();
    return 0;
}
