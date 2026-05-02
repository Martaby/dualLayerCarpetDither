#include "Utils.h"




LinearAllocator linearAlloc(100000000);



LinearAllocator::LinearAllocator(size_t _length) {
	data = malloc(_length);
	length = _length;
	idx = 0;
};

LinearAllocator::~LinearAllocator() {
	free(data);
}

void* LinearAllocator::alloc(size_t l) {
	if (l + idx > length) {
		throw std::runtime_error("TOO BIG.");
	}
	void* ret = (void*)(((char*)data) + idx);
	idx += l;
	return ret;
}

void LinearAllocator::reset() {
	idx = 0;
};









void Img::bilinear(Img* initImg, float factor, int xOffset, int yOffset) {
	// Resize image to fit the map size using bilinear interpolation filtering for upsampling

	//https://eng.aurelienpierre.com/2020/03/bilinear-interpolation-on-images-stored-as-python-numpy-ndarray/

	unsigned char* initPixels = initImg->pixels;
	int xprev, yprev, xnext, ynext, val;
	float xout, yout, xin, yin, dynext, dxnext;

	int newwidth = initImg->width / factor;
	int newheight = initImg->height / factor;

	int xgap = (newwidth == width) ? 0 : xOffset * (newwidth - width) / 100;
	int ygap = (newheight == height) ? 0 : yOffset * (newheight - height) / 100;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			xout = (float)(x+xgap) / (float)newwidth;
			yout = (float)(y+ygap) / (float)newheight;

			xin = xout * (float)initImg->width;
			yin = yout * (float)initImg->height;

			xprev = std::ceil(xin);
			yprev = std::ceil(yin);
			xnext = xprev + 1;
			ynext = yprev + 1;

			xprev = (xprev < initImg->width) ? xprev : initImg->width - 1;
			xnext = (xnext < initImg->width) ? xnext : initImg->width - 1;
			yprev = (yprev < initImg->height) ? yprev : initImg->height - 1;
			ynext = (ynext < initImg->height) ? ynext : initImg->height - 1;

			dynext = ynext - yin;
			dxnext = xnext - xin;

			for (int col = 0; col < 3; col++) {
				val = dynext * (dxnext * initPixels[4 * (yprev * initImg->width + xprev) + col] + (1 - dxnext) * initPixels[4 * (yprev * initImg->width + xnext) + col]) + (1 - dynext) * (dxnext * initPixels[4 * (ynext * initImg->width + xprev) + col] + (1 - dxnext) * initPixels[4 * (ynext * initImg->width + xnext) + col]);
				val = (val < 0) ? 0 : val;
				val = (val > 255) ? 255 : val;
				pixels[4 * (y * width + x) + col] = (unsigned char)val;
			}
		}
	}
}



void Img::average(Img* initImg, float factor, int xOffset, int yOffset) {
	// Resize image to fit the map size using average of covered pixels for downsampling

	int pixelSum = 0;
	int nbPixels = 0;

	int newwidth = initImg->width / factor;
	int newheight = initImg->height / factor;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {



			for (int col = 0; col < 3; col++) {
				pixels[4 * (y * width + x) + col] = 0;
			}
		}
	}
}



Block::Block(std::string _id, rgb _light, rgb _flat, rgb _dark) {
	id = _id;
	light = _light;
	flat = _flat;
	dark = _dark;
}

void Block::display() {
	std::cout << id << ", light shade : "; light.display(); std::cout << ", flat shade : "; flat.display(); std::cout << ", dark shade : "; dark.display();
}





//void Palette::computeGamut(rgb* colors, rgb* gamut) {
//	// Find the brightest and darkest value for R G and B channels in the given palette
//	rgb brighest(0, 0, 0);
//	rgb darkest(255, 255, 255);
//	for (int col = 0; col < nbCol; col++) {
//		for (int shade = 0; shade < 3; shade++) {
//			if (colors[col]._rgb[shade] > brighest._rgb[shade])
//				brighest._rgb[shade] = colors[col]._rgb[shade];
//			if (colors[col]._rgb[shade] < darkest._rgb[shade])
//				darkest._rgb[shade] = colors[col]._rgb[shade];
//		}
//	}
//	gamut[0] = darkest;
//	gamut[1] = brighest;
//}


