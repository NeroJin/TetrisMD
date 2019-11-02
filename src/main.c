#include <genesis.h>
#include "gfx.h"
#include "music.h"
#include "define.h"

u8 autoFallSpeedLevel[AUTO_FALL_SPEED_LEVEL_MAX] =   { 53, 48, 42, 35, 27, 10, 14, 9, 5, 4, 3};
u8 levelClearLineNum[AUTO_FALL_SPEED_LEVEL_MAX] =    { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 };
//u8 levelClearLineNum[AUTO_FALL_SPEED_LEVEL_MAX] =    { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

static void UpdatePhysic();

static void joyEventOnGamePlay(u16 joy, u16 changed, u16 state);
static void joyEventOnGameOver(u16 joy, u16 changed, u16 state);
static void joyEventOnGameTitle(u16 joy, u16 changed, u16 state);
static void joyEventOnGameOption(u16 joy, u16 changed, u16 state);
static void joyEventOnNoControl(u16 joy, u16 changed, u16 state);

void BlockMoveLeft();
void BlockMoveRight();
void BlockRotateLeft();
void BlockRotateRight();
void BlockSoftDrop();
void BlockHardDrop();
void BlockAutoFall();
void BlockLock();

void LineClearAnimInit();
void LineClearAnimUpdate(u8 step);
void HelperSpriteInit();
void GameOverAnimUpdate(u8 row);
void ShowPauseSprite(bool paused);
void ShowGameOverSprite();
void SoundInit();

void CommonInit();
void GameTitle();
void GameOption();
void GamePlay();

void OptionsInit();
void OptionMoveLeft(bool loop);
void OptionMoveRight(bool loop);
void OptionMoveUp(bool loop);
void OptionMoveDown(bool loop);
void SelectOption();

u16 LandScore(Block *block, Board *board);

bool paused;

Board board1;
Block block1;
BlockGhost blockGhost1;


BlockPreview blockPreviewArray[5];
BlockHolder blockHolder1;



Position option0Pos[10];
Position option1Pos[2];
Position option2Pos[2];
Position option3Pos[2];
Position option4Pos[1];

Game game;
Option options[5];
Option *currOption;

Sprite *lineClearSprites[4];
Sprite *spriteGameOver;
Sprite *spritePause;

u8 lineClearAnimStep = 0;
u8 gameOverAnimSpeed = 0;
u8 currCmd = 0;

int main()
{
    CommonInit();
    while (TRUE)
    {
        switch (game.scene)
        {
            case GAME_TITLE:
                GameTitle();
                break;
            case GAME_OPTION:
                GameOption();
                break;
            case GAME_PLAY:
                GamePlay();
                break;
        }

    }
    return 0;
}

void CommonInit()
{
    SYS_disableInts();

    VDP_setScreenHeight224();
    VDP_setScreenWidth320();

    SYS_enableInts();
    SoundInit();
    //SPR_init();

    game.scene = GAME_TITLE;

}

void GameTitle()
{
    SPR_init();
    SYS_disableInts();
    VDP_clearPlan(PLAN_A, TRUE);
    VDP_clearPlan(PLAN_B, TRUE);

    VDP_loadTileSet(titleImage.tileset,TILE_USERINDEX,DMA);
	VDP_setPalette(PAL1, titleImage.palette->data);
	VDP_setMap(PLAN_A, titleImage.map, TILE_ATTR_FULL(PAL1, 0, 0, 0, TILE_USERINDEX), 0, 0);
	SYS_enableInts();

    JOY_init();


    SND_startPlay_XGM(title_xgm);

	s16 frames = 0;
	game.exitCurrScene = FALSE;
	game.loop = TRUE;
	while (game.loop)
    {
        if (frames >= 60)
        {
            SYS_disableInts();
            VDP_drawText("PRESS START BUTTON" , 10, 20);
            SYS_enableInts();
            JOY_setEventHandler(joyEventOnGameTitle);
            frames = 0;
        }
        else if (frames == 30)
        {
            SYS_disableInts();
            VDP_clearTextArea(10, 20, 18, 1);
            SYS_enableInts();
        }

        if (game.exitCurrScene)
        {
            game.loop = FALSE;
            game.scene = GAME_OPTION;
        }

        frames++;

        SPR_update();
        VDP_waitVSync();
    }
}

