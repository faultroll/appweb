/**
 * \file: xifclt.c
 * \brief: xifclt(XIFCLT) module.
 */

#include "project.h"

#ifdef _XIFCLT_H_

#ifdef DBG_XIFCLT_C
#define DBG_THIS_FILE DBG_XIFCLT_C
#else
#define DBG_THIS_FILE DBG_DEFAULT_ALL_C
#endif

// ################################################################################################

#define XIF_FILE_UN_INITIAL -2 // must be <= -2
static int xifclt_app_fd[app_Count];
static int xifclt_fd = -1;

Txifclt xifclt;

bool xifclt_open(void) {
	char chars[64];

	// create the file for xifclt_fd and open it
	if (xifclt_fd < 0) {
		sprintf(chars, "/dev/" DEVICE_RXIF_NAME "%d", THIS_APP);
		xifclt_fd = open(chars, O_RDONLY);
		if (xifclt_fd < 0) {
			sysPerror(chars);
			return 0;
		}
	}
	XIF_CLEAR(xifclt_fd);

	// initialize the xifclt_app_fd[]
	int i;
	for (i = 0; i < app_Count; i++)
		xifclt_app_fd[i] = XIF_FILE_UN_INITIAL;

	// init variables
	xifclt.tryCountBydefault = 3;
	xifclt.waitTicksByDefault = 0.3 * SYSTEM_TICK_PER_SECOND;

	// set to avoid the first time error.
	xifclt.tryCount = xifclt.tryCountBydefault;
	xifclt.waitTicks = xifclt.waitTicksByDefault;

	return 1;
}

void xifclt_close(void) {
	if (xifclt_fd > 0) {
		close(xifclt_fd);
		xifclt_fd = -1;
	}

	int i;
	for (i = 0; i < app_Count; i++) {
		if (xifclt_app_fd[i] > 0) {
			close(xifclt_app_fd[i]);
			xifclt_app_fd[i] = XIF_FILE_UN_INITIAL;
		}
	}

}

////###############################################################################################
#define __code_block_message 1
#if __code_block_message

bool xifclt_send_msg(int app, TxifMessage *msg) {
	if (app >= app_Count)
		return 0;
	if (app == THIS_APP)
		return 0;
	if (xifclt_app_fd[app] == XIF_FILE_UN_INITIAL) {
		TStringPath fileName;
		sprintf(fileName.chars, "/dev/" DEVICE_XIF_NAME "%d", app);
		xifclt_app_fd[app] = open(fileName.chars, O_WRONLY);
	}
	if (xifclt_app_fd[app] <= 0)
		return 0;

	msg->sender = THIS_APP;
	return XIF_WRITE(xifclt_app_fd[app], msg);
}

TxifMessage xifclt_tx_msg;
TxifMessage xifclt_rx_msg;

// judge the message(in xifclt_rx_msg) is the right responded message to the message last sent
//   (in xifclt_tx_msg)
//
bool xifclt_isMatched(void) {
	return 1; //@@?? todo..
}

