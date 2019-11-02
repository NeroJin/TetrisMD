#include <genesis.h>
#include "define.h"
#include "gfx.h"

s16 blockBagArray[7];
s16 blockBagIndex = 0;
s16 blockBag[BLOCK_TYPE_AMOUNT * 2] = {0,1,2,3,4,5,6,0,1,2,3,4,5,6};
//Block function
//--------------------------------------------------------------------
void Block_Init(Block *block, Board *board)
{
    BlockBag_GetNextBlock(blockBagArray);
    block->state = IDLE;
    block->currType = blockBagArray[0];
    block->visible = TRUE;
    block->xTileOnBoard = BLOCK_REBORN_TILE_X;
    block->yTileOnBoard = BLOCK_REBORN_TILE_Y;
    block->xTileOnScreen = board->xTileOnScreen + BLOCK_REBORN_TILE_X;
    block->yTileOnScreen = board->yTileOnScreen;
    block->xPixelOnScreen = block->xTileOnScreen * BLOCK_TILE_WIDTH;
    block->yPixelOnScreen = block->yTileOnScreen * BLOCK_TILE_HEIGHT;
    block->rotationState = BLOCK_ROTATION_STATE_0;
    block->speedX = 0;
    block->accelX = 1;
    block->maxSpeedX = 5;
    block->autoFallSpeed = 0;
    block->autoFallAccel = 1;
    block->softDropSpeed = 0;
    block->softDropAccel = 1;
    block->softDropMaxSpeed = 3;
    block->actionCountInLock = 0;
    block->isLock = FALSE;
    block->lockDelay = LOCK_DELAY_FRAMES;
    block->landDelay = 10;
    block->state = IDLE;
    Block_GetRotationData(block->rotationData, block->currType, block->rotationState);
    VDP_setPalette(PAL2, blockSprite.palette->data);
    block->sprite = SPR_addSprite(&blockSprite, block->xPixelOnScreen, block->yPixelOnScreen, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    SPR_setAnim(block->sprite, block->currType);

}

void Block_GetRotationData(s16 rotationData[4][4], s16 blockType, s16 blockRotationState)
{
    for (s16 y = 0; y < 4; y++)
	{
		for (s16 x = 0; x < 4; x++)
		{
			 rotationData[y][x] = blockRotationDatas[blockType][blockRotationState][y][x];

		}
	}

}

s16 Block_GetRotatesLeftState(s16 rotationState)
{
    s16 result = rotationState - 1;
    if (result < 0)
        result = 3;
    return result;
}

s16 Block_GetRotatesRightState(s16 rotationState)
{
    s16 result = rotationState + 1;
    if (result == 4)
        result = 0;
    return result;
}

bool Block_CanMove(Block *block, Board *board, s16 xOffset, s16 yOffset)
{
    bool result = TRUE;
    for (s16 y = 0; y < 4; y++)
    {
        for (s16 x = 0; x < 4; x++)
        {
            if (block->yTileOnBoard + y + yOffset < 0 || block->yTileOnBoard + y + yOffset >= BOARD_HEIGHT || block->xTileOnBoard + x + xOffset < 0 || block->xTileOnBoard + x + xOffset >= BOARD_WIDTH)
            {
                //防止wall kick数组出界
                result = FALSE;
            }
            else if (block->rotationData[y][x] != BLOCK_TYPE_EMPTY && board->data[block->yTileOnBoard + y + yOffset][block->xTileOnBoard + x + xOffset] != BLOCK_TYPE_EMPTY)
            {
                result = FALSE;
            }

        }
    }
    return result;
}

void Block_MoveOnBoard(Block *block, s16 xOnBoardOffset, s16 yOnBoardOffset)
{
    block->xTileOnBoard += xOnBoardOffset;
    block->yTileOnBoard += yOnBoardOffset;
    block->xTileOnScreen += xOnBoardOffset;
    block->yTileOnScreen += yOnBoardOffset;
    block->xPixelOnScreen = block->xTileOnScreen * BLOCK_TILE_WIDTH;
    block->yPixelOnScreen = block->yTileOnScreen * BLOCK_TILE_HEIGHT;
}

void Block_Reborn(Block *block, Board *board)
{
    block->visible = TRUE;
    BlockBag_GetNextBlock(blockBagArray);
    block->currType = blockBagArray[0];
    block->xTileOnBoard = BLOCK_REBORN_TILE_X;
    block->yTileOnBoard = BLOCK_REBORN_TILE_Y;
    block->xTileOnScreen = board->xTileOnScreen + BLOCK_REBORN_TILE_X;
    block->yTileOnScreen = board->yTileOnScreen;
    block->xPixelOnScreen = block->xTileOnScreen * BLOCK_TILE_WIDTH;
    block->yPixelOnScreen = block->yTileOnScreen * BLOCK_TILE_HEIGHT;
    block->rotationState = BLOCK_ROTATION_STATE_0;
    block->speedX = 0;
    block->actionCountInLock = 0;
    block->isLock = FALSE;
    block->lockDelay = LOCK_DELAY_FRAMES;
    block->landDelay = 10;
    block->softDropSpeed = 0;
    Block_GetRotationData(block->rotationData, block->currType, block->rotationState);
    VDP_setPalette(PAL2, blockSprite.palette->data);
}

void Block_RebornFromHolder(Block *block, Board *board)
{
    block->visible = TRUE;
    block->xTileOnBoard = BLOCK_REBORN_TILE_X;
    block->yTileOnBoard = BLOCK_REBORN_TILE_Y;
    block->xTileOnScreen = board->xTileOnScreen + BLOCK_REBORN_TILE_X;
    block->yTileOnScreen = board->yTileOnScreen;
    block->xPixelOnScreen = block->xTileOnScreen * BLOCK_TILE_WIDTH;
    block->yPixelOnScreen = block->yTileOnScreen * BLOCK_TILE_HEIGHT;
    block->rotationState = BLOCK_ROTATION_STATE_0;
    block->speedX = 0;
    block->actionCountInLock = 0;
    block->isLock = FALSE;
    block->lockDelay = LOCK_DELAY_FRAMES;
    block->landDelay = 10;
    Block_GetRotationData(block->rotationData, block->currType, block->rotationState);
    VDP_setPalette(PAL2, blockSprite.palette->data);
}

void Block_Landing(Block *block, Board *board)
{
    for (s16 y = 0; y < 4; y++)
    {
        for (s16 x = 0; x < 4; x++)
        {
            if (block->rotationData[y][x] != BLOCK_TYPE_EMPTY)
                board->data[block->yTileOnBoard + y][block->xTileOnBoard + x] = block->rotationData[y][x];
        }
    }

}

void Block_Update(Block *block)
{
    //SPR_setFrame(block->sprite, block->rotationState);
    if (block->visible)
        SPR_setVisibility(block->sprite, VISIBLE);
    else
        SPR_setVisibility(block->sprite, HIDDEN);

    SPR_setAnim(block->sprite, block->currType);
    SPR_setFrame(block->sprite, block->rotationState);
    SPR_setPosition(block->sprite, block->xPixelOnScreen, block->yPixelOnScreen);
}

u8 Block_GetHeight(Block *block)
{
    for (u8 y = 3; y > 0; y--)
    {
        for (u8 x = 0; x < 4; x++)
        {
            if (block->rotationData[y][x] != BLOCK_TYPE_EMPTY)
                return y + 1;
        }
    }
    return 0;
}

u8 Block_GetWidth(Block *block)
{
    for (u8 x = 3; x > 0; x--)
    {
        for (u8 y = 0; y < 4; y++)
        {
            if (block->rotationData[y][x] != BLOCK_TYPE_EMPTY)
                return x + 1;
        }
    }
    return 0;
}

//BlockBag
//--------------------------------------------------------------------
void BlockBag_Shuffle(s16 start, s16 num, s16 shuffleTime)
{
    for (s16 i = start; i < shuffleTime; i++)
	{
		s16 pos1 = random() % num;
		s16 pos2 = random() % num;
		s16 temp = blockBag[start + pos1];
		blockBag[start + pos1] = blockBag[start + pos2];
		blockBag[start + pos2] = temp;
	}
}

void BlockBag_Init()
{
	blockBagIndex = 0;
	setRandomSeed(GET_HVCOUNTER);

	for (s16 i = 0; i < 7; i++)
	{
		BlockBag_Shuffle(0, 7, 21);
	}

	for (s16 i = 0; i < 7; i++)
	{
		BlockBag_Shuffle(7, 7, 21);
	}
}

void BlockBag_GetNextBlock(s16 blockArray[7])
{
    if (blockBagIndex == 7)
	{
		for (s16 i = 0; i < 7; i++)
		{
			blockArray[i] = blockBag[i] = blockBag[7 + i];
			blockBag[7 + i] = i;
            blockBagIndex = 0;
		}
		BlockBag_Shuffle(7, 7, 21);
	}
	else
	{
		for (s16 i = blockBagIndex; i < blockBagIndex + 7; i++)
		{
			blockArray[i - blockBagIndex] = blockBag[i];
		}
	}
	blockBagIndex++;
}
//--------------------------------------------------------------------

//BlockPreview
//--------------------------------------------------------------------
void BlockPreview_Init(BlockPreview blockPreviewArray[5])
{
    for (int i = 0; i < 5; i++)
    {
        blockPreviewArray[i].xPixelOnScreen = 212;
        blockPreviewArray[i].yPixelOnScreen = i * 32 + 40;
        blockPreviewArray[i].sprite = SPR_addSprite(&blockPreviewSprite, blockPreviewArray[i].xPixelOnScreen, blockPreviewArray[i].yPixelOnScreen, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
        SPR_setAnim(blockPreviewArray[i].sprite, blockBagArray[i + 1]);
    }

}

void BlockPreview_Update(BlockPreview blockPreviewArray[5])
{
    for (int i = 0; i < 5; i++)
    {
        SPR_setAnim(blockPreviewArray[i].sprite, blockBagArray[i + 1]);
    }

}
//--------------------------------------------------------------------

//BlockHolder
//--------------------------------------------------------------------
void BlockHolder_Init(BlockHolder *blockHolder)
{
    blockHolder->currType = BLOCK_TYPE_EMPTY;
    blockHolder->xPixelOnScreen = 52 + 32; //320
    blockHolder->yPixelOnScreen = 40;
    blockHolder->sprite = SPR_addSprite(&blockPreviewSprite, blockHolder->xPixelOnScreen, blockHolder->yPixelOnScreen, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    blockHolder->available = TRUE;
    blockHolder->firstFlag = FALSE;
    SPR_setVisibility(blockHolder->sprite, HIDDEN);
}

void Block_ToHolder(Block *block, BlockHolder *blockHolder, Board *board)
{
    if (!blockHolder->firstFlag)
    {
        //第一次使用
        blockHolder->currType = block->currType;
        SPR_setAnim(blockHolder->sprite, blockHolder->currType);
        block->state = REBORN;
        blockHolder->available = FALSE;
        SPR_setVisibility(blockHolder->sprite, VISIBLE);
        SND_startPlayPCM_XGM(SOUND_HOLD, 10, SOUND_PCM_CH2);
    }
    else if (blockHolder->available)
    {
        s16 tempType = block->currType;
        block->currType = blockHolder->currType;
        blockHolder->currType = tempType;
        SPR_setAnim(blockHolder->sprite, blockHolder->currType);
        blockHolder->available = FALSE;
        Block_RebornFromHolder(block, board);
        SND_startPlayPCM_XGM(SOUND_HOLD, 10, SOUND_PCM_CH2);
    }

}
//--------------------------------------------------------------------

//BlockGhost
//--------------------------------------------------------------------
void BlockGhost_Init(BlockGhost *blockGhost, Block *block, Board *board)
{
    if (block->canShowGhost)
        blockGhost->visible = TRUE;
    else
        blockGhost->visible = FALSE;

    blockGhost->parent = block;
    BlockGhost_UpdatePosition(blockGhost, board);



    blockGhost->sprite = SPR_addSprite(&blockSprite, blockGhost->parent->xPixelOnScreen, GAMEFILED_TOPLEFT_PIXEL_Y + (blockGhost->yTileOnBoard * BLOCK_TILE_HEIGHT), TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    SPR_setAnim(blockGhost->sprite, block->currType  + BLOCK_TYPE_AMOUNT);
}

void BlockGhost_UpdatePosition(BlockGhost *blockGhost, Board *board)
{
    s16 y = 0;
    while (Block_CanMove(blockGhost->parent, board, 0, y) == TRUE)
    {
        y++;

    }

    blockGhost->xTileOnBoard = blockGhost->parent->xTileOnBoard;
    blockGhost->yTileOnBoard = blockGhost->parent->yTileOnBoard + y - 1;
}

void BlockGhost_Update(BlockGhost *blockGhost)
{
    if (blockGhost->visible)
        SPR_setVisibility(blockGhost->sprite, VISIBLE);
    else
        SPR_setVisibility(blockGhost->sprite, HIDDEN);

    SPR_setAnim(blockGhost->sprite, blockGhost->parent->currType + BLOCK_TYPE_AMOUNT);
    SPR_setFrame(blockGhost->sprite, blockGhost->parent->rotationState);
    SPR_setPosition(blockGhost->sprite, blockGhost->parent->xPixelOnScreen, GAMEFILED_TOPLEFT_PIXEL_Y + (blockGhost->yTileOnBoard * BLOCK_TILE_HEIGHT));
}
//--------------------------------------------------------------------

void GetWallKickData(WallKickData outWallKickData[5], s16 blockType, s16 blockRotationStatus, s16 rotationDir)
{
    u8 index = 0;
    u8 i = 0;

    if (rotationDir == 1)
    {
        index = blockRotationStatus;
    }
    else if (rotationDir == -1)
    {
        index = 4 + blockRotationStatus;
    }

    if (blockType == 0)
    {
        for (i = 0; i < 5; i++)
        {
            outWallKickData[i].offsetX = wallKickDataArray[1][index][i].offsetX;
            outWallKickData[i].offsetY = wallKickDataArray[1][index][i].offsetY;
        }
    }
    else if (blockType == 1)
    {
        for (i = 0; i < 5; i++)
        {
            outWallKickData[i].offsetX = 0;
            outWallKickData[i].offsetY = 0;
        }
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
            outWallKickData[i].offsetX = wallKickDataArray[0][index][i].offsetX;
            outWallKickData[i].offsetY = wallKickDataArray[0][index][i].offsetY;
        }
    }
}
