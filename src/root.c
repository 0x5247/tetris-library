#include "base.h"

#include "frame.c"
#include "input.c"
#include "grid.c"
#include "tetrino.c"

static enum {
	MENU,
	GAME,
	OVER,
} game_state;

void
init(void)
{
	// todo: pick better colors
	grid_colors[0] = color_from_8bits(73);
	grid_colors[1] = color_from_8bits(23);
	grid_colors[2] = color_from_8bits(67);
	grid_colors[3] = color_from_8bits(88);
	grid_colors[4] = color_from_8bits(166);
	grid_colors[5] = color_from_8bits(59);
	grid_colors[6] = color_from_8bits(90);
	grid_colors[7] = color_from_8bits(209);

	tetrino_reset();
	game_state = MENU;
}

static void
handle_input(void)
{
	for (;;)
	{
		const uint8_t input = input_dequeue();
		if (input == NONE)
		{
			break;
		}

		// todo: refactor this
		switch (input)
		{
		case ROTATE:
			if (input_peek() == LEFT || input_peek() == RIGHT)
			{
				tetrino_move_rotate(input_dequeue() == LEFT ? -1 : 1, true);
			}
			else
			{
				tetrino_rotate();
			}
			break;
		case LEFT:
			if (input_peek() == ROTATE)
			{
				input_dequeue();
				tetrino_move_rotate(-1, false);
			}
			else
			{
				tetrino_move_left();
			}
			break;
		case RIGHT:
			if (input_peek() == ROTATE)
			{
				input_dequeue();
				tetrino_move_rotate(1, false);
			}
			else
			{
				tetrino_move_right();
			}
			break;
		case DOWN:
			tetrino_move_down();
			break;
		case DROP:
			tetrino_drop();
			break;
		}
	}
}

static bool
input_ticker(float dt)
{
	static float at = 0;
	at += dt;

	return at >= 0.0125f ? (at = 0, true) : false;
	//             ^ this interval has to be small enough to be not noticable
	//               and big enough to let at least two input in a single frame
}

static bool
drop_ticker(float dt)
{
	static float at = 0;
	at += dt;

	return at >= 0.5f ? (at = 0, true) : false;
}

void
update(float dt)
{
	tetrino_seed += 1;

	switch (game_state)
	{
	case MENU:
		if (input_dequeue() == DROP)
		{
			tetrino_reset();
			tetrino_reset();
			tetrino_reset();
			game_state = GAME;
			input_discard();
		}
		break;
	case GAME:
		if (input_ticker(dt))
		{
			handle_input();
		}

		if (drop_ticker(dt))
		{
			if (!tetrino_move_down())
			{
				grid_clear_lines(tetrino_lock());

				tetrino_reset();
				if (!tetrino_can_fit())
				{
					game_state = OVER;
				}
			}
		}
		break;
	case OVER:
		if (input_dequeue() == DROP)
		{
			grid_clear();
			game_state = MENU;
		}
		break;
	}
}

// todo: render frame only when needed (maybe)
void
render(void)
{
	for (size_t i = 0; i < sizeof(frame_buffer) / sizeof(frame_buffer[0][0]); i += 1)
	{
		frame_buffer[0][i] = color_from_8bits(0);
	}

	// todo: text rendering
	switch (game_state)
	{
	case MENU:
		break;
	case GAME:
		grid_draw();
		tetrino_draw();
		tetrino_draw_preview();
		break;
	case OVER:
		grid_draw();
		tetrino_draw();
		break;
	}
}
