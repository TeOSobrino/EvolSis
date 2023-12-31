#pragma once
#include <uavvtol.hpp>
#include <aux.hpp>
#include <random>

// Implemented as a Singleton
class Quadrotor : public UavVTOL<gene_t, STATE_DIMENSION, CONTROL_DIMENSION> {
public:
    typedef State_x_t x_t;
    typedef Control_u_t u_t;
    typedef Eigen::Matrix<gene_t, CONTROL_DIMENSION, MAX_ITERATIONS> u_matrix_t;
    typedef Eigen::Matrix<gene_t, STATE_DIMENSION, MAX_ITERATIONS + 1> x_matrix_t;

private:    
    // Record
    u_matrix_t u_history;
    x_matrix_t x_trajectory;
    
    int iteration; 

    Quadrotor() : UavVTOL(
    (Matrix_A_t() << 
    1.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0245250, 0.0000000, 0.0500000, 0.0000000, 0.0000000, 0.0000000, 0.0002044, 0.0000000,
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
    0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 1.0000000
    ).finished(),

    (Matrix_B_t() <<
    -0.0007069,  0.0007773, 0.0007091, -0.0007795,
     0.0007034,  0.0007747, -0.0007042, -0.0007739,
     0.0052554,  0.0052554, 0.0052554, 0.0052554,
    -0.1720966, -0.1895213, 0.1722891, 0.1893288,
    -0.1729419,  0.1901740, 0.1734809, -0.1907131,
     0.0123423, -0.0045148, -0.0174024, 0.0095748,
    -0.0565520,  0.0621869, 0.0567283, -0.0623632,
     0.0562756,  0.0619735, -0.0563386, -0.0619105,
     0.2102143,  0.2102143, 0.2102143, 0.2102143,
    -13.7677303,-15.1617018, 13.7831318, 15.1463003,
    -13.8353509, 15.2139209, 13.8784751, -15.2570451,
     0.9873856, -0.3611820, -1.3921880, 0.7659845 
    ).finished()) {};
    
    static Quadrotor* instance;

public:
    Quadrotor(const Quadrotor& quad) = delete;
    
    static auto initialize(State_x_t x0) -> void { 
        instance = new Quadrotor(); 

        instance->x_trajectory.col((instance->iteration = 0)) << x0;
    }

    static auto get_instance() -> Quadrotor* {
        return instance;
    }
    
    static auto free() -> void {
        delete instance;
    }

    auto get_curr_state() -> State_x_t {
        return instance->x_trajectory.col(instance->iteration); 
    }

    auto get_u_history() -> u_matrix_t {
        return instance->u_history;
    } 

    auto get_x_trajectory() -> x_matrix_t {
        return instance->x_trajectory; 
    }

    auto update_record(const Control_u_t& new_u) -> void { 
        State_x_t x_next = control_law(this->get_curr_state(), new_u);
        instance->u_history.col(instance->iteration) << new_u;
        instance->iteration++;
        instance->x_trajectory.col(instance->iteration) << x_next; 
    }
    
    /* Magnitude is the amplitude of the deviation that can be caused by the wind gust */
    /*auto wind_gust(int magnitude) -> void {
        // Adds Disturbances
        std::random_device rd;
        std::mt19937 gen(rd());

        for (int i = 0; i < STATE_DIMENSION; i++) {
            //gene_t pos_state = instance->x_trajectory.col(instance->iteration);
            std::uniform_real_distribution<> d(-magnitude, magnitude);
            instance->x_trajectory.col(instance->iteration).row(i)
                = ((gene_t)d(gen));
        }
    }*/
};
