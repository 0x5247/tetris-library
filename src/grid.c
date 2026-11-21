static uint8_t g_grid[23][10];

static void
grid_clear(void)
{
	for (size_t i = 0; i < sizeof(g_grid); i += 1)
	{
		g_grid[0][i] = 0;
	}
}

static bool
grid_line_filled(uint32_t line)
{
	for (size_t i = 0; i < 10; i += 1)
	{
		if (g_grid[line][i] == 0)
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
				g_grid[j][k] = g_grid[j - 1][k];
			}
		}

		lines_cleared += 1;
	}

	return lines_cleared;
}

static void
cell_draw(uint32_t off_x, uint32_t off_y, uint32_t x, uint32_t y, size_t color_idx)
{
	y -= 3;
	if (y > 20)
	{
		return;
	}

	square_fill(off_x + (g_cell_size * x), off_y + (g_cell_size * y), g_cell_size - g_cell_padding,
	            g_pallet[color_idx]);
}

static void
grid_draw(uint32_t off_x, uint32_t off_y)
{
	for (size_t y = 3; y < 23; y += 1)
	{
		for (size_t x = 0; x < 10; x += 1)
		{
			cell_draw(off_x, off_y, x, y, g_grid[y][x]);
		}
	}
}
