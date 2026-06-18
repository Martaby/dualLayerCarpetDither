Dithering tool implementing limited staircase.
Original cython implementation from jkascpkmc on discord from Map Artists of 2b2t. Made for carpet only dual layer, printed automatically on a machine adapted from https://youtu.be/ema3lSc6mog?si=0JKXa3uJBCOnl7Hq.

Requires zlib and libzip libraries for compilation.

Should work for windows and linux, but later needs to be tested.

Code is still messy and not fully optimized.

Creates 'Options.txt' with base parameters on first execution. Recreates it if the file is removed.

Can only read png images at the moment.
