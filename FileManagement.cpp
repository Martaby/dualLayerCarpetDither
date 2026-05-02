#include "FileManagement.h"

#include <fstream>

void readOptionFile(Process* process) {
	std::ifstream file("Options.txt");

	if (!file) {
		std::cerr << "Unable to open Option.txt, writing it with base options." << std::endl;
		std::ofstream createOptions("Options.txt");
		createOptions << "Number of maps horizontally(x) then vertically(y)\n- nx : 3\n- ny : 2\n\nImage offset along x, then y(note that with no zoom, only one axis will be affected by the offset)\n- xOffset : 0\n- yOffset : 0\n\nZoom on the image, between 1 (no zoom) and ..............\n- zoom = 1\n\nLimited staircase height\n- maxheight : 2\n\nRainbow support blocks(for a very specific use case, not required)\n- rainbowSupport : true\n\nBest flat color match for each pixel(just for testing, bad results)\n- noDither : false\n\nMaximum recursion depth, larger values should slightly increase the accuracy of the result but takes much longer to compute\n- maxCache : 1000";
		createOptions.close();
		return;
	}

	// Reading options from file
	std::string line;

	//while (std::getline(file, line)) {
	//	if (line == std::string("[General]")) {
	//		sectionType = 0;
	//		change = true;
	//	}
	//}
	file.close();
}





NBT::NBT() {

}