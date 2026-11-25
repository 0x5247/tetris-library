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
	const size_t new_ptr = g_ptr + len;

	if (new_ptr < g_ptr || new_ptr >= sizeof(g_memory))
	{
		return 0;
	}

	const size_t ptr = g_ptr;
	g_ptr = new_ptr;

	return &g_memory[ptr];
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
