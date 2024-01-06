#pragma once
#include <eigen3/Eigen/Dense>

template <typename T, int STATE_DIM, int CONTROL_DIM>
class UavVTOL {
public:
    typedef Eigen::Matrix<T, STATE_DIM, CONTROL_DIM> Matrix_B_t;
    typedef Eigen::Matrix<T, STATE_DIM, STATE_DIM> Matrix_A_t;
    typedef Eigen::Matrix<T, STATE_DIM, 1> State_x_t;
    typedef Eigen::Matrix<T, CONTROL_DIM, 1> Control_u_t;

    // Attributes of the Dynamic System in State Space
    Matrix_A_t A;
    Matrix_B_t B;
    
    UavVTOL() = default;

    UavVTOL(const Matrix_A_t& A, const Matrix_B_t& B) 
        : A{ A }, B{ B } {};

    auto control_law(const State_x_t& x, const Control_u_t& u) -> State_x_t {
        return A * x + B * u; 
    } 
};