//void Palette::computeAllGamuts() {
//	computeGamut(bottom_flat, bfb_gamut);
//	computeGamut(top_light, tfb_gamut);
//	computeGamut(bottom_dark, bft_gamut);
//}

void Palette::flatten(float* pixelArray, rgb* colors) {
	for (int col = 0; col < nbCol; col++) {
		for (int i = 0; i < 3; i++) {
			pixelArray[3*col + i] = (colors[col]._rgb[i]);
		}
	}
}

Palette::Palette() {}					// PAS BON DOIT INITIALISER LES VARIABLES, MAIS JAMAIS APPELE

Palette::Palette(std::vector<Block> _blocks) {

	nbCol = _blocks.size();
	bottom_flat = (rgb*)malloc(nbCol * sizeof(rgb));
	top_light = (rgb*)malloc(nbCol * sizeof(rgb));
	bottom_dark = (rgb*)malloc(nbCol * sizeof(rgb));
	for (int i = 0; i < nbCol; i++) {
		bottom_flat[i] = _blocks[i].flat;
		top_light[i] = _blocks[i].light;
		bottom_dark[i] = _blocks[i].dark;
	}
	
	//bfb_gamut = (rgb*)malloc(2 * sizeof(rgb));
	//tfb_gamut = (rgb*)malloc(2 * sizeof(rgb));
	//bft_gamut = (rgb*)malloc(2 * sizeof(rgb));
	//computeAllGamuts();


	bfb = (float*)malloc(nbCol * 3 * sizeof(float));
	tfb = (float*)malloc(nbCol * 3 * sizeof(float));
	bft = (float*)malloc(nbCol * 3 * sizeof(float));
	flatten(bfb, bottom_flat);
	flatten(tfb, top_light);
	flatten(bft, bottom_dark);

	blocks = _blocks;
}


rgb Palette::cap(rgb* color) {
	rgb retCol = *color;
	for (int i = 0; i < 3; i++) {
		if (retCol._rgb[i] < 0)
			retCol._rgb[i] = 0;
		if (retCol._rgb[i] > 255)
			retCol._rgb[i] = 255;
	}
	return retCol;
}


void Palette::errDiff(float* err, rgb* diff, rgb* c1, rgb* c2) {
	*err = (c1->_rgb[0] - c2->_rgb[0]) * (c1->_rgb[0] - c2->_rgb[0]) + (c1->_rgb[1] - c2->_rgb[1]) * (c1->_rgb[1] - c2->_rgb[1]) + (c1->_rgb[2] - c2->_rgb[2]) * (c1->_rgb[2] - c2->_rgb[2]);
	*diff = rgb(c1->_rgb[0]- c2->_rgb[0], c1->_rgb[1] - c2->_rgb[1], c1->_rgb[2] - c2->_rgb[2]);
}

void Palette::findBest(int* bestCol, float* bestErr, rgb* bestDiff, rgb* color, int shade) {
	// Old implementation, loops through all colors
	// Should create a search tree for binary search to reduce complexity
	// Problem is equivalent to closest neighbour search in 3D space
	float err = 0;
	rgb diff;
	rgb capped = cap(color);

	*bestDiff = rgb(0, 0, 0);
	*bestErr = 9999999;
	*bestCol = 0;
	float* pal;
	if (shade == 0) {
		pal = tfb;
	}
	else if (shade == 1) {
		pal = bfb;
	}
	else {
		pal = bft;
	}
	rgb col_i;
	for (int i = 0; i < nbCol * 3; i+=3) {
		col_i._rgb[0] = pal[i];
		col_i._rgb[1] = pal[i + 1];
		col_i._rgb[2] = pal[i + 2];
		errDiff(&err, &diff, &capped, &col_i);
		if (err < *bestErr) {
			*bestErr = err;
			*bestCol = i/3;
			*bestDiff = diff;
		}
	}
}


