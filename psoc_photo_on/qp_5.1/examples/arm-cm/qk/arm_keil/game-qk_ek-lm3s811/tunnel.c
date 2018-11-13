/*****************************************************************************
* Model: game.qm
* File:  ./tunnel.c
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
/* @(/3/3) .................................................................*/
#include "qp_port.h"
#include "bsp.h"
#include "game.h"
#include <string.h> /* for memmove() and memcpy() */

Q_DEFINE_THIS_FILE

/* local objects -----------------------------------------------------------*/
/* @(/2/0) .................................................................*/
typedef struct TunnelTag {
/* protected: */
    QActive super;

/* private: */
    QTimeEvt blinkTimeEvt;
    QTimeEvt screenTimeEvt;
    QHsm * mines[GAME_MINES_MAX];
    QHsm * mine1_pool[GAME_MINES_MAX];
    QHsm * mine2_pool[GAME_MINES_MAX];
    uint8_t blink_ctr;
    uint8_t last_mine_x;
    uint8_t last_mine_y;
    uint8_t wall_thickness_top;
    uint8_t wall_thickness_bottom;
    uint8_t minimal_gap;
} Tunnel;

/* private: */
static void Tunnel_advance(Tunnel * const me);
static void Tunnel_plantMine(Tunnel * const me);
static void Tunnel_addImageAt(
    Tunnel * const me,
    uint8_t bmp,
    uint8_t x_pos,
    int8_t y_pos);
static void Tunnel_dispatchToAllMines(Tunnel * const me, QEvt const * e);
static uint8_t Tunnel_isWallHit(
    Tunnel * const me,
    uint8_t bmp,
    uint8_t x_pos,
    uint8_t y_pos);

/* protected: */
static QState Tunnel_initial(Tunnel * const me, QEvt const * const e);
static QState Tunnel_active(Tunnel * const me, QEvt const * const e);
static QState Tunnel_show_logo(Tunnel * const me, QEvt const * const e);
static QState Tunnel_demo(Tunnel * const me, QEvt const * const e);
static QState Tunnel_playing(Tunnel * const me, QEvt const * const e);
static QState Tunnel_game_over(Tunnel * const me, QEvt const * const e);
static QState Tunnel_screen_saver(Tunnel * const me, QEvt const * const e);
static QState Tunnel_screen_saver_hide(Tunnel * const me, QEvt const * const e);
static QState Tunnel_screen_saver_show(Tunnel * const me, QEvt const * const e);
static QState Tunnel_final(Tunnel * const me, QEvt const * const e);

static Tunnel l_tunnel;    /* the sole instance of the Tunnel active object */

static uint32_t l_rnd;                                       /* random seed */
static uint8_t l_walls[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8];
static uint8_t l_frame[GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8];

/* local helper functions --------------------------------------------------*/
static void randomSeed(uint32_t seed);
static uint32_t random(void);

/* Public-scope objects ----------------------------------------------------*/
QActive * const AO_Tunnel = (QActive *)&l_tunnel;         /* opaque pointer */

/* helper functions --------------------------------------------------------*/

/*
* The bitmap for the "Press Button" text:
*
*     xxx.........................xxx........x...x...........
*     x..x........................x..x.......x...x...........
*     x..x.x.xx..xx...xxx..xxx....x..x.x..x.xxx.xxx..xx..xxx.
*     xxx..xx...x..x.x....x.......xxx..x..x..x...x..x..x.x..x
*     x....x....xxxx..xx...xx.....x..x.x..x..x...x..x..x.x..x
*     x....x....x.......x....x....x..x.x..x..x...x..x..x.x..x
*     x....x.....xxx.xxx..xxx.....xxx...xxx...x...x..xx..x..x
*     .......................................................
*/
static uint8_t const press_button_bits[] = {
    0x7F, 0x09, 0x09, 0x06, 0x00, 0x7C, 0x08, 0x04, 0x04, 0x00,
    0x38, 0x54, 0x54, 0x58, 0x00, 0x48, 0x54, 0x54, 0x24, 0x00,
    0x48, 0x54, 0x54, 0x24, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x49,
    0x49, 0x36, 0x00, 0x3C, 0x40, 0x40, 0x7C, 0x00, 0x04, 0x3F,
    0x44, 0x00, 0x04, 0x3F, 0x44, 0x00, 0x38, 0x44, 0x44, 0x38,
    0x00, 0x7C, 0x04, 0x04, 0x78
};