void GameOption()
{

    SPR_init();

    JOY_init();
	JOY_setEventHandler(joyEventOnGameOption);
    SYS_disableInts();

    VDP_clearPlan(PLAN_A, TRUE);
    VDP_clearPlan(PLAN_B, TRUE);
    VDP_setPalette(PAL2, optionSprite.palette->data);


    SPR_setVisibility(options[0].sprite, VISIBLE);
	SYS_enableInts();


    OptionsInit();

	SPR_update();

    game.exitCurrScene = FALSE;
	game.loop = TRUE;
	while (game.loop)
    {

        if (game.exitCurrScene)
        {
            SPR_reset();
            SPR_clear();
            game.loop = FALSE;
            game.scene = GAME_PLAY;
        }


        SYS_disableInts();
        VDP_drawText("LEVEL", 7, 3);
        VDP_drawText("0 1 2 3 4 5 6 7 8 9", 9, 4);
        VDP_drawText("WALL KICK", 7, 7);
        VDP_drawText("YES  NO", 9, 8);
        VDP_drawText("LOCK DELAY", 7, 11);
        VDP_drawText("YES  NO", 9, 12);
        VDP_drawText("SHOW GHOST", 7, 15);
        VDP_drawText("YES  NO", 9, 16);
        VDP_drawText("START", 7, 19);
        SYS_enableInts();

        SPR_setPosition(options[0].sprite, option0Pos[options[0].indexCol].x, option0Pos[options[0].indexCol].y);
        SPR_setPosition(options[1].sprite, option1Pos[options[1].indexCol].x, option1Pos[options[1].indexCol].y);
        SPR_setPosition(options[2].sprite, option2Pos[options[2].indexCol].x, option2Pos[options[2].indexCol].y);
        SPR_setPosition(options[3].sprite, option3Pos[options[3].indexCol].x, option3Pos[options[3].indexCol].y);
        SPR_setPosition(options[4].sprite, option4Pos[options[4].indexCol].x, option4Pos[options[4].indexCol].y);

        SPR_setAnim(options[0].sprite, options[0].anim);
        SPR_setAnim(options[1].sprite, options[1].anim);
        SPR_setAnim(options[2].sprite, options[2].anim);
        SPR_setAnim(options[3].sprite, options[3].anim);
        SPR_setAnim(options[4].sprite, options[4].anim);

        SPR_update();
        VDP_waitVSync();
    }
}

void GamePlay()
{
    paused = FALSE;

    VDP_clearPlan(PLAN_A, TRUE);
    VDP_clearPlan(PLAN_B, TRUE);

    //初始化SDGK的sprite引擎
    SPR_init();
    //初始化...
    BlockBag_Init();
    Board_Init(&board1);
    Block_Init(&block1, &board1);

    //TEST
    //block1.canWallKick = TRUE;
    //block1.canLockDelay = TRUE;
    //block1.canShowGhost = TRUE;

    BlockPreview_Init(blockPreviewArray);
    BlockHolder_Init(&blockHolder1);

    if (block1.canShowGhost)
    {
        BlockGhost_Init(&blockGhost1, &block1, &board1);
    }

    JOY_init();

	JOY_setEventHandler(joyEventOnGamePlay);
	HelperSpriteInit();

    // disable interrupt when accessing VDP
    SYS_disableInts();


    VDP_loadTileSet(boardImage.tileset,TILE_USERINDEX,DMA);
	VDP_setPalette(PAL1, boardImage.palette->data);

    VDP_loadTileSet(bg2Image.tileset, TILE_USERINDEX + 10, DMA);
    VDP_setPalette(PAL3, bg2Image.palette->data);


    VDP_setMap(PLAN_B, bg2Image.map, TILE_ATTR_FULL(PAL3, 0, 0, 0, TILE_USERINDEX + 10), 0, 0);

    LineClearAnimInit();

    // VDP process done, we can re enable interrupts
    SYS_enableInts();
    SPR_update();
    SND_startPlay_XGM(bgm_xgm);

    game.exitCurrScene = FALSE;
	game.loop = TRUE;
    while(game.loop)
    {

        if (!paused)
        {

            UpdatePhysic();
            if (block1.canShowGhost)
            {
                BlockGhost_UpdatePosition(&blockGhost1, &board1);
                BlockGhost_Update(&blockGhost1);
            }

            Block_Update(&block1);
            LineClearAnimUpdate(lineClearAnimStep);


            SYS_disableInts();
            Board_RedrawGameFiled(&board1);

            SYS_enableInts();
        }

        SYS_disableInts();

        char str[32];

        intToStr(board1.currLevel, str, 1);
        VDP_drawText("LEVEL", 5, 13);
        VDP_drawText(str, 5, 14);

        intToStr(board1.currScore, str, 1);
        VDP_drawText("SCORE", 5, 16);
        VDP_drawText(str, 5, 17);

        intToStr(board1.lineClearAmount, str, 1);
        VDP_drawText("LINES", 5, 19);
        VDP_drawText(str, 5, 20);

        SYS_enableInts();

        SPR_update();

        VDP_waitVSync();

        //VDP_showFPS(0);
    }


}

void joyEventOnNoControl(u16 joy, u16 changed, u16 state)
{
    ;
}

void joyEventOnGameTitle(u16 joy, u16 changed, u16 state)
{
    if (changed & state & BUTTON_START)
    {
        game.exitCurrScene = TRUE;
        SND_startPlayPCM_XGM(SOUND_START, 10, SOUND_PCM_CH2);
        //SND_stopPlay_XGM();
    }
}

void joyEventOnGameOption(u16 joy, u16 changed, u16 state)
{
    if (changed & state & BUTTON_LEFT)
    {
        OptionMoveLeft(FALSE);
    }
    else if (changed & state & BUTTON_RIGHT)
    {
        OptionMoveRight(FALSE);
    }
    else if (changed & state & BUTTON_UP)
    {
        OptionMoveUp(FALSE);
    }
    else if (changed & state & BUTTON_DOWN)
    {
        OptionMoveDown(FALSE);
    }

    if (changed & state & BUTTON_A || changed & state & BUTTON_B || changed & state & BUTTON_C || changed & state & BUTTON_START)
    {
        if (currOption->indexRow == 4)
        {
            SelectOption();
            game.exitCurrScene = TRUE;
            SND_startPlayPCM_XGM(SOUND_START, 10, SOUND_PCM_CH2);
            SND_stopPlay_XGM();
        }
    }

}

