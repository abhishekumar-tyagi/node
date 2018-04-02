'use strict';

require('../common');
const assert = require('assert');

// Test OOB
{
  const buffer = Buffer.alloc(4);

  ['Int8', 'Int16BE', 'Int16LE', 'Int32BE', 'Int32LE'].forEach((fn) => {

    // Verify that default offset works fine.
    buffer[`read${fn}`](undefined);
    buffer[`read${fn}`]();

    ['', '0', null, {}, [], () => {}, true, false].forEach((o) => {
      assert.throws(
        () => buffer[`read${fn}`](o),
        {
          code: 'ERR_INVALID_ARG_TYPE',
          name: 'TypeError [ERR_INVALID_ARG_TYPE]'
        });
    });

    [Infinity, -1, -4294967295].forEach((offset) => {
      assert.throws(
        () => buffer[`read${fn}`](offset),
        {
          code: 'ERR_OUT_OF_RANGE',
          name: 'RangeError [ERR_OUT_OF_RANGE]'
        });
    });

    [NaN, 1.01].forEach((offset) => {
      assert.throws(
        () => buffer[`read${fn}`](offset),
        {
          code: 'ERR_OUT_OF_RANGE',
          name: 'RangeError [ERR_OUT_OF_RANGE]',
          message: 'The value of "offset" is out of range. ' +
                   `It must be an integer. Received ${offset}`
        });
    });
  });
}

// Test 8 bit signed integers
{
  const data = Buffer.alloc(4);

  data[0] = 0x23;
  assert.strictEqual(data.readInt8(0), 0x23);

  data[0] = 0xff;
  assert.strictEqual(data.readInt8(0), -1);

  data[0] = 0x87;
  data[1] = 0xab;
  data[2] = 0x7c;
  data[3] = 0xef;
  assert.strictEqual(data.readInt8(0), -121);
  assert.strictEqual(data.readInt8(1), -85);
  assert.strictEqual(data.readInt8(2), 124);
  assert.strictEqual(data.readInt8(3), -17);
}

// Test 16 bit integers
{
  const buffer = Buffer.alloc(6);

  buffer[0] = 0x16;
  buffer[1] = 0x79;
  assert.strictEqual(buffer.readInt16BE(0), 0x1679);
  assert.strictEqual(buffer.readInt16LE(0), 0x7916);

  buffer[0] = 0xff;
  buffer[1] = 0x80;
  assert.strictEqual(buffer.readInt16BE(0), -128);
  assert.strictEqual(buffer.readInt16LE(0), -32513);

  buffer[0] = 0x77;
  buffer[1] = 0x65;
  buffer[2] = 0x65;
  buffer[3] = 0x6e;
  buffer[4] = 0x69;
  buffer[5] = 0x78;
  assert.strictEqual(buffer.readInt16BE(0), 0x7765);
  assert.strictEqual(buffer.readInt16BE(1), 0x6565);
  assert.strictEqual(buffer.readInt16BE(2), 0x656e);
  assert.strictEqual(buffer.readInt16BE(3), 0x6e69);
  assert.strictEqual(buffer.readInt16BE(4), 0x6978);
  assert.strictEqual(buffer.readInt16LE(0), 0x6577);
  assert.strictEqual(buffer.readInt16LE(1), 0x6565);
  assert.strictEqual(buffer.readInt16LE(2), 0x6e65);
  assert.strictEqual(buffer.readInt16LE(3), 0x696e);
  assert.strictEqual(buffer.readInt16LE(4), 0x7869);
}

// Test 32 bit integers
{
  const buffer = Buffer.alloc(6);

  buffer[0] = 0x43;
  buffer[1] = 0x53;
  buffer[2] = 0x16;
  buffer[3] = 0x79;
  assert.strictEqual(buffer.readInt32BE(0), 0x43531679);
  assert.strictEqual(buffer.readInt32LE(0), 0x79165343);

  buffer[0] = 0xff;
  buffer[1] = 0xfe;
  buffer[2] = 0xef;
  buffer[3] = 0xfa;
  assert.strictEqual(buffer.readInt32BE(0), -69638);
  assert.strictEqual(buffer.readInt32LE(0), -84934913);

  buffer[0] = 0x42;
  buffer[1] = 0xc3;
  buffer[2] = 0x95;
  buffer[3] = 0xa9;
  buffer[4] = 0x36;
  buffer[5] = 0x17;
  assert.strictEqual(buffer.readInt32BE(0), 0x42c395a9);
  assert.strictEqual(buffer.readInt32BE(1), -1013601994);
  assert.strictEqual(buffer.readInt32BE(2), -1784072681);
  assert.strictEqual(buffer.readInt32LE(0), -1449802942);
  assert.strictEqual(buffer.readInt32LE(1), 917083587);
  assert.strictEqual(buffer.readInt32LE(2), 389458325);
}

// Test Int
{
  const buffer = Buffer.alloc(8);

  // Check byteLength.
  ['readIntBE', 'readIntLE'].forEach((fn) => {
    ['', '0', null, undefined, {}, [], () => {}, true, false].forEach((len) => {
      assert.throws(
        () => buffer[fn](0, len),
        { code: 'ERR_INVALID_ARG_TYPE' });
    });

    [Infinity, -1].forEach((byteLength) => {
      assert.throws(
        () => buffer[fn](0, byteLength),
        {
          code: 'ERR_OUT_OF_RANGE',
          message: 'The value of "byteLength" is out of range. ' +
                   `It must be >= 1 and <= 6. Received ${byteLength}`
        });
    });

    [NaN, 1.01].forEach((byteLength) => {
      assert.throws(
        () => buffer[fn](0, byteLength),
        {
          code: 'ERR_OUT_OF_RANGE',
          name: 'RangeError [ERR_OUT_OF_RANGE]',
          message: 'The value of "byteLength" is out of range. ' +
                   `It must be an integer. Received ${byteLength}`
        });
    });
  });

  // Test 1 to 6 bytes.
  for (let i = 1; i < 6; i++) {
    ['readIntBE', 'readIntLE'].forEach((fn) => {
      ['', '0', null, undefined, {}, [], () => {}, true, false].forEach((o) => {
        assert.throws(
          () => buffer[fn](o, i),
          {
            code: 'ERR_INVALID_ARG_TYPE',
            name: 'TypeError [ERR_INVALID_ARG_TYPE]'
          });
      });

      [Infinity, -1, -4294967295].forEach((offset) => {
        assert.throws(
          () => buffer[fn](offset, i),
          {
            code: 'ERR_OUT_OF_RANGE',
            name: 'RangeError [ERR_OUT_OF_RANGE]',
            message: 'The value of "offset" is out of range. ' +
                     `It must be >= 0 and <= ${8 - i}. Received ${offset}`
          });
      });

      [NaN, 1.01].forEach((offset) => {
        assert.throws(
          () => buffer[fn](offset, i),
          {
            code: 'ERR_OUT_OF_RANGE',
            name: 'RangeError [ERR_OUT_OF_RANGE]',
            message: 'The value of "offset" is out of range. ' +
                     `It must be an integer. Received ${offset}`
          });
      });
    });
  }
}
