#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main(){
    GLFWwindow* window;
    if(!glfwInit()){
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Linux!", NULL, NULL);
    glfwMakeContextCurrent(window);

    while(~glfwWindowShouldClose(window)){
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}