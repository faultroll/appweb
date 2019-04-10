/**
 * \file: aftx_config.h
 * \brief: customized definitions for all apps in the aftx.
 */

#ifndef _AFTX_CONFIG_H_
#define _AFTX_CONFIG_H_


typedef enum TaftxApps {
	app_xsys = 0,
	app_pduc,
	app_xcmd,
	app_boa,
	app_xsnmp,
	app_xlcd,
	app_xmodbus,
	app_xtmp, // temporary

	/*
	 * not a enum, just for coding
	 */
	app_Count
} TaftxApps;


#endif // _AFTX_CONFIG_H_