void joyEventOnGameOver(u16 joy, u16 changed, u16 state)
{
    if (changed & state & BUTTON_A || changed & state & BUTTON_B || changed & state & BUTTON_C || changed & state & BUTTON_START)
    {
        game.exitCurrScene = TRUE;
    }

}

//手柄控制处理
void joyEventOnGamePlay(u16 joy, u16 changed, u16 state) //state = pressed , changed = released
{

    if (changed & state & BUTTON_START)
    {
        paused = !paused;

        if (paused)
        {
            SPR_setVisibility(block1.sprite, HIDDEN);
            SPR_setVisibility(blockGhost1.sprite, HIDDEN);
            block1.visible = FALSE;
            blockGhost1.visible = FALSE;
            VDP_clearPlan(PLAN_A, TRUE);
            ShowPauseSprite(paused);
            SND_pausePlay_XGM();
            SND_startPlayPCM_XGM(SOUND_PAUSE, 10, SOUND_PCM_CH2);
        }
        else
        {
            ShowPauseSprite(paused);
            block1.visible = TRUE;

            if (block1.canShowGhost)
                blockGhost1.visible = TRUE;

            Board_RedrawGameFiled(&board1);
            SND_startPlayPCM_XGM(SOUND_PAUSE, 10, SOUND_PCM_CH2);
            SND_resumePlay_XGM ();
        }

    }

    if (joy == JOY_1 && !paused)
    {
        if (state & BUTTON_LEFT)
        {

            if (block1.state == IDLE)
            {
                block1.xDir = -1;
                currCmd &= CMD_SOFT_DROP_CLR ;
            }


        }
        else if (state & BUTTON_RIGHT)
        {
            if (block1.state == IDLE)
            {
                block1.xDir = 1;
                currCmd &= CMD_SOFT_DROP_CLR;
            }

        }
        else if (changed & BUTTON_RIGHT || changed & BUTTON_LEFT)
		{
			block1.xDir = 0;

		}

        else if (state & BUTTON_UP)
        {

            if (block1.state == IDLE)
                currCmd |= CMD_HARD_DROP;
        }

        else if (state & BUTTON_DOWN)
        {

            if (block1.state == IDLE)
                currCmd |= CMD_SOFT_DROP;
        }
        else if (changed & BUTTON_DOWN)
        {
            currCmd &= CMD_SOFT_DROP_CLR;
        }






        if (state & BUTTON_A)
        {
            if (block1.state == IDLE)
            {
                Block_ToHolder(&block1, &blockHolder1, &board1);

            }

        }
        else if (state & BUTTON_B)
        {
            if (block1.state == IDLE)
                currCmd |= CMD_ROTATE_LEFT;
        }
        else if (state & BUTTON_C)
        {
            if (block1.state == IDLE)
                currCmd |= CMD_ROTATE_RIGHT;
        }

    }
}

