static uint8_t grid[23][10];

static void
grid_clear(void)
{
	for (size_t i = 0; i < sizeof grid; i += 1)
	{
		grid[0][i] = 0;
	}
}

static bool
grid_line_filled(uint32_t line)
{
	for (size_t i = 0; i < 10; i += 1)
	{
		if (grid[line][i] == 0)
		{
			return false;
		}
	}

	return true;
}

static uint32_t
grid_clear_lines(uint32_t end)
{
	uint32_t lines_cleared = 0;

	for (size_t i = 0; i < 4; i += 1)
	{
		if (!grid_line_filled(end))
		{
			if (end == 0)
			{
				return lines_cleared;
			}

			end -= 1;

			continue;
		}

		for (int32_t j = (int32_t)end; j > 0; j -= 1)
		{
			for (size_t k = 0; k < 10; k += 1)
			{
				grid[j][k] = grid[j - 1][k];
			}
		}

		lines_cleared += 1;
	}

	return lines_cleared;
}

#if 0
static const uint32_t grid_colors[] = {
    0xFF404040, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFF00FFFF, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF,
};
#else
static uint32_t grid_colors[8];
#endif

static const uint32_t cell_size = 24;
static const uint32_t cell_padding = 4;
static const uint32_t cell_offset_x = 2 * (cell_padding + cell_size);
static const uint32_t cell_offset_y = 1 * (cell_padding + cell_size);

static void
cell_draw(uint32_t x, uint32_t y, size_t color_idx)
{
	y -= 3;
	if (y > 20)
	{
		return;
	}

	square_fill(cell_offset_x + ((cell_size + cell_padding) * x),
	            cell_offset_y + ((cell_size + cell_padding) * y), cell_size,
	            grid_colors[color_idx]);
}

static void
grid_draw(void)
{
	for (size_t y = 3; y < 23; y += 1)
	{
		for (size_t x = 0; x < 10; x += 1)
		{
			cell_draw(x, y, grid[y][x]);
		}
	}
}
