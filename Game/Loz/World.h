/*
   Copyright 2016 Aldo J. Nunez

   Licensed under the Apache License, Version 2.0.
   See the LICENSE text file for details.
*/

#pragma once

#include "RoomAttrs.h"
#include "StatusBar.h"
#include "Submenu.h"

// TODO: Work around a compiler bug by including this file to define ObjectAttrs before 
//       WorldImpl::GetObjectAttrs.
#include "Object.h"

class Player;
class Object;
class Ladder;
struct CaveSpec;
struct Profile;
struct SparseRoomItem;
struct ObjectAttr;
class Credits;
class TextBox;
class Menu;
struct ProfileSummarySnapshot;
struct UWRoomFlags;


enum SpritePriority
{
    SpritePri_None,
    SpritePri_AboveBg,
    SpritePri_BelowBg,
};

enum
{
    Tile_HiStairs   = 0x0A,
    Tile_Cave       = 0x0C,
    Tile_Ground     = 0x0E,
    Tile_Waterfall  = 0x0F,
    Tile_Stairs     = 0x12,
    Tile_Rock       = 0x13,
    Tile_Headstone  = 0x14,
    Tile_Sand       = 0x37,

    Tile_Block      = 0,
    Tile_Tile       = 1,
    Tile_UW_Stairs  = 4,
    Tile_Wall       = 8,
};

enum CollisionResponse
{
    Collision_Unknown,
    Collision_Free,
    Collision_Blocked
};

enum ObjSlot
{
    MonsterSlot1,
    MonsterSlot11 = MonsterSlot1 + 10,
    BufferSlot,
    PlayerSwordSlot,
    PlayerSwordShotSlot,
    BoomerangSlot,
    // ORIGINAL: There are two slots that bombs and fires share.
    BombSlot,
    BombSlot2,
    FireSlot,
    FireSlot2,
    LadderSlot,
    FoodSlot,
    ArrowSlot,
    ItemSlot,
    FluteMusicSlot,
    // ORIGINAL: The player is first.
    PlayerSlot,
    DoorSlot,
    MaxObjects,

    FirstBombSlot = BombSlot,
    LastBombSlot = BombSlot2 + 1,

    FirstFireSlot = FireSlot,
    LastFireSlot = FireSlot2 + 1,

    LastMonsterSlot = MonsterSlot11,
    MonsterSlotEnd = MonsterSlot11 + 1,
    MaxMonsters = MonsterSlotEnd - MonsterSlot1,
    MaxObjListSize = 9,

    // Simple synonyms
    WhirlwindSlot = MonsterSlot1 + 8,
    BlockSlot = BufferSlot,

    // Object timers
    FadeTimerSlot = BufferSlot,

    // Long timers
    EdgeObjTimerSlot = BufferSlot + 2,
    RedLeeverClassTimerSlot = ArrowSlot,
    NoSwordTimerSlot = BoomerangSlot,
    ObservedPlayerTimerSlot = PlayerSwordSlot,
};

enum GameMode
{
    Mode_Demo,
    Mode_GameMenu,
    Mode_LoadLevel,
    Mode_Unfurl,
    Mode_Enter,
    Mode_Play,
    Mode_Leave,
    Mode_Scroll,
    Mode_ContinueQuestion,
    Mode_PlayCellar,
    Mode_LeaveCellar,
    Mode_PlayCave,
    Mode_PlayShortcuts,
    Mode_UnknownD__,
    Mode_Register,
    Mode_Elimination,
    Mode_Stairs,
    Mode_Death,
    Mode_EndLevel,
    Mode_WinGame,

    Mode_InitPlayCellar,
    Mode_InitPlayCave,

    Mode_Max,
};

enum DoorType
{
    DoorType_Open,
    DoorType_None,
    DoorType_FalseWall,
    DoorType_FalseWall2,
    DoorType_Bombable,
    DoorType_Key,
    DoorType_Key2,
    DoorType_Shutter
};

struct TileCollision
{
    bool    Collides;
    uint8_t TileRef;
    uint8_t FineCol;
    uint8_t FineRow;

    operator bool() const { return Collides; }
};


class World
{
public:
    static const int Rows = 11;
    static const int Columns = 16;
    static const int BaseRows = 4;
    static const int TileWidth = 16;
    static const int TileHeight = 16;
    static const int TileMapWidth = Columns * TileWidth;
    static const int TileMapHeight = Rows * TileHeight;
    static const int TileMapBaseY = 64;

