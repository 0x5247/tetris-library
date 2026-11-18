// todo: make the renderer somewhat resolution independent

#define CELL_SIZE (8)
#define FRAME_SIZE (25 * CELL_SIZE)

// todo: get pointer to frame buffer from the runtime
static uint32_t frame_buffer[FRAME_SIZE][FRAME_SIZE];

void *
get_frame_buffer(void)
{
	return frame_buffer;
}

uint32_t
get_width(void)
{
	return FRAME_SIZE;
}

uint32_t
get_height(void)
{
	return FRAME_SIZE;
}

static void
rect_fill(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color)
{
	for (uint32_t y = y0; y < y1; y += 1)
	{
		for (uint32_t x = x0; x < x1; x += 1)
		{
			frame_buffer[y][x] = color;
		}
	}
}

static void
rect_draw(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t thickness, uint32_t color)
{
	if (x1 - x0 <= thickness << 1 || y1 - y0 <= thickness << 1)
	{
		rect_fill(x0, x1, y0, y1, color);
		return;
	}

	for (const uint32_t y_end = y0 + thickness; y0 < y_end; y0 += 1)
	{
		for (uint32_t x = x0; x < x1; x += 1)
		{
			frame_buffer[y0][x] = color;
		}
	}

	for (const uint32_t y_end = y1 - thickness; y0 < y_end; y0 += 1)
	{
		for (uint32_t x = x0, x_end = x0 + thickness; x < x_end; x += 1)
		{
			frame_buffer[y0][x] = color;
		}

		for (uint32_t x = x1 - thickness, x_end = x1; x < x_end; x += 1)
		{
			frame_buffer[y0][x] = color;
		}
	}

	for (; y0 < y1; y0 += 1)
	{
		for (uint32_t x = x0; x < x1; x += 1)
		{
			frame_buffer[y0][x] = color;
		}
	}
}

static void
square_fill(uint32_t x, uint32_t y, uint16_t sz, uint32_t color)
{
	rect_fill(x, y, x + sz, y + sz, color);
}

static uint32_t
color_from_8bits(uint8_t c)
{
	// 0bRRRGGGBB
	uint8_t r = c & (7 << 5);
	r |= (r >> 3);

	uint8_t g = c & (7 << 2);
	g |= (g << 3);

	uint8_t b = (c << 2) & (3 << 2);
	b |= (b << 2) | (b << 4);

	uint32_t ret = 0xFF; /* 0xAABBGGRR */
	ret = (ret << 8) | b;
	ret = (ret << 8) | g;
	ret = (ret << 8) | r;

	return ret;
}

#include "text.c"
#include "input.c"