//
// Execute the message and wait for back the respond message
//   * The input message: xifclt_tx_msg;
//   * The input message: xifclt_rx_msg;
//
// Return: 1 for success, 0 for error
bool xifclt_exe_msg(int app) {
	int ret = 0;

#ifdef _XIFSRV_H_

	// if the message is send to me, the do not use the IPC to send and receive message
	// and directly call the server handler xifsrv_exe_msg()
	if (app == THIS_APP) {
		xifclt_tx_msg.sender = THIS_APP;
		memcpy(&xifclt_rx_msg, xifsrv_exe_msg(&xifclt_tx_msg), sizeof(xifclt_rx_msg));
		return 1;
	}
#else
	/*
	 * if this app has no server xif, return 0 for timing out result
	 */
	if((app == THIS_APP))
	return 0;
#endif

	// clear the return message buffer before waiting for the new responding message
	XIF_CLEAR(xifclt_fd);

	while (xifclt.tryCount--) {
		TICK tick;

		// clear the receive buffer
		xifclt_rx_msg.status = XIF_RETURN_TIMMING_OUT;
		xifclt_rx_msg.length = 0;

		// send the message at first
		if (!xifclt_send_msg(app, &xifclt_tx_msg)) {
			// wait for a interval.
			tick_run();
			TICK_SET_TICK(tick, xifclt.waitTicks);
			while (TICK_BEFORE(tick))
				tick_run();
			continue;
		} else // wait for a respond message once the message sent success
		{
			// wait the respond with timeout control
			tick_run();
			TICK_SET_TICK(tick, xifclt.waitTicks);
			while (1) {
				// timing out detect
				tick_run();
				if (TICK_AFTER(tick))
					break;// timing out

				// get respond message
				if (XIF_READ(xifclt_fd, &xifclt_rx_msg)) {
					if (xifclt_rx_msg.length < XIF_MESSAGE_HEADER_LENGTH)
						break;
					if (xifclt_rx_msg.length >= sizeof(xifclt_rx_msg))
						break;
					if (xifclt_isMatched()) {
						xifclt.tryCount = 0; // cause exit
						ret = 1; // success
						break;
					}
					//else: discard the message or handle it
				}
			}
		}
	}

	// restore to default value.
	xifclt.tryCount = xifclt.tryCountBydefault;
	xifclt.waitTicks = xifclt.waitTicksByDefault;

	return ret;
}
#endif //// __code_block_message
////###############################################################################################
#define __code_block_api 1
#if __code_block_api

/*
 * \fn: int xif_get_oid(int app, int oid, int index, ToidDef *toPtr)
 * \brief: get oid properties from apps
 * \param[in] app: the number of destination app
 * \param[in] oid: the index of oid
 * \param[in] index: the index of the array element in oid
 * \param[out] toPtr: the oid properties, it would be set ONLY when return XIF_RETURN_SUCCESS
 * \return: see XIF_RETURN_* in xif.h
 *
 * \note
 *   NULL is not allowed in toPtr
 *
 * \note: the global xif_cmd_prm(xifclt_tx_msg.prm) can be used for calling the function
 */
int xif_get_oid(int app, int oid, int index, ToidDef *toPtr) {
	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_GET_OID;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index; // opt or set to 0
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH;

	// send the message to destination app
	if (!xifclt_exe_msg(app))
		return XIF_RETURN_TIMMING_OUT;

	// return back data if success
	if (xifclt_rx_msg.status == XIF_RETURN_SUCCESS) {
		// check the length
		if (xifclt_rx_msg.length != (XIF_MESSAGE_HEADER_LENGTH + sizeof(ToidDef)))
			return XIF_RETURN_ERROR_VALUE;

		memcpy((char *) toPtr, xifclt_rx_msg.prm.chars, sizeof(ToidDef));
		return XIF_RETURN_SUCCESS;
	} else
		return xifclt_rx_msg.status;
}

/*
 * \fn: int xif_set_oid(int app, int oid, int index, ToidDef *fromPtr)
 * \brief: set oid properties to apps
 * \param[in] app: the number of destination app
 * \param[in] oid: the index of oid
 * \param[in] index: the index of the array element in oid
 * \param[in] fromPtr: the pointer to oid properties
 * \return: see XIF_RETURN_* in xif.h
 *
 * \note: the global xif_cmd_prm(xifclt_tx_msg.prm) can be used for calling the function
 */
int xif_set_oid(int app, int oid, int index, ToidDef *fromPtr) {
	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_SET_OID;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index;
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH + sizeof(ToidDef);
	memcpy(xifclt_tx_msg.prm.chars, (char *) fromPtr, sizeof(ToidDef));

	// send the message to destination app
	if (!xifclt_exe_msg(app))
		return XIF_RETURN_TIMMING_OUT;
	else
		return xifclt_rx_msg.status;
}

/*
 * \fn: int xif_gets_oid(int app, int oid, int index, char *toPtr, int *lengthPtr, int limit)
 * \brief: get oid value(in string type) from apps
 * \param all: the same as the function xif_gets()
 * \see: xif_gets()
 */
