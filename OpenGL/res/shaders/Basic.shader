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

uniform vec4 u_Color;

void main()
{   
	color = u_Color;
};