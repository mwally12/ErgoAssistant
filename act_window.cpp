#include <iostream>
#include <fstream> /*Para manejar archivos*/
#include <string>
#include <unistd.h>
#include <cstdlib>

using namespace std;



int main(){
	ofstream myfile;
  	myfile.open ("example.txt");
	for (int i = 0 ; i < 10 ; i++) {
    	sleep(1); 
    	system("cat /proc/$(xdotool getwindowpid $(xdotool getwindowfocus))/comm");
    	//otra opcion: system(xdotool getwindowname $(xdotool getwindowfocus))
    	myfile << "Writing this to a file.\n";
  	}
  	myfile.close();
  	cout << "Finalizando programa" << endl;
  	return 0;
}