/* bitmap of the Ship:
*
*     x....
*     xxx..
*     xxxxx
*/
static uint8_t const ship_bits[] = {
    0x07, 0x06, 0x06, 0x04, 0x04
};

/* bitmap of the Missile:
*
*     xxx
*/
static uint8_t const missile_bits[] = {
    0x01, 0x01, 0x01
};

/* bitmap of the Mine type-1:
*
*     .x.
*     xxx
*     .x.
*/
static uint8_t const mine1_bits[] = {
    0x02, 0x07, 0x02
};

/* bitmap of the Mine type-2:
*
*     x..x
*     .xx.
*     .xx.
*     x..x
*/
static uint8_t const mine2_bits[] = {
    0x09, 0x06, 0x06, 0x09
};

/* Mine type-2 is nastier than Mine type-1. The type-2 mine can
* hit the Ship with any of its "tentacles". However, it can be
* destroyed by the Missile only by hitting its center, defined as
* the following bitmap:
*
*     ....
*     .xx.
*     .xx.
*     ....
*/
static uint8_t const mine2_missile_bits[] = {
    0x00, 0x06, 0x06, 0x00
};

/*
* The bitmap of the explosion stage 0:
*
*     .......
*     .......
*     ...x...
*     ..x.x..
*     ...x...
*     .......
*     .......
*/
static uint8_t const explosion0_bits[] = {
    0x00, 0x00, 0x08, 0x14, 0x08, 0x00, 0x00
};

/*
* The bitmap of the explosion stage 1:
*
*     .......
*     .......
*     ..x.x..
*     ...x...
*     ..x.x..
*     .......
*     .......
*/
static uint8_t const explosion1_bits[] = {
    0x00, 0x00, 0x14, 0x08, 0x14, 0x00, 0x00
};

/*
* The bitmap of the explosion stage 2:
*
*     .......
*     .x...x.
*     ..x.x..
*     ...x...
*     ..x.x..
*     .x...x.
*     .......
*/
static uint8_t const explosion2_bits[] = {
    0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00
};

/*
* The bitmap of the explosion stage 3:
*
*     x..x..x
*     .x.x.x.
*     ..x.x..
*     xx.x.xx
*     ..x.x..
*     .x.x.x.
*     x..x..x
*/
static uint8_t const explosion3_bits[] = {
    0x49, 0x2A, 0x14, 0x6B, 0x14, 0x2A, 0x49
};

typedef struct BitmapTag { /* the auxiliary structure to hold const bitmaps */
    uint8_t const *bits;                          /* the bits in the bitmap */
    uint8_t width;                               /* the width of the bitmap */
} Bitmap;

static Bitmap const l_bitmap[MAX_BMP] = {
    { press_button_bits,  Q_DIM(press_button_bits)  },
    { ship_bits,          Q_DIM(ship_bits)          },
    { missile_bits,       Q_DIM(missile_bits)       },
    { mine1_bits,         Q_DIM(mine1_bits)         },
    { mine2_bits,         Q_DIM(mine2_bits)         },
    { mine2_missile_bits, Q_DIM(mine2_missile_bits) },
    { explosion0_bits,    Q_DIM(explosion0_bits)    },
    { explosion1_bits,    Q_DIM(explosion1_bits)    },
    { explosion2_bits,    Q_DIM(explosion2_bits)    },
    { explosion3_bits,    Q_DIM(explosion3_bits)    }
};

