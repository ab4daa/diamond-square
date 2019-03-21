#include "diamond_square.h"
#include "bitmap_image.hpp"

int main(int argc, char * argv[])
{
	const unsigned int sz = 256;
	ds::Daimond_Square d(sz, 1.0f, 0.5f);
	bitmap_image output(sz, sz);

	for (unsigned x = 0; x < sz; ++x)
	{
		for (unsigned y = 0; y < sz; ++y)
		{
			unsigned char c = 255.0f * d.data[x][y];
			output.set_pixel(x, y, c, c, c);
		}
	}
	output.save_image("diamond.bmp");


	ds::Plasma_Fractal p(sz, 2.0f, 0.5f);
	bitmap_image Plasma(sz, sz);
	for (unsigned x = 0; x < sz; ++x)
	{
		for (unsigned y = 0; y < sz; ++y)
		{
			unsigned char c = 255.0f * p.data[x][y];
			Plasma.set_pixel(x, y, c, c, c);
		}
	}
	Plasma.save_image("plasma.bmp");
	return 0;
}
