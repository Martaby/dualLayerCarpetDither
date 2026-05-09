#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "Utils.h"

class Schem;
class Block;


void writeMapNBT(std::vector<Block>* blocks, Schem* schem, std::string fileName);
