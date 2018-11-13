#include <project.h>
#include "dev.h"

//Qp Events and Queues
static QEvt const *l_mainFsmQueueSto[SMALLPOOLSIZE];	//storage for events, up to this many ADC service requests can be made
static union SmallEvent	{
	void *min_size;
	QEvt e;
	//other event types go into this pool
} l_smlPoolSto[SMALLPOOLSIZE];

//subscriber list declaration
static QSubscrList l_subscrSto[MAX_PUB_SIG];

int main()
{
	//instance main FSM
	MainFsm_ctor();		

	//init board level
	BSP_init();			

	//init framework
	QF_init();

	//init subscriber/publisher
	QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));		
	
	//init event pools
	QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));

	//init main FSM
	QActive_start(AO_MainFsm,	1, l_mainFsmQueueSto, Q_DIM(l_mainFsmQueueSto), (void *)0, 0U, (QEvt *)0);

	//start main FSM
	return QF_run();
}
/* [] END OF FILE */