static void UpdatePhysic()
{
    if (block1.state == IDLE)
    {
        //向右移动
        if (block1.xDir == 1)
        {
            if (block1.speedX < 0)
            {
                //刚从左边切换过来
                block1.speedX = 0;
            }

            if (block1.speedX >= block1.maxSpeedX)
            {
                block1.speedX = 0;
                currCmd |= CMD_MOVE_RIGHT;

            }
            //累加加速度
            block1.speedX += block1.accelX;
        }
        else if (block1.xDir == -1)
        {
            if (block1.speedX > 0)
            {
                //刚从右边切换过来
                block1.speedX = 0;
            }


            if (block1.speedX <= (-1 * block1.maxSpeedX))
            {
                block1.speedX = 0;
                currCmd |= CMD_MOVE_LEFT;
            }
            //累加加速度
            block1.speedX -= block1.accelX;

        }

        if (!block1.isLock)
        {
             //不在锁定时自动下降
            if (block1.autoFallSpeed >= block1.autoFallMaxSpeed)
            {
                block1.autoFallSpeed = 0;
                currCmd |= CMD_AUTO_FALL;
            }
            block1.autoFallSpeed += block1.autoFallAccel;
        }

    }

    if ( block1.state == IDLE && block1.canLockDelay )
    {
        if (block1.lockDelay != 0 && block1.isLock)
        {
            //开始锁定并倒计时。
            block1.lockDelay--;
            //Block_Moving(&block1, &board1);

        }
        else if (block1.lockDelay == 0 && block1.isLock)
        {
            //开始锁定并完成倒计时，判断是否着地，如是就直接着地，否则取消锁定。
            if (Block_CanMove(&block1, &board1, 0, 1))
            {
                currCmd |= CMD_AUTO_FALL;
            }
            else
            {
                //直接着地。
                currCmd |= CMD_LOCK;
                block1.landType = NORMAL_LAND;
            }
        }

        if (block1.actionCountInLock >= MAX_ACTION_IN_LOCK && block1.isLock)
        {
            //开始锁定并完成倒计时，判断是否着地，如是就直接着地，否则取消锁定。
            if (Block_CanMove(&block1, &board1, 0, 1))
            {
                currCmd |= CMD_AUTO_FALL;
            }
            else
            {
                //直接着地。
                currCmd |= CMD_LOCK;
                block1.landType = NORMAL_LAND;
            }
        }

    }


    switch(block1.state)
    {
        case IDLE:


            if (currCmd & CMD_LOCK)
            {
                BlockLock();
                currCmd &= CMD_LOCK_CLR;
            }

            else if (currCmd & CMD_HARD_DROP)
            {
                BlockHardDrop();

            }
            else if (currCmd & CMD_SOFT_DROP)
            {
                BlockSoftDrop();
            }
            else if (currCmd & CMD_AUTO_FALL)
            {
                BlockAutoFall();
                currCmd &= CMD_AUTO_FALL_CLR;
            }
            else if (currCmd & CMD_MOVE_LEFT)
            {
                BlockMoveLeft();
                currCmd &= CMD_MOVE_LEFT_CLR;
            }
            else if (currCmd & CMD_MOVE_RIGHT)
            {
                BlockMoveRight();
                currCmd &= CMD_MOVE_RIGHT_CLR;
            }
            else if (currCmd & CMD_ROTATE_LEFT)
            {
                BlockRotateLeft();
                currCmd &= CMD_ROTATE_LEFT_CLR;
            }
            else if (currCmd & CMD_ROTATE_RIGHT)
            {
                BlockRotateRight();
                currCmd &= CMD_ROTATE_RIGHT_CLR;
            }


            break;


        case LANDING:
            if (block1.landDelay != 0)
            {
                block1.landDelay--;

            }
            else if (block1.landDelay == 0)
            {
                block1.landRow = BOARD_SPACE_HEIGHT - (block1.yTileOnBoard + Block_GetHeight(&block1));

                board1.currScore += LandScore(&block1, &board1);
                Block_Landing(&block1, &board1);
                block1.visible = FALSE;
                blockGhost1.visible = FALSE;

                //检查是否有填充满的行
                if (Board_FilledLineCount(&block1, &board1) != 0)
                {
                    block1.state = CLEARING;
                    board1.clearLineDelay = 40;
                    //播放消行动画
                     SND_startPlayPCM_XGM(SOUND_LINE_CLEAR, 10, SOUND_PCM_CH2);

                }
                else
                {
                    //已完成落地 加分
                    block1.state = REBORN;
                }


            }

            break;

        case CLEARING:
            if (board1.clearLineDelay == 40)
            {
                lineClearAnimStep = 1;
            }
            else if (board1.clearLineDelay == 30)
            {
                lineClearAnimStep = 2;
            }
            else if (board1.clearLineDelay == 20)
            {
                lineClearAnimStep = 3;
            }
             else if (board1.clearLineDelay == 10)
            {
                lineClearAnimStep = 4;
            }
            else if (board1.clearLineDelay == 0)
            {
                lineClearAnimStep = 5;
                u8 lineClearCount = Board_ClearLine(&block1, &board1);
                board1.currLevelLineClear += lineClearCount;
                board1.lineClearAmount += lineClearCount;
                //消行得分
                if (lineClearCount == 1)
                {
                    board1.currScore += 100;
                }
                else if (lineClearCount == 2)
                {
                    board1.currScore += 400;
                }
                else if (lineClearCount == 3)
                {
                    board1.currScore += 900;
                }
                else if (lineClearCount == 4)
                {
                    board1.currScore += 2500;
                }

                if (board1.currLevelLineClear >= levelClearLineNum[board1.currLevel])
                {
                    board1.currLevelLineClear = board1.currLevelLineClear - levelClearLineNum[board1.currLevel];
                    board1.currLevel++;
                    block1.autoFallMaxSpeed = autoFallSpeedLevel[board1.currLevel];
                    if (board1.currLevel > AUTO_FALL_SPEED_LEVEL_MAX)
                        board1.currLevel = AUTO_FALL_SPEED_LEVEL_MAX;
                }
                block1.state = REBORN;
            }

            board1.clearLineDelay--;

            break;

        case REBORN:
            Block_Reborn(&block1, &board1);
            BlockPreview_Update(blockPreviewArray);
            if (!blockHolder1.available && !blockHolder1.firstFlag)
            {
                blockHolder1.available = FALSE;
                blockHolder1.firstFlag =  TRUE;
            }
            else if (!blockHolder1.available)
            {
                 blockHolder1.available = TRUE;
            }

            if (Block_CanMove(&block1, &board1, 0, 0))
            {
                block1.state = IDLE;
                u16 value = JOY_readJoypad(JOY_1);
                if (value & BUTTON_RIGHT)
                    block1.xDir = 1;
                else if (value & BUTTON_LEFT)
                    block1.xDir = -1;

                blockGhost1.visible = TRUE;
            }
            else
            {
                //game over
                blockGhost1.visible = FALSE;
                Block_Landing(&block1, &board1);
                block1.visible = FALSE;
                board1.gameOverDelay = BOARD_SPACE_HEIGHT + 1;
                gameOverAnimSpeed = ANIM_GAMEOVER_SPEDD_MAX;
                SND_stopPlay_XGM();
                SND_startPlayPCM_XGM(SOUND_GAME_OVER, 10, SOUND_PCM_CH2);
                JOY_setEventHandler(joyEventOnNoControl);
                block1.state = GAME_OVER;
            }

            break;


        case GAME_OVER:
            if (board1.gameOverDelay > 0)
            {
                if (gameOverAnimSpeed >= ANIM_GAMEOVER_SPEDD_MAX)
                {


                    board1.gameOverDelay--;
                    gameOverAnimSpeed = 0;
                    GameOverAnimUpdate(BOARD_SPACE_HEIGHT - board1.gameOverDelay);
                    Board_RedrawGameFiled(&board1);
                }
                gameOverAnimSpeed++;
            }
            else if (board1.gameOverDelay == 0)
            {
                //VDP_drawText("GAME OVER", 13, 1);
                ShowGameOverSprite();
                if (board1.currScore > game.hiScore)
                {
                    game.hiScore = board1.currScore;
                }

                block1.state = EXIT_GAME;
            }


            break;

        case EXIT_GAME:
            if (board1.exitGameDelay == GAME_OVER_SOUND_DELAY_FRAMES)
            {
                JOY_setEventHandler(joyEventOnGameOver);
            }
            else if (board1.exitGameDelay == 0)
            {
                game.exitCurrScene = TRUE;
            }

            board1.exitGameDelay--;

            if (game.exitCurrScene)
            {
                game.loop = FALSE;
                game.scene = GAME_TITLE;
            }

            break;

    }


}


