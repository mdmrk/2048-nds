#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <nds.h>
#include <nf_lib.h>

#define BOARD_SIZE 4
#define uint8_t u8
#define uint16_t u16
#define uint32_t u32
#define MOVE_TWO_TILES(a, b, c, d)  \
	{                               \
		board[a][b] += board[c][d]; \
		board[c][d] = 0;            \
		*score += board[a][b];      \
		success = true;             \
	}
#define tap() (keysDown() & KEY_TOUCH)
#define release() (keysUp() & KEY_TOUCH)
#define hold() (keysHeld() & KEY_TOUCH)

typedef enum move_dir
{
	UP,
	DOWN,
	LEFT,
	RIGHT
} move_dir_t;

void print_board(u8 board[BOARD_SIZE][BOARD_SIZE])
{
	for (u8 i = 0; i < BOARD_SIZE; i++)
	{
		for (u8 j = 0; j < BOARD_SIZE; j++)
		{
			printf("%3d", board[i][j]);
		}
		printf("\n");
	}
}

bool move(u8 board[BOARD_SIZE][BOARD_SIZE], move_dir_t *player_move, u32 *score)
{
	bool success = false;

	switch (*player_move)
	{
	case UP:
		for (u8 j = 0; j < BOARD_SIZE; j++)
		{
			for (u8 i = 1; i < BOARD_SIZE; i++)
			{
				if (board[i][j] != 0)
				{
					for (u8 k = i; k > 0; k--)
					{
						if (board[k - 1][j] == board[k][j])
						{
							MOVE_TWO_TILES(k - 1, j, k, j);
							break;
						}
						else if (board[k - 1][j] == 0)
						{
							MOVE_TWO_TILES(k - 1, j, k, j);
						}
						else
							break;
					}
				}
			}
		}
		break;
	case DOWN:
		for (u8 j = 0; j < BOARD_SIZE; j++)
		{
			for (int i = BOARD_SIZE - 2; i >= 0; i--)
			{
				if (board[i][j] != 0)
				{
					for (u8 k = i; k < BOARD_SIZE - 1; k++)
					{
						if (board[k + 1][j] == board[k][j])
						{
							MOVE_TWO_TILES(k + 1, j, k, j);
							break;
						}
						else if (board[k + 1][j] == 0)
						{
							MOVE_TWO_TILES(k + 1, j, k, j);
						}
						else
							break;
					}
				}
			}
		}
		break;
	case LEFT:
		for (u8 i = 0; i < BOARD_SIZE; i++)
		{
			for (u8 j = 1; j < BOARD_SIZE; j++)
			{
				if (board[i][j] != 0)
				{
					for (u8 k = j; k > 0; k--)
					{
						if (board[i][k - 1] == board[i][k])
						{
							MOVE_TWO_TILES(i, k - 1, i, k);
							break;
						}
						else if (board[i][k - 1] == 0)
						{
							MOVE_TWO_TILES(i, k - 1, i, k);
						}
						else
							break;
					}
				}
			}
		}
		break;
	case RIGHT:
		for (u8 i = 0; i < BOARD_SIZE; i++)
		{
			for (int j = BOARD_SIZE - 2; j >= 0; j--)
			{
				if (board[i][j] != 0)
				{
					printf("%d\n", board[i][j]);
					for (u8 k = j; k < BOARD_SIZE - 1; k++)
					{
						if (board[i][k + 1] == board[i][k])
						{
							MOVE_TWO_TILES(i, k + 1, i, k);
							break;
						}
						else if (board[i][k + 1] == 0)
						{
							MOVE_TWO_TILES(i, k + 1, i, k);
						}
						else
							break;
					}
				}
			}
		}
		break;
	}
	return success;
}

bool game_over(u8 board[BOARD_SIZE][BOARD_SIZE])
{
	for (u8 i = 0; i < BOARD_SIZE; i++)
	{
		for (u8 j = 0; j < BOARD_SIZE; j++)
		{
			if (i - 1 >= 0 && (board[i - 1][j] == board[i][j] || board[i - 1][j] == 0))
				return false;
			if (j - 1 >= 0 && (board[i][j - 1] == board[i][j] || board[i][j - 1] == 0))
				return false;
			if (
				i + 1 < BOARD_SIZE &&
				(board[i + 1][j] == board[i][j] || board[i + 1][j] == 0))
				return false;
			if (
				j + 1 < BOARD_SIZE &&
				(board[i][j + 1] == board[i][j] || board[i][j + 1] == 0))
				return false;
		}
	}
	return true;
}

