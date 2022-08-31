#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if(!(x)) __debugbreak(); //specific to MSVC
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {
    // Clear all errors
    while (glGetError() != GL_NO_ERROR);
}

static void GLCheckError() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error](" << error << ")" << std::endl;
    }
}


static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error](" << error << ") :" << function << 
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}


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

    

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate(); 
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << "OPENGL VERSION : " << glGetString(GL_VERSION) << std::endl;


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


    // VERTEX BUFFER
    
    unsigned int buffer;
    //Generate 1 buffer, pointer to unsigned int into which to write memory
    GLCall(glGenBuffers(1, &buffer));
    //select/bind buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
     
    //Put data into buffer - type of buffer, size of buffer/data, 
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW));
    

    /**
    * 2- SPECIFY LAYOUT OF DATA
    */
    //Can define and enable as long a buffer has been bound (as above)
    //index of 0 because first attribute, 2 float represent a single attribute/vertex, type of data = floats, non-normalized as floats are already normalized, # bytes between each vertex/offset to next vertex, offset to the next attribute i.e. texture coord
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));
    GLCall(glEnableVertexAttribArray(0)); //can come before or after glVertexAttribPointer, as long as buffer has been bound

    //glBindBuffer(GL_ARRAY_BUFFER, 0);  //empties buffer


    // INDEX BUFFER
    unsigned int ibo;  //Index Buffer Object - must be unsigned; can be short or char for performance
    //Generate 1 buffer, pointer to unsigned int into which to write memory
    GLCall(glGenBuffers(1, &ibo));
    //select/bind buffer
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

    //Put data into buffer - type of buffer, size of buffer/data, 
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));  //sends data to GPU


    // WRITE OUR FIRST SHADER   
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    //Bind program
    GLCall(glUseProgram(shader));


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        /**
        * 2- TELL OPENGL HOW THE DATA IS LAYED OUT
        **/
        //Draw call type 1 : Type of primitive, start index of vertices, number of vertices
        //glDrawArrays(GL_TRIANGLES, 0, 6); //used when we don't have an index buffer; draws from the last bound buffer (step 1)
        
        //Draw call type 2 : Type of primitive, number of vertices, type of index data
        //GLClearError();
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); //used with index buffer
        //GLCheckError();
        //ASSERT(GLLogCall());

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
    }
    //Clean up
    GLCall(glDeleteProgram(shader));

    GLCall(glfwTerminate());
    return 0;
}