void BlockMoveLeft()
{
    if (Block_CanMove(&block1, &board1, -1, 0))
    {
        Block_MoveOnBoard(&block1, -1, 0);
    }

    if (block1.isLock && block1.canLockDelay)
    {
        block1.actionCountInLock++;
        block1.lockDelay = LOCK_DELAY_FRAMES;
    }

}

void BlockMoveRight()
{
    if (Block_CanMove(&block1, &board1, 1, 0))
    {
        Block_MoveOnBoard(&block1, 1, 0);

    }

    if (block1.isLock && block1.canLockDelay)
    {
        block1.actionCountInLock++;
        block1.lockDelay = LOCK_DELAY_FRAMES;
    }
}

void BlockRotateLeft()
{
    if (block1.canWallKick)
    {
        u8 stepTry = 0;
        GetWallKickData(block1.wallKickData, block1.currType, block1.rotationState, BLOCK_ROTATE_LEFT_DIR);
        block1.rotationState = Block_GetRotatesLeftState(block1.rotationState);
        Block_GetRotationData(block1.rotationData, block1.currType, block1.rotationState);
        for (s16 i = 0; i < 5; i++)
        {
            //进行踢墙的5次位移尝试。
            if (Block_CanMove(&block1, &board1, block1.wallKickData[i].offsetX, block1.wallKickData[i].offsetY))
            {
                stepTry = i + 1;
                Block_MoveOnBoard(&block1, block1.wallKickData[i].offsetX, block1.wallKickData[i].offsetY);
                if (block1.isLock && block1.canLockDelay)
                {
                    block1.actionCountInLock++;
                    block1.lockDelay = LOCK_DELAY_FRAMES;
                }
                break;
            }
        }
        if (stepTry == 0)
        {
            block1.rotationState = Block_GetRotatesRightState(block1.rotationState);
            Block_GetRotationData(block1.rotationData, block1.currType, block1.rotationState);
        }
    }
    else
    {
        block1.rotationState = Block_GetRotatesLeftState(block1.rotationState);
        Block_GetRotationData(block1.rotationData, block1.currType, block1.rotationState);
        if (!Block_CanMove(&block1, &board1, 0, 0))
        {
            block1.rotationState = Block_GetRotatesRightState(block1.rotationState);
            Block_GetRotationData(block1.rotationData, block1.currType, block1.rotationState);
        }
        else
        {
            if (block1.isLock && block1.canLockDelay)
            {
                block1.actionCountInLock++;
                block1.lockDelay = LOCK_DELAY_FRAMES;
            }
        }

    }
}

void BlockRotateRight()
{
    if (block1.canWallKick)
    {
        u8 stepTry = 0;
        GetWallKickData(block1.wallKickData, block1.currType, block1.rotationState, BLOCK_ROTATE_RIGHT_DIR);

        block1.rotationState = Block_GetRotatesRightState(block1.rotationState);
        Block_GetRotationData(block1.rotationData, block1.currType, block1.rotationState);

        for (s16 i = 0; i < 5; i++)
        {
            if (Block_CanMove(&block1, &board1, block1.wallKickData[i].offsetX, block1.wallKickData[i].offsetY))
            {
                stepTry = i + 1;
                Block_MoveOnBoard(&block1, block1.wallKickData[i].offsetX, block1.wallKickData[i].offsetY);
                if (block1.isLock && block1.canLockDelay)
                {
                    block1.actionCountInLock++;
                    block1.lockDelay = LOCK_DELAY_FRAMES;
                }
                break;
            }
        }
        if (stepTry == 0)
        {
            block1.rotationState = Block_GetRotatesLeftState(block1.rotationState);
            Block_GetRotationData(block1.rotationData, block1.currType, block1.rotationState);
        }
    }
    else
    {
        block1.rotationState = Block_GetRotatesRightState(block1.rotationState);
        Block_GetRotationData(block1.rotationData, block1.currType, block1.rotationState);
        if (!Block_CanMove(&block1, &board1, 0, 0))
        {
            block1.rotationState = Block_GetRotatesLeftState(block1.rotationState);
            Block_GetRotationData(block1.rotationData, block1.currType, block1.rotationState);
        }
        else
        {
            if (block1.isLock && block1.canLockDelay)
            {
                block1.actionCountInLock++;
                block1.lockDelay = LOCK_DELAY_FRAMES;
            }
        }
    }
}

