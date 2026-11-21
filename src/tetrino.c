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

static struct {
	size_t count;
	uint8_t val[7 + 1];
} tetrino_bag;

static uint32_t
tetrino_get_random(void)
{
  tetrino_seed += tetrino_seed == 0;

  tetrino_seed ^= tetrino_seed << 13;
  tetrino_seed ^= tetrino_seed >> 17;
  tetrino_seed ^= tetrino_seed << 5;
  tetrino_seed *= *((uint32_t *)&tetrino) + (uint32_t)23157137828039747;
  tetrino_seed ^= tetrino_seed >> 16;

	return tetrino_seed;
}

static void
tetrino_reset(void)
{
	tetrino.type = tetrino_bag.val[tetrino_bag.count];

	tetrino_bag.count -= 1;

	if (tetrino_bag.count > 7)
	{
		tetrino_bag.val[0] = 0;
		tetrino_bag.val[1] = 1;
		tetrino_bag.val[2] = 2;
		tetrino_bag.val[3] = tetrino_get_random() % 7;
		tetrino_bag.val[4] = 3;
		tetrino_bag.val[5] = 4;
		tetrino_bag.val[6] = 5;
		tetrino_bag.val[7] = 6;

		{
			size_t idx = 8;

			while (idx != 0)
			{
				size_t ridx = tetrino_get_random() % idx;
				idx -= 1;

				uint8_t t = tetrino_bag.val[idx];
				tetrino_bag.val[idx] = tetrino_bag.val[ridx];
				tetrino_bag.val[ridx] = t;
			}
		}

		tetrino_bag.count = 7;
	}

	tetrino.orientation = 1;
	tetrino.x = 4;
	tetrino.y = 3;
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
		tetrino.orientation -= 1;
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
				tetrino.orientation -= 1;
				tetrino.orientation &= 3;
			}
		}
		else
		{
			tetrino.orientation -= 1;
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
tetrino_draw(uint32_t off_x, uint32_t off_y)
{
	for (size_t i = 0; i < 4; i += 1)
	{
		const uint32_t x = tetrino.x + tetrino_offsets[tetrino.type][tetrino.orientation][i][1];
		const uint32_t y = tetrino.y + tetrino_offsets[tetrino.type][tetrino.orientation][i][0];
		const uint8_t color_idx = tetrino.type + 1;

		cell_draw(off_x, off_y, x, y, color_idx);
	}
}

static void
tetrino_draw_preview(uint32_t off_x, uint32_t off_y)
{
	uint8_t next = tetrino_bag.val[tetrino_bag.count];

	for (size_t i = 0; i < 4; i += 1)
	{
		const uint32_t x = 1 + tetrino_offsets[next][1][i][1];
		const uint32_t y = tetrino_offsets[next][1][i][0];
		const uint8_t color_idx = next + 1;

		square_fill(off_x + (cell_size * x),
								off_y + (cell_size * y), cell_size - cell_padding,
								grid_colors[color_idx]);
	}
}
