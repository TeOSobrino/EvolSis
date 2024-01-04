
#ifndef COST_FUNCTION_HPP
#define COST_FUNCTION_HPP

#include "ga.hpp"
#include "rodriguez.hpp"
#include "quadrotor.hpp"
#include <eigen3/Eigen/Dense>

// Definindo a matriz R e Q
extern Eigen::Matrix<gene_t, CONTROL_DIMENSION, CONTROL_DIMENSION> R;
extern Eigen::Matrix<gene_t, STATE_DIMENSION, STATE_DIMENSION> Q;

gene_t cost_functional(individual& ind);

#endif // COST_FUNCTION_HPP
