#include "mainwindow.h"
#include <QSurfaceFormat>
#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QSurfaceFormat format;
    format.setSamples(16);
    format.setDepthBufferSize(24);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(format);

    std::string config_path = "/home/sencis/build-UGV-Desktop-Debug/FAST-LIO2/MID360_config.json";
    m_processor = new Processor(config_path, this);

    // В конструкторе MainWindow
    m_oglWidget = m_processor->widget;
    m_oglWidget->setMinimumSize(800, 600);    // минимальный размер
    m_oglWidget->resize(1280, 720);           // начальный размер

    setCentralWidget(m_oglWidget);
    m_oglWidget->setFocusPolicy(Qt::StrongFocus);

    // Передаём начальные данные в OGLWidget
    m_oglWidget->setCalibrationData(m_calibParams.rotation, m_calibParams.translation);
    m_processor->update_translation_rotation(m_calibParams);

    // Соединяем сигналы OGLWidget с нашими слотами
    connect(m_oglWidget, &OGLWidget::eulerChanged, this, &MainWindow::onEulerChanged);
    connect(m_oglWidget, &OGLWidget::matrixChanged, this, &MainWindow::onMatrixChanged);
    connect(m_oglWidget, &OGLWidget::translationChanged, this, &MainWindow::onTranslationChanged);
    connect(m_oglWidget, &OGLWidget::toEulerClicked, this, &MainWindow::onToEulerClicked);
    connect(m_oglWidget, &OGLWidget::resetClicked, this, &MainWindow::onResetClicked);
    connect(m_oglWidget, &OGLWidget::applyClicked, this, &MainWindow::onApplyClicked);
    connect(m_oglWidget, &OGLWidget::autoCalibrateClicked, this, &MainWindow::onAutoCalibrateClicked);

    connect(m_processor, &Processor::calibrationUpdated, this, [this](const CalibrationParams& params) {
        m_calibParams = params;
        m_oglWidget->setCalibrationData(m_calibParams.rotation, m_calibParams.translation);
    });

    connect(m_oglWidget, &OGLWidget::saveClicked, this, [this]() {
        m_processor->saveCloudsToFile("calibration_clouds.bin");
    });

    connect(m_oglWidget, &OGLWidget::loadClicked, this, [this]() {
        m_processor->loadCloudsFromFile("calibration_clouds.bin");
    });

    connect(m_oglWidget, &OGLWidget::captureClicked, this, [this]() {
        if (m_processor) m_processor->captureOnce();
    });

    connect(m_oglWidget, &OGLWidget::cropChanged, this, [this](
        const float* lidar_min, const float* lidar_max,
        const float* camera_min, const float* camera_max) {

        if (m_processor) {
            m_processor->setCropParams(lidar_min, lidar_max, camera_min, camera_max);
        }
    });

    connect(m_oglWidget, &OGLWidget::saveCalibrationClicked, this, [this]() {
        if (m_calibParams.save("calibration.txt")) {
            qDebug() << "Calibration saved";
        }
    });

    connect(m_oglWidget, &OGLWidget::loadCalibrationClicked, this, [this]() {
        CalibrationParams loaded;
        if (loaded.load("calibration.txt")) {
            m_calibParams = loaded;
            m_oglWidget->setCalibrationData(m_calibParams.rotation, m_calibParams.translation);
            m_processor->update_translation_rotation(m_calibParams);
            qDebug() << "Calibration loaded and applied";
        }
    });

    connect(m_oglWidget, &OGLWidget::presetSelected, this, [this](int index) {
        if (index >= 0 && index < m_presets.size()) {
            m_calibParams.applyPreset(m_presets[index]);
            m_oglWidget->setCalibrationData(m_calibParams.rotation, m_calibParams.translation);
            m_processor->update_translation_rotation(m_calibParams);
            qDebug() << "Preset applied:" << m_presets[index].name.c_str();
        }
    });

    // Для отображения захваченных облаков
    connect(m_processor, &Processor::DisplayCapturedClouds, this, [this](
        const std::vector<Eigen::Vector3f>& lidar,
        const std::vector<Eigen::Vector3f>& camera) {

        // Конвертируем в формат для OGLWidget
        GLfloat* lidar_vertices = new GLfloat[lidar.size() * 3];
        GLfloat* lidar_colors = new GLfloat[lidar.size() * 3];
        GLfloat* camera_vertices = new GLfloat[camera.size() * 3];
        GLfloat* camera_colors = new GLfloat[camera.size() * 3];

        for (size_t i = 0; i < lidar.size(); ++i) {
            lidar_vertices[i*3] = -lidar[i].y() * 1000.0f;
            lidar_vertices[i*3+1] = lidar[i].z() * 1000.0f;
            lidar_vertices[i*3+2] = -lidar[i].x() * 1000.0f;
            lidar_colors[i*3] = 1.0f;
            lidar_colors[i*3+1] = 0.0f;
            lidar_colors[i*3+2] = 0.0f;
        }

        for (size_t i = 0; i < camera.size(); ++i) {
            camera_vertices[i*3] = camera[i].x() * 1000.0f;
            camera_vertices[i*3+1] = -camera[i].y() * 1000.0f;
            camera_vertices[i*3+2] = -camera[i].z() * 1000.0f;
            camera_colors[i*3] = 0.0f;
            camera_colors[i*3+1] = 0.5f;
            camera_colors[i*3+2] = 1.0f;
        }

        m_oglWidget->DisplayLidarPointCloud(lidar_vertices, lidar_colors, lidar.size());
        m_oglWidget->DisplayCameraPointCloud(camera_vertices, camera_colors, camera.size());

        delete[] lidar_vertices;
        delete[] lidar_colors;
        delete[] camera_vertices;
        delete[] camera_colors;
    });

    m_processor->start(QThread::HighPriority);
}

