#pragma once
#include <string>
#include <eigen3/Eigen/Dense>
#include <fstream>

template <typename T, int L, int C>
auto write_data2csvfile(
        std::string filename,
        Eigen::Matrix<T, L, C> data,
        std::string header
) -> void {
    std::string buildDir("build/");
    std::string extension(".csv");
    std::ofstream csvfile(buildDir + filename + extension); 
    
    // Write header
    csvfile << header << std::endl;
    
    for(int i = 0; i < C; i++) {
         auto column = Eigen::Map<Eigen::Matrix<T, L, 1>>(data.col(i).data(), data.col(i).size());
         
        for(const auto& element : column) {
            csvfile << element << ", ";
        }
        
        csvfile << std::endl;
    }

    csvfile.close();
}