/* Active object definition ================================================*/
/* @(/2/9) .................................................................*/
void Tunnel_ctor(void) {
    uint8_t n;
    Tunnel *me = &l_tunnel;
    QActive_ctor(&me->super, (QStateHandler)&Tunnel_initial);
    QTimeEvt_ctor(&me->blinkTimeEvt,  BLINK_TIMEOUT_SIG);
    QTimeEvt_ctor(&me->screenTimeEvt, SCREEN_TIMEOUT_SIG);
    for (n = 0; n < GAME_MINES_MAX; ++n) {
        me->mine1_pool[n] = Mine1_ctor(n); /* instantiate Mine1 in the pool */
        me->mine2_pool[n] = Mine2_ctor(n); /* instantiate Mine2 in the pool */
        me->mines[n] = (QHsm *)0; /* mine 'n' is unused */
    }
    me->last_mine_x = 0; /* the last mine at the right edge of the tunnel */
    me->last_mine_y = 0;
}
/* @(/2/0) .................................................................*/
/* @(/2/0/11) ..............................................................*/
static void Tunnel_advance(Tunnel * const me) {
    uint32_t rnd;
    uint32_t bmp1; /* bimap representing 1 column of the image */

    rnd = (random() & 0xFFU);

    /* reduce the top wall thickness 18.75% of the time */
    if ((rnd < 48U) && (me->wall_thickness_top > 0U)) {
        --me->wall_thickness_top;
    }

    /* reduce the bottom wall thickness 18.75% of the time */
    if ((rnd > 208U) && (me->wall_thickness_bottom > 0U)) {
        --me->wall_thickness_bottom;
    }

    rnd = (random() & 0xFFU);

    /* grow the top wall thickness 18.75% of the time */
    if ((rnd < 48U)
        && ((GAME_SCREEN_HEIGHT
             - me->wall_thickness_top
             - me->wall_thickness_bottom) > me->minimal_gap)
        && ((me->last_mine_x < (GAME_SCREEN_WIDTH - 5U))
            || (me->last_mine_y > (me->wall_thickness_top + 1U))))
    {
        ++me->wall_thickness_top;
    }

    /* grow the bottom wall thickness 18.75% of the time */
    if ((rnd > 208U)
        && ((GAME_SCREEN_HEIGHT
             - me->wall_thickness_top
             - me->wall_thickness_bottom) > me->minimal_gap)
        && ((me->last_mine_x < (GAME_SCREEN_WIDTH - 5U))
             || (me->last_mine_y + 1U
                < (GAME_SCREEN_HEIGHT - me->wall_thickness_bottom))))
    {
        ++me->wall_thickness_bottom;
    }

    /* advance the Tunnel by 1 game step to the left */
    memmove(l_walls, l_walls + GAME_SPEED_X,
            (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U) - GAME_SPEED_X);

    bmp1 = (~(~0U << me->wall_thickness_top))
            | (~0U << (GAME_SCREEN_HEIGHT
                        - me->wall_thickness_bottom));

    l_walls[GAME_SCREEN_WIDTH - 1] = (uint8_t)bmp1;
    l_walls[GAME_SCREEN_WIDTH + GAME_SCREEN_WIDTH - 1] = (uint8_t)(bmp1 >> 8);

    /* copy the Tunnel layer to the main frame buffer */
    memcpy(l_frame, l_walls, (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));
}
/* @(/2/0/12) ..............................................................*/
static void Tunnel_plantMine(Tunnel * const me) {
    uint32_t rnd = (random() & 0xFFU);

    if (me->last_mine_x > 0U) {
        --me->last_mine_x; /* shift the last Mine 1 position to the left */
    }
    /* last mine far enough? */
    if ((me->last_mine_x + GAME_MINES_DIST_MIN < GAME_SCREEN_WIDTH)
        && (rnd < 8U)) /* place the mines only 5% of the time */
    {
        uint8_t n;
        for (n = 0U; n < Q_DIM(me->mines); ++n) { /*look for disabled mines */
            if (me->mines[n] == (QHsm *)0) {
                break;
            }
        }
        if (n < Q_DIM(me->mines)) { /* a disabled Mine found? */
            ObjectPosEvt ope; /* event to dispatch to the Mine */
            rnd = (random() & 0xFFFFU);

            if ((rnd & 1U) == 0U) { /* choose the type of the mine */
                me->mines[n] = me->mine1_pool[n];
            }
            else {
                me->mines[n] = me->mine2_pool[n];
            }

            /* new Mine is planted in the last column of the tunnel */
            me->last_mine_x = GAME_SCREEN_WIDTH;

            /* choose a random y-position for the Mine in the Tunnel */
            rnd %= (GAME_SCREEN_HEIGHT
                    - me->wall_thickness_top
                    - me->wall_thickness_bottom - 4U);
            me->last_mine_y = (uint8_t)(me->wall_thickness_top + 2U + rnd);

            ope.super.sig = MINE_PLANT_SIG;
            ope.x = me->last_mine_x;
            ope.y = me->last_mine_y;
            QHsm_dispatch(me->mines[n], (QEvt *)&ope); /* direct dispatch */
        }
    }

}
/* @(/2/0/13) ..............................................................*/
static void Tunnel_addImageAt(
    Tunnel * const me,
    uint8_t bmp,
    uint8_t x_pos,
    int8_t y_pos)
{
    uint8_t x; /* the x-index of the ship image */
    uint8_t w; /* the width of the image */

    Q_REQUIRE(bmp < Q_DIM(l_bitmap));

    w = l_bitmap[bmp].width;
    if (w > GAME_SCREEN_WIDTH - x_pos) {
        w = GAME_SCREEN_WIDTH - x_pos;
    }
    for (x = 0U; x < w; ++x) {
        uint32_t bmp1;
        if (y_pos >= 0) {
            bmp1 = (l_bitmap[bmp].bits[x] << (uint8_t)y_pos);
        }
        else {
            bmp1 = (l_bitmap[bmp].bits[x] >> (uint8_t)(-y_pos));
        }
        l_frame[x_pos + x] |= (uint8_t)bmp1;
        l_frame[x_pos + x + GAME_SCREEN_WIDTH] |= (uint8_t)(bmp1 >> 8);
    }
    (void)me; /* avoid the compiler warning */
}
/* @(/2/0/14) ..............................................................*/
static void Tunnel_dispatchToAllMines(Tunnel * const me, QEvt const * e) {
    uint8_t n;

    for (n = 0U; n < GAME_MINES_MAX; ++n) {
        if (me->mines[n] != (QHsm *)0) { /* is the mine used? */
            QHsm_dispatch(me->mines[n], e);
        }
    }
}
/* @(/2/0/15) ..............................................................*/
static uint8_t Tunnel_isWallHit(
    Tunnel * const me,
    uint8_t bmp,
    uint8_t x_pos,
    uint8_t y_pos)
{
    uint8_t x;
    uint8_t w; /* the width of the image */

    Q_REQUIRE(bmp < Q_DIM(l_bitmap));

    w = l_bitmap[bmp].width;
    if (w > GAME_SCREEN_WIDTH - x_pos) {
        w = GAME_SCREEN_WIDTH - x_pos;
    }
    for (x = 0U; x < w; ++x) {
        uint32_t bmp1 = ((uint32_t)l_bitmap[bmp].bits[x] << y_pos);
        if (((l_walls[x_pos + x] & (uint8_t)bmp1) != 0U)
            || ((l_walls[x_pos + x + GAME_SCREEN_WIDTH]
                 & (uint8_t)(bmp1 >> 8)) != 0))
        {
            return (uint8_t)1;
        }
    }
    (void)me; /* avoid the compiler warning */
    return (uint8_t)0;
}
/* @(/2/0/16) ..............................................................*/
/* @(/2/0/16/0) */
static QState Tunnel_initial(Tunnel * const me, QEvt const * const e) {
    uint8_t n;
    for (n = 0U; n < GAME_MINES_MAX; ++n) {
        QHsm_init(me->mine1_pool[n], (QEvt *)0);/*initial tran. for Mine1 */
        QHsm_init(me->mine2_pool[n], (QEvt *)0);/*initial tran. for Mine2 */
    }
    randomSeed(1234); /* seed the pseudo-random generator */

    QActive_subscribe((QActive *)me, TIME_TICK_SIG);
    QActive_subscribe((QActive *)me, PLAYER_TRIGGER_SIG);
    QActive_subscribe((QActive *)me, PLAYER_QUIT_SIG);

    /* object dictionaries... */
    QS_OBJ_DICTIONARY(&l_tunnel);
    QS_OBJ_DICTIONARY(&l_tunnel.blinkTimeEvt);
    QS_OBJ_DICTIONARY(&l_tunnel.screenTimeEvt);

    /* function dictionaries for Tunnel HSM... */
    QS_FUN_DICTIONARY(&Tunnel_initial);
    QS_FUN_DICTIONARY(&Tunnel_final);
    QS_FUN_DICTIONARY(&Tunnel_active);
    QS_FUN_DICTIONARY(&Tunnel_show_logo);
    QS_FUN_DICTIONARY(&Tunnel_demo);
    QS_FUN_DICTIONARY(&Tunnel_playing);
    QS_FUN_DICTIONARY(&Tunnel_game_over);
    QS_FUN_DICTIONARY(&Tunnel_screen_saver);
    QS_FUN_DICTIONARY(&Tunnel_screen_saver_hide);
    QS_FUN_DICTIONARY(&Tunnel_screen_saver_show);

    /* local signals... */
    QS_SIG_DICTIONARY(BLINK_TIMEOUT_SIG,  &l_tunnel);
    QS_SIG_DICTIONARY(SCREEN_TIMEOUT_SIG, &l_tunnel);
    QS_SIG_DICTIONARY(SHIP_IMG_SIG,       &l_tunnel);
    QS_SIG_DICTIONARY(MISSILE_IMG_SIG,    &l_tunnel);
    QS_SIG_DICTIONARY(MINE_IMG_SIG,       &l_tunnel);
    QS_SIG_DICTIONARY(MINE_DISABLED_SIG,  &l_tunnel);
    QS_SIG_DICTIONARY(EXPLOSION_SIG,      &l_tunnel);
    QS_SIG_DICTIONARY(SCORE_SIG,          &l_tunnel);

    (void)e;  /* avoid the "unreferenced parameter" warning */
    return Q_TRAN(&Tunnel_show_logo);
}
/* @(/2/0/16/1) ............................................................*/
static QState Tunnel_active(Tunnel * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/16/1/0) */
        case MINE_DISABLED_SIG: {
            Q_ASSERT((Q_EVT_CAST(MineEvt)->id < GAME_MINES_MAX)
                     && (me->mines[Q_EVT_CAST(MineEvt)->id] != (QHsm *)0));
            me->mines[Q_EVT_CAST(MineEvt)->id] = (QHsm *)0;
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/1) */
        case PLAYER_QUIT_SIG: {
            status_ = Q_TRAN(&Tunnel_final);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/* @(/2/0/16/1/2) ..........................................................*/
static QState Tunnel_show_logo(Tunnel * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/16/1/2) */
        case Q_ENTRY_SIG: {
            QTimeEvt_postEvery(&me->blinkTimeEvt, (QActive *)me,
                               BSP_TICKS_PER_SEC/2U); /* 1/2 sec */
            QTimeEvt_postIn(&me->screenTimeEvt, (QActive *)me,
                            BSP_TICKS_PER_SEC*5U); /* 5 sec timeout */
            me->blink_ctr = 0U;
            BSP_drawNString(0U, 0U, " Quantum  LeAps ");
            BSP_drawNString(0U, 1U, "state-machine.co");
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/2) */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->blinkTimeEvt);
            QTimeEvt_disarm(&me->screenTimeEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/2/0) */
        case SCREEN_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Tunnel_demo);
            break;
        }
        /* @(/2/0/16/1/2/1) */
        case BLINK_TIMEOUT_SIG: {
            me->blink_ctr ^= 1U; /* toggle the blink couner */
            /* @(/2/0/16/1/2/1/0) */
            if (me->blink_ctr == 0U) {
                BSP_drawNString(6U*9U, 0U,         " LeAps");
                BSP_drawNString(0U,    1U, "state-machine.co");
                status_ = Q_HANDLED();
            }
            /* @(/2/0/16/1/2/1/1) */
            else {
                BSP_drawNString(6U*9U, 0U,         "LeaPs ");
                BSP_drawNString(0U,    1U, "tate-machine.com");
                status_ = Q_HANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&Tunnel_active);
            break;
        }
    }
    return status_;
}
/* @(/2/0/16/1/3) ..........................................................*/
static QState Tunnel_demo(Tunnel * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/16/1/3) */
        case Q_ENTRY_SIG: {
            me->last_mine_x = 0U; /* last mine at right edge of the tunnel */
            me->last_mine_y = 0U;
            /* set the tunnel properties... */
            me->wall_thickness_top = 0U;
            me->wall_thickness_bottom = 0U;
            me->minimal_gap = GAME_SCREEN_HEIGHT - 3U;

            /* erase the tunnel walls */
            memset(l_walls, (uint8_t)0,
                  (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));

            QTimeEvt_postEvery(&me->blinkTimeEvt, (QActive *)me,
                               BSP_TICKS_PER_SEC/2U); /* every 1/2 sec */
            QTimeEvt_postIn(&me->screenTimeEvt, (QActive *)me,
                            BSP_TICKS_PER_SEC*20U); /* 20s timeout */

            me->blink_ctr = 0U; /* init the blink counter */
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/3) */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->blinkTimeEvt);
            QTimeEvt_disarm(&me->screenTimeEvt);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/3/0) */
        case BLINK_TIMEOUT_SIG: {
            me->blink_ctr ^= 1U; /* toggle the blink cunter */
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/3/1) */
        case SCREEN_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Tunnel_screen_saver);
            break;
        }
        /* @(/2/0/16/1/3/2) */
        case TIME_TICK_SIG: {
            Tunnel_advance(me);
            if (me->blink_ctr != 0U) {
                /* add the text bitmap into the frame buffer */
                Tunnel_addImageAt(me,
                    PRESS_BUTTON_BMP,
                    (GAME_SCREEN_WIDTH - 55U)/2U,
                    (GAME_SCREEN_HEIGHT - 8U)/2U);
            }
            BSP_drawBitmap(l_frame);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/3/3) */
        case PLAYER_TRIGGER_SIG: {
            status_ = Q_TRAN(&Tunnel_playing);
            break;
        }
        default: {
            status_ = Q_SUPER(&Tunnel_active);
            break;
        }
    }
    return status_;
}
/* @(/2/0/16/1/4) ..........................................................*/
static QState Tunnel_playing(Tunnel * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/16/1/4) */
        case Q_ENTRY_SIG: {
            static QEvt const takeoff = { TAKE_OFF_SIG, 0U, 0U };
            me->minimal_gap = GAME_SCREEN_HEIGHT - 3u;
            /* erase the walls */
            memset(l_walls, (uint8_t)0,
                  (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));
            QACTIVE_POST(AO_Ship, &takeoff, me); /* post the TAKEOFF sig */
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/4) */
        case Q_EXIT_SIG: {
            QEvt recycle;
            recycle.sig = MINE_RECYCLE_SIG;
            Tunnel_dispatchToAllMines(me, &recycle); /* recycle all Mines */
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/4/0) */
        case TIME_TICK_SIG: {
            /* render this frame on the display */
            BSP_drawBitmap(l_frame);
            Tunnel_advance(me);
            Tunnel_plantMine(me);
            Tunnel_dispatchToAllMines(me, e);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/4/1) */
        case SHIP_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            int8_t  y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;

            /* did the Ship/Missile hit the tunnel wall? */
            if (Tunnel_isWallHit(me, bmp, x, y)) {
                static QEvt const hit = { HIT_WALL_SIG, 0U, 0U };
                QACTIVE_POST(AO_Ship, &hit, me);
            }
            Tunnel_addImageAt(me, bmp, x, y);
            Tunnel_dispatchToAllMines(me, e); /* let Mines check for hits */
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/4/2) */
        case MISSILE_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            int8_t  y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;

            /* did the Ship/Missile hit the tunnel wall? */
            if (Tunnel_isWallHit(me, bmp, x, y)) {
                static QEvt const hit = { HIT_WALL_SIG, 0U, 0U };
                QACTIVE_POST(AO_Missile, &hit, me);
            }
            Tunnel_addImageAt(me, bmp, x, y);
            Tunnel_dispatchToAllMines(me, e); /* let Mines check for hits */
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/4/3) */
        case MINE_IMG_SIG: {
            Tunnel_addImageAt(me,
                              Q_EVT_CAST(ObjectImageEvt)->bmp,
                              Q_EVT_CAST(ObjectImageEvt)->x,
                              Q_EVT_CAST(ObjectImageEvt)->y);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/4/4) */
        case EXPLOSION_SIG: {
            Tunnel_addImageAt(me,
                              Q_EVT_CAST(ObjectImageEvt)->bmp,
                              Q_EVT_CAST(ObjectImageEvt)->x,
                              Q_EVT_CAST(ObjectImageEvt)->y);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/4/5) */
        case SCORE_SIG: {
            BSP_updateScore(Q_EVT_CAST(ScoreEvt)->score);
            /* increase difficulty of the game:
            * the tunnel gets narrower as the score goes up
            */
            me->minimal_gap = (uint8_t)(GAME_SCREEN_HEIGHT - 3U
                              - Q_EVT_CAST(ScoreEvt)->score/2000U);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/4/6) */
        case GAME_OVER_SIG: {
            uint16_t score = Q_EVT_CAST(ScoreEvt)->score;
            char str[5];
            BSP_updateScore(score);

            /* clear the screen */
            memset(l_frame, (uint8_t)0,
                   (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8));
            BSP_drawBitmap(l_frame);
            /* Output the final score to the screen */
            BSP_drawNString((GAME_SCREEN_WIDTH - 6U*10U)/2U, 1U, "Score:");
            str[4] = '\0'; /* zero-terminate the string */
            str[3] = '0' + (score % 10U); score /= 10U;
            str[2] = '0' + (score % 10U); score /= 10U;
            str[1] = '0' + (score % 10U); score /= 10U;
            str[0] = '0' + (score % 10U);
            BSP_drawNString((GAME_SCREEN_WIDTH - 6U*10U)/2U + 6U*6U, 1U, str);
            status_ = Q_TRAN(&Tunnel_game_over);
            break;
        }
        default: {
            status_ = Q_SUPER(&Tunnel_active);
            break;
        }
    }
    return status_;
}
/* @(/2/0/16/1/5) ..........................................................*/
static QState Tunnel_game_over(Tunnel * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/16/1/5) */
        case Q_ENTRY_SIG: {
            QTimeEvt_postEvery(&me->blinkTimeEvt, (QActive *)me,
                               BSP_TICKS_PER_SEC/2U); /* 1/2 sec */
            QTimeEvt_postIn(&me->screenTimeEvt, (QActive *)me,
                            BSP_TICKS_PER_SEC*5U); /* 5 sec timeout */
            me->blink_ctr = 0U;
            BSP_drawNString((GAME_SCREEN_WIDTH - 6U*9U)/2U, 0U, "Game Over");
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/5) */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->blinkTimeEvt);
            QTimeEvt_disarm(&me->screenTimeEvt);
            BSP_updateScore(0); /* update the score on the display */
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/5/0) */
        case BLINK_TIMEOUT_SIG: {
            me->blink_ctr ^= 1U; /* toggle the blink couner */
            BSP_drawNString((GAME_SCREEN_WIDTH - 6U*9U)/2, 0U,
                            ((me->blink_ctr == 0U)
                            ? "Game Over"
                            : "         "));
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/5/1) */
        case SCREEN_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Tunnel_demo);
            break;
        }
        default: {
            status_ = Q_SUPER(&Tunnel_active);
            break;
        }
    }
    return status_;
}
/* @(/2/0/16/1/6) ..........................................................*/
static QState Tunnel_screen_saver(Tunnel * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/16/1/6/0) */
        case Q_INIT_SIG: {
            status_ = Q_TRAN(&Tunnel_screen_saver_hide);
            break;
        }
        /* @(/2/0/16/1/6/1) */
        case PLAYER_TRIGGER_SIG: {
            status_ = Q_TRAN(&Tunnel_demo);
            break;
        }
        default: {
            status_ = Q_SUPER(&Tunnel_active);
            break;
        }
    }
    return status_;
}
/* @(/2/0/16/1/6/2) ........................................................*/
static QState Tunnel_screen_saver_hide(Tunnel * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/16/1/6/2) */
        case Q_ENTRY_SIG: {
            BSP_displayOff(); /* power down the display */
            QTimeEvt_postIn(&me->screenTimeEvt, (QActive *)me,
                            BSP_TICKS_PER_SEC*3U); /* 3s timeout */
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/6/2) */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->screenTimeEvt);
            BSP_displayOn(); /* power up the display */
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/6/2/0) */
        case SCREEN_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Tunnel_screen_saver_show);
            break;
        }
        default: {
            status_ = Q_SUPER(&Tunnel_screen_saver);
            break;
        }
    }
    return status_;
}
/* @(/2/0/16/1/6/3) ........................................................*/
static QState Tunnel_screen_saver_show(Tunnel * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/16/1/6/3) */
        case Q_ENTRY_SIG: {
            uint32_t rnd = random();
            /* clear the screen frame buffer */
            memset(l_frame, (uint8_t)0,
                   (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));
            Tunnel_addImageAt(me,
                    PRESS_BUTTON_BMP,
                    (uint8_t)(rnd % (GAME_SCREEN_WIDTH - 55U)),
                    (int8_t) (rnd % (GAME_SCREEN_HEIGHT - 8U)));
            BSP_drawBitmap(l_frame);
            QTimeEvt_postIn(&me->screenTimeEvt, (QActive *)me,
                            BSP_TICKS_PER_SEC/3U); /* 1/3 sec timeout */
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/6/3) */
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->screenTimeEvt);
            /* clear the screen frame buffer */
            memset(l_frame, (uint8_t)0,
                   (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8U));
            BSP_drawBitmap(l_frame);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/16/1/6/3/0) */
        case SCREEN_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Tunnel_screen_saver_hide);
            break;
        }
        default: {
            status_ = Q_SUPER(&Tunnel_screen_saver);
            break;
        }
    }
    return status_;
}
/* @(/2/0/16/2) ............................................................*/
static QState Tunnel_final(Tunnel * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/16/2) */
        case Q_ENTRY_SIG: {
            /* clear the screen */
            memset(l_frame, (uint8_t)0,
                  (GAME_SCREEN_WIDTH * GAME_SCREEN_HEIGHT/8));
            BSP_drawBitmap(l_frame);
            QF_stop(); /* stop QF and cleanup */
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}


