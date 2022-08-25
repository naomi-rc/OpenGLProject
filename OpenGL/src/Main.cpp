#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

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
    * 1- GIVE OPENGL THE DATA
    **/
    float positions[6] = {
        -0.5f, -0.5f, //vertex 1 - index 0
        0.0f,   0.5f, //vertex 2 - index 1
        0.5f,  -0.5f //vertex 3 - index 2
    };


    unsigned int buffer;
    //Generate 1 buffer, pointer to unsigned int into which to write memory
    glGenBuffers(1, &buffer);
    //select/bind buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
    //Put data into buffer - type of buffer, size of buffer/data, 
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /*glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f,   0.5f);
        glVertex2f(0.5f,  -0.5f);
        glEnd();*/

        /**
        * 2- TELL OPENGL HOW THE DATA IS LAYED OUT
        **/
        //Draw call type 1 : Type of primitive, start index of vertices, number of vertices
        glDrawArrays(GL_TRIANGLES, 0, 3); //used when we don't have an index buffer; draws from the last bound buffer (step 1)
        //Draw call type 2 : Type of primitive, number of vertices, type of index data
        //glDrawElements(GL_TRIANGLES, 3, ); //used with index buffer

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}