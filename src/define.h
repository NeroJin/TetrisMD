#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED

#include <genesis.h>

#define BOARD_WIDTH                        14
#define BOARD_HEIGHT                       23
#define BOARD_SPACE_WIDTH                  10
#define BOARD_SPACE_HEIGHT                 21
#define BOARD_X_TILE_OFFSET                 9 + 4 //包含虚拟墙
#define BOARD_Y_TILE_OFFSET                 5
#define BOARD_WALL_X_TILE_OFFSET            2

#define BLOCK_ROTATION_STATE_0              0
#define BLOCK_ROTATION_STATE_R              1
#define BLOCK_ROTATION_STATE_2              2
#define BLOCK_ROTATION_STATE_L              3

#define BLOCK_TYPE_AMOUNT                   7
#define BLOCK_ROTATION_STATE_AMOUNT         4
#define BLOCK_DATA_Y                        4
#define BLOCK_DATA_X                        4
#define BLOCK_WALL_KICK_TEST_AMOUNT         5
#define BLOCK_TYPE_EMPTY                    9
#define BLOCK_TYPE_I_0                      0
#define BLOCK_TYPE_O_1                      1
#define BLOCK_TYPE_T_2                      2
#define BLOCK_TYPE_S_3                      3
#define BLOCK_TYPE_Z_4                      4
#define BLOCK_TYPE_J_5                      5
#define BLOCK_TYPE_L_6                      6
#define BLOCK_TYPE_GRAY                     8
#define BLOCK_TYPE_WALL                     -1
#define BLOCK_TILE_WIDTH                    8
#define BLOCK_TILE_HEIGHT                   8
#define BLOCK_REBORN_TILE_X                 5
#define BLOCK_REBORN_TILE_Y                 0
#define BLOCK_ROTATE_LEFT_DIR               -1
#define BLOCK_ROTATE_RIGHT_DIR              1

#define GAMEFILED_TOPLEFT_PIXEL_X          72
#define GAMEFILED_TOPLEFT_PIXEL_Y          40

#define AUTO_FALL_SPEED_LEVEL_MAX          12

#define LOCK_DELAY_FRAMES                  30
#define MAX_ACTION_IN_LOCK                 10

#define CMD_MOVE_LEFT                   0x01
#define CMD_MOVE_RIGHT                  0x02
#define CMD_ROTATE_LEFT                 0x04
#define CMD_ROTATE_RIGHT                0x08
#define CMD_SOFT_DROP                   0x10
#define CMD_HARD_DROP                   0x20
#define CMD_AUTO_FALL                   0x40
#define CMD_LOCK                        0x80

#define CMD_MOVE_LEFT_CLR               ~(1)
#define CMD_MOVE_RIGHT_CLR              ~(1 << 1)
#define CMD_ROTATE_LEFT_CLR             ~(1 << 2)
#define CMD_ROTATE_RIGHT_CLR            ~(1 << 3)
#define CMD_SOFT_DROP_CLR               ~(1 << 4)
#define CMD_HARD_DROP_CLR               ~(1 << 5)
#define CMD_AUTO_FALL_CLR               ~(1 << 6)
#define CMD_LOCK_CLR                    ~(1 << 7)

#define SOUND_FALL                      64
#define SOUND_HOLD                      65
#define SOUND_PAUSE                     66
#define SOUND_LINE_CLEAR                67
#define SOUND_GAME_OVER                 68
#define SOUND_START                     69
#define SOUND_ARROW_MOVE                70
#define SOUND_ARROW_UP_DOWN             71

#define ANIM_GAMEOVER_SPEDD_MAX         5

#define OPTIONS_ROWS        5
#define OPTION_SELECT        0
#define OPTION_UNSELECT      1

#define GAME_OVER_SOUND_DELAY_FRAMES    480
#define GAME_OVER_TOTAL_DELAY_FRAMES    800

enum BLOCK_LAND_TYPE
{
    NORMAL_LAND = 1,
    SOFT_LAND = 2,
    HARD_LAND = 3
};

enum BLOCK_STATE
{
    LANDING,
    CLEARING,
    REBORN,
    IDLE,
    GAME_OVER,
    EXIT_GAME
};


typedef struct
{
	s16 offsetX;
	s16 offsetY;
} WallKickData;

