/*
    matrixssl.tst - Test MatrixSSL
 */

if (!Config.SSL) {
    tskip("ssl not enabled in ejs")

} else if (thas('ME_MATRIXSSL')) {
    let http: Http = new Http
    let bin = Path(App.getenv('TM_BIN'))

    http.retries = 0
    http.ca = bin.join('ca.crt')
    ttrue(http.verify == true)
 
    //  Verify the server cert and send a client cert 
    endpoint = tget('TM_MATRIXSSL') || "https://127.0.0.1:8443"
    http.key = bin.join('test.key')
    http.certificate = bin.join('test.crt')
    http.get(endpoint + '/index.html')
    ttrue(http.status == 200) 
    ttrue(http.info.CLIENT_S_CN == 'localhost')
    ttrue(http.info.SERVER_S_CN == 'localhost')
    ttrue(http.info.SERVER_I_OU != http.info.SERVER_S_OU)
    ttrue(http.info.SERVER_I_EMAIL == 'licensing@example.com')
    http.close()

} else {
    tskip("matrixssl not enabled")
}
