#include "rgbd_camera.h"

#include <iostream>
#include <cstring>
#include <fstream>
#include <random>
#include <nanoflann.hpp>
#include <registration/registration_helper.hpp>
#include "icp.h"

// Статический указатель на текущий экземпляр (используется в static callback)
static Processor* g_processor = nullptr;

Processor::Processor(const std::string& config_path, QObject *parent)
    : QThread(parent), config_path_(config_path)
{
    widget = new OGLWidget();

    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfw_win = glfwCreateWindow(1, 1, "GL context", nullptr, nullptr);
    if (!glfw_win) {
        throw std::runtime_error("Could not create OpenGL window");
    }
    glfwMakeContextCurrent(glfw_win);
    glfwSetWindowUserPointer(glfw_win, this);

    rs2::gl::init_processing(use_gpu_processing);
    config.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 5);

    connect(this, &Processor::DisplayCameraPointCloud, widget, &OGLWidget::DisplayCameraPointCloud);
    connect(this, &Processor::DisplayLidarPointCloud, widget, &OGLWidget::DisplayLidarPointCloud);

    g_processor = this;
}

Processor::~Processor()
{
    LivoxLidarSdkUninit();
    printf("Livox End!\n");

    if (buffers_allocated) {
        delete[] camera_vertices;
        delete[] camera_colors;
        delete[] lidar_vertices;
        delete[] lidar_colors;
    }

    if (glfw_win) {
        glfwDestroyWindow(glfw_win);
        glfwTerminate();
    }

    g_processor = nullptr;
    pipe.stop();
    widget->deleteLater();
}

bool Processor::initializeLidar()
{
    if (lidar_initialized) return true;

    if (!LivoxLidarSdkInit(config_path_.c_str()))  {
        qWarning() << "Livox Init Failed";
        return false;
    }

    SetLivoxLidarPointCloudCallBack(lidarPointCloudCallback, nullptr);
    lidar_initialized = true;
    qDebug() << "Livox initialized";
    return true;
}

bool Processor::initializeCamera()
{
    if (camera_initialized) return true;

    try {
        pipe.start(config, [this](const rs2::frame& frame) {
            if (rs2::frameset fs = frame.as<rs2::frameset>()) {
                std::lock_guard<std::mutex> lock(data_mutex);
                depth_frame = fs.get_depth_frame();
                has_camera_frame = true;
            }
        });

        // Включаем ИК-проектор
        auto depth_sensor = pipe.get_active_profile().get_device().first<rs2::depth_sensor>();
        if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED)) {
            depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 1.0f);
            qDebug() << "IR emitter enabled";
        }
        if (depth_sensor.supports(RS2_OPTION_LASER_POWER)) {
            float max_power = depth_sensor.get_option_range(RS2_OPTION_LASER_POWER).max;
            depth_sensor.set_option(RS2_OPTION_LASER_POWER, max_power);
            qDebug() << "Laser power set to max:" << max_power;
        }

        camera_initialized = true;
        qDebug() << "Camera initialized";
        return true;
    } catch (const rs2::error& e) {
        qWarning() << "Camera init failed:" << e.what();
        return false;
    }
}

void Processor::shutdownLidar()
{
    if (lidar_initialized) {
        LivoxLidarSdkUninit();
        lidar_initialized = false;
        qDebug() << "Livox shutdown";
    }
}

void Processor::shutdownCamera()
{
    if (camera_initialized) {
        pipe.stop();
        camera_initialized = false;
        qDebug() << "Camera shutdown";
    }
}

void Processor::update_translation_rotation(const CalibrationParams& params)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    m_calibParams = params;
}

void Processor::updateCalibrationParams(const CalibrationParams &params)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    m_calibParams = params;
}

void Processor::setCropParams(const float *lidar_min, const float *lidar_max, const float *camera_min, const float *camera_max)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    for (int i = 0; i < 3; ++i) {
        lidar_crop_min[i] = lidar_min[i];
        lidar_crop_max[i] = lidar_max[i];
        camera_crop_min[i] = camera_min[i];
        camera_crop_max[i] = camera_max[i];
    }

    // Сразу применяем к загруженным облакам
    applyCalibrationToCaptured();
}

