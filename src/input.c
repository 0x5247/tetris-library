enum input_type
{
	ROTATE,
	LEFT,
	RIGHT,
	DOWN,
	DROP,
	NONE = 255,
};

static struct {
	uint8_t buffer[256];
	uint8_t head;
	uint8_t tail;
} input_queue;

void
input_enqueue(enum input_type type)
{
	if ((uint8_t)(input_queue.tail + 1) == input_queue.head)
	{
		return;
	}

	input_queue.buffer[input_queue.tail] = (uint8_t)type;
	input_queue.tail += 1;
}

static uint8_t
input_dequeue(void)
{
	if (input_queue.head == input_queue.tail)
	{
		return NONE;
	}

	const uint8_t idx = input_queue.head;
	input_queue.head += 1;

	return input_queue.buffer[idx];
}

#if 0
static uint8_t
input_peek(void)
{
	if (input_queue.head == input_queue.tail)
	{
		return NONE;
	}

	return input_queue.buffer[input_queue.head];
}

static void
input_discard(void)
{
	input_queue.head = input_queue.tail;
}
#endif