    static const int WorldLimitLeft = 0;
    static const int WorldLimitRight = TileMapWidth;
    static const int WorldLimitTop = TileMapBaseY;
    static const int WorldLimitBottom = WorldLimitTop + TileMapHeight;

    static const int WorldMidX = WorldLimitLeft + TileMapWidth / 2;

    static const int WorldWidth = 16;
    static const int WorldHeight = 8;

    struct LevelInfoBlock
    {
        static const int LevelPaletteCount = 8;
        static const int LevelShortcutCount = 4;
        static const int LevelCellarCount = 10;
        static const int FadeLength = 4;
        static const int FadePals = 2;
        static const int MapLength = 16;

        uint8_t     Palettes[LevelPaletteCount][PaletteLength];
        uint8_t     StartY;
        uint8_t     StartRoomId;
        uint8_t     TriforceRoomId;
        uint8_t     BossRoomId;
        uint8_t     Song;
        uint8_t     LevelNumber;
        uint8_t     EffectiveLevelNumber;
        uint8_t     DrawnMapOffset;
        uint8_t     CellarRoomIds[LevelCellarCount];
        uint8_t     ShortcutPosition[LevelShortcutCount];
        uint8_t     DrawnMap[MapLength];
        uint8_t     Padding[2];
        uint8_t     OutOfCellarPaletteSeq[FadeLength][FadePals][PaletteLength];
        uint8_t     InCellarPaletteSeq[FadeLength][FadePals][PaletteLength];
        uint8_t     DarkPaletteSeq[FadeLength][FadePals][PaletteLength];
        uint8_t     DeathPaletteSeq[FadeLength][FadePals][PaletteLength];
    };

protected:
    World();

public:
    static void Init();
    static void Uninit();
    static void Start( int slot, const Profile& profile );

    static void Update();
    static void Draw();

    static void PauseFillHearts();
    static void LeaveRoom( Direction dir, int currentRoomId );
    static void LeaveCellar();
    static void LeaveCellarByShortcut( int targetRoomId );
    static void Die();
    static void UnfurlLevel();
    static void ChooseFile( const std::shared_ptr<ProfileSummarySnapshot>& summaries );
    static void RegisterFile( const std::shared_ptr<ProfileSummarySnapshot>& summaries );
    static void EliminateFile( const std::shared_ptr<ProfileSummarySnapshot>& summaries );
    static TileCollision CollidesWithTileStill( int x, int y );
    static TileCollision CollidesWithTileMoving( int x, int y, Direction dir, bool isPlayer );
    static void OnPushedBlock();
    static void OnActivatedArmos( int x, int y );
    static void OnTouchedPowerTriforce();
    static bool IsPlaying();
    static bool IsPlaying( GameMode mode );
    static GameMode GetMode();
    static int GetMarginRight();
    static int GetMarginLeft();
    static int GetMarginBottom();
    static int GetMarginTop();
    static void PushTile( int row, int col );
    static void TouchTile( int row, int col );
    static void CoverTile( int row, int col );
    static Player* GetPlayer();
    static Point GetObservedPlayerPos();
    static Ladder* GetLadder();
    static void SetLadder( Ladder* ladder );
    static void UseRecorder();
    static void SetTile( int x, int y, int tileType );
    static int GetInnerPalette();
    static Point GetRandomWaterTile();
    static Object* GetObject( int slot );
    static void SetObject( int slot, Object* obj );
    static int FindEmptyMonsterSlot();
    static int FindEmptyFireSlot();
    static int GetCurrentObjectSlot();
    static void SetCurrentObjectSlot( int slot );
    static int& GetObjectTimer( int slot );
    static void SetObjectTimer( int slot, int value );
    static int GetStunTimer( int slot );
    static void SetStunTimer( int slot, int value );
    static int GetRecorderUsed();
    static void SetRecorderUsed( int value );
    static bool GetCandleUsed();
    static void SetCandleUsed();
    static int  GetWhirlwindTeleporting();
    static void SetWhirlwindTeleporting( int value );
    static const uint8_t* GetString( int stringId );
    static bool IsSwordBlocked();
    static void SetSwordBlocked( bool value );

