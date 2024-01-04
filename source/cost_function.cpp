
#include "cost_function.hpp"

// Definindo a matriz R e Q
Eigen::Matrix<gene_t, CONTROL_DIMENSION, CONTROL_DIMENSION> R;
Eigen::Matrix<gene_t, STATE_DIMENSION, STATE_DIMENSION> Q;

gene_t cost_functional(individual& ind)
{
    // Implementação da função cost_functional2
    Eigen::Map<Eigen::Matrix<gene_t, CONTROL_DIMENSION, CONTROL_HORIZON>> u_map(ind.genes);
    Eigen::Matrix<gene_t, CONTROL_DIMENSION, Eigen::Dynamic> u_best(CONTROL_DIMENSION, PREDICTIVE_HORIZON);
    Eigen::Matrix<gene_t, STATE_DIMENSION, PREDICTIVE_HORIZON + 1> x_horiz;

    u_best.setZero();
    u_best.leftCols(CONTROL_HORIZON) = u_map;

    Quadrotor* drone = Quadrotor::get_instance();

    x_horiz.col(0) = drone->get_curr_state();
    gene_t sum = 0.0;

    for(int i = 0; i < PREDICTIVE_HORIZON; i++) {
        x_horiz.col(i + 1) = drone->control_law(x_horiz.col(i), u_best.col(i));
        sum += (x_horiz.col(i + 1).transpose() * Q * x_horiz.col(i + 1) + u_best.col(i).transpose() * R * u_best.col(i)).norm();
    }

    ind.fitness = 100000 / sum;

    return ind.fitness;
}