void Processor::applyCalibrationToCaptured()
{
    if (!has_captured_data) return;

    std::vector<Eigen::Vector3f> transformed_camera;
    std::vector<Eigen::Vector3f> cropped_lidar;

    Eigen::Matrix3f R;
    R << m_calibParams.rotation[0], m_calibParams.rotation[1], m_calibParams.rotation[2],
         m_calibParams.rotation[3], m_calibParams.rotation[4], m_calibParams.rotation[5],
         m_calibParams.rotation[6], m_calibParams.rotation[7], m_calibParams.rotation[8];
    Eigen::Vector3f t(m_calibParams.translation[0], m_calibParams.translation[1], m_calibParams.translation[2]);

    // Отсечение камеры + трансформация (без изменения знаков)
    for (const auto& p : captured_camera) {
        Eigen::Vector3f tp = R * p + t;
        if (tp.x() >= camera_crop_min[0] && tp.x() <= camera_crop_max[0] &&
            tp.y() >= camera_crop_min[1] && tp.y() <= camera_crop_max[1] &&
            tp.z() >= camera_crop_min[2] && tp.z() <= camera_crop_max[2]) {
            transformed_camera.push_back(tp);
        }
    }

    // Отсечение лидара (без изменения знаков)
    for (const auto& p : captured_lidar) {
        if (p.x() >= lidar_crop_min[0] && p.x() <= lidar_crop_max[0] &&
            p.y() >= lidar_crop_min[1] && p.y() <= lidar_crop_max[1] &&
            p.z() >= lidar_crop_min[2] && p.z() <= lidar_crop_max[2]) {
            cropped_lidar.push_back(p);
        }
    }

    emit DisplayCapturedClouds(cropped_lidar, transformed_camera);
}

//void Processor::autoCalibrate()
//{
//    std::lock_guard<std::mutex> lock(data_mutex);

//    if (!has_captured_data) {
//        qWarning() << "No captured data. Load or capture clouds first.";
//        return;
//    }

//    qDebug() << "========== ICP CALIBRATION START ==========";
//    qDebug() << "Current rotation matrix:";
//    qDebug() << "  [" << m_calibParams.rotation[0] << m_calibParams.rotation[1] << m_calibParams.rotation[2] << "]";
//    qDebug() << "  [" << m_calibParams.rotation[3] << m_calibParams.rotation[4] << m_calibParams.rotation[5] << "]";
//    qDebug() << "  [" << m_calibParams.rotation[6] << m_calibParams.rotation[7] << m_calibParams.rotation[8] << "]";
//    qDebug() << "Current translation: [" << m_calibParams.translation[0] << m_calibParams.translation[1] << m_calibParams.translation[2] << "]";

//    // Текущее приближение
//    Eigen::Matrix3f R_current;
//    R_current << m_calibParams.rotation[0], m_calibParams.rotation[1], m_calibParams.rotation[2],
//                 m_calibParams.rotation[3], m_calibParams.rotation[4], m_calibParams.rotation[5],
//                 m_calibParams.rotation[6], m_calibParams.rotation[7], m_calibParams.rotation[8];
//    Eigen::Vector3f t_current(m_calibParams.translation[0], m_calibParams.translation[1], m_calibParams.translation[2]);

//    // Отсечение и трансформация камеры (с правильными СК)
//    std::vector<Eigen::Vector3f> camera_cropped;
//    camera_cropped.reserve(captured_camera.size());

//    for (const auto& p : captured_camera) {
//        Eigen::Vector3f tp = R_current * p + t_current;

//        if (tp.x() >= camera_crop_min[0] && tp.x() <= camera_crop_max[0] &&
//            tp.y() >= camera_crop_min[1] && tp.y() <= camera_crop_max[1] &&
//            tp.z() >= camera_crop_min[2] && tp.z() <= camera_crop_max[2]) {

//            // Правильная СК: X_лидар = Z_камера, Y_лидар = -X_камера, Z_лидар = -Y_камера
//            camera_cropped.emplace_back(tp.z(), -tp.x(), -tp.y());
//        }
//    }
//    qDebug() << "Camera points after crop:" << camera_cropped.size();

//    // Отсечение лидара
//    std::vector<Eigen::Vector3f> lidar_cropped;
//    lidar_cropped.reserve(captured_lidar.size());

//    for (const auto& p : captured_lidar) {
//        if (p.x() >= lidar_crop_min[0] && p.x() <= lidar_crop_max[0] &&
//            p.y() >= lidar_crop_min[1] && p.y() <= lidar_crop_max[1] &&
//            p.z() >= lidar_crop_min[2] && p.z() <= lidar_crop_max[2]) {

//            lidar_cropped.push_back(p);
//        }
//    }
//    qDebug() << "Lidar points after crop:" << lidar_cropped.size();

//    // Воксельная фильтрация (мелкий воксель)
//    std::vector<Eigen::Vector3f> lidar_filtered;
//    icp::voxelFilter(lidar_cropped, lidar_filtered, 0.001f);  // 1 мм
//    qDebug() << "Lidar after voxel:" << lidar_filtered.size();

//    std::vector<Eigen::Vector3f> camera_filtered;
//    icp::voxelFilter(camera_cropped, camera_filtered, 0.001f);  // 1 мм
//    qDebug() << "Camera after voxel:" << camera_filtered.size();

//    // Запуск ICP
//    icp::ICPResult result = icp::align(camera_filtered, lidar_filtered, 100, 1e-10, 0.03f);

