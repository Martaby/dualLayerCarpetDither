#include "nbtBuilder.h"

#include <iostream>
#include <fstream>
#include "nbt_tags.h"
#include "io/stream_reader.h"
#include "io/stream_writer.h"
#include "io/izlibstream.h"
#include "io/ozlibstream.h"



void writeMapNBT(std::vector<Block>* blocks, Schem* schem, std::string fileName) {

    // Root tags
    nbt::tag_compound root;
    nbt::tag_compound schematic;

    // Palette
    nbt::tag_compound paletteTag;
    paletteTag.insert("minecraft:air", 0);
    for (int i = 0; i < schem->finalPalette.size(); i++) {
        paletteTag.insert(blocks->at(schem->finalPalette[i]).id, schem->finalPalette[i] + 1);
    }


    // Sponge format version
    schematic.insert("Version", 3);
    schematic.insert("DataVersion", 4786);

    // Dimensions of the map
    schematic.insert("Width", static_cast<int16_t>(schem->lx));
    schematic.insert("Height", static_cast<int16_t>(schem->ly));
    schematic.insert("Length", static_cast<int16_t>(schem->lz));

    // Schematic offset
    nbt::tag_int_array tag_offset;
    tag_offset.push_back(0);
    tag_offset.push_back(0);
    tag_offset.push_back(0);
    schematic.insert("Offset", tag_offset.clone());


    nbt::tag_compound blocksTag;

    blocksTag.insert("Palette", paletteTag.clone());

    nbt::tag_byte_array&& blocksData(std::move(schem->finalMapColors));
    blocksTag.insert("Data", std::move(blocksData));
    nbt::tag_list blockEntitiesTag;
    blocksTag.insert("BlockEntities", blockEntitiesTag.clone());
    schematic.insert("Blocks", blocksTag.clone());
    root.insert("Schematic", schematic.clone());

    std::string completeName = fileName + ".schem";
    std::ofstream file(completeName, std::ios::binary);


    if (!file)
    {
        throw std::runtime_error("Error creating/opening file.");
    }

    try
    {
        zlib::ozlibstream ogzs(file, -1, true);
        nbt::io::write_tag("", root, ogzs);
    }
    catch (...)
    {
        throw std::runtime_error("Error writing Schematic.");
    }
}