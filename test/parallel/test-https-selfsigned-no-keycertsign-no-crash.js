'use strict';
const common = require('../common');

// This test starts an https server and tries
// to connect to it using a self-signed certificate.
// This certificate´s keyUsage does not include the keyCertSign
// bit, which used to crash node. The test ensures node
// will not crash. Key and certificate are from #37889.
// Note: This test assumes that the connection will succeed.

if (!common.hasCrypto)
  common.skip('missing crypto');

const crypto = require('crypto');

// This test will fail for OpenSSL < 1.1.1h
const minOpenSSL = 269488271;

if (crypto.constants.OPENSSL_VERSION_NUMBER < minOpenSSL)
  common.skip('OpenSSL < 1.1.1h');

const https = require('https');

const key = `-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEA0y3pMvD2AjG5xGBkGfyjBq8QgwB13ExOOQeJHuMZ9oGu+9VM
HzKawiLPD9ApMA2a7avcQ6p5H0e3bkpkUrE4oIMx3dooeG3fwcidcgXoJB7xzk/T
GxUpkBaR6WXyiEvTkSwWlysk4Mh0d9qi84Ou1fzDVZ7GoOxSg9B0STukhInYeGZf
Ol/tCZXTRTHwn7ri/alODz8L1WClKDETiheV8kSp1IgDMYNP5Vf3oZZdOHtCouOJ
D1P3pAdQkScHHRKvis72ZjaU5AbjTxC4ItH+eDYn6VjH7TdILwuruWu23gzToV+v
xNBGCkMCORTRiK1u7KwPleC0J4WrQWLe/XPvTQIDAQABAoIBAFIlWMIVE0z1NNLb
v/SP3oaaEK00v6QLFp5+fOtD4fSOq5eQeATmtWZxDeSTz4G+uRZctNipdmYhiovf
ajj0cReXEQ3Ab9+wtcp2lDAndg6e7uaXDIJLcBh5fxawLnCwNkMRSFRTVwwNTajV
pm9dOORKZ11l3tP4OXzG2IUoKy3Wj/1SKLL4zrdHi7802+L/GstK6/BGma+NFrFz
U6yNqpvuzv7BH7w9G3nSz7u+8SjcY22Vs6q69GAQG3yf356cYCJhV7QIJXU0/VAF
GFx5UDwlsOT2NhoOd/b9Q9RexKDl+qDupXQo0YFOObHIjHs8UGLOZkBtv4apCarA
6u+BOwECgYEA9GbrP/5SfmN8xvF2XVjqjk9IUcvWAuTM4Bxav72e6aR9IOdye9vi
+GhwM6qON+LOnMVNhUKJ0+R/jjLy6Jq+00uKU65Q79x7lCBVSDDXWacV0IFIoAOp
P4LkykjRZyzpIvjK5HGL1JYqZi89im93uuOiyMjoFS2syU+19b83UUECgYEA3TNk
JVGWYLMcD3uVTe2e/yZSsX+0+QL8hm3bUSOIJ/mIe2dqCXb6MK0ndMS0aCLGtDSt
wGTWwuc4rFattHYEI8Iro+tshgQs9bLM037hmiCrZvmcQsgt+3FNuYv4oCGp5U85
mWYF5SVUYRyv8M9aZoKTjc8meR0Wv3ZGGC9iDw0CgYA0XKyAPGO+MmB0Wx1J6Jfw
P2o2JB7I5e5DAbArrluSoSwx1YSApt6c6/tGBn+L16r+iYMPTu8ql6UAeUfzr9u8
d02+mfU7Ppi3Zqn+2n/49ERHNLuzlLU5JzkPYcSDf2q/lGAby3vy4u1YkTx1IWac
gtLIg8q9ZtjDFLHeYcZfQQKBgCCOpdjQT1/gPOsSd4FGzjYjv9wcPdjA1cY7eSJS
JoIruijfqb3G40Ay3DHVmfAR3kk7z68XqHx7Z94Fy/9Zt3ZD6ARybEC1cKChNoCS
lkYHNPMtHhC+QfZWUOhUb72x9r2nkYTAfXGisu6wOD0rZ9TatzkSGkmNPIHluJ9q
qfYpAoGAPJiBBdSt7DC9ZZraQGMEHfRkE5CxEIRbIHJ9+U3Z7LTQT6MJ1y3VfcGs
PetHcWtbU0Cl8blShaSwpxyCI01x3tUPw/b7tXMan/ImzjUgRe7kQXh2sf39V3b/
fvzKXWBvOvc1lgG0pFgI/2xtGQQGTe74MzX5xFgw6eadRUnJeKI=
-----END RSA PRIVATE KEY-----`;

