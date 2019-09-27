#include "project.h"
#include "appweb.h"

const char   mainFrameDoctype[] =
	"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\n""<html xmlns=\"http://www.w3.org/1999/xhtml\">";

const char   refresh[] =
			"<html xmlns=\"http://www.w3.org/1999/xhtml\"><head>		<meta http-equiv=\"refresh\" content=\"0\"/><title>Showing Result...</title></head>	<body>	<p>Showing Result...</p>	</body></html>";
	
char* escapeNumberSign(char *buf) {
	char   esc[256*3];
	char   *p0, *p1;
	
	memset(esc, 0, sizeof(esc));
	for (p0 = buf, p1 = esc; *p0; p0++, p1++) {
		*p1 = *p0;
		if (*p0 == '%') {
			++p1;
			*p1 = *p0;
		}
	}
	return esc;
}				
				
bool action_output_file(char *filepath, HttpQueue *q) {
	FILE   *fd;
	char   buf[256/2];

	fd = fopen(filepath, "r");
	if (!fd) {
		fd = fopen("/xprod/xweb/xfs/error.html", "r");
		if (!fd)   return 0;
	}
	memset(buf, 0, sizeof(buf));
	sleep(1);
	int size;
	while ((size = fread(buf, 1, sizeof(buf)-1, fd)) > 0) {
		char * test = escapeNumberSign(buf);
		mprLog("error appweb", 0, "\nsize = %d\n%s", size, test);
		httpWrite(q, test);
		memset(buf, 0, sizeof(buf));
	}
	fclose(fd);
	return 1;
}

PUBLIC void action_get(HttpConn *conn)
{
	HttpQueue   *q = conn->writeq;
	char   prm[128];
	char   *page = NULL;
	
	page = httpGetParam(conn, "p", NULL);
	if (page !=  NULL) 
		sprintf(prm, "username=%s,/%s",conn->username, page);
	else
		return;
	int ret = xif_sets(app_pduc, pduc_Oid_webRequest, 0, prm);
	if (ret == 0) {
		httpSetStatus(conn, 200);
		httpWrite(q, mainFrameDoctype);
		/*
			Add desired headers. "Set" will overwrite, add will create if not already defined.
		 */		
		httpAddHeaderString(conn, "Content-Type", "text/html");
		httpSetHeaderString(conn, "Cache-Control", "no-cache");
		if (!action_output_file("/tmp/boa/result.html", q)) {
			httpWrite(q, "file loss");
		}
	   /*
			Call finalize output and close the request.
			Delay closing if you want to do asynchronous output and close later.
		 */			
		httpFinalize(conn);
	} 
}

PUBLIC void action_post(HttpConn *conn)
{
	HttpQueue   *q = conn->writeq;
	char   decodedChars[1024];
	char   *str = NULL;
	
	str = httpGetParamsString(conn);
	memset(decodedChars, 0, sizeof(decodedChars));
	if (strlen(str) < 1024) {
		strcpy(decodedChars, str);
	}
	else {
		mprLog("error appweb", 0, "post data bytes >= 1024");
		return;
	}
	
	char *p = decodedChars;
	char *d = decodedChars;
	while (*p) {
		uchar c;
		if (*p == '%') {
			uchar i;
			// 2nd char
			p++;
			i = *p;
			if (i == '\0')
				break;

			if ((i >= '0') && (i <= '9')) {
				i = i - '0';
			} else if ((i <= 'F') && (i >= 'A')) {
				i = i - 'A' + 10;
			} else
				continue;
			c = i;

			// 3th char
			p++;
			i = *p;
			if (i == '\0')
				break;

			if ((i >= '0') && (i <= '9')) {
				i = i - '0';
			} else if ((i <= 'F') && (i >= 'A')) {
				i = i - 'A' + 10;
			} else
				continue; // while
			c = c * 16 + i;
		} else if (*p == '+') {
			c = ' ';
		} else if (*p == '=') { // replace '=' with '\001'
			c = '\001';
		} else if (*p == '&') { // replace '&' with '\002'
			c = '\002';
		} else
			c = *p;
		*d++ = c;
		p++;
	}
	sprintf(d, "\002#loginName\001");
	d += strlen("\002#loginName\001");
	if (conn->username) {
		sprintf(d, conn->username);
	}
	else {
		mprLog("error appweb", 0, "conn->username is NULL");
		return;
	}
	d += strlen(conn->username);
	sprintf(d, "\002");
	mprLog("error appweb", 0, "\n***\n%s\n---\n", decodedChars);

	int ret = xif_sets(app_pduc, pduc_Oid_webPost, 0, decodedChars);
	if (ret == 1) {

		httpSetStatus(conn, 200);
		// httpWrite(q, mainFrameDoctype);
		/*
			Add desired headers. "Set" will overwrite, add will create if not already defined.
		 */		
		httpAddHeaderString(conn, "Content-Type", "text/html");
		 httpSetHeaderString(conn, "Cache-Control", "no-cache");
		httpWrite(q, refresh);

		// if (!action_output_file("/tmp/boa/result.html", q)) {
			// httpWrite(q, "file loss");
		// }
	   /*
			Call finalize output and close the request.
			Delay closing if you want to do asynchronous output and close later.
		 */			
		httpFinalize(conn);
	
	} 
}