//    qDebug() << "ICP result:";
//    qDebug() << "  Converged:" << result.converged;
//    qDebug() << "  Iterations:" << result.iterations;
//    qDebug() << "  Mean error:" << result.mean_error;
//    qDebug() << "  Rotation delta:";
//    qDebug() << "    [" << result.rotation(0,0) << result.rotation(0,1) << result.rotation(0,2) << "]";
//    qDebug() << "    [" << result.rotation(1,0) << result.rotation(1,1) << result.rotation(1,2) << "]";
//    qDebug() << "    [" << result.rotation(2,0) << result.rotation(2,1) << result.rotation(2,2) << "]";
//    qDebug() << "  Translation delta: [" << result.translation.x() << result.translation.y() << result.translation.z() << "]";

//    float rotation_angle = acos((result.rotation.trace() - 1.0f) / 2.0f) * 180.0f / M_PI;
//    qDebug() << "  Rotation angle:" << rotation_angle << "deg";

//    // Итоговая калибровка
//    Eigen::Matrix3f R_final = result.rotation * R_current;
//    Eigen::Vector3f t_final = result.rotation * t_current + result.translation;

//    m_calibParams.rotation[0] = R_final(0,0); m_calibParams.rotation[1] = R_final(0,1); m_calibParams.rotation[2] = R_final(0,2);
//    m_calibParams.rotation[3] = R_final(1,0); m_calibParams.rotation[4] = R_final(1,1); m_calibParams.rotation[5] = R_final(1,2);
//    m_calibParams.rotation[6] = R_final(2,0); m_calibParams.rotation[7] = R_final(2,1); m_calibParams.rotation[8] = R_final(2,2);

//    m_calibParams.translation[0] = t_final.x();
//    m_calibParams.translation[1] = t_final.y();
//    m_calibParams.translation[2] = t_final.z();

//    qDebug() << "Final rotation matrix:";
//    qDebug() << "  [" << m_calibParams.rotation[0] << m_calibParams.rotation[1] << m_calibParams.rotation[2] << "]";
//    qDebug() << "  [" << m_calibParams.rotation[3] << m_calibParams.rotation[4] << m_calibParams.rotation[5] << "]";
//    qDebug() << "  [" << m_calibParams.rotation[6] << m_calibParams.rotation[7] << m_calibParams.rotation[8] << "]";
//    qDebug() << "Final translation: [" << m_calibParams.translation[0] << m_calibParams.translation[1] << m_calibParams.translation[2] << "]";
//    qDebug() << "========== ICP CALIBRATION END ==========";

//    applyCalibrationToCaptured();
//    emit calibrationUpdated(m_calibParams);
//}

