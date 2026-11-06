/*
 * tetrino type order: IJLTSZO
 * [type][orientation][cell][y|x]
 * note: the y value of the last cell should be the highest
 *       that's what the function `tetrino_lock` assumes
 */
static const int8_t tetrino_offsets[7][4][4][2] = {
    {
        {
            {-1, 0},
            {0, 0},
            {1, 0},
            {2, 0},
        },
        {
            {0, -1},
            {0, 0},
            {0, 1},
            {0, 2},
        },
        {
            {-1, 0},
            {0, 0},
            {1, 0},
            {2, 0},
        },
        {
            {0, -1},
            {0, 0},
            {0, 1},
            {0, 2},
        },
    },
    {
        {
            {-1, 0},
            {0, 0},
            {-1, 1},
            {1, 0},
        },
        {
            {0, -1},
            {0, 0},
            {0, 1},
            {1, 1},
        },
        {
            {1, -1},
            {-1, 0},
            {0, 0},
            {1, 0},
        },
        {
            {-1, -1},
            {0, -1},
            {0, 0},
            {0, 1},
        },
    },
    {
        {
            {-1, 0},
            {0, 0},
            {1, 0},
            {1, 1},
        },
        {
            {0, -1},
            {0, 1},
            {0, 0},
            {1, -1},
        },
        {
            {-1, -1},
            {-1, 0},
            {0, 0},
            {1, 0},
        },
        {
            {0, -1},
            {0, 0},
            {-1, 1},
            {0, 1},
        },
    },
    {
        {
            {-1, 0},
            {0, 0},
            {0, 1},
            {1, 0},
        },
        {
            {0, 1},
            {0, 0},
            {0, -1},
            {1, 0},
        },
        {
            {0, -1},
            {-1, 0},
            {0, 0},
            {1, 0},
        },
        {
            {-1, 0},
            {0, 0},
            {0, -1},
            {0, 1},
        },
    },
    {
        {
            {-1, 0},
            {0, 0},
            {0, 1},
            {1, 1},
        },
        {
            {0, 0},
            {0, 1},
            {1, -1},
            {1, 0},
        },
        {
            {-1, 0},
            {0, 0},
            {0, 1},
            {1, 1},
        },
        {
            {0, 0},
            {0, 1},
            {1, -1},
            {1, 0},
        },
    },
    {
        {
            {0, 0},
            {0, 1},
            {-1, 1},
            {1, 0},
        },
        {
            {0, -1},
            {0, 0},
            {1, 0},
            {1, 1},
        },
        {
            {0, 0},
            {0, 1},
            {-1, 1},
            {1, 0},
        },
        {
            {0, -1},
            {0, 0},
            {1, 0},
            {1, 1},
        },
    },
    {
        {
            {0, 0},
            {1, 0},
            {0, 1},
            {1, 1},
        },
        {
            {0, 0},
            {1, 0},
            {0, 1},
            {1, 1},
        },
        {
            {0, 0},
            {1, 0},
            {0, 1},
            {1, 1},
        },
        {
            {0, 0},
            {1, 0},
            {0, 1},
            {1, 1},
        },
    },
};

struct tetrino_state
{
	uint8_t type;
	uint8_t orientation;
	uint8_t x;
	uint8_t y;
};

static uint32_t tetrino_seed = 0;

static struct tetrino_state tetrino;

static uint8_t next_tetrino;

static void
tetrino_reset(void)
{
	tetrino.type = next_tetrino;
	tetrino.orientation = 1;
	tetrino.x = 4;
	tetrino.y = 3;

	// todo: use that 7 + 1 bag randomizer thing
	tetrino_seed ^= next_tetrino + 5;
	next_tetrino = tetrino_seed % 7;
}

static bool
tetrino_can_fit(void)
{
	for (size_t i = 0; i < 4; i += 1)
	{
		const uint32_t x = tetrino.x + tetrino_offsets[tetrino.type][tetrino.orientation][i][1];
		const uint32_t y = tetrino.y + tetrino_offsets[tetrino.type][tetrino.orientation][i][0];

		if (x >= 10 || y >= 23 || grid[y][x] != 0)
		{
			return false;
		}
	}

	return true;
}

