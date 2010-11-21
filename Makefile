distfield: distfield.c stb_image.c stb_image_write.h
	gcc -O3 -fno-strict-aliasing -lm distfield.c -o distfield

# stb_image_write will segfault without -fno-strict-aliasing beacuse of the
# stretchy buffer code