void Processor::autoCalibrate()
{
    std::lock_guard<std::mutex> lock(data_mutex);

    if (!has_captured_data) {
        qWarning() << "No captured data. Load or capture clouds first.";
        return;
    }

    qDebug() << "========== GICP CALIBRATION START ==========";
    qDebug() << "Current rotation matrix:";
    qDebug() << "  [" << m_calibParams.rotation[0] << m_calibParams.rotation[1] << m_calibParams.rotation[2] << "]";
    qDebug() << "  [" << m_calibParams.rotation[3] << m_calibParams.rotation[4] << m_calibParams.rotation[5] << "]";
    qDebug() << "  [" << m_calibParams.rotation[6] << m_calibParams.rotation[7] << m_calibParams.rotation[8] << "]";
    qDebug() << "Current translation: [" << m_calibParams.translation[0] << m_calibParams.translation[1] << m_calibParams.translation[2] << "]";

    // Текущее приближение
    Eigen::Matrix3f R_current;
    R_current << m_calibParams.rotation[0], m_calibParams.rotation[1], m_calibParams.rotation[2],
                 m_calibParams.rotation[3], m_calibParams.rotation[4], m_calibParams.rotation[5],
                 m_calibParams.rotation[6], m_calibParams.rotation[7], m_calibParams.rotation[8];
    Eigen::Vector3f t_current(m_calibParams.translation[0], m_calibParams.translation[1], m_calibParams.translation[2]);

    // Отсечение и трансформация камеры
    std::vector<Eigen::Vector4f> camera_cropped;
    camera_cropped.reserve(captured_camera.size());

    for (const auto& p : captured_camera) {
        Eigen::Vector3f tp = R_current * p + t_current;

        if (tp.x() >= camera_crop_min[0] && tp.x() <= camera_crop_max[0] &&
            tp.y() >= camera_crop_min[1] && tp.y() <= camera_crop_max[1] &&
            tp.z() >= camera_crop_min[2] && tp.z() <= camera_crop_max[2]) {

            camera_cropped.emplace_back(tp.z(), -tp.x(), -tp.y(), 1.0f);
        }
    }
    qDebug() << "Camera points after crop:" << camera_cropped.size();

    // Отсечение лидара
    std::vector<Eigen::Vector4f> lidar_cropped;
    lidar_cropped.reserve(captured_lidar.size());

    for (const auto& p : captured_lidar) {
        if (p.x() >= lidar_crop_min[0] && p.x() <= lidar_crop_max[0] &&
            p.y() >= lidar_crop_min[1] && p.y() <= lidar_crop_max[1] &&
            p.z() >= lidar_crop_min[2] && p.z() <= lidar_crop_max[2]) {

            lidar_cropped.emplace_back(p.x(), p.y(), p.z(), 1.0f);
        }
    }
    qDebug() << "Lidar points after crop:" << lidar_cropped.size();

    // Настройка GICP
    small_gicp::RegistrationSetting setting;
    setting.type = small_gicp::RegistrationSetting::GICP;
    setting.downsampling_resolution = 0.01;
    setting.max_correspondence_distance = 0.05;
    setting.max_iterations = 50;
    setting.num_threads = 4;

    // Запуск GICP
    Eigen::Isometry3d init_T = Eigen::Isometry3d::Identity();
    small_gicp::RegistrationResult result = small_gicp::align(
        lidar_cropped, camera_cropped, init_T, setting);

    qDebug() << "GICP result:";
    qDebug() << "  Converged:" << result.converged;
    qDebug() << "  Iterations:" << result.iterations;
    qDebug() << "  Error:" << result.error;
    qDebug() << "  Num inliers:" << result.num_inliers;
    qDebug() << "  Rotation delta:";
    qDebug() << "    [" << result.T_target_source(0,0) << result.T_target_source(0,1)
             << result.T_target_source(0,2) << result.T_target_source(0,3) << "]";
    qDebug() << "    [" << result.T_target_source(1,0) << result.T_target_source(1,1)
             << result.T_target_source(1,2) << result.T_target_source(1,3) << "]";
    qDebug() << "    [" << result.T_target_source(2,0) << result.T_target_source(2,1)
             << result.T_target_source(2,2) << result.T_target_source(2,3) << "]";

    Eigen::Matrix3f R_icp = result.T_target_source.linear().cast<float>();
    Eigen::Vector3f t_icp = result.T_target_source.translation().cast<float>();

    float rotation_angle = acos((R_icp.trace() - 1.0f) / 2.0f) * 180.0f / M_PI;
    qDebug() << "  Rotation angle:" << rotation_angle << "deg";

    // БЕЗ ОГРАНИЧЕНИЙ — всегда применяем результат
    Eigen::Matrix3f R_final = R_icp * R_current;
    Eigen::Vector3f t_final = R_icp * t_current + t_icp;

    m_calibParams.rotation[0] = R_final(0,0); m_calibParams.rotation[1] = R_final(0,1); m_calibParams.rotation[2] = R_final(0,2);
    m_calibParams.rotation[3] = R_final(1,0); m_calibParams.rotation[4] = R_final(1,1); m_calibParams.rotation[5] = R_final(1,2);
    m_calibParams.rotation[6] = R_final(2,0); m_calibParams.rotation[7] = R_final(2,1); m_calibParams.rotation[8] = R_final(2,2);

    m_calibParams.translation[0] = t_final.x();
    m_calibParams.translation[1] = t_final.y();
    m_calibParams.translation[2] = t_final.z();

    qDebug() << "Final rotation matrix:";
    qDebug() << "  [" << m_calibParams.rotation[0] << m_calibParams.rotation[1] << m_calibParams.rotation[2] << "]";
    qDebug() << "  [" << m_calibParams.rotation[3] << m_calibParams.rotation[4] << m_calibParams.rotation[5] << "]";
    qDebug() << "  [" << m_calibParams.rotation[6] << m_calibParams.rotation[7] << m_calibParams.rotation[8] << "]";
    qDebug() << "Final translation: [" << m_calibParams.translation[0] << m_calibParams.translation[1] << m_calibParams.translation[2] << "]";
    qDebug() << "========== GICP CALIBRATION END ==========";

    applyCalibrationToCaptured();
    emit calibrationUpdated(m_calibParams);
}

//void Processor::autoCalibrate()
//{
//    std::lock_guard<std::mutex> lock(data_mutex);

//    if (!has_captured_data) {
//        qWarning() << "No captured data. Load or capture clouds first.";
//        return;
//    }

//    qDebug() << "========== GICP CALIBRATION (BALANCED) START ==========";
//    qDebug() << "Current rotation matrix:";
//    qDebug() << "  [" << m_calibParams.rotation[0] << m_calibParams.rotation[1] << m_calibParams.rotation[2] << "]";
//    qDebug() << "  [" << m_calibParams.rotation[3] << m_calibParams.rotation[4] << m_calibParams.rotation[5] << "]";
//    qDebug() << "  [" << m_calibParams.rotation[6] << m_calibParams.rotation[7] << m_calibParams.rotation[8] << "]";
//    qDebug() << "Current translation: [" << m_calibParams.translation[0] << m_calibParams.translation[1] << m_calibParams.translation[2] << "]";

