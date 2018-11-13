/*****************************************************************************
* Model: game.qm
* File:  ./mine2.c
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
/* @(/3/5) .................................................................*/
#include "qp_port.h"
#include "bsp.h"
#include "game.h"

Q_DEFINE_THIS_FILE

/* encapsulated delcaration of the Mine2 HSM -------------------------------*/
/* @(/2/4) .................................................................*/
typedef struct Mine2Tag {
/* protected: */
    QHsm super;

/* private: */
    uint8_t x;
    uint8_t y;

/* public: */
    uint8_t exp_ctr;
} Mine2;

/* protected: */
static QState Mine2_initial(Mine2 * const me, QEvt const * const e);
static QState Mine2_unused(Mine2 * const me, QEvt const * const e);
static QState Mine2_used(Mine2 * const me, QEvt const * const e);
static QState Mine2_planted(Mine2 * const me, QEvt const * const e);
static QState Mine2_exploding(Mine2 * const me, QEvt const * const e);


/* local objects -----------------------------------------------------------*/
static Mine2 l_mine2[GAME_MINES_MAX];             /* a pool of type-2 mines */

                             /* helper macro to provide the ID of this mine */
#define MINE_ID(me_)    ((uint8_t)((me_) - l_mine2))

/* Mine2 class definition --------------------------------------------------*/
/* @(/2/13) ................................................................*/
QHsm * Mine2_ctor(uint8_t id) {
    Mine2 *me;
    Q_REQUIRE(id < GAME_MINES_MAX);
    me = &l_mine2[id];
    /* superclass' ctor */
    QHsm_ctor(&me->super, (QStateHandler)&Mine2_initial);
    return (QHsm *)me;
}
/* @(/2/4) .................................................................*/
/* @(/2/4/3) ...............................................................*/
/* @(/2/4/3/0) */
static QState Mine2_initial(Mine2 * const me, QEvt const * const e) {
    static  uint8_t dict_sent;

    if (!dict_sent) {
        /* object dictionaries for Mine2 pool... */
        QS_OBJ_DICTIONARY(&l_mine2[0]);
        QS_OBJ_DICTIONARY(&l_mine2[1]);
        QS_OBJ_DICTIONARY(&l_mine2[2]);
        QS_OBJ_DICTIONARY(&l_mine2[3]);
        QS_OBJ_DICTIONARY(&l_mine2[4]);

        /*function dictionaries for Mine2 HSM... */
        QS_FUN_DICTIONARY(&Mine2_initial);
        QS_FUN_DICTIONARY(&Mine2_unused);
        QS_FUN_DICTIONARY(&Mine2_used);
        QS_FUN_DICTIONARY(&Mine2_planted);
        QS_FUN_DICTIONARY(&Mine2_exploding);

        dict_sent = 1U;
    }
    /* local signals... */
    QS_SIG_DICTIONARY(MINE_PLANT_SIG,    me);
    QS_SIG_DICTIONARY(MINE_DISABLED_SIG, me);
    QS_SIG_DICTIONARY(MINE_RECYCLE_SIG,  me);
    QS_SIG_DICTIONARY(SHIP_IMG_SIG,      me);
    QS_SIG_DICTIONARY(MISSILE_IMG_SIG,   me);

    (void)e; /* avoid the "unreferenced parameter" warning */
    return Q_TRAN(&Mine2_unused);
}
/* @(/2/4/3/1) .............................................................*/
static QState Mine2_unused(Mine2 * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/4/3/1/0) */
        case MINE_PLANT_SIG: {
            me->x = Q_EVT_CAST(ObjectPosEvt)->x;
            me->y = Q_EVT_CAST(ObjectPosEvt)->y;
            status_ = Q_TRAN(&Mine2_planted);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/* @(/2/4/3/2) .............................................................*/
static QState Mine2_used(Mine2 * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/4/3/2) */
        case Q_EXIT_SIG: {
            /* tell the Tunnel that this mine is becoming disabled */
            MineEvt *mev = Q_NEW(MineEvt, MINE_DISABLED_SIG);
            mev->id = MINE_ID(me);
            QACTIVE_POST(AO_Tunnel, (QEvt *)mev, me);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/4/3/2/0) */
        case MINE_RECYCLE_SIG: {
            status_ = Q_TRAN(&Mine2_unused);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/* @(/2/4/3/2/1) ...........................................................*/
static QState Mine2_planted(Mine2 * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/4/3/2/1/0) */
        case TIME_TICK_SIG: {
            /* @(/2/4/3/2/1/0/0) */
            if (me->x >= GAME_SPEED_X) {
                ObjectImageEvt *oie;
                 me->x -= GAME_SPEED_X; /* move the mine 1 step */
                /* tell the Tunnel to draw the Mine */
                oie = Q_NEW(ObjectImageEvt, MINE_IMG_SIG);
                oie->x   = me->x;
                oie->y   = me->y;
                oie->bmp = MINE2_BMP;
                QACTIVE_POST(AO_Tunnel, (QEvt *)oie, me);
                status_ = Q_HANDLED();
            }
            /* @(/2/4/3/2/1/0/1) */
            else {
                status_ = Q_TRAN(&Mine2_unused);
            }
            break;
        }
        /* @(/2/4/3/2/1/1) */
        case SHIP_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            uint8_t y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;
            /* @(/2/4/3/2/1/1/0) */
            if (do_bitmaps_overlap(MINE2_BMP, me->x, me->y, bmp, x, y)) {
                static MineEvt const mine1_hit = {
                    { HIT_MINE_SIG, 0U, 0U }, /* the QEvt base instance */
                    2U  /* type of the mine (2 for Mine type-2) */
                };
                 QACTIVE_POST(AO_Ship, (QEvt *)&mine1_hit, me);
                 /* go straight to 'disabled' and let the Ship do
                 * the exploding */
                status_ = Q_TRAN(&Mine2_unused);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        /* @(/2/4/3/2/1/2) */
        case MISSILE_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            uint8_t y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;
            /* @(/2/4/3/2/1/2/0) */
            if (do_bitmaps_overlap(MINE2_MISSILE_BMP, me->x, me->y, bmp, x, y)) {
                /* NOTE: Mine type-2 is nastier than Mine type-1.
                * The type-2 mine can hit the Ship with any of its
                * "tentacles". However, it can be destroyed by the
                * Missile only by hitting its center, defined as
                * a smaller bitmap MINE2_MISSILE_BMP.
                */
                static ScoreEvt const mine2_destroyed = {
                    { DESTROYED_MINE_SIG, 0U, 0U },  /* the QEvt base instance */
                    45U  /* score for destroying Mine type-2 */
                };
                QACTIVE_POST(AO_Missile, (QEvt *)&mine2_destroyed, me);
                status_ = Q_TRAN(&Mine2_exploding);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&Mine2_used);
            break;
        }
    }
    return status_;
}
/* @(/2/4/3/2/2) ...........................................................*/
static QState Mine2_exploding(Mine2 * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/4/3/2/2) */
        case Q_ENTRY_SIG: {
            me->exp_ctr = 0U;
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/4/3/2/2/0) */
        case TIME_TICK_SIG: {
            /* @(/2/4/3/2/2/0/0) */
            if ((me->x >= GAME_SPEED_X) && (me->exp_ctr < 15U)) {
                ObjectImageEvt *oie;
                ++me->exp_ctr;  /* advance the explosion counter */
                 me->x -= GAME_SPEED_X; /* move explosion by 1 step */

                /* tell the Game to render the current stage of Explosion */
                oie = Q_NEW(ObjectImageEvt, EXPLOSION_SIG);
                oie->x   = me->x + 1U;  /* x of explosion */
                oie->y   = (int8_t)((int)me->y - 4 + 2); /* y of explosion */
                oie->bmp = EXPLOSION0_BMP + (me->exp_ctr >> 2);
                QACTIVE_POST(AO_Tunnel, (QEvt *)oie, me);
                status_ = Q_HANDLED();
            }
            /* @(/2/4/3/2/2/0/1) */
            else {
                status_ = Q_TRAN(&Mine2_unused);
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&Mine2_used);
            break;
        }
    }
    return status_;
}

