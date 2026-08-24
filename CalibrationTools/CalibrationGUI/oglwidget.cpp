#include "oglwidget.h"
#include "camera_3d.h"
#include "cuberander.h"

#include "QtImGui.h"
#include "ImGuiRenderer.h"

#include <QOpenGLContext>
#include <QtMath>
#include <QDebug>

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    function = new QOpenGLFunctions_4_5_Core();
    m_camera = new Camera_3D;
    m_cameraPointCloud = new CubeRander();
    m_lidarPointCloud = new CubeRander();

    // Таймер для регулярного обновления (60 FPS)
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    m_updateTimer->start(16);  // ~60 FPS
}

OGLWidget::~OGLWidget()
{
    m_updateTimer->stop();
    delete m_camera;
    delete m_cameraPointCloud;
    delete m_lidarPointCloud;
    delete function;
}

void OGLWidget::initializeGL()
{
    function->initializeOpenGLFunctions();
    function->glEnable(GL_DEPTH_TEST);
    function->glDepthFunc(GL_LEQUAL);
    function->glEnable(GL_CULL_FACE);
    function->glCullFace(GL_BACK);
    function->glFrontFace(GL_CCW);
    function->glLineWidth(6.0f);
    function->glPointSize(12.0f);

    initShaders();

    m_cameraPointCloud->init_fast();
    m_lidarPointCloud->init_fast();

    QtImGui::initialize(this);
}

void OGLWidget::resizeGL(int w, int h)
{
    float aspect = w / (h ? static_cast<float>(h) : 1);
    m_PojectionMatrix.setToIdentity();
    m_PojectionMatrix.perspective(90, aspect, 1.0f, 3000.0f);
}

void OGLWidget::paintGL()
{
    function->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    function->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Program.bind();
    m_Program.setUniformValue("u_projectionMatrix", m_PojectionMatrix);

    m_camera->draw(&m_Program, function);

    m_cameraPointCloud->draw(&m_Program, function, m_camera->GetPositionCamera());
    m_lidarPointCloud->draw(&m_Program, function, m_camera->GetPositionCamera());

    m_Program.release();

    DrawCalibrationUI();
}
// ================== Слоты для облаков ==================

void OGLWidget::DisplayCameraPointCloud(GLfloat* vertices, GLfloat* colors, unsigned long long count)
{
    m_cameraPointCloud->translate(vertices, colors, count);
}

void OGLWidget::DisplayLidarPointCloud(GLfloat* vertices, GLfloat* colors, unsigned long long count)
{
    m_lidarPointCloud->translate(vertices, colors, count);
}

// ================== Вспомогательные методы ==================

void OGLWidget::setCalibrationData(const double* rotation, const double* translation)
{
    for (int i = 0; i < 9; ++i) m_RotationMatrix[i] = rotation[i];
    for (int i = 0; i < 3; ++i) m_Translation[i] = translation[i];
    Matrix_to_Euler();
}

void OGLWidget::Matrix_to_Euler()
{
        double sy = sqrt(m_RotationMatrix[0]*m_RotationMatrix[0] +
                         m_RotationMatrix[3]*m_RotationMatrix[3]);
        bool singular = sy < 1e-6;

        if (!singular) {
            m_Euler[0] = atan2(m_RotationMatrix[7], m_RotationMatrix[8]) * 180.0 / M_PI;
            m_Euler[1] = atan2(-m_RotationMatrix[6], sy) * 180.0 / M_PI;
            m_Euler[2] = atan2(m_RotationMatrix[3], m_RotationMatrix[0]) * 180.0 / M_PI;
        } else {
            m_Euler[0] = atan2(-m_RotationMatrix[5], m_RotationMatrix[4]) * 180.0 / M_PI;
            m_Euler[1] = atan2(-m_RotationMatrix[6], sy) * 180.0 / M_PI;
            m_Euler[2] = 0.0;
        }
}