//    // Текущее приближение
//    Eigen::Matrix3f R_current;
//    R_current << m_calibParams.rotation[0], m_calibParams.rotation[1], m_calibParams.rotation[2],
//                 m_calibParams.rotation[3], m_calibParams.rotation[4], m_calibParams.rotation[5],
//                 m_calibParams.rotation[6], m_calibParams.rotation[7], m_calibParams.rotation[8];
//    Eigen::Vector3f t_current(m_calibParams.translation[0], m_calibParams.translation[1], m_calibParams.translation[2]);

//    // Отсечение камеры + трансформация + преобразование СК
//    std::vector<Eigen::Vector4f> camera_cropped;
//    camera_cropped.reserve(captured_camera.size());

//    for (const auto& p : captured_camera) {
//        Eigen::Vector3f tp = R_current * p + t_current;
//        if (tp.x() >= camera_crop_min[0] && tp.x() <= camera_crop_max[0] &&
//            tp.y() >= camera_crop_min[1] && tp.y() <= camera_crop_max[1] &&
//            tp.z() >= camera_crop_min[2] && tp.z() <= camera_crop_max[2]) {
//            // X_лидар = Z_камера, Y_лидар = -X_камера, Z_лидар = -Y_камера
//            camera_cropped.emplace_back(tp.z(), -tp.x(), -tp.y(), 1.0f);
//        }
//    }
//    qDebug() << "Camera points after crop:" << camera_cropped.size();

//    // Отсечение лидара
//    std::vector<Eigen::Vector4f> lidar_cropped;
//    lidar_cropped.reserve(captured_lidar.size());

//    for (const auto& p : captured_lidar) {
//        if (p.x() >= lidar_crop_min[0] && p.x() <= lidar_crop_max[0] &&
//            p.y() >= lidar_crop_min[1] && p.y() <= lidar_crop_max[1] &&
//            p.z() >= lidar_crop_min[2] && p.z() <= lidar_crop_max[2]) {
//            lidar_cropped.emplace_back(p.x(), p.y(), p.z(), 1.0f);
//        }
//    }
//    qDebug() << "Lidar points after crop:" << lidar_cropped.size();

//    // === Балансировка количества точек ===
//    const size_t target_points = 8000;

//    // Случайный даунсемплинг камеры до target_points
//    std::vector<Eigen::Vector4f> camera_balanced;
//    if (camera_cropped.size() > target_points) {
//        std::mt19937 rng(42);
//        std::shuffle(camera_cropped.begin(), camera_cropped.end(), rng);
//        camera_balanced.assign(camera_cropped.begin(), camera_cropped.begin() + target_points);
//    } else {
//        camera_balanced = camera_cropped;
//    }

//    // Лидар либо оставляем, либо тоже приводим к target_points
//    std::vector<Eigen::Vector4f> lidar_balanced;
//    if (lidar_cropped.size() > target_points) {
//        std::mt19937 rng(42);
//        std::shuffle(lidar_cropped.begin(), lidar_cropped.end(), rng);
//        lidar_balanced.assign(lidar_cropped.begin(), lidar_cropped.begin() + target_points);
//    } else {
//        lidar_balanced = lidar_cropped;
//    }

//    qDebug() << "Balanced points - lidar:" << lidar_balanced.size()
//             << "camera:" << camera_balanced.size();

//    // === Настройка GICP ===
//    small_gicp::RegistrationSetting setting;
//    setting.type = small_gicp::RegistrationSetting::GICP;
//    setting.downsampling_resolution = 0.01;      // можно оставить, после балансировки не сильно повлияет
//    setting.max_correspondence_distance = 0.05;  // 5 см
//    setting.max_iterations = 50;
//    setting.num_threads = 4;

//    // === Запуск GICP ===
//    Eigen::Isometry3d init_T = Eigen::Isometry3d::Identity();
//    small_gicp::RegistrationResult result = small_gicp::align(
//        lidar_balanced, camera_balanced, init_T, setting);

//    qDebug() << "GICP result:";
//    qDebug() << "  Converged:" << result.converged;
//    qDebug() << "  Iterations:" << result.iterations;
//    qDebug() << "  Error:" << result.error;
//    qDebug() << "  Num inliers:" << result.num_inliers;
//    qDebug() << "  Rotation delta:";
//    qDebug() << "    [" << result.T_target_source(0,0) << result.T_target_source(0,1)
//             << result.T_target_source(0,2) << result.T_target_source(0,3) << "]";
//    qDebug() << "    [" << result.T_target_source(1,0) << result.T_target_source(1,1)
//             << result.T_target_source(1,2) << result.T_target_source(1,3) << "]";
//    qDebug() << "    [" << result.T_target_source(2,0) << result.T_target_source(2,1)
//             << result.T_target_source(2,2) << result.T_target_source(2,3) << "]";

