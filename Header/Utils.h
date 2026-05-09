#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <bitset>
#include <format>
#include <string>
#include <Windows.h>
#include <direct.h>
#include <fstream>
#include <cmath>
#include <exception>
#include <memory>

#include <unordered_map>

#include "lodepng.h"
#include "nbtBuilder.h"




class LinearAllocator {
public:

	LinearAllocator(size_t _length);
	~LinearAllocator();

	void* alloc(size_t l);

	void reset();

	void* data;
	size_t length;
	size_t idx;
};



extern LinearAllocator linearAlloc;




template <typename T>
class Vector {
public:

	Vector(size_t initSize = 10) {
		//data = new T[initSize];
		data = (T*)linearAlloc.alloc(initSize * sizeof(T));
		length = initSize;
		idx = 0;
	};

	Vector(const Vector<T>& _vec) {
		//data = new T[_vec.length];
		data = (T*)linearAlloc.alloc(_vec.length * sizeof(T));
		length = _vec.length;
		idx = _vec.idx;
		memmove(data, _vec.data, idx * sizeof(T));
		//for (size_t i = 0; i < idx; i++) {
		//	data[i] = _vec.data[i];
		//}
	}

	~Vector() {
		//delete[] data;
	}


	T& operator[](size_t _idx) const
	{
		if (_idx < 0 || _idx >= idx) {
			throw std::runtime_error("Index out of bounds.");
		}
		return data[_idx];
	}

	Vector<T>& operator=(const Vector<T>& _vec)
	{
		if (&_vec != this) {
			//delete[] data;
			//data = new T[_vec.length];
			data = (T*)linearAlloc.alloc(_vec.length * sizeof(T));
			length = _vec.length;
			idx = _vec.idx;
			memmove(data, _vec.data, idx * sizeof(T));
			//for (size_t i = 0; i < idx; i++) {
			//	data[i] = _vec.data[i];
			//}
		}
		return *this;
	}


	void resize(size_t newSize) {
		if (newSize == length) {
			return;
		}
		if (idx > newSize) {
			throw std::runtime_error("New size is too short.");
		}

		//T* dataTemp = new T[newSize];
		T* dataTemp = (T*)linearAlloc.alloc(newSize * sizeof(T));
		memmove(dataTemp, data, idx * sizeof(T));
		//for (size_t i = 0; i < idx; i++) {
		//	dataTemp[i] = data[i];
		//}
		//delete[] data;
		data = dataTemp;
		length = newSize;
	}

	void push_back(const T& element) {
		idx += 1;
		if (idx >= length) {
			resize(length * 2);

			//if (idx > instantiations) instantiations = idx;
		}
		data[idx - 1] = element;
	}
	
	T back() {
		if (idx > 0) {
			return data[idx - 1];
		}
		throw std::runtime_error("Empty vector has no back.");
	}

	T pop_back() {
		if (idx > 0) {
			idx--;
			return data[idx];
		}
		throw std::runtime_error("Empty vector has no back.");
	}


	T* data;
	size_t length;
	size_t idx;
};



class rgb
{
public:
	rgb() {
		_rgb[0] = 0;
		_rgb[1] = 0;
		_rgb[2] = 0;
	};

	rgb(float _r, float _g, float _b) {
		_rgb[0] = _r;
		_rgb[1] = _g;
		_rgb[2] = _b;
	}

	void display() {
		std::cout << "(" << _rgb[0] << ", " << _rgb[1] << ", " << _rgb[2] << ")";
	}

	bool operator==(const rgb& _pixel) const
	{
		return (_rgb[0] == _pixel._rgb[0] && _rgb[1] == _pixel._rgb[1] && _rgb[2] == _pixel._rgb[2]);
	}


	float _rgb[3];
};





class Img {
public:
	Img(){
		width = 0;
		height = 0;
	}

	Img(unsigned char* _pixels, unsigned _width, unsigned _height) {
		width = _width;
		height = _height;
		pixels = (unsigned char*)malloc(width * height * 4 * sizeof(unsigned char));
		if (pixels) {
			for (int i = 0; i < width * height * 4; i++) {
				pixels[i] = _pixels[i];
			}
		}
		else {
			std::cout << "Error allocating memory for the image.\n";
		}
	}

	Img(Img& _img) {
		width = _img.width;
		height = _img.height;
		pixels = (unsigned char*)malloc(width * height * 4 *sizeof(unsigned char));
		if (pixels) {
			for (int i = 0; i < width * height * 4; i++) {
				pixels[i] = _img.pixels[i];
			}
		}
		else {
			std::cout << "Error allocating memory for the image.\n";
		}
	}

	~Img() {
		free(pixels);
	}

	void bilinear(Img* initImg, float factor, int xOffset, int yOffset);
	void average(Img* initImg, float factor, int xOffset, int yOffset);

	unsigned width;
	unsigned height;
	unsigned char* pixels = 0;	// Contains width * height * 4 chars
};



class Block {
public:
	Block(std::string _id, rgb _light, rgb _flat, rgb _dark);
	void display();

	std::string id;
	rgb light;
	rgb flat;
	rgb dark;
};

class Palette {
public:
	Palette();
	Palette(std::vector<Block> _blocks);
	~Palette() {

		free(bfb);
		free(tfb);
		free(bft);

		free(bottom_flat);
		free(top_light);
		free(bottom_dark);
	}