    static Profile& GetProfile();
    static uint8_t GetItem( int itemSlot );
    static void SetItem( int itemSlot, int value );
    static void PostRupeeWin( uint8_t value );
    static void PostRupeeLoss( uint8_t value );
    static void AddItem( int itemId );
    static void DecrementItem( int itemSlot );
    static void FillHearts( int heartValue );
    static bool HasCurrentMap();
    static bool HasCurrentCompass();
    static bool HasCurrentLevelItem( int itemSlot1To8, int itemSlot9 );
    static bool IsLiftingItem();
    static bool IsUWCellar();
    static void EndLevel();
    static void WinGame();
    static void AddUWRoomItem();
    static Direction GetDoorwayDir();
    static void SetDoorwayDir( Direction dir );
    static int  GetFromUnderground();
    static void SetFromUnderground( int value );
    static int  GetActiveShots();
    static void SetActiveShots( int count );
    static Direction GetShuttersPassedDirs();
    static void SetShuttersPassedDirs( Direction dir );
    static int  GetTriggeredDoorCmd();
    static void SetTriggeredDoorCmd( int value );
    static Direction GetTriggeredDoorDir();
    static void SetTriggeredDoorDir( Direction dir );
    static int  GetRoomId();
    static bool IsOverworld();
    static bool DoesRoomSupportLadder();
    static int  GetTileAction( int tileRef );
    static DoorType GetDoorType( Direction dir );
    static DoorType GetDoorType( int roomId, Direction dir );
    static bool GetDoorState( int door );
    static UWRoomFlags& GetUWRoomFlags( int curRoomId );

    static const LevelInfoBlock* GetLevelInfo();
    static bool IsUWMain( int roomId );
    static bool IsPlayingCave();

    static bool GotItem();
    static bool GotItem( int roomId );
    static void TakeSecret();
    static void MarkItem();
    static void LiftItem( int itemId, uint16_t timer = 0x80 );
    static void OpenShutters();
    static void ResetKilledObjectCount();
    static void IncrementKilledObjectCount( bool allowBombDrop );
    static void IncrementRoomKillCount();
    static void SetBombItemDrop();
    static int  GetRoomObjCount();
    static void SetRoomObjCount( int value );
    static int  GetRoomObjId();
    static void SetObservedPlayerPos( int x, int y );

    static ObjectAttr GetObjectAttrs( int type );
    static int GetObjectMaxHP( int type );
    static int GetPlayerDamage( int type );

    static void SetPersonWallY( int y );
    static int  GetFadeStep();
    static void BeginFadeIn();
    static void FadeIn();
    static bool HasLivingObjects();
    static void EnablePersonFireballs();
    static Util::Array<uint8_t> GetShortcutRooms();
};


class WorldImpl : private World
{
public:
    static const int Rooms = 128;
    static const int UniqueRooms = 124;
    static const int ColumnTables = 16;
    static const int ScrollSpeed = 4;
    static const int TileTypes = 56;
    static const int TileActions = 16;
    static const int LoadingTileActions = 4;
    static const int SparseAttrs = 11;
    static const int RoomHistoryLength = 6;
    static const int Modes = Mode_Max;

    struct LevelDirectory
    {
        typedef char FixedString[32];

        FixedString LevelInfoBlock;
        FixedString RoomCols;
        FixedString ColTables;
        FixedString TileAttrs;
        FixedString TilesImage;
        FixedString PlayerImage;
        FixedString PlayerSheet;
        FixedString NpcImage;
        FixedString NpcSheet;
        FixedString BossImage;
        FixedString BossSheet;
        FixedString RoomAttrs;
        FixedString LevelInfoEx;
        FixedString ObjLists;
        FixedString Extra1;
        FixedString Extra2;
        FixedString Extra3;
        FixedString Extra4;
    };

    struct RoomCols
    {
        uint8_t     ColumnDesc[Columns];
    };

    struct TileMap
    {
        uint8_t     tileRefs[Rows][Columns];
    };

    enum TileInteraction
    {
        TInteract_Load,
        TInteract_Push,
        TInteract_Touch,
        TInteract_Cover,
    };

    struct SparsePos
    {
        uint8_t     roomId;
        uint8_t     pos;
    };

    struct SparsePos2
    {
        uint8_t     roomId;
        uint8_t     x;
        uint8_t     y;
    };

