#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <nds.h>
#include <nf_lib.h>

#define BOARD_SIZE 4
#if BOARD_SIZE < 4
#error BOARD_SIZE should be >=4 for proper gameplay
#endif
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
	u8 tile_num = 2;
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

int main(int argc, char **argv)
{
	u8 board[BOARD_SIZE][BOARD_SIZE];
	u32 score = 0;
	move_dir_t player_move;

	consoleDemoInit();
	consoleClear();
	setBrightness(3, 0);

	srand(time(NULL));
	init_board(board);

	while (1)
	{
		consoleClear();
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
		swiWaitForVBlank();
	}

	return 0;
}
