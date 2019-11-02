#include <genesis.h>
#include "define.h"

void Board_Init(Board *board)
{
    board->currScore = 0;
    //board->currLevel = 0;
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

s16 BoardGetFullLineRows(Board *board, s16 lines[4])
{
    s16 clearLineCounter = 0;
    s16 y = BOARD_SPACE_HEIGHT - 1;
    s16 index = 0;

    while (y >= 0)
    {
        //当前行没有任何方块空行的情况
        if (IsEmptyLine(board, y))
        {
            y = 0;
        }

        //当前行全部有方块满行的情况
        if (IsFilledLine(board, y))
        {
            clearLineCounter++;

            //board->data[y][x] = BLOCK_TYPE_EMPTY;
            lines[index] = y;
            index++;
        }
        else
        {
            y--;
        }

    }
    return clearLineCounter;
}

s16 Board_FilledLineCount(Block *block, Board *board)
{
    s16 clearLineCounter = 0;
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

s16 Board_ClearLine2(Block *block, Board *board)
{
    s16 clearLineCounter = 0;


    for (s16 y = 3; y >= 0; y--)
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

s16 Board_ClearLine(Board *board)
{
    s16 clearLineCounter = 0;
    s16 y = BOARD_SPACE_HEIGHT - 1;

    while (y >= 0)
    {
        //当前行没有任何方块空行的情况
        if (IsEmptyLine(board, y))
        {
            y = 0;
        }

        //当前行全部有方块满行的情况
        if (IsFilledLine(board, y))
        {
            clearLineCounter++;
            //使所有行下降
            for (s16 index = y; index >= 0; index--)
            {
                if (index > 0)
                {
                    for (s16 x = 2; x < BOARD_SPACE_WIDTH + 2; x++)
                    {
                        board->data[index][x] = board->data[index - 1][x];
                    }
                }
                else
                {
                    for (s16 x = 2; x < BOARD_SPACE_WIDTH + 2; x++)
                    {
                        board->data[index][x] = BLOCK_TYPE_EMPTY;
                    }
                }
            }

        }
        else
        {
            y--;
        }

    }
    return clearLineCounter;

}



void Board_RedrawGameFiled(Board *board)
{
    /*
        u16 tiles[28][40];
    while (TRUE)
    {
        VDP_showFPS(TRUE);
        for (int y = 0; y < 28; y++) {
            for (int x = 0; x < 40; x++) {
                tiles[y][x] = TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, random() % 16);
            }
            VDP_setTileMapData(VDP_PLAN_B, tiles[y], y * 64, 40, DMA_QUEUE);
        }
        VDP_waitVSync();
    }
    */
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
