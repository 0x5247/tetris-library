#include "base.h"

#include "frame.c"
#include "grid.c"
#include "tetrino.c"

static enum {
	MENU,
	GAME,
	OVER,
} g_game_state;

static uint32_t g_lines_cleared = 0;

void
init(void)
{
	tetrino_reset();
	g_game_state = MENU;
}

static void
handle_input(void)
{
	for (;;)
	{
		switch (input_dequeue())
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
		case NONE:
			return;
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

	return at >= 0.25f ? (at = 0, true) : false;
}

void
update(float dt)
{
	g_tetrino_seed += *((uint32_t *)&g_tetrino) + 1;

	switch (g_game_state)
	{
	case MENU:
		if (input_dequeue() == DROP)
		{
			tetrino_reset();

			g_game_state = GAME;
			g_lines_cleared = 0;

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
				g_lines_cleared += grid_clear_lines(tetrino_lock());

				tetrino_reset();
				if (!tetrino_can_fit())
				{
					g_game_state = OVER;
				}
			}
		}
		break;
	case OVER:
		if (input_dequeue() == DROP)
		{
			grid_clear();
			g_game_state = MENU;
		}
		break;
	}
}

// todo: render frame only when needed (maybe)
void
render(void)
{
	for (size_t i = 0, len = g_frame_width * g_frame_height; i < len; i += 1)
	{
		g_frame_pointer[i] = g_pallet[0];
	}

	switch (g_game_state)
	{
	case MENU:
	{
		uint16_t scale = g_unit_size * 3;

		uint32_t pos_x = (g_frame_width >> 1) - ((glyph_width(scale) * 6) >> 1);
		uint32_t pos_y = (g_frame_height >> 1) - (glyph_height(scale) >> 1);

		const char *text = "TETRIS";

		for (size_t i = 3; *text != 0; i += 1, text += 1)
		{
			glyph_fill(pos_x, pos_y, scale, g_pallet[i], *text - 'A' + 10);
			pos_x += glyph_width(scale);
		}
	}
	break;
	case OVER:
	{
		uint16_t scale = g_unit_size;

		uint32_t pos_x = g_cell_size * 16;
		uint32_t pos_y = g_cell_size * 18;

		uint32_t color = rgb8((uint8_t)tetrino_get_random());

		text_fill(pos_x, pos_y, scale, color, "GAME OVER!!!");
	}
	case GAME:
		grid_draw(g_cell_size * 3, g_cell_size * 2);
		tetrino_draw(g_cell_size * 3, g_cell_size * 2);

		{
			uint16_t scale = g_unit_size;

			uint32_t pos_x = g_cell_size * 16;
			uint32_t pos_y = g_cell_size * 2;

			text_fill(pos_x, pos_y, scale, g_pallet[1], "LINES:");

			pos_y += g_cell_size;

			number_fill(pos_x, pos_y, scale, g_pallet[1], g_lines_cleared);

			pos_y += g_cell_size * 2;

			text_fill(pos_x, pos_y, scale, g_pallet[1], "SCORE?");

			pos_y += g_cell_size;

			text_fill(pos_x, pos_y, scale, g_pallet[1], "00000000");

			pos_y += g_cell_size * 5;

			text_fill(pos_x, pos_y, scale, g_pallet[1], "next:");

			pos_y += g_cell_size;

			rect_draw(pos_x, pos_y, pos_x + g_cell_size * 4 + g_cell_padding * 3,
			          pos_y + g_cell_size * 2 + g_cell_padding * 3, g_cell_padding, g_pallet[1]);

			tetrino_draw_preview(pos_x + g_cell_padding * 2, pos_y + g_cell_padding * 2);
		}
		break;
	}
}
