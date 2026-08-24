#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_5_Core>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QQuaternion>
#include <QTimer>

class Camera_3D;
class CubeRander;  // рендерер точечных облаков

class OGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    OGLWidget(QWidget *parent = nullptr);
    ~OGLWidget();

    // Установка текущих параметров калибровки (из MainWindow)
    void setCalibrationData(const double* rotation, const double* translation);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void initShaders();

public slots:
    // Отображение облака точек камеры
    void DisplayCameraPointCloud(GLfloat* vertices_buffer, GLfloat* color_buffer,
                                 unsigned long long counter);

    // Отображение облака точек лидара
    void DisplayLidarPointCloud(GLfloat* vertices_buffer, GLfloat* color_buffer,
                                unsigned long long counter);

signals:
    // Сигналы для MainWindow при изменении параметров в ImGui
    void eulerChanged(double roll, double pitch, double yaw);
    void matrixChanged(const double* rotation);
    void translationChanged(const double* translation);
    void toEulerClicked();
    void resetClicked();
    void applyClicked();
    void captureClicked();
    void saveClicked();
    void loadClicked();
    void autoCalibrateClicked();
    void cropChanged(const float* lidar_min, const float* lidar_max, const float* camera_min, const float* camera_max);
    void presetSelected(int index);
    void saveCalibrationClicked();
    void loadCalibrationClicked();

private:
    QOpenGLFunctions_4_5_Core * function;
    QMatrix4x4 m_PojectionMatrix;
    QOpenGLShaderProgram m_Program;
    Camera_3D *m_camera;
    QTimer* m_updateTimer;

    // Рендереры облаков точек
    CubeRander* m_cameraPointCloud;  // облако камеры
    CubeRander* m_lidarPointCloud;   // облако лидара

    // Управление камерой мышью
    bool camera;
    double lastX = 0.0, lastY = 0.0;

    // ImGui состояние
    bool m_showCalibWindow = true;
    double m_RotationMatrix[9] = {1,0,0,
                                  0,1,0,
                                  0,0,1};
    double m_Translation[3] = {0,0,0};
    double m_Euler[3] = {0,0,0};

    float m_lidarCropMin[3] = {0.75f, -1.50f, -0.30f};
    float m_lidarCropMax[3] = {4.00f, 1.65f, 0.10f};
    float m_cameraCropMin[3] = {-1.65f, -0.10f, 0.75f};
    float m_cameraCropMax[3] = {1.50f, 0.30f, 4.00f};

    void DrawCalibrationUI();
    void Matrix_to_Euler();
    void Euler_to_Matrix();
};

#endif // OGLWIDGET_H