void readOptionFile(Process* process) {
	std::ifstream file("Options.txt");

	if (!file) {
		std::cerr << "Unable to open Option.txt, writing it with base options." << std::endl;
		std::ofstream createOptions("Options.txt");
		// Writing options to file
		createOptions << "Number of maps horizontally(x) then vertically(y)\n- nx : 3\n- ny : 2\n\nCrop (if set to false, image will be stretched to fit the total map size)\n- crop : 1\n\nImage offset % along x, then y (With no zoom, only one axis will be affected by the offset, value between 0 and 100)\n- xOffset : 0\n- yOffset : 0\n\nZoom on the image, between 1 (no zoom) and ..............\n- zoom = 1\n\nLimited staircase height\n- maxheight : 2\n\nRainbow support blocks(for a very specific use case, not required)\n- rainbowSupport : 1\n\nBest flat color match for each pixel(just for testing, bad results)\n- noDither : 0\n\nMaximum recursion depth, larger values should slightly increase the accuracy of the result but takes much longer to compute\n- maxCache : 1000";
		createOptions.close();
		return;
	}

	// Reading options from file
	std::string line;

	
	try {
		while (std::getline(file, line)) {
			if (line.substr(0, 4) == std::string("- nx")) {
				process->nx = std::stoi(line.substr(7));
			}
			else if (line.substr(0, 4) == std::string("- ny")) {
				process->ny = std::stoi(line.substr(7));
			}
			else if (line.substr(0, 6) == std::string("- crop")) {
				process->crop = (std::stoi(line.substr(9))) ? true : false;
			}
			else if (line.substr(0, 9) == std::string("- xOffset")) {
				process->xOffset = std::stoi(line.substr(12));
			}
			else if (line.substr(0, 9) == std::string("- yOffset")) {
				process->yOffset = std::stoi(line.substr(12));
			}
			else if (line.substr(0, 6) == std::string("- zoom")) {
				process->zoom = std::stoi(line.substr(9));
			}
			else if (line.substr(0, 11) == std::string("- maxheight")) {
				process->maxheight = std::stoi(line.substr(14));
			}
			else if (line.substr(0, 16) == std::string("- rainbowSupport")) {
				process->rainbowSupport = (std::stoi(line.substr(19))) ? true : false;
			}
			else if (line.substr(0, 10) == std::string("- noDither")) {
				process->noDither = (std::stoi(line.substr(13))) ? true : false;
			}
			else if (line.substr(0, 10) == std::string("- maxCache")) {
				process->maxCache = std::stoi(line.substr(13));
			}
		}
	}
	catch (...) {
		std::cout << "Error reading Options.txt. Try changing the values to the correct types or delete the file to reset base parameters next run\nUsing the base parameters instead.\n";
	}
	file.close();
}







Result::Result() {}


Result::Result(bool _success) {
	success = _success;
}






Process::Process(Palette* _palette) {
	palette = _palette;
}

void Process::findFileName(std::string path) {
	int lastIndex = 0;
	for (int i = 0; i < path.size(); i++) {
		if (path[i] == '\\') {
			lastIndex = i;
		}	
	}
	fileName = path.substr(lastIndex+1);
}


