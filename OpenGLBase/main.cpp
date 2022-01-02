#include "Teapot.h"
#include "Application.h"


int main( int argc, char * argv[] ) {
	Application::Init();

	Teapot tp_engine;
	tp_engine.Run();

	Application::Terminate();
	return 0;
}