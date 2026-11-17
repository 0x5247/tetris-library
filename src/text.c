#include "glyph.c"

static const uint32_t glyph_row = GLYPH_ROW;
static const uint32_t glyph_col = GLYPH_COL;

static void
glyph_fill(uint32_t off_x, uint32_t off_y, uint32_t scale, uint32_t color, size_t glyph_idx)
{
	uint32_t xs[2 * ((GLYPH_COL >> 1) + (GLYPH_COL & 1))];

	const bool *glyph = glyphs[glyph_idx];

	uint32_t y = off_y;

	for (uint32_t i = 0; i < glyph_col * glyph_row;)
	{
		uint32_t xs_len = 0;

		for (uint32_t j = 0; j < glyph_col; j += 1)
		{
			if (glyph[i++] != (xs_len & 1))
			{
				xs[xs_len++] = off_x + scale * j;
			}
		}

		if (xs_len & 1)
		{
			xs[xs_len++] = off_x + scale * glyph_col;
		}

		for (const uint32_t y_end = y + scale; y < y_end; y += 1)
		{
			for (uint32_t j = 0; j < xs_len; j += 2)
			{
				const uint32_t x_end = xs[j + 1];
				for (uint32_t x = xs[j]; x < x_end; x += 1)
				{
					frame_buffer[y][x] = color;
				}
			}
		}
	}
}

#undef GLYPH_ROW
#undef GLYPH_COL

static void
u32_fill(uint32_t x, uint32_t y, uint32_t scale, uint32_t color, uint32_t n)
{
	x += scale * (glyph_col + 1) * 8;
	uint32_t i = 0;
	do {
		x -= scale * (glyph_col + 1);
		glyph_fill(x, y, scale, color, n & 0xF);
		n >>= 4;
		i += 1;
	} while (i < 8);
}

static uint32_t
glyph_width(uint32_t scale)
{
	return scale * (glyph_col + 1);
}

static uint32_t
glyph_height(uint32_t scale)
{
	return scale * (glyph_row + 1);
}

#if 0
this no longer works :(
static uint32_t
get_text_width(uint32_t scale, uint32_t len)
{
	return len * glyph_width(scale);
}
#endif

static void
text_fill(uint32_t off_x, uint32_t off_y, uint32_t scale, uint32_t color, const char *text)
{
	uint32_t x = off_x, y = off_y;

	for (size_t i = 0; text[i] != 0; i += 1)
	{
		const char c = text[i];

		switch (c)
		{
		case ' ':
			x += glyph_width(scale);
			break;
		case '\t':
			x += glyph_width(scale) * 4;
			break;
		case '\r':
			x = off_x;
			break;
		case '\n':
			y += glyph_height(scale);
			break;
		case '\v':
			y += glyph_height(scale) * 3;
			break;
		default:
			{
				size_t off = 0;

				if (c >= '0' && c <= '9')
				{
					glyph_fill(x, y, scale, color, c - '0');
					x += glyph_width(scale);
					break;
				}
				else
				{
					off += '9' - '0' + 1;
				}

				if (c >= 'A' && c <= 'Z')
				{
					glyph_fill(x, y, scale, color, c - 'A' + off);
					x += glyph_width(scale);
					break;
				}
				else if (c >= 'a' && c <= 'z')
				{
					glyph_fill(x, y, scale, color, c - 'a' + off);
					x += glyph_width(scale);
					break;
				}
				else
				{
					off += 'Z' - 'A' + 1;
				}

				if (c >= '!' && c <= '/')
				{
					glyph_fill(x, y, scale, color, c - '!' + off);
					x += glyph_width(scale);
					break;
				}
				else
				{
					off += '/' - '!' + 1;
				}

				if (c >= ':' && c <= '@')
				{
					glyph_fill(x, y, scale, color, c - ':' + off);
					x += glyph_width(scale);
					break;
				}
				else
				{
					off += '@' - ':' + 1;
				}

				if (c >= '[' && c <= '`')
				{
					glyph_fill(x, y, scale, color, c - '[' + off);
					x += glyph_width(scale);
					break;
				}
				else
				{
					off += '`' - '[' + 1;
				}

				if (c >= '{' && c <= '~')
				{
					glyph_fill(x, y, scale, color, c - '{' + off);
				}
				else
				{
					glyph_fill(x, y, scale, color, (sizeof(glyphs) / sizeof(glyphs[0])) - 1);
				}

				x += glyph_width(scale);
			}
			break;
		}
	}
}
