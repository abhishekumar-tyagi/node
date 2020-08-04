'use strict';

require('../common');

const { BlockList } = require('net');
const assert = require('assert');

{
  const blockList = new BlockList();

  [1, [], {}, null, 1n, undefined, null].forEach((i) => {
    assert.throws(() => blockList.addAddress(i), {
      code: 'ERR_INVALID_ARG_TYPE'
    });
  });

  [1, [], {}, null, 1n, null].forEach((i) => {
    assert.throws(() => blockList.addAddress('1.1.1.1', i), {
      code: 'ERR_INVALID_ARG_TYPE'
    });
  });

  assert.throws(() => blockList.addAddress('1.1.1.1', 'foo'), {
    code: 'ERR_INVALID_ARG_VALUE'
  });

  [1, [], {}, null, 1n, undefined, null].forEach((i) => {
    assert.throws(() => blockList.addRange(i), {
      code: 'ERR_INVALID_ARG_TYPE'
    });
    assert.throws(() => blockList.addRange('1.1.1.1', i), {
      code: 'ERR_INVALID_ARG_TYPE'
    });
  });

  [1, [], {}, null, 1n, null].forEach((i) => {
    assert.throws(() => blockList.addRange('1.1.1.1', '1.1.1.2', i), {
      code: 'ERR_INVALID_ARG_TYPE'
    });
  });

  assert.throws(() => blockList.addRange('1.1.1.1', '1.1.1.2', 'foo'), {
    code: 'ERR_INVALID_ARG_VALUE'
  });
}

{
  const blockList = new BlockList();
  blockList.addAddress('1.1.1.1');
  blockList.addAddress('8592:757c:efae:4e45:fb5d:d62a:0d00:8e17', 'ipv6');
  blockList.addAddress('::ffff:1.1.1.2', 'ipv6');

  assert(blockList.check('1.1.1.1'));
  assert(!blockList.check('1.1.1.1', 'ipv6'));
  assert(!blockList.check('8592:757c:efae:4e45:fb5d:d62a:0d00:8e17'));
  assert(blockList.check('8592:757c:efae:4e45:fb5d:d62a:0d00:8e17', 'ipv6'));

  assert(blockList.check('::ffff:1.1.1.1', 'ipv6'));

  assert(blockList.check('1.1.1.2'));

  assert(!blockList.check('1.2.3.4'));
  assert(!blockList.check('::1', 'ipv6'));
}

{
  const blockList = new BlockList();
  blockList.addRange('1.1.1.1', '1.1.1.10');
  blockList.addRange('::1', '::f', 'ipv6');

  assert(!blockList.check('1.1.1.0'));
  for (let n = 1; n <= 10; n++)
    assert(blockList.check(`1.1.1.${n}`));
  assert(!blockList.check('1.1.1.11'));

  assert(!blockList.check('::0', 'ipv6'));
  for (let n = 0x1; n <= 0xf; n++) {
    assert(blockList.check(`::${n.toString(16)}`, 'ipv6'),
           `::${n.toString(16)} check failed`);
  }
  assert(!blockList.check('::10', 'ipv6'));
}

{
  const blockList = new BlockList();
  blockList.addSubnet('1.1.1.0', 16);
  blockList.addSubnet('8592:757c:efae:4e45::', 64, 'ipv6');

  assert(blockList.check('1.1.0.1'));
  assert(blockList.check('1.1.1.1'));
  assert(!blockList.check('1.2.0.1'));
  assert(blockList.check('::ffff:1.1.0.1', 'ipv6'));

  assert(blockList.check('8592:757c:efae:4e45:f::', 'ipv6'));
  assert(blockList.check('8592:757c:efae:4e45::f', 'ipv6'));
  assert(!blockList.check('8592:757c:efae:4f45::f', 'ipv6'));
}

{
  const blockList = new BlockList();
  blockList.addAddress('1.1.1.1');
  blockList.addRange('10.0.0.1', '10.0.0.10');
  blockList.addSubnet('8592:757c:efae:4e45::', 64, 'ipv6');

  const rulesCheck = [
    'Subnet: IPv6 8592:757c:efae:4e45::/64',
    'Range: IPv4 10.0.0.1-10.0.0.10',
    'Address: IPv4 1.1.1.1'
  ];
  assert.deepStrictEqual(blockList.rules, rulesCheck);
  console.log(blockList);

  assert(blockList.check('1.1.1.1'));
  assert(blockList.check('10.0.0.5'));
  assert(blockList.check('::ffff:10.0.0.5', 'ipv6'));
  assert(blockList.check('8592:757c:efae:4e45::f', 'ipv6'));

  assert(!blockList.check('123.123.123.123'));
  assert(!blockList.check('8592:757c:efaf:4e45:fb5d:d62a:0d00:8e17', 'ipv6'));
  assert(!blockList.check('::ffff:123.123.123.123', 'ipv6'));
}

{
  // This test validates boundaries of non-aligned CIDR bit prefixes
  const blockList = new BlockList();
  blockList.addSubnet('10.0.0.0', 27);
  blockList.addSubnet('8592:757c:efaf::', 51, 'ipv6');

  for (let n = 0; n <= 31; n++)
    assert(blockList.check(`10.0.0.${n}`));
  assert(!blockList.check('10.0.0.32'));

  assert(blockList.check('8592:757c:efaf:0:0:0:0:0', 'ipv6'));
  assert(blockList.check('8592:757c:efaf:1fff:ffff:ffff:ffff:ffff', 'ipv6'));
  assert(!blockList.check('8592:757c:efaf:2fff:ffff:ffff:ffff:ffff', 'ipv6'));
}
