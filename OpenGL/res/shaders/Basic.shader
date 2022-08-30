#shader vertex
#version 330 core

layout(location = 0) in vec4 position; //vec4 because gl_Position is vec4 and opengl can convert our vec2 to vec4

void main()
{
   gl_Position = position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color; //layout stuff not necessary

void main()
{
   //color = vec4(0, 1.0, 0.58, 1.0); //colors in graphics programming are rgba floats between 0 and 1 - this is red
   color = vec4(0.00, 1.0, 0.12, 1.0); //colors in graphics programming are rgba floats between 0 and 1 - this is red
};