    typedef Util::Table<uint8_t> ColumnResTable;
    typedef Util::Table<uint8_t> SparseAttrTable;
    typedef Util::Table<uint8_t> OWExtraTable;
    typedef Util::Table<uint8_t> ObjListTable;
    typedef Util::Table<uint8_t> TextTable;
    typedef void (WorldImpl::*UpdateFunc)();
    typedef void (WorldImpl::*DrawFunc)();
    typedef void (WorldImpl::*TileActionFunc)( int row, int col, TileInteraction interaction );

    static TileActionFunc sActionFuncs[TileActions];
    static UpdateFunc sModeFuncs[Modes];
    static DrawFunc sDrawFuncs[Modes];

    LevelDirectory  directory;
    LevelInfoBlock  infoBlock;
    RoomCols        roomCols[UniqueRooms];
    ColumnResTable  colTables;
    TileMap         tileMaps[2];
    RoomAttrs       roomAttrs[Rooms];
    int             curRoomId;
    int             curTileMapIndex;
    uint8_t         tileAttrs[TileTypes];
    SparseAttrTable sparseRoomAttrs;
    OWExtraTable    extraData;
    ObjListTable    objLists;
    TextTable       textTable;

    int             rowCount;
    int             colCount;
    int             startRow;
    int             startCol;
    int             tileTypeCount;
    int             marginRight;
    int             marginLeft;
    int             marginBottom;
    int             marginTop;
    ALLEGRO_BITMAP* wallsBmp;
    ALLEGRO_BITMAP* doorsBmp;

    GameMode        lastMode;
    GameMode        curMode;
    StatusBar       statusBar;
    Submenu         menu;
    Credits*        credits;
    TextBox*        textBox1;
    TextBox*        textBox2;
    Menu*           gameMenu;
    Menu*           nextGameMenu;

    struct PlayState
    {
        enum Substate
        {
            Active,
        };

        enum RoomType
        {
            Regular,
            Cellar,
            Cave,
        };

        Substate    substate;
        int         timer;

        bool        animatingRoomColors;
        bool        allowWalkOnWater;
        bool        uncoveredRecorderSecret;
        RoomType    roomType;
        uint16_t    liftItemTimer;
        int         liftItemId;
        int         personWallY;
    };

    struct PlayCellarState
    {
        enum Substate
        {
            Start,
            FadeOut,
            LoadRoom,
            FadeIn,
            Walk,

            MaxSubstate
        };

        Substate        substate;
        SpritePriority  playerPriority;
        int             targetY;
        int             fadeTimer;
        int             fadeStep;
    };

    struct LeaveCellarState
    {
        enum Substate
        {
            Start,
            FadeOut,
            LoadRoom,
            FadeIn,
            Walk,

            Wait,
            LoadOverworldRoom,

            MaxSubstate
        };

        Substate        substate;
        int             fadeTimer;
        int             fadeStep;
        int             timer;
    };

    struct PlayCaveState
    {
        enum Substate
        {
            Start,
            Wait,
            LoadRoom,
            Walk,

            MaxSubstate
        };

        Substate        substate;
        int             timer;
        int             targetY;
        SpritePriority  playerPriority;
    };

    struct ScrollState
    {
        enum Substate
        {
            Start,
            AnimatingColors,
            FadeOut,
            LoadRoom,
            Scroll,

            MaxSubstate
        };

        static const int StateTime = 32;

        Substate    substate;
        int         timer;
        Direction   scrollDir;
        int         nextRoomId;
        int         curRoomId;

        int         offsetX;
        int         offsetY;
        int         speedX;
        int         speedY;
        int         oldTileMapIndex;
        int         oldRoomId;
        int         oldMapToNewMapDistX;
        int         oldMapToNewMapDistY;
    };

    struct LeaveState
    {
        static const int StateTime = 2;

        int         timer;
        Direction   scrollDir;
        int         curRoomId;
    };

    struct EnterState
    {
        enum Substate
        {
            Start,
            Wait,
            FadeIn,
            Walk,
            WalkCave,

            MaxSubstate
        };

        static const int StateTime = 2;

        Substate    substate;
        int         timer;
        Direction   scrollDir;
        int         targetX;
        int         targetY;
        int         playerSpeed;
        int         playerFraction;
        SpritePriority  playerPriority;
    };

    struct LoadLevelState
    {
        enum Substate
        {
            Load,
            Wait,
        };

        static const int StateTime = 18;