const cert = `-----BEGIN CERTIFICATE-----
MIIC9jCCAd6gAwIBAgIJANHflGRpZM1IMA0GCSqGSIb3DQEBCwUAMBQxEjAQBgNV
BAMMCWxvY2FsaG9zdDAeFw0yMTAzMTUwOTEzMjdaFw0yMjAzMTUwOTEzMjdaMBQx
EjAQBgNVBAMMCWxvY2FsaG9zdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC
ggEBANMt6TLw9gIxucRgZBn8owavEIMAddxMTjkHiR7jGfaBrvvVTB8ymsIizw/Q
KTANmu2r3EOqeR9Ht25KZFKxOKCDMd3aKHht38HInXIF6CQe8c5P0xsVKZAWkell
8ohL05EsFpcrJODIdHfaovODrtX8w1WexqDsUoPQdEk7pISJ2HhmXzpf7QmV00Ux
8J+64v2pTg8/C9VgpSgxE4oXlfJEqdSIAzGDT+VX96GWXTh7QqLjiQ9T96QHUJEn
Bx0Sr4rO9mY2lOQG408QuCLR/ng2J+lYx+03SC8Lq7lrtt4M06Ffr8TQRgpDAjkU
0YitbuysD5XgtCeFq0Fi3v1z700CAwEAAaNLMEkwCwYDVR0PBAQDAgWgMBMGA1Ud
JQQMMAoGCCsGAQUFBwMBMCUGA1UdEQQeMByCCTEyNy4wLjAuMYIJbG9jYWxob3N0
hwR/AAABMA0GCSqGSIb3DQEBCwUAA4IBAQDAUCt/8Le2EO0ONOkQYUcPmSut6Siz
UIQrJ8Lwfs0fb+Zk9ElNGLwYTzooKDgzK8cLQ8g8F2WkolBEPXDsy1Ab+e66WkJH
NH/zAgEyG6cXXRNc+ObM5KbjY0YuDGiajKcndknuuCB+onlC1Pv5oFUSNa3/06+S
sziFloGbg5S0AHT6lYnwZSM6G7Pre8mcRNRxL6Yw1FOOUpQZKPd7juy4GBRlCucn
wmp/Fl0wIBDs91Vprig2TO+U6GvtqJ3n/RKXUz1ykUKETtRneSkqa6hFYjwRzawd
ANpjy/orrVkqXriAbI/1xvBMInWdcMpXNeiOkxQeQdy8TLBk0ZViSJnf
-----END CERTIFICATE-----`;

const serverOptions = {
  key: key,
  cert: cert
};

// Start the server
const httpsServer = https.createServer(serverOptions, (req, res) => {
  res.writeHead(200);
  res.end('hello world\n');
});
httpsServer.listen(0);

httpsServer.on('listening', () => {
  // Once the server started listening, built the client config
  // with the server´s used port
  const clientOptions = {
    hostname: '127.0.0.1',
    port: httpsServer.address().port,
    ca: cert
  };
  // Try to connect
  const req = https.request(clientOptions, common.mustCall((res) => {
    httpsServer.close();
  }));

  req.on('error', common.mustNotCall());
  req.end();
});