void BlockAutoFall()
{
    if (Block_CanMove(&block1, &board1, 0, 1))
    {
        //下落一行
        Block_MoveOnBoard(&block1, 0, 1);

        //马上判断是否落地----------------------------------
        if (block1.canLockDelay)
        {
            //重置lockDelay计时器
            block1.lockDelay = LOCK_DELAY_FRAMES;
            block1.isLock = FALSE;
            block1.actionCountInLock = 0;

        }
        if (!Block_CanMove(&block1, &board1, 0, 1))
        {
            if (block1.canLockDelay)
            {
                block1.isLock = TRUE;
            }
            else
            {
                currCmd |= CMD_LOCK;
                block1.landType = NORMAL_LAND;
            }
        }

    }
    else
    {
        if (!block1.isLock && block1.canLockDelay)
        {
            //start locking
            block1.isLock = TRUE;
        }
        else
        {
            //不能下移且锁定为真
            currCmd |= CMD_LOCK;
            block1.landType = NORMAL_LAND;
        }

    }
}

void BlockSoftDrop()
{
    block1.xDir = 0;
    block1.speedX = 0;
    block1.softDropSpeed += block1.softDropAccel;
    if (block1.softDropSpeed >= block1.softDropMaxSpeed)
    {
        block1.softDropSpeed = 0;

        if (Block_CanMove(&block1, &board1, 0, 1))
        {
            Block_MoveOnBoard(&block1, 0, 1);
            if (block1.canLockDelay)
            {
                block1.lockDelay = LOCK_DELAY_FRAMES;
                block1.isLock = FALSE;
                block1.actionCountInLock = 0;
            }

        }
        else
        {
            if (block1.canLockDelay)
            {
                block1.isLock = TRUE;
                block1.lockDelay = 0;
                block1.landType = SOFT_LAND;
            }
            else
            {
                currCmd |= CMD_LOCK;
                block1.landType = SOFT_LAND;
            }

        }
    }
}

void BlockHardDrop()
{
    block1.xDir = 0;
    block1.speedX = 0;
    if (Block_CanMove(&block1, &board1, 0, 1))
    {
        Block_MoveOnBoard(&block1, 0, 1);
    }
    else
    {
        currCmd &= CMD_HARD_DROP_CLR;
        currCmd |= CMD_LOCK;
        block1.landType = HARD_LAND;
    }
}

void BlockLock()
{

    SND_startPlayPCM_XGM(SOUND_FALL, 10, SOUND_PCM_CH2);
    static const u16 palette_white[16] = { 0x0000,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff};
    VDP_setPalette(PAL2, palette_white);
    block1.landDelay = 10;
    block1.state = LANDING;
}

u16 LandScore(Block *block, Board *board)
{
    u16 score;
    score = min((block->landType * (board->currLevel + 1) * ((board->currLevel + 1) + block->landRow)), 999);
    return score;
}

