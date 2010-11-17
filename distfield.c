/* Copyright 2010 Joakim Hårsman. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following condition is met:
 *
 *     Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *     
 * THIS SOFTWARE IS PROVIDED BY Joakim Hårsman "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define STBI_NO_HDR
#include "stb_image.c"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX(a, b)((a) > (b) ? a : b)
#define MIN(a, b)((a) < (b) ? a : b)

void usage(void){
    printf("Usage: distfield imagein imageout outsize\n");
    printf("input image can be many formats (JPEG, PNG, TGA etc)\n");
    printf("Output image is always in PNG format.\n");
    printf("Both input and output image size needs to be a power of 2\n");
    printf("Note that output PNG is poorly optimized for size.\n");
}

float mindist(char* in, int w, int h, int x, int y, int r, float maxdist){
    int i, j, startx, starty, stopx, stopy, hit;
    float d, dx, dy;
    float mind = maxdist;
    unsigned char p;
    int outside = 0 == in[y * w + x];

    startx = MAX(0, x - r);
    starty = MAX(0, y - r);
    stopx = MIN(w, x + r);
    stopy = MIN(h, y + r);

    for(i = starty; i < stopy; i++){
        for(j = startx; j < stopx; j++){
            p = in[i * w + j];
            dx = j - x;
            dy = i - y;
            d = dx * dx + dy * dy;
            hit = (p != 0) == outside;
            if( hit && (d < mind)){
                mind = d;
            }
            if(d > maxdist)
                printf("Too big\n");
        }
    }

    if(outside)
        return sqrtf(mind);
    else
        return -sqrtf(mind);
}

void render(char* in, int w, int h, char* out, int outsize){
    int x, y, ix, iy;
    float d;
    unsigned char di;
    int sx = w / outsize;
    int sy = h / outsize;
    /* No sense of searching further with only 8-bits of output
     * precision
     */
    int r = 90; 
    float maxsq = 2 * r * r;
    float max = sqrtf(maxsq);

    for(y = 0; y < outsize; y++){
        for(x = 0; x < outsize; x++){
            ix = (x * sx) + (sx / 2);
            iy = (y * sy) + (sy / 2);
            d = mindist(in, w, h, ix, iy, r, maxsq);
            di = (unsigned char) 127.5f + 127.5f * (-d / max);
            out[y * outsize + x] = di;
        }
    }
}

int main(int argc, char** argv){
    unsigned char *img, *imgin, *imgout;
    int w, h, ncomps, comp;
    int i, outsize;
    
    if(argc != 4){
        usage();
        return 0;
    }

    outsize = atoi(argv[3]);
    if(outsize < 0){
        printf("Supplied output size isn't valid\n");
        usage();
    }

	img = stbi_load(argv[1], &w, &h, &ncomps, 0);
	if(!img){
        printf("Failed to load %s as input image\n", argv[1]); 
		return 0;
    }

	printf( "Loaded %i x %i image\n", w, h );

	comp = ncomps > 2 ? 2 : 0;
	printf("Using component %i as the input\n", 1 + comp );

    imgin = malloc(w * h);
    imgout = malloc(outsize * outsize);

    for(i = 0; i < w * h; i++){
        imgin[i] = img[(ncomps * i) + comp];
    }

    stbi_image_free(img);
    render(imgin, w, h, imgout, outsize);

    if(!stbi_write_png(argv[2], outsize, outsize, 1, imgout, outsize))
        printf("Failed to write output image %s\n", argv[2]);
    else
        printf("Wrote output to %s\n", argv[2]);
    
    return 0;
}

