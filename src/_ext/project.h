#ifndef _PROJECT_H_
#define _PROJECT_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "projectdefs.h"

#define sysPerror(x)

// The name of this app
#define THIS_APP_NAME "boa"
// The index number of this app
#define THIS_APP app_boa
#define THIS_APP_NUMBER THIS_APP

#define pduc_Oid_webRequest 2
#define pduc_Oid_webPost 3 // see the xsys program.

#ifndef DEFAULT_STRING_LENGTH
#define DEFAULT_STRING_LENGTH (32)
#endif

#include "aftx_config.h"
#include "tick.h"
#include "xif.h"
#include "xifclt.h"



#endif // _PROJECT_H_