void LineClearAnimInit()
{
    lineClearSprites[0] = SPR_addSprite(&lineClearSprite, 99, 99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    lineClearSprites[1] = SPR_addSprite(&lineClearSprite, 99, 99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    lineClearSprites[2] = SPR_addSprite(&lineClearSprite, 99, 99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    lineClearSprites[3] = SPR_addSprite(&lineClearSprite, 99, 99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    SPR_setVisibility(lineClearSprites[0], HIDDEN);
    SPR_setVisibility(lineClearSprites[1], HIDDEN);
    SPR_setVisibility(lineClearSprites[2], HIDDEN);
    SPR_setVisibility(lineClearSprites[3], HIDDEN);
}

void LineClearAnimUpdate(u8 step)
{
    switch (step)
    {
        case 0:
            break;
        case 1:
        {
            for (u8 i = 0; i < 4; i++)
            {
                if (board1.clearLineRows[i] != -1)
                {

                    SPR_setPosition(lineClearSprites[i], (BOARD_X_TILE_OFFSET + BOARD_WALL_X_TILE_OFFSET) * BLOCK_TILE_WIDTH, (BOARD_Y_TILE_OFFSET + board1.clearLineRows[i]) * BLOCK_TILE_HEIGHT);
                    SPR_setVisibility(lineClearSprites[i], VISIBLE);
                    SPR_setAnimAndFrame(lineClearSprites[i], 0, 0);
                }
                else
                {
                    SPR_setVisibility(lineClearSprites[i], HIDDEN);
                    SPR_setPosition(lineClearSprites[i], 99, 99);
                }
            }
            break;
        }
        case 2:
        {
            SPR_setAnimAndFrame(lineClearSprites[0], 0, 1);
            SPR_setAnimAndFrame(lineClearSprites[1], 0, 1);
            SPR_setAnimAndFrame(lineClearSprites[2], 0, 1);
            SPR_setAnimAndFrame(lineClearSprites[3], 0, 1);
            break;
        }
        case 3:
        {
            SPR_setAnimAndFrame(lineClearSprites[0], 0, 2);
            SPR_setAnimAndFrame(lineClearSprites[1], 0, 2);
            SPR_setAnimAndFrame(lineClearSprites[2], 0, 2);
            SPR_setAnimAndFrame(lineClearSprites[3], 0, 2);
            break;
        }
        case 4:
        {
            SPR_setAnimAndFrame(lineClearSprites[0], 0, 3);
            SPR_setAnimAndFrame(lineClearSprites[1], 0, 3);
            SPR_setAnimAndFrame(lineClearSprites[2], 0, 3);
            SPR_setAnimAndFrame(lineClearSprites[3], 0, 3);
            break;
        }
        case 5:
        {
            SPR_setVisibility(lineClearSprites[0], HIDDEN);
            SPR_setVisibility(lineClearSprites[1], HIDDEN);
            SPR_setVisibility(lineClearSprites[2], HIDDEN);
            SPR_setVisibility(lineClearSprites[3], HIDDEN);
            lineClearAnimStep = 0;
            break;
        }
    }
}

void GameOverAnimUpdate(u8 row)
{
    for (u8 x = 2; x < 2 + BOARD_SPACE_WIDTH; x++)
    {
        if (board1.data[row][x] != BLOCK_TYPE_EMPTY)
        {
            board1.data[row][x] = BLOCK_TYPE_GRAY;
        }

    }
}

void SoundInit()
{
    SND_setPCM_XGM(SOUND_FALL, fall_wav, sizeof(fall_wav));
    SND_setPCM_XGM(SOUND_HOLD, hold_wav, sizeof(hold_wav));
    SND_setPCM_XGM(SOUND_PAUSE, pause_wav, sizeof(pause_wav));
    SND_setPCM_XGM(SOUND_LINE_CLEAR, lineClear_wav, sizeof(lineClear_wav));
    SND_setPCM_XGM(SOUND_GAME_OVER, gameOver_wav, sizeof(gameOver_wav));
    SND_setPCM_XGM(SOUND_START, start_wav, sizeof(start_wav));
    SND_setPCM_XGM(SOUND_ARROW_MOVE, arrowMove_wav, sizeof(arrowMove_wav));
    SND_setPCM_XGM(SOUND_ARROW_UP_DOWN, arrowUpDown_wav, sizeof(arrowUpDown_wav));
}

void HelperSpriteInit()
{
    spriteGameOver = SPR_addSprite(&gameOverSprite, 99, 99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    SPR_setVisibility(spriteGameOver, HIDDEN);
    spritePause = SPR_addSprite(&pauseSprite, 16* BLOCK_TILE_WIDTH, BLOCK_TILE_HEIGHT, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    SPR_setVisibility(spritePause, HIDDEN);
}

void ShowGameOverSprite()
{
    SPR_setPosition(spriteGameOver, 17 * BLOCK_TILE_WIDTH, 13 * BLOCK_TILE_HEIGHT);
    SPR_setVisibility(spriteGameOver, VISIBLE);
}

void ShowPauseSprite(bool paused)
{
    if (paused)
    {
        SPR_setVisibility(spritePause, VISIBLE);
    }
    else
    {
        SPR_setVisibility(spritePause, HIDDEN);
    }
}


void OptionsInit()
{
    for (u8 i = 0; i < 10; i++)
    {
        option0Pos[i].x = 2 * (4 + i)  * 8;
        option0Pos[i].y = 8 * 4;
    }
    options[0].indexRow = 0;
    options[0].indexCol = 0;
    options[0].length = 10;
    options[0].anim = OPTION_SELECT;

    option1Pos[0].x = 8 * 8;
    option1Pos[0].y = 8 * 8;
    option1Pos[1].x = 13 * 8;
    option1Pos[1].y = 8 * 8;
    options[1].indexRow = 1;
    options[1].indexCol = 0;
    options[1].length = 2;
    options[1].anim = OPTION_UNSELECT;

    option2Pos[0].x = 8  * 8;
    option2Pos[0].y = 12 * 8;
    option2Pos[1].x = 13 * 8;
    option2Pos[1].y = 12 * 8;
    options[2].indexRow = 2;
    options[2].indexCol = 0;
    options[2].length = 2;
    options[2].anim = OPTION_UNSELECT;

    option3Pos[0].x = 8 * 8;
    option3Pos[0].y = 16 * 8;
    option3Pos[1].x = 13 * 8;
    option3Pos[1].y = 16 * 8;
    options[3].indexRow = 3;
    options[3].indexCol = 0;
    options[3].length = 2;
    options[3].anim = OPTION_UNSELECT;

    option4Pos[0].x = 8 * 6;
    option4Pos[0].y = 19 * 8;
    options[4].indexRow = 4;
    options[4].indexCol = 0;
    options[4].length = 1;
    options[4].anim = OPTION_UNSELECT;

    options[0].sprite = SPR_addSprite(&optionSprite, 64, 32, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    options[1].sprite = SPR_addSprite(&optionSprite, 64, 64, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    options[2].sprite = SPR_addSprite(&optionSprite, 64, 96, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    options[3].sprite = SPR_addSprite(&optionSprite, 64, 128, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    options[4].sprite = SPR_addSprite(&optionSprite, 48, 144, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));

    currOption = &options[0];

}

void OptionMoveLeft(bool loop)
{
    if (loop)
    {
        if (currOption->indexCol == 0)
        {
            currOption->indexCol = currOption->length - 1;
            SND_startPlayPCM_XGM(SOUND_ARROW_MOVE, 10, SOUND_PCM_CH2);
        }
        else
        {
            currOption->indexCol--;
            SND_startPlayPCM_XGM(SOUND_ARROW_MOVE, 10, SOUND_PCM_CH2);
        }
    }
    else
    {
        if (currOption->indexCol == 0)
        {
            currOption->indexCol = 0;
            SND_startPlayPCM_XGM(SOUND_ARROW_MOVE, 10, SOUND_PCM_CH2);
        }
        else
        {
            currOption->indexCol--;
            SND_startPlayPCM_XGM(SOUND_ARROW_MOVE, 10, SOUND_PCM_CH2);
        }
    }


}

void OptionMoveRight(bool loop)
{
    if (loop)
    {
        if (currOption->indexCol == currOption->length - 1)
        {
            currOption->indexCol = 0;
            SND_startPlayPCM_XGM(SOUND_ARROW_MOVE, 10, SOUND_PCM_CH2);
        }
        else
        {
            currOption->indexCol++;
            SND_startPlayPCM_XGM(SOUND_ARROW_MOVE, 10, SOUND_PCM_CH2);
        }
    }
    else
    {
        if (currOption->indexCol == currOption->length - 1)
        {
            currOption->indexCol = currOption->length - 1;
            SND_startPlayPCM_XGM(SOUND_ARROW_MOVE, 10, SOUND_PCM_CH2);
        }
        else
        {
            currOption->indexCol++;
            SND_startPlayPCM_XGM(SOUND_ARROW_MOVE, 10, SOUND_PCM_CH2);
        }
    }

}

void OptionMoveUp(bool loop)
{
    if (loop)
    {
        if (currOption->indexRow == 0)
        {
            currOption->anim = OPTION_UNSELECT;
            currOption = &options[OPTIONS_ROWS - 1];
            currOption->anim = OPTION_SELECT;
            SND_startPlayPCM_XGM(SOUND_ARROW_UP_DOWN, 10, SOUND_PCM_CH2);
        }
        else
        {
            currOption->anim = OPTION_UNSELECT;
            currOption = &options[currOption->indexRow - 1];
            currOption->anim = OPTION_SELECT;
            SND_startPlayPCM_XGM(SOUND_ARROW_UP_DOWN, 10, SOUND_PCM_CH2);
        }

    }
    else
    {
        if (currOption->indexRow == 0)
        {
            currOption = &options[0];
            SND_startPlayPCM_XGM(SOUND_ARROW_UP_DOWN, 10, SOUND_PCM_CH2);
        }
        else
        {
            currOption->anim = OPTION_UNSELECT;
            currOption = &options[currOption->indexRow - 1];
            currOption->anim = OPTION_SELECT;
            SND_startPlayPCM_XGM(SOUND_ARROW_UP_DOWN, 10, SOUND_PCM_CH2);
        }
    }

}

void OptionMoveDown(bool loop)
{
    if (loop)
    {
        if (currOption->indexRow == OPTIONS_ROWS - 1)
        {
            currOption->anim = OPTION_UNSELECT;
            currOption = &options[0];
            currOption->anim = OPTION_SELECT;
            SND_startPlayPCM_XGM(SOUND_ARROW_UP_DOWN, 10, SOUND_PCM_CH2);
        }
        else
        {
            currOption->anim = OPTION_UNSELECT;
            currOption = &options[currOption->indexRow + 1];
            currOption->anim = OPTION_SELECT;
            SND_startPlayPCM_XGM(SOUND_ARROW_UP_DOWN, 10, SOUND_PCM_CH2);
        }

    }
    else
    {
        if (currOption->indexRow == OPTIONS_ROWS - 1)
        {
            currOption = &options[OPTIONS_ROWS - 1];
            SND_startPlayPCM_XGM(SOUND_ARROW_UP_DOWN, 10, SOUND_PCM_CH2);
        }
        else
        {
            currOption->anim = OPTION_UNSELECT;
            currOption = &options[currOption->indexRow + 1];
            currOption->anim = OPTION_SELECT;
            SND_startPlayPCM_XGM(SOUND_ARROW_UP_DOWN, 10, SOUND_PCM_CH2);
        }
    }
}

void SelectOption()
{
    board1.currLevel = options[0].indexCol;
    block1.autoFallMaxSpeed = autoFallSpeedLevel[board1.currLevel];

    if (options[1].indexCol == 0)
        block1.canLockDelay = TRUE;
    else
        block1.canLockDelay = FALSE;

    if (options[2].indexCol == 0)
        block1.canWallKick = TRUE;
    else
        block1.canWallKick = FALSE;

    if (options[3].indexCol == 0)
        block1.canShowGhost = TRUE;
    else
        block1.canShowGhost = FALSE;
}
