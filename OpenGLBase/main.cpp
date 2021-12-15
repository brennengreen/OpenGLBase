#include "Teapot.h"
#include "Application.h"


int main( int argc, char * argv[] ) {
	Application::Init();

	Teapot tp_engine;
	tp_engine.Draw();

	Application::Terminate();
	return 0;
}