//    Eigen::Matrix3f R_icp = result.T_target_source.linear().cast<float>();
//    Eigen::Vector3f t_icp = result.T_target_source.translation().cast<float>();

//    float rotation_angle = acos((R_icp.trace() - 1.0f) / 2.0f) * 180.0f / M_PI;
//    qDebug() << "  Rotation angle:" << rotation_angle << "deg";

//    // === Итоговая калибровка ===
//    Eigen::Matrix3f R_final = R_icp * R_current;
//    Eigen::Vector3f t_final = R_icp * t_current + t_icp;

//    m_calibParams.rotation[0] = R_final(0,0); m_calibParams.rotation[1] = R_final(0,1); m_calibParams.rotation[2] = R_final(0,2);
//    m_calibParams.rotation[3] = R_final(1,0); m_calibParams.rotation[4] = R_final(1,1); m_calibParams.rotation[5] = R_final(1,2);
//    m_calibParams.rotation[6] = R_final(2,0); m_calibParams.rotation[7] = R_final(2,1); m_calibParams.rotation[8] = R_final(2,2);

//    m_calibParams.translation[0] = t_final.x();
//    m_calibParams.translation[1] = t_final.y();
//    m_calibParams.translation[2] = t_final.z();

//    qDebug() << "Final rotation matrix:";
//    qDebug() << "  [" << m_calibParams.rotation[0] << m_calibParams.rotation[1] << m_calibParams.rotation[2] << "]";
//    qDebug() << "  [" << m_calibParams.rotation[3] << m_calibParams.rotation[4] << m_calibParams.rotation[5] << "]";
//    qDebug() << "  [" << m_calibParams.rotation[6] << m_calibParams.rotation[7] << m_calibParams.rotation[8] << "]";
//    qDebug() << "Final translation: [" << m_calibParams.translation[0] << m_calibParams.translation[1] << m_calibParams.translation[2] << "]";
//    qDebug() << "========== GICP CALIBRATION END ==========";

//    applyCalibrationToCaptured();
//    emit calibrationUpdated(m_calibParams);
//}

void Processor::saveCloudsToFile(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(data_mutex);

    if (!has_captured_data || captured_lidar.empty() || captured_camera.empty()) {
        qWarning() << "No captured data to save";
        return;
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        qWarning() << "Cannot open file for writing:" << filename.c_str();
        return;
    }

    uint32_t lidar_count = captured_lidar.size();
    uint32_t camera_count = captured_camera.size();
    file.write(reinterpret_cast<const char*>(&lidar_count), sizeof(lidar_count));
    file.write(reinterpret_cast<const char*>(&camera_count), sizeof(camera_count));

    for (const auto& p : captured_lidar) {
        float x = p.x(), y = p.y(), z = p.z();
        file.write(reinterpret_cast<const char*>(&x), sizeof(x));
        file.write(reinterpret_cast<const char*>(&y), sizeof(y));
        file.write(reinterpret_cast<const char*>(&z), sizeof(z));
    }

    for (const auto& p : captured_camera) {
        float x = p.x(), y = p.y(), z = p.z();
        file.write(reinterpret_cast<const char*>(&x), sizeof(x));
        file.write(reinterpret_cast<const char*>(&y), sizeof(y));
        file.write(reinterpret_cast<const char*>(&z), sizeof(z));
    }

    file.close();
    qDebug() << "Clouds saved to" << filename.c_str()
             << "| lidar:" << lidar_count << "camera:" << camera_count;
}

void Processor::loadCloudsFromFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        qWarning() << "Cannot open file:" << filename.c_str();
        return;
    }

    uint32_t lidar_count = 0, camera_count = 0;
    file.read(reinterpret_cast<char*>(&lidar_count), sizeof(lidar_count));
    file.read(reinterpret_cast<char*>(&camera_count), sizeof(camera_count));

    captured_lidar.clear();
    captured_camera.clear();

    captured_lidar.reserve(lidar_count);
    for (uint32_t i = 0; i < lidar_count; ++i) {
        float x, y, z;
        file.read(reinterpret_cast<char*>(&x), sizeof(x));
        file.read(reinterpret_cast<char*>(&y), sizeof(y));
        file.read(reinterpret_cast<char*>(&z), sizeof(z));
        captured_lidar.emplace_back(x, y, z);
    }

    captured_camera.reserve(camera_count);
    for (uint32_t i = 0; i < camera_count; ++i) {
        float x, y, z;
        file.read(reinterpret_cast<char*>(&x), sizeof(x));
        file.read(reinterpret_cast<char*>(&y), sizeof(y));
        file.read(reinterpret_cast<char*>(&z), sizeof(z));
        captured_camera.emplace_back(x, y, z);
    }

    file.close();
    has_captured_data = true;

    qDebug() << "Clouds loaded from" << filename.c_str()
             << "| lidar:" << captured_lidar.size()
             << "camera:" << captured_camera.size();

    emit DisplayCapturedClouds(captured_lidar, captured_camera);
}

