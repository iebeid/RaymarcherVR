#include <iostream>
#include "App.h"
using namespace std;
int main(int argc, char * argv[]){
	cout << "Initialize Main App" << endl;
	MainApp::getInstance().run(argc, argv);
	return 0;
}