int xif_gets_oid(int app, int oid, int index, char *toPtr, int *lengthPtr, int limit) {
	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_GETS_OID;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index;
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH;

	// send the message to destination app
	if (!xifclt_exe_msg(app)) {
		// the *lengthPtr would not be set
		return XIF_RETURN_TIMMING_OUT;
	}

	// return back data if success
	if (xifclt_rx_msg.status == XIF_RETURN_SUCCESS) {
		// get the length returned
		int length = xifclt_rx_msg.length - XIF_MESSAGE_HEADER_LENGTH;

		// check the length and the terminal character
		if (!length || (length > limit) || (xifclt_rx_msg.prm.chars[length - 1] != '\0')) {
			// the *lengthPtr would not be set
			return XIF_RETURN_ERROR_VALUE;
		}

		*lengthPtr = length;
		memcpy(toPtr, xifclt_rx_msg.prm.chars, length);
		return XIF_RETURN_SUCCESS;
	} else {
		// the *lengthPtr would not be set
		return xifclt_rx_msg.status;
	}
}

/*
 * \fn: int xif_get(int app, int oid, int index, void *toPtr, int length)
 * \brief: get oid value from apps
 * \param[in] app: the number of destination app
 * \param[in] oid: the index of oid
 * \param[in] index: the index of the array element in oid
 * \param[out] toPtr: the value would be set ONLY when return XIF_RETURN_SUCCESS
 * \param[in] length: the length of *toPtr. the return data size must be the same as length.
 * \return: see XIF_RETURN_* in xif.h
 *
 * \note
 *   NULL is not allowed in toPtr
 *
 * \note: the global xif_cmd_prm(xifclt_tx_msg.prm) can be used for calling the function
 */
int xif_get(int app, int oid, int index, void *toPtr, int length) {
	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_GET;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index;
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH;

	// send the message to destination app
	if (!xifclt_exe_msg(app))
		return XIF_RETURN_TIMMING_OUT;

	// return back data if success
	if (xifclt_rx_msg.status == XIF_RETURN_SUCCESS) {
		// check the length
		if (length != (xifclt_rx_msg.length - XIF_MESSAGE_HEADER_LENGTH))
			return XIF_RETURN_ERROR_VALUE;

		memcpy(toPtr, xifclt_rx_msg.prm.chars, length);
		return XIF_RETURN_SUCCESS;
	} else
		return xifclt_rx_msg.status;
}

/*
 * \fn: (int app, int oid, int index, char *toPtr, int *lengthPtr, int limit)
 * \brief: get oid value(in string type) from apps
 * \param[in] app: the number of destination app
 * \param[in] oid: the index of oid
 * \param[in] index: the index of the array element in oid
 * \param[out] toPtr: the value would be set ONLY when return XIF_RETURN_SUCCESS
 * \param[out] lengthPtr: the length of *toPtr, it would be set ONLY when return XIF_RETURN_SUCCESS
 * \param[in] limit: the length limit of *chars.
 * \return: see XIF_RETURN_* in xif.h
 *
 * \note: range of param limit:
 *   [0, sizeof(ToidPrm)]
 *   So the limit must less than sizeof(ToidPrm) to avoid overflow error.
 *
 * \note
 *   NULL is not allowed in toPtr and lengthPtr
 *
 * \note: the global xif_cmd_prm(xifclt_tx_msg.prm) can be used for calling the function
 */
int xif_gets(int app, int oid, int index, char *toPtr, int *lengthPtr, int limit) {
	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_GETS;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index;
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH;

	// send the message to destination app
	if (!xifclt_exe_msg(app)) {
		// the *lengthPtr would not be set
		return XIF_RETURN_TIMMING_OUT;
	}

	// return back data if success
	if (xifclt_rx_msg.status == XIF_RETURN_SUCCESS) {
		// get the length returned
		int length = xifclt_rx_msg.length - XIF_MESSAGE_HEADER_LENGTH;

		// check the length and the terminal character
		if (!length || (length > limit) || (xifclt_rx_msg.prm.chars[length - 1] != '\0')) {
			// the *lengthPtr would not be set
			return XIF_RETURN_ERROR_VALUE;
		}

		*lengthPtr = length;
		memcpy(toPtr, xifclt_rx_msg.prm.chars, length);
		return XIF_RETURN_SUCCESS;
	} else {
		// the *lengthPtr would not be set
		return xifclt_rx_msg.status;
	}
}

