#include "ARBpch.h"
#include "Engine.h"

int main() {
	ARB::Engine* ourApp = new ARB::Engine(1000, 800, "Arbitronic Editor", 50, 100);
	ourApp->runEditor();
	delete ourApp;
	return 0;
}
