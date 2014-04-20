#include <iostream>
#include <string>

#include "../include/read_write.h"
#include "../include/sorters.h"

using namespace std;

int main(const int argc, const char* argv[]) {
	if(argc != 4) {
		cout << "usage: pixelsort [average|dark|light] [source.jpg] [destination.jpg]" << endl;
		return 1;
	}

	struct Image * image = read_image(argv[2]);

	string method(argv[1]);
	if("dark" == method) {
		dark_sorter(image, 45);
	} else if("light" == method) {
		light_sorter(image, 210);
	} else {
		average_sorter(image);
	}

	write_image(image, argv[3]);
}
