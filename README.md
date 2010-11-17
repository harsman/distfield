
distfield - a signed distance field texture generator
=====================================================
Usage:

    distfield input_image output_image output_size

the output size is a single number (only square images are supported). The
input image can be many formats (PNG, JPEG, TGA, BMP, PSD etc) courtesy of
[Sean Barrett's][0] excellent [stb_image.c][1]. The output image is always in
PNG format and is poorly optimized for size.

The program uses a stupid brute force algorithm, and is therefore slow with large
output sizes.


[0]: http://nothings.org/
[1]: http://nothings.org/stb_image.c
