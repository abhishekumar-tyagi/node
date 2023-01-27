'use strict';

const common = require('../common');
const assert = require('assert');
const { ReadableStream, WritableStream } = require('stream/web');
const { finished } = require('stream');
const { finished: finishedPromise } = require('stream/promises');

{
  const rs = new ReadableStream({
    start(controller) {
      controller.enqueue('asd');
      controller.close();
    },
  });
  finished(rs, common.mustSucceed());
  async function test() {
    const values = [];
    for await (const chunk of rs) {
      values.push(chunk);
    }
    assert.deepStrictEqual(values, ['asd']);
  }
  test();
}

{
  const rs = new ReadableStream({
    start(controller) {
      controller.error(new Error('asd'));
    }
  });

  finished(rs, common.mustCall((err) => {
    assert.strictEqual(err?.message, 'asd');
  }));
}

{
  const rs = new ReadableStream({
    async start(controller) {
      throw new Error('asd');
    }
  });

  finished(rs, common.mustCall((err) => {
    assert.strictEqual(err?.message, 'asd');
  }));
}

{
  const rs = new ReadableStream({
    start(controller) {
      controller.enqueue('asd');
      controller.close();
    }
  });

  async function test() {
    const values = [];
    for await (const chunk of rs) {
      values.push(chunk);
    }
    assert.deepStrictEqual(values, ['asd']);
  }

  finishedPromise(rs).then(common.mustSucceed());

  test();
}

{
  const rs = new ReadableStream({
    start(controller) {
      controller.error(new Error('asd'));
    }
  });

  finishedPromise(rs).then(common.mustNotCall()).catch(common.mustCall((err) => {
    assert.strictEqual(err?.message, 'asd');
  }));
}

{
  const rs = new ReadableStream({
    async start(controller) {
      throw new Error('asd');
    }
  });

  finishedPromise(rs).then(common.mustNotCall()).catch(common.mustCall((err) => {
    assert.strictEqual(err?.message, 'asd');
  }));
}

{
  const rs = new ReadableStream({
    start(controller) {
      controller.enqueue('asd');
      controller.close();
    }
  });

  const { 0: s1, 1: s2 } = rs.tee();

  finished(s1, common.mustSucceed());
  finished(s2, common.mustSucceed());

  async function test(stream) {
    const values = [];
    for await (const chunk of stream) {
      values.push(chunk);
    }
    assert.deepStrictEqual(values, ['asd']);
  }

  Promise.all([
    test(s1),
    test(s2),
  ]).then(common.mustCall());
}

{
  const rs = new ReadableStream({
    start(controller) {
      controller.error(new Error('asd'));
    }
  });

  const { 0: s1, 1: s2 } = rs.tee();

  finished(s1, common.mustCall((err) => {
    assert.strictEqual(err?.message, 'asd');
  }));

  finished(s2, common.mustCall((err) => {
    assert.strictEqual(err?.message, 'asd');
  }));
}

{
  const rs = new ReadableStream({
    start(controller) {
      controller.enqueue('asd');
      controller.close();
    }
  });

  finished(rs, common.mustSucceed());

  rs.cancel();
}

{
  let str = '';
  const ws = new WritableStream({
    write(chunk) {
      str += chunk;
    }
  });

  finished(ws, common.mustSucceed(() => {
    assert.strictEqual(str, 'asd');
  }));

  const writer = ws.getWriter();
  writer.write('asd');
  writer.close();
}

{
  const ws = new WritableStream({
    async write(chunk) {
      throw new Error('asd');
    }
  });

  finished(ws, common.mustCall((err) => {
    assert.strictEqual(err?.message, 'asd');
  }));

  const writer = ws.getWriter();
  writer.write('asd').catch((err) => {
    assert.strictEqual(err?.message, 'asd');
  });
}

{
  let str = '';
  const ws = new WritableStream({
    write(chunk) {
      str += chunk;
    }
  });

  finishedPromise(ws).then(common.mustSucceed(() => {
    assert.strictEqual(str, 'asd');
  }));

  const writer = ws.getWriter();
  writer.write('asd');
  writer.close();
}

{
  const ws = new WritableStream({
    write(chunk) { }
  });
  finished(ws, common.mustCall((err) => {
    assert.strictEqual(err?.message, 'asd');
  }));

  const writer = ws.getWriter();
  writer.abort(new Error('asd'));
}

{
  const ws = new WritableStream({
    async write(chunk) {
      throw new Error('asd');
    }
  });

  finishedPromise(ws).then(common.mustNotCall()).catch(common.mustCall((err) => {
    assert.strictEqual(err?.message, 'asd');
  }));

  const writer = ws.getWriter();
  writer.write('asd').catch((err) => {
    assert.strictEqual(err?.message, 'asd');
  });
}