static void
tetrino_rotate(void)
{
	tetrino.orientation += 1;
	tetrino.orientation &= 3;

	if (!tetrino_can_fit())
	{
		tetrino.orientation += 3;
		tetrino.orientation &= 3;
	}
}

static void
tetrino_move_left(void)
{
	tetrino.x -= 1;

	if (!tetrino_can_fit())
	{
		tetrino.x += 1;
	}
}

static void
tetrino_move_right(void)
{
	tetrino.x += 1;

	if (!tetrino_can_fit())
	{
		tetrino.x -= 1;
	}
}

static bool
tetrino_move_down(void)
{
	tetrino.y += 1;

	if (!tetrino_can_fit())
	{
		tetrino.y -= 1;
		return false;
	}

	return true;
}

static void
tetrino_move_rotate(int8_t dir, bool prioritize_rotation)
{
	tetrino.x += dir;
	tetrino.orientation += 1;
	tetrino.orientation &= 3;

	if (!tetrino_can_fit())
	{
		if (prioritize_rotation)
		{
			tetrino.x -= dir;

			if (!tetrino_can_fit())
			{
				tetrino.orientation += 3;
				tetrino.orientation &= 3;
			}
		}
		else
		{
			tetrino.orientation += 3;
			tetrino.orientation &= 3;

			if (!tetrino_can_fit())
			{
				tetrino.x -= dir;
			}
		}
	}
}

static void
tetrino_drop(void)
{
	while (tetrino_move_down())
	{
	}
}

static uint32_t
tetrino_lock(void)
{
	uint32_t x, y;

	for (size_t i = 0; i < 4; i += 1)
	{
		x = tetrino.x + tetrino_offsets[tetrino.type][tetrino.orientation][i][1];
		y = tetrino.y + tetrino_offsets[tetrino.type][tetrino.orientation][i][0];

		grid[y][x] = tetrino.type + 1;
	}

	return y;
}

static void
tetrino_draw(void)
{
	for (size_t i = 0; i < 4; i += 1)
	{
		const uint32_t x = tetrino.x + tetrino_offsets[tetrino.type][tetrino.orientation][i][1];
		const uint32_t y = tetrino.y + tetrino_offsets[tetrino.type][tetrino.orientation][i][0];
		const uint8_t color_idx = tetrino.type + 1;

		cell_draw(x, y, color_idx);
	}
}

static void
tetrino_draw_preview(void)
{
	// todo: untangle this mess
	rect_fill(cell_offset_x - (cell_padding * 2) + ((cell_size + cell_padding) * 38),
	          cell_offset_y - (cell_padding * 2) + ((cell_size + cell_padding) * 7),
	          cell_offset_x + cell_padding + ((cell_size + cell_padding) * 38) +
	              ((cell_size + cell_padding) * 4),
	          cell_offset_y + cell_padding + ((cell_size + cell_padding) * 7) +
	              ((cell_size + cell_padding) * 2),
	          0xFFFFFFFF);

	rect_fill(cell_offset_x - cell_padding + ((cell_size + cell_padding) * 38),
	          cell_offset_y - cell_padding + ((cell_size + cell_padding) * 7),
	          cell_offset_x + ((cell_size + cell_padding) * 38) + ((cell_size + cell_padding) * 4),
	          cell_offset_y + ((cell_size + cell_padding) * 7) + ((cell_size + cell_padding) * 2),
	          0xFF000000);

	for (size_t i = 0; i < 4; i += 1)
	{
		const uint32_t x = 39 + tetrino_offsets[next_tetrino][1][i][1];
		const uint32_t y = 7 + tetrino_offsets[next_tetrino][1][i][0];
		const uint8_t color_idx = next_tetrino + 1;

		square_fill(cell_offset_x + ((cell_size + cell_padding) * x),
		            cell_offset_y + ((cell_size + cell_padding) * y), cell_size,
		            grid_colors[color_idx]);
	}
}
