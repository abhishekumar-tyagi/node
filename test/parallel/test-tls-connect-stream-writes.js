var fs = require('fs'),
    path = require('path'),
    tls = require('tls'),
    stream = require('stream'),
    net = require('net');

var common = require('../common');

var server;
var cert_dir = path.resolve(__dirname, '../fixtures'),
    options = { key: fs.readFileSync(cert_dir + '/test_key.pem'),
                cert: fs.readFileSync(cert_dir + '/test_cert.pem'),
                ca: [ fs.readFileSync(cert_dir + '/test_ca.pem') ],
                ciphers: 'AES256-GCM-SHA384' };

server = tls.createServer(options);
server.listen(common.PORT, function() {
  var raw = net.connect(common.PORT);

  var pending = false;
  raw.on('readable', function() {
    if (pending)
      p._read();
  });

  var p = new stream.Duplex({
    read: function read() {
      pending = false;

      var chunk = raw.read();
      if (chunk) {
        this.push(chunk);
      } else {
        pending = true;
      }
    },
    write: function write(data, enc, cb) {
      raw.write(data, enc, cb);
    }
  });

  var socket = tls.connect({
    socket: p,
    rejectUnauthorized: false
  }, function() {
    for (var i = 0; i < 50; ++i)
      socket.write('hello world');
    socket.end();
  });
});
