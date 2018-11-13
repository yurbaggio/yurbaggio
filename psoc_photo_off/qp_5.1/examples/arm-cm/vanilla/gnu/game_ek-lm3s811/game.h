/*****************************************************************************
* Model: game.qm
* File:  ./game.h
*
* This code has been generated by QM tool (see state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*****************************************************************************/
/* @(/3/0) .................................................................*/
#ifndef game_h
#define game_h

enum GameSignals {                              /* signals used in the game */
    TIME_TICK_SIG = Q_USER_SIG,                  /* published from tick ISR */
    PLAYER_TRIGGER_SIG, /* published by Player (ISR) to trigger the Missile */
    PLAYER_QUIT_SIG,          /* published by Player (ISR) to quit the game */
    GAME_OVER_SIG,          /* published by Ship when it finishes exploding */

    /* insert other published signals here ... */
    MAX_PUB_SIG,                               /* the last published signal */

    PLAYER_SHIP_MOVE_SIG,  /* posted by Player (ISR) to the Ship to move it */


    BLINK_TIMEOUT_SIG,           /* signal for Tunnel's blink timeout event */
    SCREEN_TIMEOUT_SIG,         /* signal for Tunnel's screen timeout event */

    TAKE_OFF_SIG,    /* from Tunnel to Ship to grant permission to take off */
    HIT_WALL_SIG,            /* from Tunnel to Ship when Ship hits the wall */
    HIT_MINE_SIG,     /* from Mine to Ship or Missile when it hits the mine */
    SHIP_IMG_SIG,     /* from Ship to the Tunnel to draw and check for hits */
    MISSILE_IMG_SIG,  /* from Missile the Tunnel to draw and check for hits */
    MINE_IMG_SIG,            /* sent by Mine to the Tunnel to draw the mine */
    MISSILE_FIRE_SIG,                /* sent by Ship to the Missile to fire */
    DESTROYED_MINE_SIG, /* from Missile to Ship when Missile destroyed Mine */
    EXPLOSION_SIG,     /* from any exploding object to render the explosion */
    MINE_PLANT_SIG,                  /* from Tunnel to the Mine to plant it */
    MINE_DISABLED_SIG,      /* from Mine to Tunnel when it becomes disabled */
    MINE_RECYCLE_SIG,         /* sent by Tunnel to Mine to recycle the mine */
    SCORE_SIG,   /* from Ship to Tunnel to adjust game level based on score */

    MAX_SIG                           /* the last signal (keep always last) */
};

/* @(/1/0) .................................................................*/
typedef struct ObjectPosEvtTag {
/* protected: */
    QEvt super;

/* public: */
    uint8_t x;
    uint8_t y;
} ObjectPosEvt;

/* @(/1/1) .................................................................*/
typedef struct ObjectImageEvtTag {
/* protected: */
    QEvt super;

/* public: */
    uint8_t x;
    int8_t y;
    uint8_t bmp;
} ObjectImageEvt;

/* @(/1/2) .................................................................*/
typedef struct MineEvtTag {
/* protected: */
    QEvt super;

/* public: */
    uint8_t id;
} MineEvt;

/* @(/1/3) .................................................................*/
typedef struct ScoreEvtTag {
/* protected: */
    QEvt super;

/* public: */
    uint16_t score;
} ScoreEvt;


#define GAME_SCREEN_WIDTH          BSP_SCREEN_WIDTH
#define GAME_SCREEN_HEIGHT         BSP_SCREEN_HEIGHT
#define GAME_MINES_MAX             5
#define GAME_MINES_DIST_MIN        10
#define GAME_SPEED_X               1
#define GAME_MISSILE_SPEED_X       2
#define GAME_SHIP_X                10
#define GAME_SHIP_Y                (GAME_SCREEN_HEIGHT / 2)

enum GameBitmapIds {
    PRESS_BUTTON_BMP,
    SHIP_BMP,
    MISSILE_BMP,
    MINE1_BMP,
    MINE2_BMP,
    MINE2_MISSILE_BMP,
    EXPLOSION0_BMP,
    EXPLOSION1_BMP,
    EXPLOSION2_BMP,
    EXPLOSION3_BMP,
    MAX_BMP
};

/* active objects' "constructors" */
/* @(/2/9) .................................................................*/
void Tunnel_ctor(void);

/* @(/2/10) ................................................................*/
void Ship_ctor(void);

/* @(/2/11) ................................................................*/
void Missile_ctor(void);


/* instantiation of the Mines orthogonal components */
/* @(/2/12) ................................................................*/
QHsm * Mine1_ctor(uint8_t id);

/* @(/2/13) ................................................................*/
QHsm * Mine2_ctor(uint8_t id);


/* opaque pointers to active objects in the application */
extern QActive * const AO_Tunnel;

extern QActive * const AO_Ship;

extern QActive * const AO_Missile;


/* helper function for all AOs */
/* @(/2/8) .................................................................*/
uint8_t do_bitmaps_overlap(
    uint8_t bmp_id1,
    uint8_t x1,
    uint8_t y1,
    uint8_t bmp_id2,
    uint8_t x2,
    uint8_t y2);


#endif  /* game_h */
