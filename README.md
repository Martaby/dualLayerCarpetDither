Dithering tool for minecraft mapart implementing limited staircase.

Staircasing allows to build a mapart with 3 times more shades for each available color (https://minecraft.wiki/w/Map_item_format).
The additional shades come from the change in altitude in the North/South orientation.
Building a full staircase mapart can however be tedious, especially on non-hack servers.
The technique known as limited staircase makes use of the changes in altitude, enabling the full palette for each block type while restricting the altitude to a given value.
The results cannot be better than staircasing, but approaching it really well for a much simpler build.


The original cython implementation is from jkascpkmc on discord from Map Artists of 2b2t.
This version was made for carpet only dual layer, printed automatically on a machine adapted from https://youtu.be/ema3lSc6mog?si=0JKXa3uJBCOnl7Hq.
An additional functionality was added to allow for a constant background color, building the map on redstone blocks for instance (red carpets are just not it).

Requires zlib and libzip libraries for compilation.

Should work for windows and linux, but latter needs to be tested.

Code is still messy and not fully optimized.

Creates 'Options.txt' with base parameters on first execution. Recreates it if the file is removed.

Can only read png images at the moment.
