#include <Systems\Window\Window.hpp>

int Window::globalID = -1;

Window::Window()
{
	globalID++;
	CurrentID = globalID;
}
