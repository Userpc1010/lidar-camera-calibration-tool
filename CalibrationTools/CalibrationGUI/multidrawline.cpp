#include "multidrawline.h"
MultiDrawLine::MultiDrawLine()
{}

void MultiDrawLine::draw_lines(QOpenGLShaderProgram *program, QOpenGLFunctions_4_5_Core *functions)
{
  for(auto o: m_Objects_Line) o->draw(program, functions);
}

void MultiDrawLine::Add_Lines(GLfloat* vertices_buffer, QVector3D color_buffer, unsigned long long counter)
{
  m_Objects_Line.append(new DrawLine());

  m_Objects_Line.at( ObjectCounter++ )->DrawLines(vertices_buffer, color_buffer, counter);

//  qDebug()<<"Add Lines: "<<m_Objects_Line.size();
}


void MultiDrawLine::Remove_Lines(uint16_t index)
{
  ObjectCounter--;

  m_Objects_Line.remove(index);

}

void MultiDrawLine::Clear_Lines()
{
 ObjectCounter = 0;

 m_Objects_Line.clear();

// qDebug()<<"Clear Lines";
}

