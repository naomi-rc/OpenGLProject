#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"



struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

/**
* Parses file containing shader code
* return - struct containing code for each type of shader
**/
static ShaderProgramSource ParseShader(const std::string& filePath) {
    std::ifstream stream(filePath); //opens file

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) { //if found #shader in that line
            if (line.find("vertex") != std::string::npos) {
                // set mode to vertex
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                // set mode to fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else{
            ss[(int)type] << line << '\n';
        }        
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    GLCall(unsigned int id = glCreateShader(type));

    //Requires raw string instead of std::string
    const char* src = source.c_str(); //returns pointer to data inside of source so must exist
    //Specifies source of shader : shader count, pointer to the actual pointer/memory address of source, length
    GLCall(glShaderSource(id, 1, &src, NULL));
    GLCall(glCompileShader(id));

    int result;
    //Query compiled shader - iv = integer and vector : shader id, parameter name (compile status), int parameter which is a pointer
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE) {
        int length;
        //Query error message
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)_malloca(length * sizeof(char)); //allocate dynamically on stack or could use new and delete afterwards
        GLCall(glGetShaderInfoLog(id, length, &length, message));

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

/**
* Compiles and links the shaders into a program. Returns an identifier for the compiled shader
* vertexShader - source code of vertex shader
* fragmentShader - source code of fragment shader
*/
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    //Attach shaders to program - like linking the two files into one program
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    //Delete the "intermediates" (like objs) as they have now been linked to a program
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));
    //Technically should detach shaders, but this is a minimal optimization which also reduces debugging capability so we'll leave that out
    //glDetachShader(program, vs);
    //glDetachShader(program, fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
        
    // Use core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Without this, we are using GLFW_OPENGL_COMPAT_PROFILE


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate(); 
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << "OPENGL VERSION : " << glGetString(GL_VERSION) << std::endl;

    {
        /**
        * 1- GIVE OPENGL THE DATA AND BIND BUFFER
        **/
        float positions[] = {
            -0.5f, -0.5f, // 0
            0.5f,  -0.5f, // 1
            0.5f,   0.5f, // 2
            -0.5f,  0.5f, // 3
        };

        unsigned int indices[] = {
            0,1,2,
            2,3,0
        };

        VertexArray va;
        
        // VERTEX BUFFER
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));


        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);


        // INDEX BUFFER
        IndexBuffer ib(indices, 6);

        // WRITE OUR FIRST SHADER   
        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        //Bind program
        GLCall(glUseProgram(shader));

        // Use uniforms
        GLCall(int location = glGetUniformLocation(shader, "u_Color")); // get location of variable - uses same name "u_Color" as in fragment shader code
        ASSERT(location != -1); //if -1, could not find uniform - not necessarily error because could have been removed
        GLCall(glUniform4f(location, 0.0f, 1.0f, 0.12f, 1.0f));

        // Unbind everything
        va.Unbind();
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            GLCall(glUseProgram(shader));

            GLCall(glUniform4f(location, r, 1.0f, 0.12f, 1.0f));

            va.Bind();
            ib.Bind();


            /**
            * 2- TELL OPENGL HOW THE DATA IS LAYED OUT
            **/
            //Draw call type 1 : Type of primitive, start index of vertices, number of vertices
            //glDrawArrays(GL_TRIANGLES, 0, 6); //used when we don't have an index buffer; draws from the last bound buffer (step 1)

            //Draw call type 2 : Type of primitive, number of vertices, type of index data
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); //used with index buffer


            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }
        //Clean up
        GLCall(glDeleteProgram(shader));
    }

    glfwTerminate();
    return 0;
}