/*
 * \fn: int xif_get_limit(int app, int oid, int index, void *toPtr, int *lengthPtr, int limit)
 * \brief: get oid value from apps
 * \param[in] app: the number of destination app
 * \param[in] oid: the index of oid
 * \param[in] index: the index of the array element in oid
 * \param[out] toPtr: the value is set ONLY when return XIF_RETURN_SUCCESS
 * \param[out] lengthPtr: the length of *toPtr, it is set when return XIF_RETURN_SUCCESS
 * \param[in] limit: the length limit of *toPtr.
 * \return: see XIF_RETURN_* in xif.h
 *
 * \note: range of param limit:
 *   [0, sizeof(ToidPrm)]
 *   So the limit must less than sizeof(ToidPrm) to avoid overflow error.
 *
 * \note
 *   NULL is not allowed in toPtr and lengthPtr
 *
 * \note: the global xif_cmd_prm(xifclt_tx_msg.prm) can be used for calling the function
 */
int xif_get_limit(int app, int oid, int index, void *toPtr, int *lengthPtr, int limit) {
	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_GET;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index;
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH;

	// send the message to destination app
	if (!xifclt_exe_msg(app)) {
		// the *lengthPtr would not be set
		return XIF_RETURN_TIMMING_OUT;
	}

	// return back data if success
	if (xifclt_rx_msg.status == XIF_RETURN_SUCCESS) {
		// get the length returned
		int length = xifclt_rx_msg.length - XIF_MESSAGE_HEADER_LENGTH;

		// check the length
		if (length > limit) {
			// the *lengthPtr would not be set
			return XIF_RETURN_ERROR_VALUE;
		}

		*lengthPtr = length;
		memcpy(toPtr, xifclt_rx_msg.prm.chars, length);
		return XIF_RETURN_SUCCESS;
	} else {
		// the *lengthPtr would not be set
		return xifclt_rx_msg.status;
	}
}

/* \fn: int xif_get_int(int app, int oid, int index, int *intPtr)
 * \brief: the function is most often used, design a stand along function for fast speed.
 *   and not used the xif_get().
 *
 * \param all: see xif_get()
 * \see: xif_get()
 */
int xif_get_int(int app, int oid, int index, int *intPtr) {
	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_GET;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index; // opt or set to 0
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH;

	// send the message to destination app
	if (!xifclt_exe_msg(app))
		return XIF_RETURN_TIMMING_OUT;

	// return back data if success
	if (xifclt_rx_msg.status == XIF_RETURN_SUCCESS) {
		// check the length
		if (xifclt_rx_msg.length != (XIF_MESSAGE_HEADER_LENGTH + sizeof(int)))
			return XIF_RETURN_ERROR_VALUE;

		*intPtr = xifclt_rx_msg.prm.intValue;
		return XIF_RETURN_SUCCESS;
	} else
		return xifclt_rx_msg.status;
}

/* \fn: int xif_get_enum(int app, int oid, int index, TenumText *enumPtr)
 * \brief: the function is most often used, design a stand along function for fast speed.
 *   and not used the xif_get().
 *
 * \param all: see xif_get()
 * \see: xif_get()
 */
int xif_get_enum(int app, int oid, int index, TenumText *enumPtr) {
	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_GET;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index; // opt or set to 0
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH;

	// send the message to destination app
	if (!xifclt_exe_msg(app))
		return XIF_RETURN_TIMMING_OUT;

	// return back data if success
	if (xifclt_rx_msg.status == XIF_RETURN_SUCCESS) {
		// check the length
		if (xifclt_rx_msg.length != (XIF_MESSAGE_HEADER_LENGTH + sizeof(int)))
			return XIF_RETURN_ERROR_VALUE;

		*enumPtr = (TenumText) xifclt_rx_msg.prm.intValue;
		return XIF_RETURN_SUCCESS;
	} else
		return xifclt_rx_msg.status;
}

