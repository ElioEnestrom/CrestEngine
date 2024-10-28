#include <iostream>
#include <glfw3.h>

int main() 
{

	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	GLFWwindow* window;
	window = glfwCreateWindow(640, 480, "Hello, World!", NULL, NULL);

	glfwMakeContextCurrent(window);

	while (true)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glClearColor(0.4, 0.3, 0.2, 1);

		glfwSwapBuffers(window);
	}

	return 0;
}