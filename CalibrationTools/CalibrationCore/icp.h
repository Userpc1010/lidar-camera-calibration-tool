#ifndef ICP_H
#define ICP_H

#include <Eigen/Dense>
#include <vector>
#include <unordered_map>

namespace icp {

struct ICPResult {
    Eigen::Matrix3f rotation = Eigen::Matrix3f::Identity();
    Eigen::Vector3f translation = Eigen::Vector3f::Zero();
    float mean_error = 0.0f;
    int iterations = 0;
    bool converged = false;
};

// Воксельная фильтрация для уменьшения количества точек
void voxelFilter(const std::vector<Eigen::Vector3f>& points,
                 std::vector<Eigen::Vector3f>& filtered,
                 float voxel_size = 0.05f);

// Основная функция ICP
ICPResult align(const std::vector<Eigen::Vector3f>& source,
                const std::vector<Eigen::Vector3f>& target,
                int max_iterations = 30,
                float tolerance = 1e-6,
                float max_distance = 0.5f);

} // namespace icp

#endif // ICP_H
