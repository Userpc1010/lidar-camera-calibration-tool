#ifndef MULTIDRAWLINE_H
#define MULTIDRAWLINE_H
#include "drawline.h"

class MultiDrawLine
{
public:

    MultiDrawLine();

    void draw_lines(QOpenGLShaderProgram* program, QOpenGLFunctions_4_5_Core *functions);

    void Add_Lines (GLfloat* vertices_buffer, QVector3D color_buffer, unsigned long long counter);
    void Remove_Lines (uint16_t index);
    void Clear_Lines ();

private:

uint16_t ObjectCounter = 0;

QVector<DrawLine*> m_Objects_Line;

};

#endif // MULTIDRAWLINE_H
