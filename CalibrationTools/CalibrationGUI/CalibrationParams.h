#ifndef CALIBRATIONPARAMS_H
#define CALIBRATIONPARAMS_H

struct CalibrationParams {
    double rotation[9] = {
        0.999048, -0.018916, 0.0393044,
        0.0, 0.901077, 0.433659,
        -0.0436194, -0.433246, 0.900219
    };
    double translation[3] = {-0.065, 0.089, 0.055};
};

#endif // CALIBRATIONPARAMS_H
