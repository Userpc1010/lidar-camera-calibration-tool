#include "icp.h"
#include "nanoflann.hpp"

#include <QDebug>
#include <cmath>

namespace icp {

// ==================== Адаптер для nanoflann ====================
struct PointCloudAdapter {
    const std::vector<Eigen::Vector3f>* points;

    inline size_t kdtree_get_point_count() const { return points->size(); }
    inline float kdtree_get_pt(const size_t idx, const size_t dim) const {
        return (*points)[idx][dim];
    }
    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }
};

using kd_tree_t = nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<float, PointCloudAdapter>,
    PointCloudAdapter,
    3>;

// ==================== Воксельная фильтрация ====================
void voxelFilter(const std::vector<Eigen::Vector3f>& points,
                 std::vector<Eigen::Vector3f>& filtered,
                 float voxel_size)
{
    std::map<std::tuple<int, int, int>, Eigen::Vector3f> voxel_map;
    std::map<std::tuple<int, int, int>, int> voxel_count;

    for (const auto& p : points) {
        int ix = static_cast<int>(std::floor(p.x() / voxel_size));
        int iy = static_cast<int>(std::floor(p.y() / voxel_size));
        int iz = static_cast<int>(std::floor(p.z() / voxel_size));

        auto key = std::make_tuple(ix, iy, iz);
        if (voxel_count.find(key) == voxel_count.end()) {
            voxel_map[key] = p;
            voxel_count[key] = 1;
        } else {
            voxel_map[key] += p;
            voxel_count[key]++;
        }
    }

    filtered.clear();
    filtered.reserve(voxel_map.size());

    for (const auto& [key, sum] : voxel_map) {
        int count = voxel_count[key];
        filtered.push_back(sum / count);
    }
}
// ==================== Вычисление оптимального преобразования ====================
void bestFitTransform(const std::vector<Eigen::Vector3f>& A,
                      const std::vector<Eigen::Vector3f>& B,
                      Eigen::Matrix3f& R,
                      Eigen::Vector3f& t)
{
    assert(A.size() == B.size());

    Eigen::Vector3f centroid_A(0,0,0), centroid_B(0,0,0);
    for (size_t i = 0; i < A.size(); ++i) {
        centroid_A += A[i];
        centroid_B += B[i];
    }
    centroid_A /= A.size();
    centroid_B /= B.size();

    Eigen::Matrix3f H = Eigen::Matrix3f::Zero();
    for (size_t i = 0; i < A.size(); ++i) {
        H += (A[i] - centroid_A) * (B[i] - centroid_B).transpose();
    }

    Eigen::JacobiSVD<Eigen::Matrix3f> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);
    Eigen::Matrix3f U = svd.matrixU();
    Eigen::Matrix3f V = svd.matrixV();

    R = V * U.transpose();

    if (R.determinant() < 0) {
        V.col(2) *= -1;
        R = V * U.transpose();
    }

    t = centroid_B - R * centroid_A;
}

// ==================== Основная функция ICP ====================
ICPResult align(const std::vector<Eigen::Vector3f>& source,
                const std::vector<Eigen::Vector3f>& target,
                int max_iterations,
                float tolerance,
                float max_distance)
{
    ICPResult result;

    if (source.size() < 3 || target.size() < 3) {
        qWarning() << "ICP: not enough points";
        return result;
    }

    // Строим kd-tree по target
    PointCloudAdapter adapter{&target};
    kd_tree_t kdtree(3, adapter, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    kdtree.buildIndex();

    // Начальное преобразование
    Eigen::Matrix3f R_total = Eigen::Matrix3f::Identity();
    Eigen::Vector3f t_total = Eigen::Vector3f::Zero();

    std::vector<Eigen::Vector3f> src = source;

    float prev_error = 0.0f;

    for (int iter = 0; iter < max_iterations; ++iter) {
        // Находим ближайшие точки
        std::vector<Eigen::Vector3f> src_matched, tgt_matched;

        for (const auto& p : src) {
            float query_pt[3] = {p.x(), p.y(), p.z()};
            size_t num_results = 1;
            size_t ret_index;
            float out_dist_sqr;

            nanoflann::KNNResultSet<float> resultSet(num_results);
            resultSet.init(&ret_index, &out_dist_sqr);
            kdtree.findNeighbors(resultSet, query_pt, nanoflann::SearchParameters(10));

            if (out_dist_sqr < max_distance * max_distance) {
                src_matched.push_back(p);
                tgt_matched.push_back(target[ret_index]);
            }
        }

        if (src_matched.size() < 3) {
            qWarning() << "ICP: not enough correspondences at iter" << iter;
            break;
        }

        // Вычисляем преобразование
        Eigen::Matrix3f R_opt;
        Eigen::Vector3f t_opt;
        bestFitTransform(src_matched, tgt_matched, R_opt, t_opt);

        // Применяем к source
        for (auto& p : src) {
            p = R_opt * p + t_opt;
        }

        // Накапливаем
        R_total = R_opt * R_total;
        t_total = R_opt * t_total + t_opt;

        // Средняя ошибка
        float mean_error = 0.0f;
        for (size_t i = 0; i < src_matched.size(); ++i) {
            mean_error += (src_matched[i] - tgt_matched[i]).norm();
        }
        mean_error /= src_matched.size();

        qDebug() << "ICP iter" << iter << "error:" << mean_error;

        if (std::abs(prev_error - mean_error) < tolerance) {
            result.converged = true;
            result.mean_error = mean_error;
            result.iterations = iter + 1;
            break;
        }

        prev_error = mean_error;
    }

    result.rotation = R_total;
    result.translation = t_total;

    return result;
}

} // namespace icp
