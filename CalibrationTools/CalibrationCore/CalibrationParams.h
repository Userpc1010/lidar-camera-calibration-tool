#ifndef CALIBRATIONPARAMS_H
#define CALIBRATIONPARAMS_H

#include <fstream>
#include <sstream>
#include <vector>
#include <QDebug>

struct CalibrationParams {
    double rotation[9] = {
        0.999048, -0.018916, 0.0393044,
        0.0, 0.901077, 0.433659,
        -0.0436194, -0.433246, 0.900219
    };
    double translation[3] = {-0.065, 0.089, 0.055};

    // Предустановленные крепления
    struct Preset {
        std::string name;
        double rotation[9];
        double translation[3];
    };

    static std::vector<Preset> getPresets() {
        return {
            {
                "New Mount",
                {0.999048, -0.018916, 0.0393044,
                 0.0, 0.901077, 0.433659,
                 -0.0436194, -0.433246, 0.900219},
                {-0.065, 0.089, 0.055}
            },
            {
                "Old Mount",
                {0.99973, 0.00235, -0.02316,
                 -0.00523, 0.99212, -0.12521,
                 0.02269, 0.12530, 0.99186},
                {-0.085, 0.090, 0.090}
            }
        };
    }

    void applyPreset(const Preset& preset) {
        for (int i = 0; i < 9; ++i) rotation[i] = preset.rotation[i];
        for (int i = 0; i < 3; ++i) translation[i] = preset.translation[i];
    }

    bool save(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            qWarning() << "Cannot open file for writing:" << filename.c_str();
            return false;
        }

        file << "# Calibration parameters\n";
        file << "# Format: rotation matrix (3x3) + translation (3x1)\n\n";

        file << "rotation:\n";
        for (int i = 0; i < 9; ++i) {
            file << rotation[i] << " ";
            if ((i + 1) % 3 == 0) file << "\n";
        }

        file << "\ntranslation:\n";
        file << translation[0] << " " << translation[1] << " " << translation[2] << "\n";

        file.close();
        qDebug() << "Calibration saved to" << filename.c_str();
        return true;
    }

    bool load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            qWarning() << "Cannot open file:" << filename.c_str();
            return false;
        }

        std::string line;
        bool reading_rotation = false;
        bool reading_translation = false;
        int rotation_idx = 0;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            if (line == "rotation:") {
                reading_rotation = true;
                reading_translation = false;
                continue;
            }
            if (line == "translation:") {
                reading_rotation = false;
                reading_translation = true;
                continue;
            }

            if (reading_rotation && rotation_idx < 9) {
                std::stringstream ss(line);
                double val;
                while (ss >> val && rotation_idx < 9) {
                    rotation[rotation_idx++] = val;
                }
            }
            if (reading_translation) {
                std::stringstream ss(line);
                ss >> translation[0] >> translation[1] >> translation[2];
                reading_translation = false;
            }
        }

        file.close();
        qDebug() << "Calibration loaded from" << filename.c_str();
        return true;
    }
};

#endif // CALIBRATIONPARAMS_H
