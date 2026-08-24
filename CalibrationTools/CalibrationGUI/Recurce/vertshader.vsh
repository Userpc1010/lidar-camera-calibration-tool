attribute highp vec4 a_position;
attribute highp vec2 a_textcoord;
attribute highp vec3 a_normal;

uniform highp mat4 u_projectionMatrix;
uniform highp mat4 u_viewMatrix;
uniform highp mat4 u_modelMatrix;

attribute vec3 position_vector;
attribute vec3 color_position;

uniform int p_id;

varying highp vec4 v_position;
varying highp vec2 v_textcoord;
varying highp vec3 v_color;
varying highp vec3 v_normal;

void main(void)
{
    if (p_id == 2) {
        gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;
        v_color = color_position;   // <-- ВАЖНО: передаём цвет
    }

    if (p_id == 1) {
        mat4 position_matrix = mat4(1.0, 0.0, 0.0, 0.0,
                                    0.0, 1.0, 0.0, 0.0,
                                    0.0, 0.0, 1.0, 0.0,
                                    position_vector.x, position_vector.y, position_vector.z, 1.0);

        gl_Position = u_projectionMatrix * u_viewMatrix * position_matrix * a_position;

        mat4 mv_matrix = u_viewMatrix * position_matrix;
        v_normal = normalize(vec3(mv_matrix * vec4(a_normal, 0.0)));
        v_position = mv_matrix * a_position;
        v_color = color_position;
    }

    if (p_id == 0) {
        gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;
        v_textcoord = a_textcoord;
    }
}