/*..........................................................................*/
uint32_t random(void) {      /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * (3*7*11*13*23);
    return l_rnd >> 8;
}
/*..........................................................................*/
void randomSeed(uint32_t seed) {
    l_rnd = seed;
}
/*..........................................................................*/
uint8_t do_bitmaps_overlap(uint8_t bmp_id1, uint8_t x1, uint8_t y1,
                           uint8_t bmp_id2, uint8_t x2, uint8_t y2)
{
    uint8_t x;
    uint8_t x0;
    uint8_t w;
    uint32_t bits1;
    uint32_t bits2;
    Bitmap const *bmp1;
    Bitmap const *bmp2;

    Q_REQUIRE((bmp_id1 < Q_DIM(l_bitmap)) && (bmp_id2 < Q_DIM(l_bitmap)));

    bmp1 = &l_bitmap[bmp_id1];
    bmp2 = &l_bitmap[bmp_id2];

             /* is the incoming object starting to overlap the Mine bitmap? */
    if ((x1 <= x2) && (x1 + bmp2->width > x2)) {
        x0 = x2 - x1;
        w  = x1 + bmp2->width - x2;
        if (w > bmp1->width) {
            w = bmp1->width;
        }
        for (x = 0; x < w; ++x) {      /* scan over the overlapping columns */
            bits1 = ((uint32_t)bmp2->bits[x + x0] << y2);
            bits2 = ((uint32_t)bmp1->bits[x] << y1);
            if ((bits1 & bits2) != 0) {             /* do the bits overlap? */
                return (uint8_t)1;                                  /* yes! */
            }
        }
    }
    else {
        if ((x1 > x2) && (x2 + bmp1->width > x1)) {
            x0 = x1 - x2;
            w  = x2 + bmp1->width - x1;
            if (w > bmp2->width) {
                w = bmp2->width;
            }
            for (x = 0; x < w; ++x) {  /* scan over the overlapping columns */
                bits1 = ((uint32_t)bmp1->bits[x + x0] << y1);
                bits2 = ((uint32_t)bmp2->bits[x] << y2);
                if ((bits1 & bits2) != 0) {         /* do the bits overlap? */
                    return (uint8_t)1;                              /* yes! */
                }
            }
        }
    }
    return (uint8_t)0;                        /* the bitmaps do not overlap */
}
