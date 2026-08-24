#ifndef RGBD_CAMERA_H
#define RGBD_CAMERA_H

#include <QThread>
#include <QVector>
#include <QDebug>

#include <Eigen/Dense>

#include <librealsense2/rs.hpp>
#include <librealsense2-gl/rs_processing_gl.hpp>
#include <GLFW/glfw3.h>

#include "oglwidget.h"
#include "CalibrationParams.h"
#include "livox_lidar_def.h"
#include "livox_lidar_api.h"
#include "LivoxData.hpp"

#include <mutex>
#include <condition_variable>
#include <chrono>

#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLU

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

// Цвет точки (для визуализации)
struct ColorRGB {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
};

class Processor : public QThread
{
    Q_OBJECT

public:
    Processor(const std::string& config_path, QObject *parent = nullptr);
    ~Processor();

    void update_translation_rotation(const CalibrationParams& params);

    void updateCalibrationParams(const CalibrationParams& params);

    void setCropParams(const float* lidar_min, const float* lidar_max, const float* camera_min, const float* camera_max);
    void applyCalibrationToCaptured();

    // Автоматическая калибровка (ICP) – будет реализована
    void autoCalibrate();

    void captureOnce();

    void saveCloudsToFile(const std::string& filename);
    void loadCloudsFromFile(const std::string& filename);

    bool initializeLidar();
    bool initializeCamera();
    void shutdownLidar();
    void shutdownCamera();
    bool isLidarInitialized() const { return lidar_initialized; }
    bool isCameraInitialized() const { return camera_initialized; }

    OGLWidget *widget;  // виджет для отображения облаков

    bool has_captured_data = false;

private:
    // ====== RealSense ======
    rs2::gl::pointcloud pointcloud;
    rs2::gl::uploader uploader;
    rs2::gl::colorizer colorizer;
    rs2::config config;
    rs2::pipeline pipe;
    rs2::points points;
    rs2::frame depth_frame;
    rs2::frame depth_texture;
    GLFWwindow* glfw_win = nullptr;
    bool use_gpu_processing = true;

    bool lidar_initialized = false;
    bool camera_initialized = false;

    std::string config_path_;

    uint64_t capture_start_time = 0;
    const uint64_t CAPTURE_DELAY_NS = 15000000000ULL; // 15 секунд

    // Для работы с файлами
    std::vector<Eigen::Vector3f> captured_lidar;
    std::vector<Eigen::Vector3f> captured_camera;
    bool capture_mode = false;
    bool capture_requested = false;

    // ====== Livox ======
    CustomMsg lidar_cloud_msg;      // последнее облако лидара

    // ====== Буферы для отображения ======
    GLfloat *camera_vertices = nullptr;
    GLfloat *camera_colors = nullptr;
    GLfloat *lidar_vertices = nullptr;
    GLfloat *lidar_colors = nullptr;
    bool buffers_allocated = false;

    // ====== Калибровочные параметры ======
    CalibrationParams m_calibParams;

    static inline const uint8_t kLineNumberMid360 = 4;
    static inline const uint8_t kLineNumberDefault = 1;
    static inline const uint8_t kLineNumberHAP = 6;

    float lidar_crop_min[3] = {0.75f, -1.50f, -0.30f};
    float lidar_crop_max[3] = {4.00f, 1.65f, 0.10f};
    float camera_crop_min[3] = {-1.65f, -0.10f, 0.75f};
    float camera_crop_max[3] = {1.50f, 0.30f, 4.00f};

    // ====== Синхронизация ======
    std::mutex data_mutex;
    bool has_camera_frame = false;
    bool has_lidar_frame = false;

    // ====== Внутренние методы ======
    void processPointCloud(const rs2::vertex *vertices, unsigned long size);
    ColorRGB getColorByDepth(float depth, float min_depth, float max_depth);

    // Callback'и Livox
    void onLidarPointCloud(const CustomMsg& msg);
    static void lidarPointCloudCallback(uint32_t handle, const uint8_t dev_type,
                                        LivoxLidarEthernetPacket *data, void *client_data);
    static uint64_t getPacketTimestamp(uint8_t timestamp_type, uint8_t* time_stamp, uint8_t size);

    // Livox SDK статические данные (общие для всех экземпляров, но у нас один)
    static inline CustomMsg accumulated_cloud;
    static inline bool start_new_cloud = true;
    static inline uint64_t last_cloud_time = 0;
    static inline bool lock_cloud = false;

protected:
    void run() override;

signals:
    void DisplayCameraPointCloud(GLfloat* vertices, GLfloat* colors, unsigned long long count);
    void DisplayLidarPointCloud(GLfloat* vertices, GLfloat* colors, unsigned long long count);
    void calibrationUpdated(const CalibrationParams& params);
    void DisplayCapturedClouds(const std::vector<Eigen::Vector3f>& lidar, const std::vector<Eigen::Vector3f>& camera);
};

#endif // RGBD_CAMERA_H