typedef struct
{
        enum BLOCK_STATE state;
        s16 currType; //当前方块类型，范围是0-6,对应I,O,T,S,Z,J,L。
        bool visible;
        s16 xPixelOnScreen;
        s16 yPixelOnScreen;
        s16 xTileOnBoard; //包含了虚拟墙的位置，如要得到不包括墙的位置，要减去2
        s16 yTileOnBoard;
        s16 xTileOnScreen;
        s16 yTileOnScreen;
        s16 rotationState;//0-3 0：初始位置 1：R位，初始位置右转一下，2：初始位置转2下，3：L位初始位置左转一下。
        s16 xDir; //方块x轴移动方向，-1向左，1向右，0不动。
        s16 speedX; //方块x轴移动速度。
        s16 accelX; //方块x轴移动加速度。
        s16 maxSpeedX; //方块x轴最高移动速度限制。这三个变量用于限制移动速度。以帧数为单位。
        s16 autoFallSpeed;
        s16 autoFallAccel;
        s16 autoFallMaxSpeed;
        s16 softDropSpeed;
        s16 softDropAccel;
        s16 softDropMaxSpeed;
        bool canShowGhost;
        bool canLockDelay;
        bool isLock; //是否锁定
        s16 lockDelay; //方块下降遇到障碍物时开始倒计时，每帧减1，到0时方块落地。
        s16 actionCountInLock; //停止无限旋转或移动来重置lockDelay的设计,锁定后的动作计数，每次移动或旋转都能使其加一，满MAX_ACTION_IN_LOCK次后落地。
        s16 landDelay;
        Sprite *sprite;
        bool canWallKick;
        s16 rotationData[BLOCK_DATA_Y][BLOCK_DATA_X]; //保存当前方块的4*4的碰撞占位数据，用函数GetBlockData根据currType和rotationState获取。
        WallKickData wallKickData[BLOCK_WALL_KICK_TEST_AMOUNT]; //包含了方块wall kick后的位移方法。
        enum BLOCK_LAND_TYPE landType;
        u8 landRow;
} Block;

typedef struct
{
    s16 xTileOnBoard;
    s16 yTileOnBoard;
    bool visible;
    Block *parent;
    Sprite *sprite;
} BlockGhost;

//保存7种方块4*4的碰撞占位数据，9为空，其他0-6代表不同方块的子块,-1为墙。
static s8 blockRotationDatas[BLOCK_TYPE_AMOUNT][BLOCK_ROTATION_STATE_AMOUNT][BLOCK_DATA_Y][BLOCK_DATA_X] = {
																								9,9,9,9,0,0,0,0,9,9,9,9,9,9,9,9,9,9,0,9,9,9,0,9,9,9,0,9,9,9,0,9,9,9,9,9,9,9,9,9,0,0,0,0,9,9,9,9,9,0,9,9,9,0,9,9,9,0,9,9,9,0,9,9,
                                                                                                9,1,1,9,9,1,1,9,9,9,9,9,9,9,9,9,9,1,1,9,9,1,1,9,9,9,9,9,9,9,9,9,9,1,1,9,9,1,1,9,9,9,9,9,9,9,9,9,9,1,1,9,9,1,1,9,9,9,9,9,9,9,9,9,
                                                                                                9,2,9,9,2,2,2,9,9,9,9,9,9,9,9,9,9,2,9,9,9,2,2,9,9,2,9,9,9,9,9,9,9,9,9,9,2,2,2,9,9,2,9,9,9,9,9,9,9,2,9,9,2,2,9,9,9,2,9,9,9,9,9,9,
                                                                                                9,3,3,9,3,3,9,9,9,9,9,9,9,9,9,9,9,3,9,9,9,3,3,9,9,9,3,9,9,9,9,9,9,9,9,9,9,3,3,9,3,3,9,9,9,9,9,9,3,9,9,9,3,3,9,9,9,3,9,9,9,9,9,9,
                                                                                                4,4,9,9,9,4,4,9,9,9,9,9,9,9,9,9,9,9,4,9,9,4,4,9,9,4,9,9,9,9,9,9,9,9,9,9,4,4,9,9,9,4,4,9,9,9,9,9,9,4,9,9,4,4,9,9,4,9,9,9,9,9,9,9,
                                                                                                5,9,9,9,5,5,5,9,9,9,9,9,9,9,9,9,9,5,5,9,9,5,9,9,9,5,9,9,9,9,9,9,9,9,9,9,5,5,5,9,9,9,5,9,9,9,9,9,9,5,9,9,9,5,9,9,5,5,9,9,9,9,9,9,
                                                                                                9,9,6,9,6,6,6,9,9,9,9,9,9,9,9,9,9,6,9,9,9,6,9,9,9,6,6,9,9,9,9,9,9,9,9,9,6,6,6,9,6,9,9,9,9,9,9,9,6,6,9,9,9,6,9,9,9,6,9,9,9,9,9,9
																							};


static WallKickData wallKickDataArray[2][8][5] = {
											//J,L,S,T,Z
            0,0,-1,0,-1,-1,0,2,-1,2,//0 R 1
            0,0,1,0,1,1,0,-2,1,-2, //1 R 2
            0,0,1,0,1,-1,0,2,1,2, //2 R 3
            0,0,-1,0,-1,1,0,-2,-1,-2, //3 R 0
            0,0,1,0,1,-1,0,2,1,2, //0 L 3
            0,0,1,0,1,1,0,-2,1,-2, //1 L 0
            0,0,-1,0,-1,-1,0,2,-1,2, //2 L 1
            0,0,-1,0,-1,1,0,-2,-1,-2, //3 L 2
            //I
            0,0,-2,0,1,0,-2,1,1,-2,
            0,0,-1,0,2,0,-1,-2,2,1,
            0,0,2,0,-1,0,2,-1,-1,2,
            0,0,1,0,-2,0,1,2,-2,-1,
            0,0,-1,0,2,0,-1,-2,2,1,
            0,0,2,0,-1,0,2,-1,-1,2,
            0,0,1,0,-2,0,1,2,-2,-1,
            0,0,-2,0,1,0,-2,1,1,-2,
											};