void Process::decodePng(const char* path) {
	// Reads image as std::vector, 4 bytes per pixel, R-G-B-A
	findFileName(path);
	initImg = new Img();
	unsigned error = lodepng_decode32_file(&initImg->pixels, &initImg->width, &initImg->height, path);
	if (error) std::cout << "Png decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

void Process::encodePng(const char* path) {
	unsigned error = lodepng_encode32_file(path, ditheredImg->pixels, ditheredImg->width, ditheredImg->height);
	if (error) std::cout << "Png encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

void Process::preprocess() {
	readOptionFile(this);
	//preprocessedImg = new Img(initImg->pixels, nx * 128, ny * 128);
	preprocessedImg = new Img();
	preprocessedImg->pixels = (unsigned char*)malloc(nx*128*ny*128*4*sizeof(unsigned char));
	preprocessedImg->width = 128 * nx;
	preprocessedImg->height = 128 * ny;
	
	
	if (crop) {	// Crop
		std::cout << "Cropping image\n";

		float factor = (std::min)(initImg->width / (float)(128 * nx), initImg->height / (float)(128 * ny));
		if (factor > 1) {	// Down sample using the average of the colors in the area each pixel covers
			//preprocessedImg->average(initImg, factor, xOffset, yOffset);
			preprocessedImg->bilinear(initImg, factor, xOffset, yOffset);
		}
		else {	// Up sample
			preprocessedImg->bilinear(initImg, factor, xOffset, yOffset);
		}
		
	}
	else {	// Stretch

	}
}






std::unique_ptr<Result> Process::memoCollumnDitherDiffuse(int pos, int end, int lastHeight, int lastBlock, rgb lastErr) {
	
	if (pos == end) {
		return std::make_unique<Result>(true);
	}

	rgb pixel((float)currentColumn[pos*3] + errorMap[pos*3] + lastErr._rgb[0],
			  (float)currentColumn[pos*3+1] + errorMap[pos*3+1] + lastErr._rgb[1],
			  (float)currentColumn[pos*3+2] + errorMap[pos*3+2] + lastErr._rgb[2]);

	//CacheKey cachekey(pos, lastHeight, lastBlock, pixel);
	//if (memocache.contains(cachekey)) {
	//	//std::cout << "Cache reused" << std::endl;
	//	return memocache[cachekey];
	//}


	if (memoSize > maxCache) {
		return std::make_unique<Result>(false);
	}

	int bestColSame, bestColBelow, bestColAbove;
	float bestErrSame, bestErrBelow, bestErrAbove;
	rgb bestDiffSame, bestDiffBelow, bestDiffAbove;
	palette->findBest(&bestColBelow, &bestErrBelow, &bestDiffBelow, &pixel, 0);
	palette->findBest(&bestColSame, &bestErrSame, &bestDiffSame, &pixel, 1);
	palette->findBest(&bestColAbove, &bestErrAbove, &bestDiffAbove, &pixel, 2);

	int bestBlock = 0;
	int bestHeight = 0;
	float bestErr = 99999999999;
	rgb bestDiff;
	std::unique_ptr<Result> bestCont = std::make_unique<Result>(true);



	for (int height = 0;height < maxheight; height++) {
		float err = 0;
		rgb diff;
		int block = 0;

		if (lastHeight == -1) {
			err = 0;
			diff = rgb();
			block = 0;
		}
		else if (height == lastHeight) {
			err = bestErrSame;
			diff = bestDiffSame;
			block = bestColSame;
		}
		else if (height > lastHeight) {
			err = bestErrBelow;
			diff = bestDiffBelow;
			block = bestColBelow;
		}
		else if (height < lastHeight) {
			err = bestErrAbove;
			diff = bestDiffAbove;
			block = bestColAbove;
		}

		rgb nextErr = rgb(diff._rgb[0] * 7/16, diff._rgb[1] * 7/16, diff._rgb[2] * 7/16);	// South error diffusion (Floyd Steinberg)

		//Result continuation = memoCollumnDitherDiffuse(pos+1, end, height, block, nextErr);
		std::unique_ptr<Result> continuation = memoCollumnDitherDiffuse(pos + 1, end, height, block, nextErr);
		// Necessaire d'utiliser le = ? set a la place pour gérer avec pointeurs

		if (!continuation->success) {
			return std::make_unique<Result>(false);
		}

		if (continuation->bestErr + err < bestErr) {
			bestBlock = block;
			bestErr = continuation->bestErr + err;
			bestDiff = diff;
			bestHeight = height;
			bestCont = std::move(continuation);
		}
	}
	bestCont->bestBlocks.push_back(bestBlock);
	bestCont->bestHeights.push_back(bestHeight);
	bestCont->bestDiffs.push_back(bestDiff);

	bestCont->bestErr = bestErr;

	//memocache.insert({ cachekey , bestCont });
	memoSize += 1;

	return std::move(bestCont);
}




struct Tuple{
	int first;
	int second;
};



bool Process::dither() {
	ditheredImg = new Img(*preprocessedImg);	// Copy preprocessed img as initializer
	// Initialize the image to black with no transparency
	for (int progress = 0; progress < (int)ditheredImg->width; progress++) {
		for (int row = 0; row < (int)ditheredImg->height; row++) {
			for (int i = 0; i < 3; i++) {
				ditheredImg->pixels[4 * (row * ditheredImg->width + progress)+i] = 0;
			}
			ditheredImg->pixels[4 * (row * ditheredImg->width + progress) + 3] = 255;
		}
	}



	if (noDither) {					// Best match for each pixel, no dithering
		rgb currentColor;
		static float bestErr;
		static rgb bestDiff;
		static int bestCol;
		for (int progress = 0; progress < (int)ditheredImg->width; progress++) {
			for (int row = 0; row < (int)ditheredImg->height; row++) {
				currentColor = rgb(preprocessedImg->pixels[4 * (row * ditheredImg->width + progress)], preprocessedImg->pixels[4 * (row * ditheredImg->width + progress) + 1], preprocessedImg->pixels[4 * (row * ditheredImg->width + progress) + 2]);
				palette->findBest(&bestCol, &bestErr, &bestDiff, &currentColor, 1);
				for (int i = 0; i < 3; i++) {
					ditheredImg->pixels[4 * (row * ditheredImg->width + progress) + i] = palette->bfb[3 * bestCol + i];
				}
			}
		}

		return true;
	}


	else // Floyd Steinberg dithering
	{
		



		currentColumn = (unsigned char*)malloc((int)(ditheredImg->height + 2) * 3 * sizeof(unsigned char));

		float* nextErrorMap = (float*)malloc((int)(ditheredImg->height + 2) * 3 * sizeof(float));
		errorMap = (float*)malloc((int)(ditheredImg->height + 2) * 3 * sizeof(float));
		for (int i = 0; i < (int)(ditheredImg->height+2)*3; i++) {
			nextErrorMap[i] = 0;
		}
		
		rgb diff;
		int totalErr = 0;

		int* finalBlockMap = (int*)malloc((int)preprocessedImg->width * ((int)preprocessedImg->height + 1) * sizeof(int));	// Column per column and padding
		int* finalHeightMap = (int*)malloc((int)preprocessedImg->width * ((int)preprocessedImg->height + 1) * sizeof(int));	// Same
		int* blockmap = (int*)malloc(((int)preprocessedImg->height + 2) * sizeof(int));
		int* heightmap = (int*)malloc(((int)preprocessedImg->height + 2) * sizeof(int));


		srand(0);	// Same seed everytime


		for (int progress = 0; progress < (int)preprocessedImg->width; progress++) {	// For each column of the map

			currentColumn[0] = 0;	// Noobline padding
			currentColumn[1] = 0;
			currentColumn[2] = 0;
			for (int row = 0; row < (int)preprocessedImg->height; row++) {
				for (int i = 0; i < 3; i++) {
					currentColumn[3 + 3*row + i] = preprocessedImg->pixels[4 * (row * preprocessedImg->width + progress) + i];
				}
			}
			currentColumn[3 + (int)preprocessedImg->height * 3] = 0;	// Bottom padding for recursion
			currentColumn[4 + (int)preprocessedImg->height * 3] = 0;
			currentColumn[5 + (int)preprocessedImg->height * 3] = 0;


			blockmap[0] = progress % 16 - 1;	// Noobline block, rainbow

			
			for (int i = 0; i < (int)(ditheredImg->height + 2) * 3; i++) {
				errorMap[i] = nextErrorMap[i];
				nextErrorMap[i] = 0;
			}
			std::unique_ptr<Result> result = std::make_unique<Result>(true);

			int lastHeight = -1;
			rgb lastErr;

			std::vector<Tuple> subdivisions;
			Tuple firstEl = { .first = 0, .second = (int)preprocessedImg->height + 1 };	// currentColumn.size()/3 - 1
			subdivisions.push_back(firstEl);

			while (subdivisions.size() > 0) {
				Tuple section = subdivisions.back();
				subdivisions.pop_back();

				int size = section.second - section.first;

				if (size > maxDepth) {		// Too tall to process
					std::cout << "Too tall to process\n";
					result->success = false;
				}
				else {
					//memocache.clear();
					memoSize = 0;
					result = memoCollumnDitherDiffuse(section.first, section.second, lastHeight, -1, lastErr);
				}

				if (result->success == false) {
					//std::cout << "Subdivided\n";
					int pivot = section.first + (int)(size/4) + rand() % (int)(size/2);
					Tuple firstSection = {.first = pivot, .second = section.second};
					Tuple secondSection = { .first = section.first, .second = pivot };
					subdivisions.push_back(firstSection);
					subdivisions.push_back(secondSection);
				}
				else {
					lastHeight = result->bestHeights[0];
					totalErr += result->bestErr;
					
					for (int i = section.first; i < section.second; i++) {
						blockmap[i] = result->bestBlocks.pop_back();
						heightmap[i] = result->bestHeights.pop_back();
						diff = result->bestDiffs.pop_back();

						//blockmap[i] = result->bestBlocks.back();
						//result->bestBlocks.pop_back();
						//heightmap[i] = result->bestHeights.back();
						//result->bestHeights.pop_back();
						//diff = result->bestDiffs.back();
						//result->bestDiffs.pop_back();

						int pos = 3 * i;

						errorMap[pos + 3] += diff._rgb[0] * 7 / 16; // South
						errorMap[pos + 4] += diff._rgb[1] * 7 / 16;
						errorMap[pos + 5] += diff._rgb[2] * 7 / 16;

						nextErrorMap[pos - 3] += diff._rgb[0] * 3 / 16; // North East
						nextErrorMap[pos - 2] += diff._rgb[1] * 3 / 16;
						nextErrorMap[pos - 1] += diff._rgb[2] * 3 / 16;

						nextErrorMap[pos] += diff._rgb[0] * 5 / 16;	// East
						nextErrorMap[pos + 1] += diff._rgb[1] * 5 / 16;
						nextErrorMap[pos + 2] += diff._rgb[2] * 5 / 16;

						nextErrorMap[pos + 3] += diff._rgb[0] * 1 / 16;	// South East
						nextErrorMap[pos + 4] += diff._rgb[1] * 1 / 16;
						nextErrorMap[pos + 5] += diff._rgb[2] * 1 / 16;
					}
				}
			}

			for (int row = 0; row < (int)preprocessedImg->height + 1; row++) {
				finalBlockMap[progress * ((int)preprocessedImg->height + 1) + row] =  blockmap[row];
				finalHeightMap[progress * ((int)preprocessedImg->height + 1) + row] = heightmap[row];
			}

			std::cout << progress + 1 << "/" << preprocessedImg->width << " Done\n";
			linearAlloc.reset();
		}

		//std::cout << "Nb de vecteurs créés : " << instantiations << "\n";

		free(nextErrorMap);
		free(errorMap);
		free(currentColumn);
		free(blockmap);
		free(heightmap);

		// Saving image and NBT
		for (int column = 0; column < (int)ditheredImg->width; column++) {

			int lastheight = finalHeightMap[column * ((int)ditheredImg->height + 2)];

			for (int row = 0; row < (int)ditheredImg->height; row++) {
				int blocktype = finalBlockMap[column * ((int)ditheredImg->height + 1) + row + 1];
				int height = finalHeightMap[column * ((int)ditheredImg->height + 1) + row + 1];
				
				int pixelPos = 4 * (row * ditheredImg->width + column);

				for (int i = 0; i < 3; i++) {	// Compact writing, but slower execution IMPROVE ON THIS BY COPY PASTING LINES OF CODE
					if (lastheight == height) { // Flat shade
						ditheredImg->pixels[pixelPos + i] = palette->blocks[blocktype].flat._rgb[i];
					}
					else if (lastheight < height) {	// Light shade
						ditheredImg->pixels[pixelPos + i] = palette->blocks[blocktype].light._rgb[i];
					}
					else {	// Dark shade
						ditheredImg->pixels[pixelPos + i] = palette->blocks[blocktype].dark._rgb[i];
					}
				}
				lastheight = height;
			}
		}

		free(finalBlockMap);
		free(finalHeightMap);
	}

	return true;
}