void Processor::captureOnce()
{
    if (capture_requested) {
        qWarning() << "Capture already in progress";
        return;
    }

    capture_requested = true;
    capture_start_time = 0;

    {
        std::lock_guard<std::mutex> lock(data_mutex);
        has_lidar_frame = false;
        has_camera_frame = false;
        captured_lidar.clear();
        captured_camera.clear();
        has_captured_data = false;
    }

    if (!lidar_initialized) {
        if (!initializeLidar()) {
            capture_requested = false;
            return;
        }
    }

    if (!camera_initialized) {
        if (!initializeCamera()) {
            capture_requested = false;
            return;
        }
    }

    qDebug() << "Waiting for synchronized frame...";
}

void Processor::processPointCloud(const rs2::vertex *vertices, unsigned long size)
{
    if (!vertices) return;
    if (size == 0) return;

    std::lock_guard<std::mutex> lock(data_mutex);

    if (lidar_cloud_msg.point_num == 0) return;

    // Фильтрация точек камеры по расстоянию (до 2.5 м)
    std::vector<rs2::vertex> filtered_vertices;
    filtered_vertices.reserve(size);

    for (unsigned long i = 0; i < size; ++i) {
        float depth = vertices[i].z;  // z - расстояние от камеры
        if (depth > 0.1f && depth < 2.5f) {  // от 10 см до 2.5 м
            filtered_vertices.push_back(vertices[i]);
        }
    }

    size = filtered_vertices.size();
    if (size == 0) {
        qWarning() << "No points after distance filter";
        return;
    }

    // Освобождаем старые буферы
    if (buffers_allocated) {
        delete[] camera_vertices;
        delete[] camera_colors;
        delete[] lidar_vertices;
        delete[] lidar_colors;
    }

    camera_vertices = new GLfloat[3 * size];
    camera_colors = new GLfloat[3 * size];
    lidar_vertices = new GLfloat[3 * lidar_cloud_msg.point_num];
    lidar_colors = new GLfloat[3 * lidar_cloud_msg.point_num];
    buffers_allocated = true;

    // Лидар (красный)
    for (unsigned long i = 0; i < lidar_cloud_msg.point_num; ++i) {
        lidar_vertices[i * 3]     = -lidar_cloud_msg.points[i].y * 1000.0f;
        lidar_vertices[i * 3 + 1] =  lidar_cloud_msg.points[i].z * 1000.0f;
        lidar_vertices[i * 3 + 2] = -lidar_cloud_msg.points[i].x * 1000.0f;
        lidar_colors[i * 3]       = 1.0f;
        lidar_colors[i * 3 + 1]   = 0.0f;
        lidar_colors[i * 3 + 2]   = 0.0f;
    }

    // Камера (синий) с фильтрацией
    for (unsigned long i = 0; i < size; ++i) {
        float x = filtered_vertices[i].x;
        float y = filtered_vertices[i].y;
        float z = filtered_vertices[i].z;

        float out_x = m_calibParams.rotation[0] * x + m_calibParams.rotation[3] * y + m_calibParams.rotation[6] * z;
        float out_y = m_calibParams.rotation[1] * x + m_calibParams.rotation[4] * y + m_calibParams.rotation[7] * z;
        float out_z = m_calibParams.rotation[2] * x + m_calibParams.rotation[5] * y + m_calibParams.rotation[8] * z;

        out_x += m_calibParams.translation[0];
        out_y += m_calibParams.translation[1];
        out_z += m_calibParams.translation[2];

        camera_vertices[i * 3]     = out_x * 1000.0f;
        camera_vertices[i * 3 + 1] = -out_y * 1000.0f;
        camera_vertices[i * 3 + 2] = -out_z * 1000.0f;

        camera_colors[i * 3]     = 0.0f;
        camera_colors[i * 3 + 1] = 0.5f;
        camera_colors[i * 3 + 2] = 1.0f;
    }

    emit DisplayCameraPointCloud(camera_vertices, camera_colors, size);
    emit DisplayLidarPointCloud(lidar_vertices, lidar_colors, lidar_cloud_msg.point_num);
}

ColorRGB Processor::getColorByDepth(float depth, float min_depth, float max_depth)
{
    ColorRGB color;
    float range = max_depth - min_depth;
    if (range <= 0.0f) return color;

    float t = (depth - min_depth) / range;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    const float r = t < 0.5f ? 0.0f : (t - 0.5f) * 2.0f;
    const float g = t < 0.5f ? t * 2.0f : (1.0f - t) * 2.0f;
    const float b = t < 0.5f ? (0.5f - t) * 2.0f : 0.0f;

    color.r = r;
    color.g = g;
    color.b = b;
    return color;
}

void Processor::onLidarPointCloud(const CustomMsg& msg)
{
    std::lock_guard<std::mutex> lock(data_mutex);
    lidar_cloud_msg = msg;
    has_lidar_frame = true;
}

