#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "oglwidget.h"
#include "rgbd_camera.h"
#include "CalibrationParams.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Processor* m_processor;
    OGLWidget* m_oglWidget;

    CalibrationParams m_calibParams;

    const std::vector<CalibrationParams::Preset> m_presets = CalibrationParams::getPresets();

private slots:
    void onEulerChanged(double roll, double pitch, double yaw);
    void onMatrixChanged(const double* rotation);
    void onTranslationChanged(const double* translation);
    void onToEulerClicked();
    void onResetClicked();
    void onApplyClicked();
    void onAutoCalibrateClicked();
};

#endif // MAINWINDOW_H
