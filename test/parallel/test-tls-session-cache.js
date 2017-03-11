'use strict';
const common = require('../common');

if (!common.opensslCli) {
  common.skip('node compiled without OpenSSL CLI.');
  return;
}

if (!common.hasCrypto) {
  common.skip('missing crypto');
  return;
}

doTest({ tickets: false }, function() {
  doTest({ tickets: true }, function() {
    console.error('all done');
  });
});

function doTest(testOptions, callback) {
  const assert = require('assert');
  const tls = require('tls');
  const fs = require('fs');
  const join = require('path').join;
  const spawn = require('child_process').spawn;

  const keyFile = join(common.fixturesDir, 'agent.key');
  const certFile = join(common.fixturesDir, 'agent.crt');
  const key = fs.readFileSync(keyFile);
  const cert = fs.readFileSync(certFile);
  const options = {
    key: key,
    cert: cert,
    ca: [cert],
    requestCert: true
  };
  let requestCount = 0;
  let resumeCount = 0;
  let session;

  const server = tls.createServer(options, function(cleartext) {
    cleartext.on('error', function(er) {
      // We're ok with getting ECONNRESET in this test, but it's
      // timing-dependent, and thus unreliable. Any other errors
      // are just failures, though.
      if (er.code !== 'ECONNRESET')
        throw er;
    });
    ++requestCount;
    cleartext.end();
  });
  server.on('newSession', function(id, data, cb) {
    // Emulate asynchronous store
    setTimeout(function() {
      assert.ok(!session);
      session = {
        id: id,
        data: data
      };
      cb();
    }, 1000);
  });
  server.on('resumeSession', function(id, callback) {
    ++resumeCount;
    assert.ok(session);
    assert.equal(session.id.toString('hex'), id.toString('hex'));

    // Just to check that async really works there
    setTimeout(function() {
      callback(null, session.data);
    }, 100);
  });

  server.listen(0, function() {
    const args = [
      's_client',
      '-tls1',
      '-connect', `localhost:${this.address().port}`,
      '-servername', 'ohgod',
      '-key', join(common.fixturesDir, 'agent.key'),
      '-cert', join(common.fixturesDir, 'agent.crt'),
      '-reconnect'
    ].concat(testOptions.tickets ? [] : '-no_ticket');

    // for the performance and stability issue in s_client on Windows
    if (common.isWindows)
      args.push('-no_rand_screen');

    const client = spawn(common.opensslCli, args, {
      stdio: [ 0, 1, 'pipe' ]
    });
    client.stderr.setEncoding('utf8');
    client.on('exit', function(code) {
      console.error('done');
      assert.equal(code, 0);
      server.close(function() {
        setTimeout(callback, 100);
      });
    });
  });

  process.on('exit', function() {
    if (testOptions.tickets) {
      assert.equal(requestCount, 6);
      assert.equal(resumeCount, 0);
    } else {
      // initial request + reconnect requests (5 times)
      assert.ok(session);
      assert.equal(requestCount, 6);
      assert.equal(resumeCount, 5);
    }
  });
}