// !!! sizeof(*toPtr) must >= sizeof(ToidPrm)
int xif_get_enumText(int app, int oid, int index, char *toPtr) {
	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_GETENUM;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index;
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH;

	// send the message to destination app
	if (!xifclt_exe_msg(app)) {
		// the *lengthPtr would not be set
		return XIF_RETURN_TIMMING_OUT;
	}

	// return back data if success
	if (xifclt_rx_msg.status == XIF_RETURN_SUCCESS) {
		// get the length returned
		int length = xifclt_rx_msg.length - XIF_MESSAGE_HEADER_LENGTH;

		// check the length and the terminal character
		if (!length || (xifclt_rx_msg.prm.chars[length - 1] != '\0')) {
			// the *lengthPtr would not be set
			return XIF_RETURN_ERROR_VALUE;
		}
		memcpy(toPtr, xifclt_rx_msg.prm.chars, length);
		*(toPtr + length - 1) = '\0';
		return XIF_RETURN_SUCCESS;
	} else {
		// the *lengthPtr would not be set
		return xifclt_rx_msg.status;
	}
}

inline int xif_get_float(int app, int oid, int index, float *floatPtr) {
	return xif_get(app, oid, index, floatPtr, sizeof(float));
}

/*
 * \fn: int xif_set(int app, int oid, int index, void *fromPtr, int length)
 * \brief: set oid value to apps
 * \param[in] app: the number of destination app
 * \param[in] oid: the index of oid
 * \param[in] index: the index of the array element in oid
 * \param[in] fromPtr: the value pointer
 * \param[in] length: the length of *fromPtr
 * \return: see XIF_RETURN_* in xif.h
 *
 * \note: the global xif_cmd_prm(xifclt_tx_msg.prm) can be used for calling the function
 */
int xif_set(int app, int oid, int index, void *fromPtr, int length) {
	// check the length of input
	if (length > XIF_MESSAGE_DATA_LENGTH)
		return XIF_RETURN_ERROR_VALUE;

	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_SET;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index;
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH + length;
	memcpy(xifclt_tx_msg.prm.chars, fromPtr, length);

	// send the message to destination app
	if (!xifclt_exe_msg(app))
		return XIF_RETURN_TIMMING_OUT;
	else
		return xifclt_rx_msg.status;
}

/*
 * \fn: int xif_sets(int app, int oid, int index, char *fromPtr)
 * \brief: set oid value(in string type) to apps
 * \param[in] app: the number of destination app
 * \param[in] oid: the index of oid
 * \param[in] index: the index of the array element in oid
 * \param[in] fromPtr: the value to be set
 * \return: see XIF_RETURN_* in xif.h
 *
 * \note
 *   NULL is not allowed in chars
 *
 * \note
 *   the TjoinedStrings string is not allowed here. only the first string is set.
 *   to set TjoinedStrings, please use xif_set().
 *
 * \note: the global xif_cmd_prm(xifclt_tx_msg.prm) can be used for calling the function
 */
int xif_sets(int app, int oid, int index, char *fromPtr) {
	int length = strlen(fromPtr) + 1;

	// check the length of input
	if (length > XIF_MESSAGE_DATA_LENGTH)
		return XIF_RETURN_ERROR_VALUE;

	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_SETS;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index;
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH + length;
	memcpy(xifclt_tx_msg.prm.chars, fromPtr, length);

	// send the message to destination app
	if (!xifclt_exe_msg(app))
		return XIF_RETURN_TIMMING_OUT;
	else
		return xifclt_rx_msg.status;
}

/* \fn: int xif_set_int(int app, int oid, int index, int intValue)
 * \brief: the function is most often used, design a stand along function for fast speed.
 *   and not used the xif_set().
 *
 * \param all: see xif_set()
 * \see: xif_set()
 */