        Substate    substate;
        int         timer;
        int         level;
        bool        restartOW;
    };

    struct UnfurlState
    {
        enum Substate
        {
            Start,
            Unfurl,
        };

        static const int StateTime = 11;

        Substate    substate;
        int         timer;
        int         stepTimer;
        int         left;
        int         right;
        bool        restartOW;
    };

    struct EndLevelState
    {
        enum Substate
        {
            Start,
            Wait1,
            Flash,
            FillHearts,
            Wait2,
            Furl,
            Wait3,

            MaxSubstate
        };

        static const int Wait1Time = 0x30;
        static const int FlashTime = 0x30;
        static const int Wait2Time = 0x80;
        static const int Wait3Time = 0x80;

        Substate    substate;
        int         timer;
        int         stepTimer;
        int         left;
        int         right;
    };

    struct WinGameState
    {
        enum Substate
        {
            Start,
            Text1,
            Stand,
            Hold1,
            Colors,
            Hold2,
            Text2,
            Hold3,
            NoObjects,
            Credits,

            MaxSubstate
        };

        enum NpcVisual
        {
            Npc_None,
            Npc_Stand,
            Npc_Lift,
        };

        enum
        {
            TextBox2Top = 0xA8,
        };

        Substate    substate;
        int         timer;
        int         stepTimer;
        int         left;
        int         right;
        NpcVisual   npcVisual;
    };

    struct StairsState
    {
        enum Substate
        {
            Start,
            Walk,
            WalkCave,

            MaxSubstate
        };

        Substate        substate;
        Direction       scrollDir;
        int             targetX;
        int             targetY;
        int             playerSpeed;
        int             playerFraction;
        int             tileRef;
        SpritePriority  playerPriority;
    };

    struct DeathState
    {
        enum Substate
        {
            Start,
            Flash,
            Wait1,
            Turn,
            Fade,
            GrayLink,
            Spark,
            Wait2,
            GameOver,

            MaxSubstate
        };

        Substate    substate;
        int         timer;
        int         step;
    };

    struct ContinueState
    {
        enum Substate
        {
            Start,
            Idle,
            Chosen,

            MaxSubstate
        };

        Substate    substate;
        int         timer;
        int         selectedIndex;
    };

    union State
    {
        PlayState           play;
        PlayCellarState     playCellar;
        LeaveCellarState    leaveCellar;
        PlayCaveState       playCave;
        ScrollState         scroll;
        LeaveState          leave;
        EnterState          enter;
        LoadLevelState      loadLevel;
        UnfurlState         unfurl;
        StairsState         stairs;
        EndLevelState       endLevel;
        WinGameState        winGame;
        DeathState          death;
        ContinueState       continueQuestion;
    };

    State           state;
    int             curColorSeqNum;
    int             darkRoomFadeStep;
    int             curMazeStep;
    int             spotIndex;
    int             tempShutterRoomId;
    int             tempShutterDoorDir;
    int             tempShuttersRoomId;
    bool            tempShutters;
    bool            prevRoomWasCellar;
    int             savedOWRoomId;
    int             edgeX;
    int             edgeY;
    int             nextRoomHistorySlot;    // 620
    int             roomObjCount;           // 34E
    int             roomObjId;              // 35F
    uint8_t         worldKillCycle;         // 52A
    uint8_t         worldKillCount;         // 627
    uint8_t         helpDropCounter;        // 50
    uint8_t         helpDropValue;          // 51
    int             roomKillCount;          // 34F
    bool            roomAllDead;            // 34D
    bool            madeRoomItem;
    bool            enablePersonFireballs;
    bool            swordBlocked;           // 52E
    uint8_t         whirlwindTeleporting;   // 522
    uint8_t         teleportingRoomIndex;   // 523
    uint8_t         pause;                  // E0
    uint8_t         submenu;                // E1
    int             submenuOffsetY;         // EC
    bool            statusBarVisible;
    uint8_t         levelKillCounts[LevelBlockRooms];
    uint8_t         roomHistory[RoomHistoryLength];

    static UpdateFunc sLeaveCellarFuncs[LeaveCellarState::MaxSubstate];
    static UpdateFunc sScrollFuncs[ScrollState::MaxSubstate];
    static UpdateFunc sEndLevelFuncs[EndLevelState::MaxSubstate];
    static UpdateFunc sWinGameFuncs[WinGameState::MaxSubstate];
    static UpdateFunc sDeathFuncs[DeathState::MaxSubstate];

public:
    WorldImpl();
    ~WorldImpl();

