enum input_type
{
	ROTATE,
	LEFT,
	RIGHT,
	DOWN,
	DROP,
	NONE = -1,
};

static struct
{
	uint8_t head;
	uint8_t tail;
	uint8_t buffer[256];
} g_input_queue;

static void
input_discard(void)
{
	g_input_queue.head = 0;
	g_input_queue.tail = 0;
}

void
input_enqueue(enum input_type type)
{
	if ((uint8_t)(g_input_queue.tail + 1) == g_input_queue.head)
	{
		return;
	}

	g_input_queue.buffer[g_input_queue.tail] = (uint8_t)type;
	g_input_queue.tail += 1;
}

static enum input_type
input_dequeue(void)
{
	if (g_input_queue.head == g_input_queue.tail)
	{
		input_discard(); // not necessary
		return NONE;
	}

	const uint8_t idx = g_input_queue.head;
	g_input_queue.head += 1;

	return g_input_queue.buffer[idx];
}

static enum input_type
input_peek(void)
{
	if (g_input_queue.head == g_input_queue.tail)
	{
		return NONE;
	}

	return g_input_queue.buffer[g_input_queue.head];
}
