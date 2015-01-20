var common = require('../common');

if (!common.opensslCli) {
  console.error('Skipping because node compiled without OpenSSL CLI.');
  process.exit(0);
}

var assert = require('assert');
var exec = require('child_process').exec;
var tls = require('tls');
var fs = require('fs');

var options = {
  key: fs.readFileSync(common.fixturesDir + '/keys/agent2-key.pem'),
  cert: fs.readFileSync(common.fixturesDir + '/keys/agent2-cert.pem'),
  ciphers: 'ECDHE-RSA-RC4-SHA',
  ecdhCurve: false
};

var nconns = 0;

process.on('exit', function() {
  assert.equal(nconns, 0);
});

var server = tls.createServer(options, function(conn) {
  conn.end();
  nconns++;
});

server.listen(common.PORT, '127.0.0.1', function() {
  var cmd = common.opensslCli + ' s_client -cipher ' + options.ciphers +
            ' -connect 127.0.0.1:' + common.PORT;

  exec(cmd, function(err, stdout, stderr) {
    // Old versions of openssl will still exit with 0 so we
    // can't just check if err is not null.
    assert.notEqual(stderr.indexOf('handshake failure'), -1);
    server.close();
  });
});
