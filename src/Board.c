#include <genesis.h>
#include "define.h"

void Board_Init(Board *board)
{
    board->currScore = 0;
    board->currLevelLineClear = 0;
    board->lineClearAmount = 0;
    board->xTileOnScreen = BOARD_X_TILE_OFFSET;
    board->yTileOnScreen = BOARD_Y_TILE_OFFSET;
    board->exitGameDelay = GAME_OVER_TOTAL_DELAY_FRAMES;

    for (s16 y = 0; y < BOARD_HEIGHT; y++)
	{
		for (s16 x = 0; x < BOARD_WIDTH; x++)
		{
			if (x == 0 || x == 1 || x == 12 || x == 13 || y == 21 || y == 22)
			{
				board->data[y][x] = BLOCK_TYPE_WALL;
			}
			else
			{
				board->data[y][x] = BLOCK_TYPE_EMPTY;
			}

		}
	}
	for (s16 i = 0; i < 4; i++)
        board->clearLineRows[i] = -1;
}

bool IsFilledLine(Board *board, s16 line)
{
    if (line >= BOARD_SPACE_HEIGHT)
        return FALSE;

    for (int x = 2; x < 2 + BOARD_SPACE_WIDTH; x++)
    {
        if (board->data[line][x] == BLOCK_TYPE_EMPTY)
        {
            return FALSE;
        }
    }
    return TRUE;
}

bool IsEmptyLine(Board *board, s16 line)
{
    for (int x = 2; x < 2 + BOARD_SPACE_WIDTH; x++)
    {
        if (board->data[line][x] != BLOCK_TYPE_EMPTY)
        {
            return FALSE;
        }
    }
    return TRUE;
}

u8 Board_FilledLineCount(Block *block, Board *board)
{
    u8 clearLineCounter = 0;
    for (s16 y = 0; y < 4; y++)
    {
        if (IsFilledLine(board, block->yTileOnBoard + y))
        {
            board->clearLineRows[y] = block->yTileOnBoard + y;
            clearLineCounter++;
        }
        else
        {
            board->clearLineRows[y] = -1;
        }
    }

    return clearLineCounter;
}

u8 Board_ClearLine(Block *block, Board *board)
{
    u8 clearLineCounter = 0;


    for (s8 y = 3; y >= 0; y--)
    {

        if (IsFilledLine(board, block->yTileOnBoard + y + clearLineCounter))
        {
            s16 boardTemp[BOARD_HEIGHT - 2][BOARD_WIDTH];
            memcpy(boardTemp, board->data[0], (block->yTileOnBoard + y + clearLineCounter) * BOARD_WIDTH * 2);
            memcpy(board->data[1], boardTemp,  (block->yTileOnBoard + y + clearLineCounter) * BOARD_WIDTH * 2);
            s16 newLine[BOARD_WIDTH] = {BLOCK_TYPE_WALL, BLOCK_TYPE_WALL, BLOCK_TYPE_EMPTY, BLOCK_TYPE_EMPTY, BLOCK_TYPE_EMPTY, BLOCK_TYPE_EMPTY, BLOCK_TYPE_EMPTY, BLOCK_TYPE_EMPTY, BLOCK_TYPE_EMPTY, BLOCK_TYPE_EMPTY, BLOCK_TYPE_EMPTY, BLOCK_TYPE_EMPTY, BLOCK_TYPE_WALL, BLOCK_TYPE_WALL};
            memcpy(board->data[0], newLine, BOARD_WIDTH *  2);
            clearLineCounter++;
        }
    }
    return clearLineCounter;
}


void Board_RedrawGameFiled(Board *board)
{

    u16 tiles[BOARD_HEIGHT - 2][BOARD_WIDTH - 4];

    for (s16 y = 0; y < BOARD_HEIGHT - 2; y++)
    {
        for (s16 x = 0; x < BOARD_WIDTH - 4; x++)
        {
            tiles[y][x] = TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,TILE_USERINDEX + board->data[y][x + 2]);
        }

    }
    SYS_disableInts();
    VDP_setTileMapDataRect(PLAN_A, tiles[0], board->xTileOnScreen + 2, board->yTileOnScreen,  BOARD_WIDTH - 4, BOARD_HEIGHT - 2);
    SYS_enableInts();
}
