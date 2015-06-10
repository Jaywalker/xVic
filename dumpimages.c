#include <stdio.h>
#include <stdlib.h>
#include <gd.h>

typedef struct {
	int width;
	int height;
	unsigned char *data; //(height + width) / 8
} FWImage;

int dumpImage(FWImage *img, char *name) {
	gdImagePtr im;
	int white, black, x, y;
	FILE *fp;
	unsigned char row, printPixel;
	int datalen = (img->height * img->width) / 8;


	//Create our image
	im = gdImageCreate(img->width, img->height);
	if (!im) {
		fprintf(stderr, "can't create image");
		return 1;
	}

	fp = fopen(name, "wb");
	if (!fp) {
		fprintf(stderr, "can't create file %s", name);
		return 1;
	}

	white = gdImageColorAllocate(im, 255, 255, 255);
	black = gdImageColorAllocate(im,  0, 0, 0);
	
	//Loop through the data
	for (x = 0; x < datalen; x++) {
		row = img->data[x];
		for (y = 0; y < 8; y++) {
			//Shift the data to see if we print this pixel
			printPixel = row << (7 - y);
			printPixel = printPixel >> 7;
			if (printPixel) { //Fill a pixel
				//Swap  every 2 rows (No idea why they do this)
				int fixX = x;
				if (fixX == 0 || fixX % 2 == 0) {
					fixX++;
				} else {
					fixX--;
				}
				gdImageFilledRectangle(im, fixX, y, fixX, y, black);
			}
		}
	}
	gdImagePng(im, fp);
	fclose(fp);
}


int main(int argc, char **args) {
	FWImage img;
	char name[255];
	unsigned char bytes[9];
	int datalen;
	unsigned int location = 0;
	FILE *fp = fopen(args[1], "rb");
	if (!fp)
	{
		printf("Can't open file: %s\n", args[1]);
		return -1;
	}

	while (location < 64000)
	{
		fread(bytes, 1, 8, fp);
		// Locate image "0": 06 08 7E 00 81 81 7E 81
		if (bytes[0] == 0x06 && bytes[1] == 0x08 && bytes[2] == 0x7e && bytes[3] == 0x00 &&
		    bytes[4] == 0x81 && bytes[5] == 0x81 && bytes[6] == 0x7e && bytes[7] == 0x81)
		{
			break;
		}
		location++;
		fseek(fp, location, SEEK_SET);
	}

	printf("Location found: %04x\n", location);
	fseek(fp, location, SEEK_SET);

	do {
		location = ftell(fp);
		img.width = fgetc(fp);
		img.height = fgetc(fp);

		datalen = (img.height * img.width) / 8;

		img.data = (char *)malloc(datalen);
		fread(img.data, 1, datalen, fp);

		sprintf(&name, "%X.png\0", location);
		//sprintf(&name, "%X.png\0", ftell(fp));

		printf("Dumping image %s (%dx%d)\n", name, img.width, img.height);
		dumpImage(&img, name);
	} while (ftell(fp) < 0x3D0D);
	
	return 0;
}