    void Init();
    void Start( int slot, const Profile& profile );

    void Update();
    void Draw();

public:
    bool IsUWCellar( int roomId );
    bool GotShortcut( int roomId );
    bool GotSecret();
    void TakeShortcut();
    bool UseKey();
    bool GetDoorState( int door );
    bool GetDoorState( int roomId, int door );
    void SetDoorState( int roomId, int door );
    bool IsRoomInHistory();
    void AddRoomToHistory();
    void SaveObjectCount();
    void CalcObjCountToMake( int& objId, int& count );
    void OnLeavePlay();
    void ClearLevelData();

    bool FindSparseFlag( int attrId, int roomId );
    const SparsePos* FindSparsePos( int attrId, int roomId );
    const SparsePos2* FindSparsePos2( int attrId, int roomId );
    const SparseRoomItem* FindSparseItem( int attrId, int roomId );
    const ObjectAttr* GetObjectAttrs();

    void LoadLevel( int level );
    void LoadRoom( int roomId, int tileMapIndex );
    void LoadMap( int roomId, int tileMapIndex );
    void LoadLayout( int uniqueRoomId, int tileMapIndex, bool owTileFormat );
    void LoadCaveRoom( int uniqueRoomId );

    void LoadOverworldContext();
    void LoadUnderworldContext();
    void LoadCellarContext();

    void LoadOpenRoomContext();
    void LoadClosedRoomContext();
    void LoadMapResourcesFromDirectory( int uniqueRoomCount );

    void DrawRoom();
    void DrawMap( int roomId, int mapIndex, int offsetX, int offsetY );
    void DrawDoors( int roomId, bool above, int offsetX, int offsetY );

    int  GetNextTeleportingRoomIndex();
    void UpdateRoomColors();
    void CheckBombables();
    void CheckSecrets();
    void CheckShutters();
    void UpdateDoors2();
    bool CalcHasLivingObjects();
    void AddUWRoomItem( int roomId );
    void AddUWRoomStairs();
    void KillAllObjects();
    void MoveRoomItem();
    void UpdateStatues();
    void UpdateObservedPlayerPos();
    void UpdateRupees();
    void UpdateLiftItem();
    void DrawObjects( Object** objOverPlayer = nullptr );
    void DrawLinkLiftingItem( int itemId );
    void DrawSubmenu();

    void MakeObjects( Direction entryDir );
    void MakeCellarObjects();
    void MakeCaveObjects();
    void MakeUnderworldPerson( int objId );
    void MakePersonRoomObjects( int type, const CaveSpec* spec );
    void MakeWhirlwind();
    bool FindSpawnPos( int type, const RSpot* spots, int len, int& x, int& y );
    bool CalcMazeStayPut( Direction dir );
    void PutEdgeObject();
    void HandleNormalObjectDeath();
    void TryDroppingItem( int origType, int x, int y );
    void FillHeartsStep();
    void UpdateSubmenu();

    void GotoPlay( PlayState::RoomType roomType = PlayState::Regular );
    void UpdatePlay();
    void DrawPlay();

    void GotoScroll( Direction dir );
    void GotoScroll( Direction dir, int currentRoomId );
    void UpdateScroll();
    void UpdateScroll_Start();
    void UpdateScroll_AnimatingColors();
    void UpdateScroll_FadeOut();
    void UpdateScroll_LoadRoom();
    void UpdateScroll_Scroll();
    void DrawScroll();

    void GotoLeave( Direction dir );
    void GotoLeave( Direction dir, int currentRoomId );
    void UpdateLeave();
    void DrawLeave();

    void GotoEnter( Direction dir );
    void UpdateEnter();
    void DrawEnter();

    void GotoLoadLevel( int level, bool restartOW = false );
    void UpdateLoadLevel();
    void DrawLoadLevel();

    void GotoUnfurl( bool restartOW = false );
    void UpdateUnfurl();
    void DrawUnfurl();

    void GotoEndLevel();
    void UpdateEndLevel();
    void UpdateEndLevel_Start();
    void UpdateEndLevel_Wait();
    void UpdateEndLevel_Flash();
    void UpdateEndLevel_FillHearts();
    void UpdateEndLevel_Furl();
    void DrawEndLevel();

