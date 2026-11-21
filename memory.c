typedef _Bool bool;
#define true 1
#define false 0

typedef __UINT8_TYPE__ uint8_t;
typedef __SIZE_TYPE__ size_t;

static uint8_t g_memory[1024 * (1 << 16)];
static size_t g_ptr = 0;

void *
mem_alloc(size_t len)
{
	const size_t r = g_ptr;
	g_ptr += len;

	if (g_ptr >= sizeof(g_memory))
	{
		g_ptr = sizeof(g_memory);
		return 0;
	}

	return &g_memory[r];
}

void
mem_reset(bool clear)
{
	if (clear)
	{
		for (size_t i = 0; i < g_ptr; i += 1)
		{
			g_memory[i] = 0;
		}
	}

	g_ptr = 0;
}
