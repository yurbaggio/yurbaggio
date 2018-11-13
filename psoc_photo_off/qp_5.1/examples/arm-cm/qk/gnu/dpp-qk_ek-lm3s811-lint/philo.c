/*****************************************************************************
* Model: dpp.qm
* File:  ./philo.c
*
* This file has been generated automatically by QP Modeler (QM).
* DO NOT EDIT THIS FILE MANUALLY.
*
* Please visit www.state-machine.com/qm for more information.
*****************************************************************************/
/* @(/3/1) .................................................................*/
#include "qp_port.h"
#include "dpp.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

/* Active object class -----------------------------------------------------*/
/* @(/2/0) .................................................................*/
typedef struct PhiloTag {
/* protected: */
    QActive super;

/* private: */
    QTimeEvt timeEvt;
} Philo;

/* protected: */
static QState Philo_initial(Philo * const me, QEvt const * const e);
static QState Philo_thinking(Philo * const me, QEvt const * const e);
static QState Philo_hungry(Philo * const me, QEvt const * const e);
static QState Philo_eating(Philo * const me, QEvt const * const e);


/* Local objects -----------------------------------------------------------*/
static Philo l_philo[N_PHILO];   /* storage for all Philos */

#define THINK_TIME  \
    (QTimeEvtCtr)((BSP_random() % BSP_TICKS_PER_SEC) + (BSP_TICKS_PER_SEC/2U))
#define EAT_TIME    \
    (QTimeEvtCtr)((BSP_random() % BSP_TICKS_PER_SEC) + BSP_TICKS_PER_SEC)

/* helper macro to provide the ID of Philo "me_" */
#define PHILO_ID(me_)    ((uint8_t)((me_) - l_philo))

enum InternalSignals {           /* internal signals */
    TIMEOUT_SIG = MAX_SIG
};

/* Global objects ----------------------------------------------------------*/
QActive * const AO_Philo[N_PHILO] = { /* "opaque" pointers to Philo AO */
    &l_philo[0].super,
    &l_philo[1].super,
    &l_philo[2].super,
    &l_philo[3].super,
    &l_philo[4].super
};

/* Philo definition --------------------------------------------------------*/
/* @(/2/4) .................................................................*/
void Philo_ctor(void) {
    uint8_t n;
    Philo *me;
    for (n = 0U; n < N_PHILO; ++n) {
        me = &l_philo[n];
        QActive_ctor(&me->super, Q_STATE_CAST(&Philo_initial));
        QTimeEvt_ctor(&me->timeEvt, TIMEOUT_SIG);
    }
}
/* @(/2/0) .................................................................*/
/* @(/2/0/1) ...............................................................*/
/* @(/2/0/1/0) */
static QState Philo_initial(Philo * const me, QEvt const * const e) {
    static uint8_t registered = (uint8_t)0; /* starts off with 0, per C-standard */
    (void)e; /* suppress the compiler warning about unused parameter */
    if (registered == (uint8_t)0) {
        registered = (uint8_t)1;

        QS_OBJ_DICTIONARY(&l_philo[0]);
        QS_OBJ_DICTIONARY(&l_philo[0].timeEvt);
        QS_OBJ_DICTIONARY(&l_philo[1]);
        QS_OBJ_DICTIONARY(&l_philo[1].timeEvt);
        QS_OBJ_DICTIONARY(&l_philo[2]);
        QS_OBJ_DICTIONARY(&l_philo[2].timeEvt);
        QS_OBJ_DICTIONARY(&l_philo[3]);
        QS_OBJ_DICTIONARY(&l_philo[3].timeEvt);
        QS_OBJ_DICTIONARY(&l_philo[4]);
        QS_OBJ_DICTIONARY(&l_philo[4].timeEvt);

        QS_FUN_DICTIONARY(&Philo_initial);
        QS_FUN_DICTIONARY(&Philo_thinking);
        QS_FUN_DICTIONARY(&Philo_hungry);
        QS_FUN_DICTIONARY(&Philo_eating);
    }
    QS_SIG_DICTIONARY(HUNGRY_SIG, me);  /* signal for each Philos */
    QS_SIG_DICTIONARY(TIMEOUT_SIG, me); /* signal for each Philos */

    QActive_subscribe(&me->super, EAT_SIG);
    return Q_TRAN(&Philo_thinking);
}
/* @(/2/0/1/1) .............................................................*/
static QState Philo_thinking(Philo * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/1/1) */
        case Q_ENTRY_SIG: {
            QTimeEvt_postIn(&me->timeEvt, &me->super, THINK_TIME);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/1/1/0) */
        case TIMEOUT_SIG: {
            status_ = Q_TRAN(&Philo_hungry);
            break;
        }
        /* @(/2/0/1/1/1) */
        case EAT_SIG: /* intentionally fall through */
        case DONE_SIG: {
            /* EAT or DONE must be for other Philos than this one */
            Q_ASSERT(Q_EVT_CAST(TableEvt)->philoNum != PHILO_ID(me));
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
/* @(/2/0/1/2) .............................................................*/
static QState Philo_hungry(Philo * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/1/2) */
        case Q_ENTRY_SIG: {
            TableEvt *pe = Q_NEW(TableEvt, HUNGRY_SIG);
            pe->philoNum = PHILO_ID(me);
            QACTIVE_POST(AO_Table, &pe->super, me);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/1/2/0) */
        case EAT_SIG: {
            /* @(/2/0/1/2/0/0) */
            if (Q_EVT_CAST(TableEvt)->philoNum == PHILO_ID(me)) {
                status_ = Q_TRAN(&Philo_eating);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        /* @(/2/0/1/2/1) */
        case DONE_SIG: {
            /* DONE must be for other Philos than this one */
            Q_ASSERT(Q_EVT_CAST(TableEvt)->philoNum != PHILO_ID(me));
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
/* @(/2/0/1/3) .............................................................*/
static QState Philo_eating(Philo * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* @(/2/0/1/3) */
        case Q_ENTRY_SIG: {
            QTimeEvt_postIn(&me->timeEvt, &me->super, EAT_TIME);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/1/3) */
        case Q_EXIT_SIG: {
            TableEvt *pe = Q_NEW(TableEvt, DONE_SIG);
            pe->philoNum = PHILO_ID(me);
            QF_PUBLISH(&pe->super, me);
            status_ = Q_HANDLED();
            break;
        }
        /* @(/2/0/1/3/0) */
        case TIMEOUT_SIG: {
            status_ = Q_TRAN(&Philo_thinking);
            break;
        }
        /* @(/2/0/1/3/1) */
        case EAT_SIG: /* intentionally fall through */
        case DONE_SIG: {
            /* EAT or DONE must be for other Philos than this one */
            Q_ASSERT(Q_EVT_CAST(TableEvt)->philoNum != PHILO_ID(me));
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

