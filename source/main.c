#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <nds.h>
#include <nf_lib.h>

#define BOARD_SIZE 4
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define MOVE_TILES(i, j, k, m)      \
	{                               \
		board[i][j] += board[k][m]; \
		board[k][m] = 0;            \
		success = true;             \
	}

#define CHECK_TILE_MOVE(i, j, k, m)     \
	{                                   \
		if (board[i][j] == board[k][m]) \
		{                               \
			MOVE_TILES(i, j, k, m);     \
			*score += board[i][j];      \
			break;                      \
		}                               \
		else if (board[i][j] == 0)      \
		{                               \
			MOVE_TILES(i, j, k, m);     \
		}                               \
		else                            \
		{                               \
			break;                      \
		}                               \
	}

#define REGISTER_SPRITE(name, id, width, height)   \
	{                                              \
		NF_LoadSpriteGfx(name, id, width, height); \
		NF_LoadSpritePal(name, id);                \
		NF_VramSpriteGfx(1, id, id, false);        \
		NF_VramSpritePal(1, id, id);               \
	}

#define TAP() (keysDown() & KEY_TOUCH)
#define RELEASE() (keysUp() & KEY_TOUCH)
#define HOLD() (keysHeld() & KEY_TOUCH)

enum SPRITE_IDS
{
	SPRT_TAP = 0,
	SPRT_2 = 1,
	SPRT_4 = 2,
	SPRT_8 = 3,
	SPRT_16 = 4,
	SPRT_32 = 5,
	SPRT_64 = 6,
	SPRT_128 = 7,
	SPRT_256 = 8,
	SPRT_512 = 9,
	SPRT_1024 = 10,
	SPRT_2048 = 11
};

void print_board(u16 board[BOARD_SIZE][BOARD_SIZE])
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

bool move(u16 board[BOARD_SIZE][BOARD_SIZE], long int player_move, u32 *score)
{
	bool success = false;

	switch (player_move)
	{
	case KEY_LEFT:
		for (u8 j = 0; j < BOARD_SIZE; j++)
		{
			for (u8 i = 1; i < BOARD_SIZE; i++)
			{
				if (board[i][j] != 0)
				{
					for (u8 k = i; k > 0; k--)
					{
						CHECK_TILE_MOVE(k - 1, j, k, j);
					}
				}
			}
		}
		break;
	case KEY_RIGHT:
		for (u8 j = 0; j < BOARD_SIZE; j++)
		{
			for (int i = BOARD_SIZE - 2; i >= 0; i--)
			{
				if (board[i][j] != 0)
				{
					for (u8 k = i; k < BOARD_SIZE - 1; k++)
					{
						CHECK_TILE_MOVE(k + 1, j, k, j);
					}
				}
			}
		}
		break;
	case KEY_UP:
		for (u8 i = 0; i < BOARD_SIZE; i++)
		{
			for (u8 j = 1; j < BOARD_SIZE; j++)
			{
				if (board[i][j] != 0)
				{
					for (u8 k = j; k > 0; k--)
					{
						CHECK_TILE_MOVE(i, k - 1, i, k);
					}
				}
			}
		}
		break;
	case KEY_DOWN:
		for (u8 i = 0; i < BOARD_SIZE; i++)
		{
			for (int j = BOARD_SIZE - 2; j >= 0; j--)
			{
				if (board[i][j] != 0)
				{
					for (u8 k = j; k < BOARD_SIZE - 1; k++)
					{
						CHECK_TILE_MOVE(i, k + 1, i, k);
					}
				}
			}
		}
		break;
	}
	return success;
}

bool game_over(u16 board[BOARD_SIZE][BOARD_SIZE])
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

bool win(u16 board[BOARD_SIZE][BOARD_SIZE])
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

void place_rand(u16 board[BOARD_SIZE][BOARD_SIZE])
{
	u8 empty_tiles[BOARD_SIZE * BOARD_SIZE][2], empty_tiles_count = 0;

	srand(time(NULL));
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
	u16 tile_num = ((rand() % 100) < 80) ? 2 : 4;
	board[empty_tiles[tile_idx][0]][empty_tiles[tile_idx][1]] = tile_num;
}

void init_board(u16 board[BOARD_SIZE][BOARD_SIZE])
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

	NF_LoadRawSound("sounds/tap", 0, 22050, 0);

	NF_LoadTextFont("fnt/font", "normal", 256, 256, 0);
	NF_CreateTextLayer(0, 0, 0, "normal");
	NF_DefineTextColor(0, 0, 0, 255, 183, 3);
	NF_SetTextColor(0, 0, 0);

	NF_LoadTiledBg("bg/top", "top", 256, 256);
	NF_CreateTiledBg(0, 1, "top");
	NF_LoadTiledBg("bg/bottom", "bottom", 256, 256);
	NF_CreateTiledBg(1, 0, "bottom");

	REGISTER_SPRITE("sprite/tap", SPRT_TAP, 16, 16);
	REGISTER_SPRITE("sprite/2", SPRT_2, 32, 32);
	REGISTER_SPRITE("sprite/4", SPRT_4, 32, 32);
	REGISTER_SPRITE("sprite/8", SPRT_8, 32, 32);
	REGISTER_SPRITE("sprite/16", SPRT_16, 32, 32);
	REGISTER_SPRITE("sprite/32", SPRT_32, 32, 32);
	REGISTER_SPRITE("sprite/64", SPRT_64, 32, 32);
	REGISTER_SPRITE("sprite/128", SPRT_128, 32, 32);
	REGISTER_SPRITE("sprite/256", SPRT_256, 32, 32);
	REGISTER_SPRITE("sprite/512", SPRT_512, 32, 32);
	REGISTER_SPRITE("sprite/1024", SPRT_1024, 32, 32);
	REGISTER_SPRITE("sprite/2048", SPRT_2048, 32, 32);
}