    void GotoWinGame();
    void UpdateWinGame();
    void UpdateWinGame_Start();
    void UpdateWinGame_Text1();
    void UpdateWinGame_Stand();
    void UpdateWinGame_Hold1();
    void UpdateWinGame_Colors();
    void UpdateWinGame_Hold2();
    void UpdateWinGame_Text2();
    void UpdateWinGame_Hold3();
    void UpdateWinGame_NoObjects();
    void UpdateWinGame_Credits();
    void DrawWinGame();

    void GotoStairs( int tileRef );
    void UpdateStairsState();
    void DrawStairsState();

    void GotoPlayCellar();
    void UpdatePlayCellar();
    void DrawPlayCellar();

    void GotoLeaveCellar();
    void UpdateLeaveCellar();
    void UpdateLeaveCellar_Start();
    void UpdateLeaveCellar_FadeOut();
    void UpdateLeaveCellar_LoadRoom();
    void UpdateLeaveCellar_FadeIn();
    void UpdateLeaveCellar_Walk();
    void UpdateLeaveCellar_Wait();
    void UpdateLeaveCellar_LoadOverworldRoom();
    void DrawLeaveCellar();

    void GotoPlayCave();
    void UpdatePlayCave();
    void DrawPlayCave();

    void GotoDie();
    void UpdateDie();
    void UpdateDie_Start();
    void UpdateDie_Flash();
    void UpdateDie_Wait1();
    void UpdateDie_Turn();
    void UpdateDie_Fade();
    void UpdateDie_GrayLink();
    void UpdateDie_Spark();
    void UpdateDie_Wait2();
    void UpdateDie_GameOver();
    void DrawDie();

    void GotoContinueQuestion();
    void UpdateContinueQuestion();
    void DrawContinueQuestion();

    void GotoFileMenu();
    void GotoFileMenu( const std::shared_ptr<ProfileSummarySnapshot>& summaries );
    void GotoRegisterMenu( const std::shared_ptr<ProfileSummarySnapshot>& summaries );
    void GotoEliminateMenu( const std::shared_ptr<ProfileSummarySnapshot>& summaries );
    void UpdateGameMenu();
    void UpdateRegisterMenu();
    void UpdateEliminateMenu();
    void DrawGameMenu();

    int FindCellarRoomId( int mainRoomId, bool& isLeft );
    void SetPlayerExitPosOW( int roomId );
    const uint8_t* GetStringImpl( int stringId );

    void DrawRoomNoObjects( SpritePriority playerPriority = SpritePri_AboveBg );

    bool CollidesWithUWBorder( int fineRow, int fineCol1, int fineCol2 );
    TileCollision CollidesWithTile( int x, int y, Direction dir, int offset );
    bool CollidesTile( int row, int col );
    void OnPushedBlockImpl();
    void OnActivatedArmosImpl( int x, int y );
    void CheckPowerTriforceFanfare();
    void AdjustInventory();
    void WarnLowHPIfNeeded();
    void PlayAmbientSounds();
    void ShowShortcutStairs( int roomId, int tileMapIndex );

    void UseRecorderImpl();
    void SummonWhirlwind();
    void MakeFluteSecret();
    int GetMapTile( int row, int col );
    void SetTileImpl( int x, int y, int tileType );
    int GetInnerPaletteImpl();
    Point GetRandomWaterTileImpl();
    void FadeInImpl();
    void InteractTile( int row, int col, TileInteraction interaction );
    void MakeActivatedObject( int type, int row, int col );

    void NoneTileAction( int row, int col, TileInteraction interaction );
    void PushTileAction( int row, int col, TileInteraction interaction );
    void BombTileAction( int row, int col, TileInteraction interaction );
    void BurnTileAction( int row, int col, TileInteraction interaction );
    void HeadstoneTileAction( int row, int col, TileInteraction interaction );
    void LadderTileAction( int row, int col, TileInteraction interaction );
    void RaftTileAction( int row, int col, TileInteraction interaction );
    void CaveTileAction( int row, int col, TileInteraction interaction );
    void StairsTileAction( int row, int col, TileInteraction interaction );
    void GhostTileAction( int row, int col, TileInteraction interaction );
    void ArmosTileAction( int row, int col, TileInteraction interaction );
    void BlockTileAction( int row, int col, TileInteraction interaction );
};
