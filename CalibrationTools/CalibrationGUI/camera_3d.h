#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#include <QQuaternion>
#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>

class Camera_3D
{
public:
    Camera_3D();

    // Отрисовка (передаёт матрицу вида в шейдер)
    void draw(QOpenGLShaderProgram* program, QOpenGLFunctions_4_5_Core* functions = nullptr);

    // Вращение камеры
    void rotate_camera(double x, double y);

    // Приближение/отдаление
    void camera_zoom(bool zoom);

    // Перемещение
    void Front_move();
    void Back_move();
    void right_move();
    void left_move();
    void Up_move();
    void Down_move();

    // Геттеры
    QVector3D GetPositionCamera();
    QVector3D getFront() const { return front; }
    double getYaw() const { return yaw; }
    double getPitch() const { return pitch; }

    // Сеттеры
    void setPosition(const QVector3D& pos);
    void setYaw(double yawDeg);
    void setPitch(double pitchDeg);
    void lookAt(const QVector3D& target);

    QVector<float> GetRaycastPosition();

private:
    void updateViewMatrix();

private:
    QVector3D m_Translate = QVector3D(0.0f, 0.0f, 30.0f);  // Начальная позиция камеры
    QVector3D m_camera_up = QVector3D(0.0f, 1.0f, 0.0f);
    QVector3D front;

    QMatrix4x4 m_GlobalTransform;
    QMatrix4x4 m_ViewMatrix;

    double pitch = 0.0;   // в градусах
    double yaw = -90.0;   // в градусах (смотрим вдоль -Z)

    double sensitivity_x = 0.5;
    double sensitivity_y = 0.5;

    QVector<float> cam_data;
};

#endif // CAMERA_3D_H