bool win(u8 board[BOARD_SIZE][BOARD_SIZE])
{
	for (u8 i = 0; i < BOARD_SIZE; i++)
	{
		for (u8 j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j] >= 2048)
			{
				return true;
			}
		}
	}
	return false;
}

void place_rand(u8 board[BOARD_SIZE][BOARD_SIZE])
{
	u8 empty_tiles[BOARD_SIZE * BOARD_SIZE][2], empty_tiles_count = 0;

	for (u8 i = 0; i < BOARD_SIZE; i++)
	{
		for (u8 j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j] == 0)
			{
				empty_tiles[empty_tiles_count][0] = i;
				empty_tiles[empty_tiles_count][1] = j;
				empty_tiles_count++;
			}
		}
	}

	if (empty_tiles_count == 0)
		return;

	u8 tile_idx = rand() % empty_tiles_count;
	u8 tile_num = ((rand() % 100) < 80) ? 2 : 4;
	board[empty_tiles[tile_idx][0]][empty_tiles[tile_idx][1]] = tile_num;
}

void init_board(u8 board[BOARD_SIZE][BOARD_SIZE])
{
	for (u8 i = 0; i < BOARD_SIZE; i++)
	{
		for (u8 j = 0; j < BOARD_SIZE; j++)
		{
			board[i][j] = 0;
		}
	}
	place_rand(board);
	place_rand(board);
}

void setup()
{
	srand(time(NULL));
	consoleDemoInit();
	NF_SetRootFolder("NITROFS");

	soundEnable();
	NF_InitRawSoundBuffers();

	NF_Set2D(0, 0);
	NF_Set2D(1, 0);

	NF_InitTiledBgBuffers();
	NF_InitSpriteBuffers();
	NF_InitTiledBgSys(0);
	NF_InitTiledBgSys(1);
	NF_InitSpriteSys(0);
	NF_InitSpriteSys(1);
	NF_InitTextSys(0);
	NF_InitTextSys(1);

	NF_LoadTiledBg("bg/bottom", "bottom", 256, 256);
	NF_CreateTiledBg(1, 0, "bottom");

	NF_LoadSpriteGfx("sprite/touch_icon", 0, 16, 16);
	NF_LoadSpritePal("sprite/touch_icon", 0);
	NF_VramSpriteGfx(1, 0, 0, false);
	NF_VramSpritePal(1, 0, 0);
}

void handle_touch_sprite(u32 *frame, touchPosition *touch)
{
	static u8 touch_frame = 0;

	if (tap())
	{
		if (touch_frame > 0)
		{
			NF_DeleteSprite(1, 0);
			touch_frame = 0;
		}
		NF_CreateSprite(1, 0, 0, 0, touch->px - 8, touch->py - 8);
		touch_frame++;
	}
	else if (touch_frame > 0 && *frame % 5 == 0)
	{
		if (touch_frame > 3)
		{
			NF_DeleteSprite(1, 0);
			touch_frame = 0;
		}
		else
		{
			NF_SpriteFrame(1, 0, touch_frame++);
		}
	}
}

int main_loop()
{
	u8 board[BOARD_SIZE][BOARD_SIZE];
	u32 score = 0;
	move_dir_t player_move;
	touchPosition touch;
	u32 frame = 0;

	init_board(board);
	while (1)
	{
		scanKeys();
		touchRead(&touch);

		handle_touch_sprite(&frame, &touch);
		/*consoleClear();
		scanKeys();
		long int key = keysDown();

		if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN)
		{
			switch (key)
			{
			case KEY_UP:
				player_move = UP;
				break;
			case KEY_DOWN:
				player_move = DOWN;
				break;
			case KEY_LEFT:
				player_move = LEFT;
				break;
			case KEY_RIGHT:
				player_move = RIGHT;
				break;
			default:
			}
			if (move(board, &player_move, &score))
			{
				place_rand(board);
			}
		}
		print_board(board);
		printf("\nScore%8ld\n", score);*/
		NF_SpriteOamSet(0);
		swiWaitForVBlank();
		oamUpdate(&oamMain);
		NF_SpriteOamSet(1);
		oamUpdate(&oamSub);
		frame++;
		frame %= 120;
	}
}

int main(int argc, char **argv)
{
	setup();
	return main_loop();
}