void Processor::lidarPointCloudCallback(uint32_t handle, const uint8_t dev_type,
                                        LivoxLidarEthernetPacket *data, void *client_data)
{
    Q_UNUSED(client_data);
    Q_UNUSED(dev_type);
    if (!data) return;

    if (lock_cloud) {
        lock_cloud = false;
        accumulated_cloud.points.clear();
        accumulated_cloud.point_num = 0;
    }

    if (data->data_type == kLivoxLidarCartesianCoordinateHighData) {
        uint64_t timestamp = getPacketTimestamp(data->time_type, data->timestamp, sizeof(data->timestamp));
        LivoxLidarCartesianHighRawPoint *p_points = (LivoxLidarCartesianHighRawPoint *)data->data;

        if (start_new_cloud) {
            start_new_cloud = false;
            last_cloud_time = timestamp;
        }

        for (uint32_t i = 0; i < data->dot_num; ++i) {
            CustomPoint point;
            point.x = p_points[i].x / 1000.0;
            point.y = p_points[i].y / 1000.0;
            point.z = p_points[i].z / 1000.0;
            point.reflectivity = p_points[i].reflectivity;
            point.line = i % kLineNumberMid360;
            point.tag = p_points[i].tag;
            point.offset_time = timestamp + i * (data->time_interval * 100 / data->dot_num);
            accumulated_cloud.points.push_back(point);
        }
        accumulated_cloud.point_num += data->dot_num;

        if (timestamp - last_cloud_time >= 2000000000) {
            if (accumulated_cloud.points.empty()) {
                qWarning() << "accumulated cloud empty";
                last_cloud_time += 2000000000;
                return;
            }

            accumulated_cloud.header.stamp = Time(accumulated_cloud.points[0].offset_time / 1000000000.0);
            lock_cloud = true;
            accumulated_cloud.lidar_id = handle;
            accumulated_cloud.header.msg_seq++;
            accumulated_cloud.timebase = accumulated_cloud.points[0].offset_time;

            if (g_processor) {
                g_processor->onLidarPointCloud(accumulated_cloud);
            }

            last_cloud_time += 2000000000;
        }
    }
}

uint64_t Processor::getPacketTimestamp(uint8_t timestamp_type, uint8_t *time_stamp, uint8_t size)
{
    LdsStamp time;
    memcpy(time.stamp_bytes, time_stamp, size);

    if (timestamp_type == kTimestampTypeGptpOrPtp || timestamp_type == kTimestampTypeGps)
        return time.stamp;

    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

void Processor::run()
{
    qDebug() << "[run] Processor thread started";

    uint64_t capture_start_time = 0;
    const uint64_t CAPTURE_DELAY_NS = 15000000000ULL; // 15 секунд

    while (true) {
        bool has_camera = false;
        bool has_lidar = false;

        {
            std::lock_guard<std::mutex> lock(data_mutex);
            has_camera = has_camera_frame;
            has_lidar = has_lidar_frame;

            if (has_camera && has_lidar) {
                has_camera_frame = false;
                has_lidar_frame = false;
            }
        }

        if (has_camera && has_lidar) {
            depth_frame = uploader.process(depth_frame);
            depth_texture = colorizer.colorize(depth_frame);
            pointcloud.map_to(depth_texture);
            points = pointcloud.calculate(uploader.process(depth_frame));

            processPointCloud(points.get_vertices(), points.size());

            if (capture_requested) {
                if (capture_start_time == 0) {
                    capture_start_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                    qDebug() << "Capture started, accumulating data...";
                    continue;
                }

                uint64_t now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                if (now - capture_start_time < CAPTURE_DELAY_NS) {
                    continue;
                }

                qDebug() << "Capture delay complete, saving data...";

                {
                    std::lock_guard<std::mutex> lock(data_mutex);
                    captured_lidar.clear();
                    captured_camera.clear();

                    for (uint32_t i = 0; i < lidar_cloud_msg.point_num; ++i) {
                        captured_lidar.emplace_back(
                            lidar_cloud_msg.points[i].x,
                            lidar_cloud_msg.points[i].y,
                            lidar_cloud_msg.points[i].z
                        );
                    }

                    rs2::vertex* vertices = const_cast<rs2::vertex*>(points.get_vertices());
                    for (size_t i = 0; i < points.size(); ++i) {
                        captured_camera.emplace_back(
                            vertices[i].x,
                            vertices[i].y,
                            vertices[i].z
                        );
                    }

                    has_captured_data = true;
                }

                qDebug() << "Captured clouds: lidar =" << captured_lidar.size()
                         << "camera =" << captured_camera.size();

                capture_requested = false;
                capture_start_time = 0;

                shutdownLidar();
                shutdownCamera();

                emit DisplayCapturedClouds(captured_lidar, captured_camera);
            }
        }

        usleep(1000);
    }
}
