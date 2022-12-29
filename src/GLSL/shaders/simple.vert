uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
in vec4 vertex;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vertex;
}
