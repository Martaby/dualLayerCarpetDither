#include "Utils.h"



int main()
{
	std::vector<Block> blocks = { Block("minecraft:white_carpet", rgb(255, 255, 255), rgb(220, 220, 220),rgb(180, 180, 180)),
								 Block("minecraft:light_gray_carpet", rgb(153, 153, 153), rgb(132, 132, 132), rgb(108, 108, 108)),
								 Block("minecraft:gray_carpet", rgb(76, 76, 76), rgb(65, 65, 65), rgb(53, 53, 53)),
								 Block("minecraft:black_carpet", rgb(25, 25, 25), rgb(21, 21, 21), rgb(17, 17, 17)),
								 Block("minecraft:brown_carpet", rgb(102, 76, 51), rgb(88, 65, 44), rgb(72, 53, 36)),
								 Block("minecraft:red_carpet", rgb(153, 51, 51), rgb(132, 44, 44), rgb(108, 36, 36)),
								 Block("minecraft:pink_carpet", rgb(242, 127, 165), rgb(208, 109, 142), rgb(170, 89, 116)),
								 Block("minecraft:orange_carpet", rgb(216, 127, 51), rgb(186, 109, 44), rgb(152, 89, 36)),
								 Block("minecraft:yellow_carpet", rgb(229, 229, 51), rgb(197, 197, 44), rgb(161, 161, 36)),
								 Block("minecraft:green_carpet", rgb(102, 127, 51), rgb(88, 109, 44), rgb(72, 89, 36)),
								 Block("minecraft:lime_carpet", rgb(127, 204, 25), rgb(109, 176, 21), rgb(89, 144, 17)),
								 Block("minecraft:blue_carpet", rgb(51, 76, 178), rgb(44, 65, 153), rgb(36, 53, 125)),
								 Block("minecraft:cyan_carpet", rgb(76, 127, 153), rgb(65, 109, 132), rgb(53, 89, 108)),
								 Block("minecraft:light_blue_carpet", rgb(102, 153, 216), rgb(88, 132, 186), rgb(72, 108, 152)),
								 Block("minecraft:purple_carpet", rgb(127, 63, 178), rgb(109, 54, 153), rgb(89, 44, 125)),
								 Block("minecraft:magenta_carpet", rgb(178, 76, 216), rgb(153, 65, 186), rgb(125, 53, 152)) };


	Palette palette(blocks);

	std::cout << "Enter image path, or drag and drop the file on the command line / terminal.\n The file MUST BE A PNG." << std::endl;
	std::string imgPath;
	std::cin >> imgPath;

	Process process(&palette);

	process.decodePng(imgPath.c_str());

	process.preprocess();

	std::chrono::time_point<std::chrono::system_clock> t0 = std::chrono::system_clock::now();
	if (process.dither()) {
		std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::now();
		std::cout << "Dither sucessful, completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << "ms." << std::endl;
		
		std::time_t endT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char time[26];
		ctime_s(time, sizeof time, &endT);

		std::chrono::time_point t = std::chrono::system_clock::now();
		// Create a folder to place the newly generated files

		std::string newName = std::format("{:%Y%m%d%H%M}", t) + "_" + std::to_string(process.maxheight) + "layers_" + process.fileName;
		std::string folder = "output/" + newName;
		#ifdef _WIN32
			_mkdir(folder.c_str());
		#elif defined(__linux__)
			mkdir(folder.c_str(), 0777);	// Not tested for linux (0777 might be giving too many perms...)
		#endif
		process.encodePng((folder+"/"+newName).c_str());
		std::cout << "Files written in : " << folder << std::endl;
	}
}