MainWindow::~MainWindow()
{
    delete m_processor;
}

void MainWindow::onEulerChanged(double roll, double pitch, double yaw)
{
    // Можно игнорировать, т.к. matrixChanged тоже придёт
}

void MainWindow::onMatrixChanged(const double* rotation)
{
    for (int i = 0; i < 9; ++i) m_calibParams.rotation[i] = rotation[i];
    m_processor->update_translation_rotation(m_calibParams);

    qDebug() << "Matrix changed:";
    qDebug() << "  [" << m_calibParams.rotation[0] << m_calibParams.rotation[1] << m_calibParams.rotation[2] << "]";
    qDebug() << "  [" << m_calibParams.rotation[3] << m_calibParams.rotation[4] << m_calibParams.rotation[5] << "]";
    qDebug() << "  [" << m_calibParams.rotation[6] << m_calibParams.rotation[7] << m_calibParams.rotation[8] << "]";

    if (m_processor->has_captured_data) {
        m_processor->applyCalibrationToCaptured();
    }
}

void MainWindow::onTranslationChanged(const double* translation)
{
    for (int i = 0; i < 3; ++i) m_calibParams.translation[i] = translation[i];
    m_processor->update_translation_rotation(m_calibParams);

    qDebug() << "Translation changed: [" << m_calibParams.translation[0]
             << m_calibParams.translation[1] << m_calibParams.translation[2] << "]";

    if (m_processor->has_captured_data) {
        m_processor->applyCalibrationToCaptured();
    }
}

void MainWindow::onApplyClicked()
{
    m_processor->update_translation_rotation(m_calibParams);

    qDebug() << "Apply clicked:";
    qDebug() << "  Rotation:";
    qDebug() << "    [" << m_calibParams.rotation[0] << m_calibParams.rotation[1] << m_calibParams.rotation[2] << "]";
    qDebug() << "    [" << m_calibParams.rotation[3] << m_calibParams.rotation[4] << m_calibParams.rotation[5] << "]";
    qDebug() << "    [" << m_calibParams.rotation[6] << m_calibParams.rotation[7] << m_calibParams.rotation[8] << "]";
    qDebug() << "  Translation: [" << m_calibParams.translation[0]
             << m_calibParams.translation[1] << m_calibParams.translation[2] << "]";

    if (m_processor->has_captured_data) {
        m_processor->applyCalibrationToCaptured();
    }
}

void MainWindow::onToEulerClicked()
{
    // Ничего не делаем, матрица уже синхронизирована
}

void MainWindow::onResetClicked()
{
    for (int i = 0; i < 9; ++i)
        m_calibParams.rotation[i] = (i % 4 == 0) ? 1.0 : 0.0;
    for (int i = 0; i < 3; ++i)
        m_calibParams.translation[i] = 0.0;

    m_processor->update_translation_rotation(m_calibParams);
    m_oglWidget->setCalibrationData(m_calibParams.rotation, m_calibParams.translation);
}

void MainWindow::onAutoCalibrateClicked()
{
    qDebug() << "Auto Calibrate clicked";
    if (m_processor) {
        m_processor->autoCalibrate();
    }
}
