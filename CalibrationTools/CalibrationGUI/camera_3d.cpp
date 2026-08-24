#include "camera_3d.h"
#include <QtMath>

Camera_3D::Camera_3D()
    : m_Translate(0.0f, 0.0f, 30.0f),   // камера на расстоянии 30 метров от начала
      m_camera_up(0.0f, 1.0f, 0.0f),
      pitch(0.0),
      yaw(-90.0),
      sensitivity_x(0.5),
      sensitivity_y(0.5)
{
    // Вычисляем начальный вектор front
    double yawRad = qDegreesToRadians(yaw);
    double pitchRad = qDegreesToRadians(pitch);
    front.setX(cos(yawRad) * cos(pitchRad));
    front.setY(sin(pitchRad));
    front.setZ(sin(yawRad) * cos(pitchRad));
    front.normalize();

    updateViewMatrix();
}

void Camera_3D::draw(QOpenGLShaderProgram *program, QOpenGLFunctions_4_5_Core *functions)
{
    Q_UNUSED(functions); // параметр не используется
    updateViewMatrix();
    program->setUniformValue("u_viewMatrix", m_ViewMatrix);
}

void Camera_3D::rotate_camera(double x, double y)
{
    yaw += x * sensitivity_x;
    pitch += y * sensitivity_y;

    // Ограничиваем pitch, чтобы избежать опрокидывания
    if (pitch > 89.9)  pitch = 89.9;
    if (pitch < -89.9) pitch = -89.9;

    double yawRad = qDegreesToRadians(yaw);
    double pitchRad = qDegreesToRadians(pitch);

    front.setX(cos(yawRad) * cos(pitchRad));
    front.setY(sin(pitchRad));
    front.setZ(sin(yawRad) * cos(pitchRad));
    front.normalize();
}

void Camera_3D::camera_zoom(bool zoom)
{
    if (zoom)
        m_Translate += front * 10.0f;   // приближение
    else
        m_Translate -= front * 10.0f;   // отдаление
}

void Camera_3D::Front_move()
{
    m_Translate += front * 10.0f;
}

void Camera_3D::Back_move()
{
    m_Translate -= front * 10.0f;
}

void Camera_3D::right_move()
{
    QVector3D right = QVector3D::crossProduct(front, m_camera_up).normalized();
    m_Translate += right * 10.0f;
}

void Camera_3D::left_move()
{
    QVector3D right = QVector3D::crossProduct(front, m_camera_up).normalized();
    m_Translate -= right * 10.0f;
}

void Camera_3D::Up_move()
{
    m_Translate += m_camera_up * 10.0f;
}

void Camera_3D::Down_move()
{
    m_Translate -= m_camera_up * 10.0f;
}

QVector3D Camera_3D::GetPositionCamera()
{
    return m_Translate;
}

void Camera_3D::setPosition(const QVector3D &pos)
{
    m_Translate = pos;
}

void Camera_3D::setYaw(double yawDeg)
{
    yaw = yawDeg;
    double yawRad = qDegreesToRadians(yaw);
    double pitchRad = qDegreesToRadians(pitch);
    front.setX(cos(yawRad) * cos(pitchRad));
    front.setY(sin(pitchRad));
    front.setZ(sin(yawRad) * cos(pitchRad));
    front.normalize();
}

void Camera_3D::setPitch(double pitchDeg)
{
    pitch = pitchDeg;
    if (pitch > 89.9)  pitch = 89.9;
    if (pitch < -89.9) pitch = -89.9;
    double yawRad = qDegreesToRadians(yaw);
    double pitchRad = qDegreesToRadians(pitch);
    front.setX(cos(yawRad) * cos(pitchRad));
    front.setY(sin(pitchRad));
    front.setZ(sin(yawRad) * cos(pitchRad));
    front.normalize();
}

void Camera_3D::lookAt(const QVector3D &target)
{
    QVector3D direction = target - m_Translate;
    front = direction.normalized();

    pitch = qRadiansToDegrees(asin(front.y()));
    yaw = qRadiansToDegrees(atan2(front.z(), front.x()));
}

QVector<float> Camera_3D::GetRaycastPosition()
{
    cam_data.clear();
    cam_data.push_back(m_Translate.x());
    cam_data.push_back(m_Translate.y());
    cam_data.push_back(m_Translate.z());
    cam_data.push_back(pitch);
    cam_data.push_back(yaw);
    return cam_data;
}

void Camera_3D::updateViewMatrix()
{
    m_ViewMatrix.setToIdentity();
    m_ViewMatrix.lookAt(m_Translate,
                        m_Translate + front,
                        m_camera_up);
    m_ViewMatrix = m_ViewMatrix * m_GlobalTransform.inverted();
}
