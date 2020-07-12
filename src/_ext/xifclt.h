/**
 * \file: xifclt.h
 * \brief: xifclt(XIFCLT) module.
 */

#ifndef _XIFCLT_H_
#define _XIFCLT_H_

/*
 * xifclt: XIF interface of this app, act as a client of operations to other app.
 */

#include "projectdefs.h"
#include "xif.h"

// @@define external: XIFCLT_USE_LOCAL_COPY
// #define XIFCLT_USE_LOCAL_COPY

typedef struct Txifclt {
	// timing out control
	uint tryCount;
	TICK waitTicks;
	uint tryCountBydefault;
	TICK waitTicksByDefault;

#if XIFCLT_USE_LOCAL_COPY
// local copies of oids of apps
int localOid_Count[app_Count];
ToidDef *localOidDefs[app_Count];
ToidText *localOidTexts[app_Count];
#endif

} Txifclt;
extern Txifclt xifclt;

bool xifclt_open(void);
void xifclt_close(void);

// timing out control
#define xifclt_setTryCount(count) xifclt.tryCount = (count)
#define xifclt_setWaitTick(ticks) xifclt.waitTicks = (ticks)

#define XIFCLT_SET_UNBLOCK() { xifclt.tryCount = 1; xifclt.waitTicks = 0; }
extern TxifMessage xifclt_tx_msg;

////###############################################################################################
#define __code_block_api 1
#if __code_block_api

// !!! define for the below functions: xif_set_oid ... xif_cmd_unblock
#define xif_cmd_prm xifclt_tx_msg.prm

/*
 * oid operations
 */
int xif_get_oid(int app, int oid, int index, ToidDef *toPtr);
int xif_set_oid(int app, int oid, int index, ToidDef *fromPtr);
int xif_gets_oid(int app, int oid, int index, char *toPtr, int *lengthPtr,
		int limit);

/*
 * get operations
 */
int xif_get(int app, int oid, int index, void *toPtr, int length);
int xif_gets(int app, int oid, int index, char *toPtr, int *lengthPtr,
		int limit);
int xif_get_limit(int app, int oid, int index, void *toPtr, int *lengthPtr,
		int limit);
// extended functions
int xif_get_int(int app, int oid, int index, int *intPtr);
int xif_get_enum(int app, int oid, int index, TenumText *enumPtr);
int xif_get_enumText(int app, int oid, int index, char *toPtr);
inline int xif_get_float(int app, int oid, int index, float *floatPtr);

/*
 * set operations
 */
int xif_set(int app, int oid, int index, void *fromPtr, int length);
int xif_sets(int app, int oid, int index, char *fromPtr);
// extended functions
int xif_set_int(int app, int oid, int index, int intValue);
inline int xif_set_enum(int app, int oid, int index, TenumText et);

/*
 * app defined commands
 */
int xif_cmd(int app, int oid, int index, int command, ToidPrm *prmPtr,
		int *lengthPtr);
inline int xif_cmd_unblock(int app, int oid, int index, int command,
		ToidPrm *prmPtr, int *lengthPtr);

#endif //// __code_block_api
////###############################################################################################
#define __code_block_fastApi 1
#if __code_block_fastApi
char *xif_fast_gets(int oid, int index, char *defaultString);
#endif //// __code_block_fastApi

#endif // _XIFCLT_H_