int xif_set_int(int app, int oid, int index, int intValue) {
	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_SET;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index;
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH + sizeof(int);
	xifclt_tx_msg.prm.intValue = intValue;

	// send the message to destination app
	if (!xifclt_exe_msg(app))
		return XIF_RETURN_TIMMING_OUT;
	else
		return xifclt_rx_msg.status;
}

inline int xif_set_enum(int app, int oid, int index, TenumText et) {
	return xif_set_int(app, oid, index, (int) et);
}

/*
 * \fn: int xif_cmd(int app, int oid, int index, int command, ToidPrm *prmPtr, int *lengthPtr)
 * \brief: send a app defined command to external apps
 * \param[in] app: the number of destination app
 * \param[in] oid: the index of oid
 * \param[in] index: the index of the array element in oid
 * \param[in/out] prmPtr: the data or value
 * \param[in/out] lengthPtr: the length of *prmPtr
 * \return: see XIF_RETURN_* in xif.h
 *
 * \note: the global xif_cmd_prm(xifclt_tx_msg.prm) can be used for calling the function
 */
int xif_cmd(int app, int oid, int index, int command, ToidPrm *prmPtr, int *lengthPtr) {
	// check the length of input
	if (*lengthPtr > XIF_MESSAGE_DATA_LENGTH) {
		*lengthPtr = 0;
		return XIF_RETURN_ERROR_VALUE;
	}

	// prepare message to send
	xifclt_tx_msg.command = XIF_COMMAND_APP;
	xifclt_tx_msg.oid = oid;
	xifclt_tx_msg.index = index;
	xifclt_tx_msg.status = XIF_RESPOND_BLOCKED;
	xifclt_tx_msg.length = XIF_MESSAGE_HEADER_LENGTH + *lengthPtr;
	memcpy(xifclt_tx_msg.prm.chars, prmPtr->chars, *lengthPtr);

	// send the message to destination app
	if (!xifclt_exe_msg(app)) {
		*lengthPtr = 0;
		return XIF_RETURN_TIMMING_OUT;
	}

	// get the length returned
	int length = xifclt_rx_msg.length - XIF_MESSAGE_HEADER_LENGTH;

	// return back data and its length
	*lengthPtr = length;
	memcpy(prmPtr->chars, xifclt_rx_msg.prm.chars, length);
	return xifclt_rx_msg.status;
}

/*
 * \fn: inline int xif_cmd_unblock(int app, int oid, int index, int command, ToidPrm *prmPtr, int *lengthPtr)
 * \brief: send a app defined command to external apps
 * \param[in] app: the number of destination app
 * \param[in] oid: the index of oid
 * \param[in] index: the index of the array element in oid
 * \param[in/out] prmPtr: the data or value
 * \param[in/out] lengthPtr: the length of *prmPtr
 * \return: a XIF_RETURN_TIMMING_OUT would be returned
 *
 * \note: the global xif_cmd_prm(xifclt_tx_msg.prm) can be used for calling the function
 *
 */
inline int xif_cmd_unblock(int app, int oid, int index, int command, ToidPrm *prmPtr,
	int *lengthPtr) {
	XIFCLT_SET_UNBLOCK();
	return xif_cmd(app, oid, index, command, prmPtr, lengthPtr);
}

#endif //// __code_block_api
////###############################################################################################
#define __code_block_fastApi 1
#if __code_block_fastApi

ToidPrm xif_fast_prm;
char *xif_fast_gets(int oid, int index, char *defaultString) {
	int length;
	xif_fast_prm.chars[0] = '\0';
	int ret = xif_gets(THIS_APP, oid, index, xif_fast_prm.chars, &length,
		sizeof(xif_fast_prm.chars));

	if (ret == XIF_RETURN_SUCCESS) {
		return xif_fast_prm.chars;
	} else {
		return defaultString;
	}
}

#endif //// __code_block_fastApi
#endif // _XIFCLT_H_
