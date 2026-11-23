#define CELL_COUNT 24
#define CELL_SIZE 12
#define CELL_PADDING 2

static uint32_t *g_frame_pointer = 0;

static uint32_t g_frame_width = 0;
static uint32_t g_frame_height = 0;

static uint8_t g_unit_size = 0;
static uint16_t g_cell_size = 0;
static uint16_t g_cell_padding = 0;

static const uint32_t g_pallet[] = {
	0XFF000000,
	0XFFFCFCFC,
	0XFF544848,
	0XFF0000FC,
	0XFF006CFC,
	0XFF00D8FC,
	0XFF549024,
	0XFFFCB400,
	0XFFFC486C,
	0XFFA86CFC,
};

uint32_t
calc_frame_size(uint8_t unit_size)
{
	uint32_t length = CELL_COUNT * CELL_SIZE * unit_size;
	return length * length * sizeof(uint32_t);
}

void
set_frame_pointer(uint32_t *fp)
{
	g_frame_pointer = fp;
}

void
set_unit_size(uint8_t unit_size)
{
	uint32_t length = CELL_COUNT * CELL_SIZE * unit_size;

	g_unit_size = unit_size;

	g_cell_size = CELL_SIZE * unit_size;
	g_cell_padding = CELL_PADDING * unit_size;

	g_frame_width = length;
	g_frame_height = length;
}

#undef CELL_COUNT
#undef CELL_SIZE
#undef CELL_PADDING

uint32_t
get_width(void)
{
	return g_frame_width;
}

uint32_t
get_height(void)
{
	return g_frame_height;
}

static void
rect_fill(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color)
{
	for (uint32_t y = y0; y < y1; y += 1)
	{
		for (uint32_t x = x0; x < x1; x += 1)
		{
			g_frame_pointer[y * g_frame_width + x] = color;
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
			g_frame_pointer[y0 * g_frame_width + x] = color;
		}
	}

	for (const uint32_t y_end = y1 - thickness; y0 < y_end; y0 += 1)
	{
		for (uint32_t x = x0, x_end = x0 + thickness; x < x_end; x += 1)
		{
			g_frame_pointer[y0 * g_frame_width + x] = color;
		}

		for (uint32_t x = x1 - thickness, x_end = x1; x < x_end; x += 1)
		{
			g_frame_pointer[y0 * g_frame_width + x] = color;
		}
	}

	for (; y0 < y1; y0 += 1)
	{
		for (uint32_t x = x0; x < x1; x += 1)
		{
			g_frame_pointer[y0 * g_frame_width + x] = color;
		}
	}
}

static void
square_fill(uint32_t x, uint32_t y, uint16_t sz, uint32_t color)
{
	rect_fill(x, y, x + sz, y + sz, color);
}

static uint32_t
rgb8(uint8_t c)
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

#include "input.c"
#include "text.c"