void handle_touch_sprite(u32 *frame, touchPosition *touch)
{
	static u8 touch_frame = 0;

	if (TAP())
	{
		if (touch_frame > 0)
		{
			NF_DeleteSprite(1, 0);
			touch_frame = 0;
		}
		NF_CreateSprite(1, 0, 0, 0, touch->px - 8, touch->py - 8);
		NF_PlayRawSound(0, 127, 64, false, 0);
		touch_frame++;
	}
	else if (touch_frame > 0 && *frame % 4 == 0)
	{
		if (touch_frame > 3)
		{
			NF_DeleteSprite(1, 0);
			soundKill(0);
			touch_frame = 0;
		}
		else
		{
			NF_SpriteFrame(1, 0, touch_frame++);
		}
	}
}

u8 get_sprite_id(u16 tile_num)
{
	switch (tile_num)
	{
	case 2:
		return SPRT_2;
		break;
	case 4:
		return SPRT_4;
		break;
	case 8:
		return SPRT_8;
		break;
	case 16:
		return SPRT_16;
		break;
	case 32:
		return SPRT_32;
		break;
	case 64:
		return SPRT_64;
		break;
	case 128:
		return SPRT_128;
		break;
	case 256:
		return SPRT_256;
		break;
	case 512:
		return SPRT_512;
		break;
	case 1024:
		return SPRT_1024;
		break;
	case 2048:
		return SPRT_2048;
		break;
	default:
		return SPRT_2048;
	}
}

// Deal with incorrect read from file :(
void parse_numbers(char *arr, u8 arr_len)
{
	for (u8 i = 0; i < arr_len; i++)
	{
		switch (arr[i])
		{
		case '0':
			arr[i] = 'P';
			break;
		case '1':
			arr[i] = 'Q';
			break;
		case '2':
			arr[i] = 'R';
			break;
		case '3':
			arr[i] = 'S';
			break;
		case '4':
			arr[i] = 'T';
			break;
		case '5':
			arr[i] = 'U';
			break;
		case '6':
			arr[i] = 'V';
			break;
		case '7':
			arr[i] = 'W';
			break;
		case '8':
			arr[i] = 'X';
			break;
		case '9':
			arr[i] = 'Y';
		}
	}
}

void spawn_tile_sprites(u16 board[BOARD_SIZE][BOARD_SIZE], u8 tile_sprites[BOARD_SIZE * BOARD_SIZE], u8 tile_coords[BOARD_SIZE * BOARD_SIZE][2])
{
	for (u8 i = 0; i < BOARD_SIZE; i++)
	{
		for (u8 j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j] != 0)
			{
				u8 sprite_id = get_sprite_id(board[i][j]);

				NF_CreateSprite(1, 10 + i * BOARD_SIZE + j, sprite_id, sprite_id, tile_coords[i * BOARD_SIZE + j][0], tile_coords[i * BOARD_SIZE + j][1]);
				tile_sprites[i * BOARD_SIZE + j] = 1;
			}
		}
	}
}

void clear_tile_sprites(u16 board[BOARD_SIZE][BOARD_SIZE], u8 tile_sprites[BOARD_SIZE * BOARD_SIZE])
{
	for (u8 i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (tile_sprites[i] != 0)
		{
			NF_DeleteSprite(1, 10 + i);
			tile_sprites[i] = 0;
		}
	}
}

void print_score(u32 score)
{
	char str[32];

	sprintf(str, "score %ld", score);
	parse_numbers(str, 32);
	NF_WriteText(0, 0, 1, 22, str);
}

int main_loop()
{
	u16 board[BOARD_SIZE][BOARD_SIZE];
	u8 tile_coords[BOARD_SIZE * BOARD_SIZE][2];
	u8 tile_sprites[BOARD_SIZE * BOARD_SIZE];
	u32 score = 0, frame = 0;
	touchPosition touch;

	init_board(board);

	for (u8 i = 36 + 15.5, m = 0; m < BOARD_SIZE; i += 31, m++)
	{
		for (u8 j = 28 + 15.5, n = 0; n < BOARD_SIZE; j += 31, n++)
		{
			tile_coords[m * BOARD_SIZE + n][0] = i - 15.5;
			tile_coords[m * BOARD_SIZE + n][1] = j - 15.5;
		}
	}

	for (u8 i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		tile_sprites[i] = 0;
	}

	spawn_tile_sprites(board, tile_sprites, tile_coords);
	while (1)
	{
		scanKeys();
		touchRead(&touch);
		handle_touch_sprite(&frame, &touch);

		long int key = keysDown();
		switch (key)
		{
		case KEY_UP:
		case KEY_DOWN:
		case KEY_LEFT:
		case KEY_RIGHT:
			clear_tile_sprites(board, tile_sprites);
			if (move(board, key, &score))
			{
				place_rand(board);
			}
			spawn_tile_sprites(board, tile_sprites, tile_coords);
			break;
		default:
		}
		print_score(score);
		NF_UpdateTextLayers();
		NF_SpriteOamSet(0);
		oamUpdate(&oamMain);
		NF_SpriteOamSet(1);
		oamUpdate(&oamSub);
		swiWaitForVBlank();
		if (frame++ == 60)
		{
			frame = 0;
		}
	}
	return 1;
}

int main(int argc, char **argv)
{
	setup();
	return main_loop();
}