typedef struct
{
    s16 currScore;
    s16 currLevel;
    s16 lineClearAmount;
    s16 xTileOnScreen;  //游戏棋盘方块移动空间的屏幕x坐标，不包括虚拟墙，以tile为单位。
    s16 yTileOnScreen;  //游戏棋盘方块移动空间的屏幕y坐标，不包括虚拟墙，以tile为单位。
    s16 data[BOARD_HEIGHT][BOARD_WIDTH];       //用于表现游戏棋盘的数组23*14，左右各有2列为虚拟墙，底部也有2行为虚拟墙，游戏棋盘空间为21*10，设置多余的墙用于简化编程。
    s8 clearLineRows[4];
    s16 clearLineDelay;
    s16 gameOverDelay;
    s16 exitGameDelay;
} Board;

typedef struct
{
    s16 xPixelOnScreen;
    s16 yPixelOnScreen;
    Sprite* sprite;
} BlockPreview;

typedef struct
{
    s16 currType;
    s16 xPixelOnScreen;
    s16 yPixelOnScreen;
    Sprite* sprite;
    bool available;
    bool firstFlag;
} BlockHolder;

typedef struct
{
    u8 filledLineNumber[4];
    u8 length;
} FilledLineHelper;

enum GAME_SCENE
{
    GAME_TITLE,
    GAME_OPTION,
    GAME_PLAY
};

typedef struct
{
    enum GAME_SCENE scene;
    bool exitCurrScene;
    bool loop;
    s16 hiScore;
} Game;

typedef struct
{
    u8 indexRow;
    u8 indexCol;
    u8 length;
    Sprite *sprite;
    u8 anim;
} Option;

typedef struct
{
    u8 x;
    u8 y;
} Position;
//static u8 autoFallSpeedLevel[AUTO_FALL_SPEED_LEVEL_MAX] = { 53, 49, 45, 41, 37, 33, 28, 22, 17, 11, 8, 6, 4, 3, 2, 1};
static u8 autoFallSpeedLevel[AUTO_FALL_SPEED_LEVEL_MAX] =   { 53, 48, 42, 35, 27, 10, 14,  9,  5,  4,  3};
static u8 levelClearLineNum[AUTO_FALL_SPEED_LEVEL_MAX] =    { 30, 60, 90, 120, 150,	180, 210, 240, 270,	300, 330, 360 };


void Board_Init(Board *board);
void Board_RedrawGameFiled(Board *board);
s16 BoardGetFullLineRows(Board *board, s16 lines[4]); //返回需要消除的行数，参数返回行数数组。
s16 Board_FilledLineCount(Block *block, Board *board);
s16 Board_ClearLine(Board *board);
s16 Board_ClearLine2(Block *block, Board *board);
bool IsFilledLine(Board *board, s16 line);
bool IsEmptyLine(Board *board, s16 line);


void Block_Init(Block *block, Board *board);
void Block_GetRotationData(s16 rotationData[4][4], s16 blockType, s16 blockRotationState);
s16 Block_GetRotatesLeftState(s16 rotationState); //返回方块转左的状态。
s16 Block_GetRotatesRightState(s16 rotationState); //返回方块转右的状态。
u8 Block_GetHeight(Block *block);
u8 Blcok_GetWidth(Block *block);
bool Block_CanMove(Block *block, Board *board, s16 xOffset, s16 yOffset);
bool Block_CanMoveForTry(s16 rotationData[BLOCK_DATA_Y][BLOCK_DATA_X], Board *board, s16 block_xTileOnBoard, s16 block_yTileOnBoard,  s16 xOffset, s16 yOffset);
void Block_MoveOnBoard(Block *block, s16 xOnBoardOffset, s16 yOnBoardOffset);
void Block_Reborn(Block *block, Board *board);
void Block_Landing(Block *block, Board *board);
void Block_ToHolder(Block *block, BlockHolder *blockHolder, Board *board);
void Block_RebornFromHolder(Block *block, Board *board);
void Block_Update(Block *block);

void BlockGhost_Init(BlockGhost *blockGhost, Block *block, Board *board);
void BlockGhost_UpdatePosition(BlockGhost *blockGhost, Board *board);
void BlockGhost_Update(BlockGhost *blockGhost);

void BlockPreview_Init(BlockPreview blockPreviewArray[5]);
void BlockPreview_Update(BlockPreview blockPreviewArray[5]);

void BlockHolder_Init(BlockHolder *blockHolder);

void BlockBag_Init();
void BlockBag_Shuffle(s16 start, s16 num, s16 shuffleTime);
void BlockBag_GetNextBlock(s16 blockArray[7]);

void GetWallKickData(WallKickData outWallKickData[5], s16 blockType, s16 blockRotationStatus, s16 rotationDir);

#endif // DEFINE_H_INCLUDED
