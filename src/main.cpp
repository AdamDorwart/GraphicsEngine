#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>


void ErrorCallback(int error, const char* description)
{
    fputs(description, stderr);
}

int main(int argc, char *argv[])
{
	// Init GLFW
	if (!glfwInit()) {
    	exit(EXIT_FAILURE);	
    }
    // Set error callback for GLFW
    glfwSetErrorCallback(ErrorCallback);

    // Create window
    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!window) {
	    glfwTerminate();
	    exit(EXIT_FAILURE);
	}
	// Set the OpenGL context
	glfwMakeContextCurrent(window);

	// Run until user closes the window
	while (!glfwWindowShouldClose(window))
    {
        
        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