	void flatten(float* pixelArray, rgb* colors);
	rgb cap(rgb* color);
	void errDiff(float* err, rgb* diff, rgb* c1, rgb* c2);
	void findBest(int* bestCol, float* bestErr, rgb* bestDiff, rgb* color, int shade);

	std::vector<Block> blocks;

	std::vector<Block> usedBlocks;

	rgb* bottom_flat;
	rgb* top_light;
	rgb* bottom_dark;
	int nbCol;

	float* bfb;	// Below from below		[r0,g0,b0,r1,g1,b1,...]
	float* tfb;	// Top from below
	float* bft;	// Below from top
};






class CacheKey {
public:
	CacheKey(int _pos,	int _lastHeight, int _lastBlock, rgb _pixel) {
		pos = _pos;
		lastHeight = _lastHeight;
		lastBlock = _lastBlock;
		pixel = _pixel;
	};

	bool operator==(const CacheKey& _key) const
	{
		return (pos == _key.pos && lastHeight == _key.lastHeight && lastBlock == _key.lastBlock && pixel == _key.pixel);
	}

	int pos;
	int lastHeight;
	int lastBlock;
	rgb pixel;			// Pourri car doubles : pas d'égalité possible (ou quasi pas)
};

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <>
struct std::hash<CacheKey>
{
	std::size_t operator()(const CacheKey& k) const
	{
		std::size_t seed = 0;
		hash_combine(seed, k.pos);
		hash_combine(seed, k.lastHeight);
		hash_combine(seed, k.lastBlock);
		hash_combine(seed, k.pixel._rgb[0]);
		hash_combine(seed, k.pixel._rgb[1]);
		hash_combine(seed, k.pixel._rgb[2]);	// Pretty bad hash...
		return seed;
	}
};

//struct compareCacheKey {
//	bool operator() (const CacheKey& a, const CacheKey& b) const {
//		return (a.pos < b.pos) ||
//			(a.pos == b.pos && a.lastHeight < b.lastHeight) ||
//			(a.pos == b.pos && a.lastHeight == b.lastHeight && a.lastBlock < b.lastBlock) ||
//			(a.pos == b.pos && a.lastHeight == b.lastHeight && a.lastBlock == b.lastBlock && a.pixel._rgb[0] < b.pixel._rgb[0]) ||
//			(a.pos == b.pos && a.lastHeight == b.lastHeight && a.lastBlock == b.lastBlock && a.pixel._rgb[0] == b.pixel._rgb[0] && a.pixel._rgb[1] < b.pixel._rgb[1]) ||
//			(a.pos == b.pos && a.lastHeight == b.lastHeight && a.lastBlock == b.lastBlock && a.pixel._rgb[0] == b.pixel._rgb[0] && a.pixel._rgb[1] == b.pixel._rgb[1] && a.pixel._rgb[2] < b.pixel._rgb[2]);
//	}
//};

class Result {
public:
	Result();
	Result(bool _success);
	~Result() {
	}

	std::vector<int> bestBlocks;
	std::vector<int> bestHeights;
	std::vector<rgb> bestDiffs;
	//Vector<int> bestBlocks;
	//Vector<int> bestHeights;
	//Vector<rgb> bestDiffs;
	float bestErr = 9999999;
	bool success = false;
};


class Schem {
public:
	Schem(int _lx, int _ly, int _lz) {
		lx = _lx;
		ly = _ly;
		lz = _lz;
	}

	int lx, ly, lz;
	std::vector<int8_t> finalMapColors;
	std::vector<int> finalPalette;
};



class Process {
public:
	Process(Palette* _palette);
	~Process() {
		delete initImg;
		delete preprocessedImg;
		delete ditheredImg;
		free(finalBlockMap);
		free(finalHeightMap);
	}

	void findFileName(std::string path);
	void decodePng(const char* path);
	void encodePng(const char* path);
	void preprocess();	// Img pre-processing to fit the map standard
	std::unique_ptr<Result> memoCollumnDitherDiffuse(int pos, int end, int lastHeight, int lastBlock, rgb lastErr);
	bool dither();			// Dither img and store result in ditheredImg
	void generateImage(std::string fileName);
	void generateNBT(std::string fileName);

	Palette* palette;

	//std::map<CacheKey, Result, compareCacheKey> memocache;
	std::unordered_map<CacheKey, Result> memocache;
	int memoSize = 0;
	unsigned char* currentColumn;
	float* errorMap;

	Img* initImg;			// Initial image
	Img* preprocessedImg;	// Resized image
	Img* ditheredImg;		// Final dithered image
	std::string fileName;

	//Preprocessing parameters
	bool crop = true;
	int nx = 1;					// Number of maps along x
	int ny = 1;					// Number of maps along y
	float zoom = 1;
	int xOffset = 0;		// Offset along x (if image is not already fitted along x)
	int yOffset = 0;		// Offset along y (if image is not already fitted along y)
	bool rainbowSupport = true;
	bool genIndividual = true;
	int scaffoldBlock = 0;

	int maxDepth = 950;
	int maxCache = 1000;
	int maxheight = 2;
	bool noDither = false;


	int* finalBlockMap;
	int* finalHeightMap;	// Contains noobline (height matters for the shade of the first block)
};