void OGLWidget::Euler_to_Matrix()
{
    double sin_R = sin(m_Euler[0] * M_PI / 180.0);
    double cos_R = cos(m_Euler[0] * M_PI / 180.0);
    double sin_P = sin(m_Euler[1] * M_PI / 180.0);
    double cos_P = cos(m_Euler[1] * M_PI / 180.0);
    double sin_Y = sin(m_Euler[2] * M_PI / 180.0);
    double cos_Y = cos(m_Euler[2] * M_PI / 180.0);

    m_RotationMatrix[0] = cos_P * cos_Y;
    m_RotationMatrix[1] = sin_R * sin_P * cos_Y - cos_R * sin_Y;
    m_RotationMatrix[2] = sin_R * sin_Y + cos_R * sin_P * cos_Y;
    m_RotationMatrix[3] = cos_P * sin_Y;
    m_RotationMatrix[4] = cos_R * cos_Y + sin_R * sin_P * sin_Y;
    m_RotationMatrix[5] = cos_R * sin_P * sin_Y - sin_R * cos_Y;
    m_RotationMatrix[6] = -sin_P;
    m_RotationMatrix[7] = sin_R * cos_P;
    m_RotationMatrix[8] = cos_R * cos_P;
}

// ================== ImGui UI ==================

void OGLWidget::DrawCalibrationUI()
{
    if (!m_showCalibWindow) return;

    QtImGui::newFrame();

    ImGui::SetNextWindowSizeConstraints(ImVec2(350, 350), ImVec2(FLT_MAX, FLT_MAX));

    ImGui::Begin("Calibration", &m_showCalibWindow);

    // ---- Euler Angles ----
    ImGui::Text("Euler Angle (deg)");
    bool euler_updated = false;
    euler_updated |= ImGui::InputDouble("Pitch", &m_Euler[0], 0.01, 1.0, "%.5f");
    euler_updated |= ImGui::InputDouble("Yaw", &m_Euler[1], 0.01, 1.0, "%.5f");
    euler_updated |= ImGui::InputDouble("Roll", &m_Euler[2], 0.01, 1.0, "%.5f");
    if (euler_updated) {
        Euler_to_Matrix();
        emit eulerChanged(m_Euler[0], m_Euler[1], m_Euler[2]);
        emit matrixChanged(m_RotationMatrix);
    }

    ImGui::Separator();

    // ---- Rotation Matrix ----
    ImGui::Text("Rotation Matrix");
    bool matrix_updated = false;
    ImGui::PushItemWidth(100.0f);
    for (int row = 0; row < 3; ++row) {
        ImGui::PushID(row);
        for (int col = 0; col < 3; ++col) {
            int idx = row * 3 + col;
            if (col > 0) ImGui::SameLine(0, 10.0f);
            char label[32];
            snprintf(label, sizeof(label), "##M_%d_%d", row, col);
            matrix_updated |= ImGui::InputDouble(label, &m_RotationMatrix[idx], 0.001, 1.0, "%.5f");
        }
        ImGui::PopID();
        if (row < 2) ImGui::Spacing();
    }
    ImGui::PopItemWidth();
    if (matrix_updated) {
        Matrix_to_Euler();
        emit matrixChanged(m_RotationMatrix);
        emit eulerChanged(m_Euler[0], m_Euler[1], m_Euler[2]);
    }

    ImGui::Separator();

    // ---- Translation ----
    ImGui::Text("Translation (m)");
    bool trans_updated = false;
    trans_updated |= ImGui::InputDouble("X", &m_Translation[0], 0.001, 1.0, "%.5f");
    trans_updated |= ImGui::InputDouble("Y", &m_Translation[1], 0.001, 1.0, "%.5f");
    trans_updated |= ImGui::InputDouble("Z", &m_Translation[2], 0.001, 1.0, "%.5f");
    if (trans_updated) {
        emit translationChanged(m_Translation);
    }

    ImGui::Separator();
    ImGui::Text("Lidar Crop (m)");
    bool lidar_crop_changed = false;
    lidar_crop_changed |= ImGui::InputFloat("Lidar Min X", &m_lidarCropMin[0], 0.1f, 1.0f, "%.2f");
    lidar_crop_changed |= ImGui::InputFloat("Lidar Max X", &m_lidarCropMax[0], 0.1f, 1.0f, "%.2f");
    lidar_crop_changed |= ImGui::InputFloat("Lidar Min Y", &m_lidarCropMin[1], 0.1f, 1.0f, "%.2f");
    lidar_crop_changed |= ImGui::InputFloat("Lidar Max Y", &m_lidarCropMax[1], 0.1f, 1.0f, "%.2f");
    lidar_crop_changed |= ImGui::InputFloat("Lidar Min Z", &m_lidarCropMin[2], 0.1f, 1.0f, "%.2f");
    lidar_crop_changed |= ImGui::InputFloat("Lidar Max Z", &m_lidarCropMax[2], 0.1f, 1.0f, "%.2f");

    ImGui::Text("Camera Crop (m)");
    bool camera_crop_changed = false;
    camera_crop_changed |= ImGui::InputFloat("Cam Min X", &m_cameraCropMin[0], 0.1f, 1.0f, "%.2f");
    camera_crop_changed |= ImGui::InputFloat("Cam Max X", &m_cameraCropMax[0], 0.1f, 1.0f, "%.2f");
    camera_crop_changed |= ImGui::InputFloat("Cam Min Y", &m_cameraCropMin[1], 0.1f, 1.0f, "%.2f");
    camera_crop_changed |= ImGui::InputFloat("Cam Max Y", &m_cameraCropMax[1], 0.1f, 1.0f, "%.2f");
    camera_crop_changed |= ImGui::InputFloat("Cam Min Z", &m_cameraCropMin[2], 0.1f, 1.0f, "%.2f");
    camera_crop_changed |= ImGui::InputFloat("Cam Max Z", &m_cameraCropMax[2], 0.1f, 1.0f, "%.2f");

    if (lidar_crop_changed || camera_crop_changed) {
        emit cropChanged(m_lidarCropMin, m_lidarCropMax, m_cameraCropMin, m_cameraCropMax);
    }

    ImGui::Separator();

    ImGui::Separator();
    if (ImGui::Button("Save Calibration")) {
        emit saveCalibrationClicked();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Calibration")) {
        emit loadCalibrationClicked();
    }

    ImGui::Separator();
    ImGui::Text("Mount Preset");

    const char* presets[] = {"New Mount", "Old Mount"};
    static int selected_preset = 0;

    if (ImGui::BeginCombo("##MountPreset", presets[selected_preset])) {
        for (int i = 0; i < 2; ++i) {
            bool is_selected = (selected_preset == i);
            if (ImGui::Selectable(presets[i], is_selected)) {
                selected_preset = i;
                emit presetSelected(i);
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    // ---- Buttons ----
    if (ImGui::Button("To Euler")) {
        Matrix_to_Euler();
        emit toEulerClicked();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        for (int i = 0; i < 9; ++i)
            m_RotationMatrix[i] = (i % 4 == 0) ? 1.0 : 0.0;
        for (int i = 0; i < 3; ++i) {
            m_Translation[i] = 0.0;
            m_Euler[i] = 0.0;
        }
        emit resetClicked();
    }
    ImGui::SameLine();
    if (ImGui::Button("Apply")) {
        Euler_to_Matrix();
        emit applyClicked();
    }

    ImGui::Separator();

    if (ImGui::Button("Capture")) {
        emit captureClicked();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        emit saveClicked();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        emit loadClicked();
    }
    if (ImGui::Button("Auto Calibrate (ICP)")) {
        emit autoCalibrateClicked();
    }

    ImGui::End();

    ImGui::Render();
    QtImGui::render();
}

// ================== Обработчики мыши/клавиш ==================

void OGLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::RightButton) {
        lastX = event->localPos().x();
        lastY = event->localPos().y();
        camera = false;
    }
    update();
    event->accept();
}

void OGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::RightButton) {
        if (camera) {
            lastX = event->localPos().x();
            lastY = event->localPos().y();
            camera = false;
        }
        double xoffset = event->localPos().x() - lastX;
        double yoffset = lastY - event->localPos().y();
        lastX = event->localPos().x();
        lastY = event->localPos().y();
        m_camera->rotate_camera(xoffset, yoffset);
    }
    update();
    event->accept();
}

void OGLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) m_camera->camera_zoom(true);
    else m_camera->camera_zoom(false);
    update();
    event->accept();
}

void OGLWidget::keyPressEvent(QKeyEvent *event)
{
    // F2 — показать/скрыть окно калибровки
    if (event->key() == Qt::Key_F2) {
        m_showCalibWindow = !m_showCalibWindow;
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_W) m_camera->Front_move();
    if (event->key() == Qt::Key_S) m_camera->Back_move();
    if (event->key() == Qt::Key_A) m_camera->left_move();
    if (event->key() == Qt::Key_D) m_camera->right_move();
    if (event->key() == Qt::Key_Space) m_camera->Up_move();
    if (event->key() == Qt::Key_Alt) m_camera->Down_move();

    update();
    event->accept();
}

void OGLWidget::initShaders()
{
    if(!m_Program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertshader.vsh"))
        this->close();
    if(!m_Program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragshader.fsh"))
        this->close();
    if(!m_Program.link())
        this->close();
}
