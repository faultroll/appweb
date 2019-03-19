/*
    status.tst - Test CGI program status responses
 */

const HTTP = tget('TM_HTTP') || "127.0.0.1:4100"
load("cgi.es")

let http = new Http
http.setHeader("SWITCHES", "-s%20711")
http.get(HTTP + "/cgi-bin/cgiProgram")
ttrue(http.status == 711)
http.close()
