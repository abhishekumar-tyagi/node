// todo

'use strict';
const common = require('../common');
const fixtures = require('../common/fixtures');
const assert = require('assert');
const http2 = require('http2');
if (!common.hasCrypto)
  common.skip('missing crypto');

const serverOptions = {
  key: fixtures.readKey('agent1-key.pem'),
  cert: fixtures.readKey('agent1-cert.pem')
};
const server = http2.createSecureServer(
    serverOptions,
    common.mustCall((req, res) => {
const request=req
      assert(true===request.socket.encrypted)
      assert("encrypted" in request.socket)
res.end()
    }));
server.listen(common.mustCall(() => {
const port=server.address().port

const client = http2.connect('https://localhost:'+port, {
  ca:  fixtures.readKey('agent1-cert.pem'),
rejectUnauthorized: false
});
    const req = client.request({
     
      
      
    });
    req.on('response',common.mustCall( (headers, flags) => {
 console.log(headers)

server.close(common.mustCall(()=>{}))
}));
    req.end();
    
  }));
