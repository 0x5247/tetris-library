/*
 * tetrino type order: IJLTSZO
 * [type][orientation][cell][y|x]
 * note: the y value of the last cell should be the highest
 *       that's what the function `tetrino_lock` assumes
 */
static const int8_t g_tetrino_offsets[7][4][4][2] = {
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

static uint64_t g_tetrino_seed = 0;

static struct tetrino_state g_tetrino;

static struct
{
	size_t count;
	uint8_t val[7 + 1];
} g_tetrino_bag;

// SplitMix64
// https://prng.di.unimi.it/splitmix.c
static uint64_t
tetrino_get_random(void)
{
	g_tetrino_seed += 0x9e3779b97f4a7c15;

	uint64_t z = g_tetrino_seed;

	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;

	return z ^ (z >> 31);
}

static void
tetrino_reset(void)
{
	g_tetrino.type = g_tetrino_bag.val[g_tetrino_bag.count];

	g_tetrino_bag.count -= 1;

	if (g_tetrino_bag.count > 7)
	{
		g_tetrino_bag.val[0] = 0;
		g_tetrino_bag.val[1] = 1;
		g_tetrino_bag.val[2] = 2;
		g_tetrino_bag.val[3] = tetrino_get_random() % 7;
		g_tetrino_bag.val[4] = 3;
		g_tetrino_bag.val[5] = 4;
		g_tetrino_bag.val[6] = 5;
		g_tetrino_bag.val[7] = 6;

		{
			size_t idx = 8;

			while (idx != 0)
			{
				size_t ridx = tetrino_get_random() % idx;
				idx -= 1;

				uint8_t t = g_tetrino_bag.val[idx];
				g_tetrino_bag.val[idx] = g_tetrino_bag.val[ridx];
				g_tetrino_bag.val[ridx] = t;
			}
		}

		g_tetrino_bag.count = 7;
	}

	g_tetrino.orientation = 1;
	g_tetrino.x = 4;
	g_tetrino.y = 3;
}

static bool
tetrino_can_fit(void)
{
	for (size_t i = 0; i < 4; i += 1)
	{
		const uint32_t x =
		    g_tetrino.x + g_tetrino_offsets[g_tetrino.type][g_tetrino.orientation][i][1];
		const uint32_t y =
		    g_tetrino.y + g_tetrino_offsets[g_tetrino.type][g_tetrino.orientation][i][0];

		if (x >= 10 || y >= 23 || g_grid[y][x] != 0)
		{
			return false;
		}
	}

	return true;
}

static void
tetrino_rotate(void)
{
	g_tetrino.orientation += 1;
	g_tetrino.orientation &= 3;

	if (!tetrino_can_fit())
	{
		g_tetrino.orientation -= 1;
		g_tetrino.orientation &= 3;
	}
}

static void
tetrino_move_left(void)
{
	g_tetrino.x -= 1;

	if (!tetrino_can_fit())
	{
		g_tetrino.x += 1;
	}
}

static void
tetrino_move_right(void)
{
	g_tetrino.x += 1;

	if (!tetrino_can_fit())
	{
		g_tetrino.x -= 1;
	}
}

static bool
tetrino_move_down(void)
{
	g_tetrino.y += 1;

	if (!tetrino_can_fit())
	{
		g_tetrino.y -= 1;
		return false;
	}

	return true;
}

static void
tetrino_move_rotate(int8_t dir, bool prioritize_rotation)
{
	g_tetrino.x += dir;
	g_tetrino.orientation += 1;
	g_tetrino.orientation &= 3;

	if (!tetrino_can_fit())
	{
		if (prioritize_rotation)
		{
			g_tetrino.x -= dir;

			if (!tetrino_can_fit())
			{
				g_tetrino.orientation -= 1;
				g_tetrino.orientation &= 3;
			}
		}
		else
		{
			g_tetrino.orientation -= 1;
			g_tetrino.orientation &= 3;

			if (!tetrino_can_fit())
			{
				g_tetrino.x -= dir;
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
		x = g_tetrino.x + g_tetrino_offsets[g_tetrino.type][g_tetrino.orientation][i][1];
		y = g_tetrino.y + g_tetrino_offsets[g_tetrino.type][g_tetrino.orientation][i][0];

		g_grid[y][x] = g_tetrino.type + 1;
	}

	return y;
}

static void
tetrino_draw(uint32_t off_x, uint32_t off_y)
{
	for (size_t i = 0; i < 4; i += 1)
	{
		const uint32_t x =
		    g_tetrino.x + g_tetrino_offsets[g_tetrino.type][g_tetrino.orientation][i][1];
		const uint32_t y =
		    g_tetrino.y + g_tetrino_offsets[g_tetrino.type][g_tetrino.orientation][i][0];
		const uint8_t color_idx = g_tetrino.type + 1;

		cell_fill(off_x, off_y, x, y, color_idx);
	}
}

static void
tetrino_draw_preview(uint32_t off_x, uint32_t off_y)
{
	uint8_t next = g_tetrino_bag.val[g_tetrino_bag.count];

	for (size_t i = 0; i < 4; i += 1)
	{
		const uint32_t x = 1 + g_tetrino_offsets[next][1][i][1];
		const uint32_t y = g_tetrino_offsets[next][1][i][0];
		const uint8_t color_idx = next + 1;

		cell_fill(off_x, off_y, x, y + 3, color_idx);
	}
}
