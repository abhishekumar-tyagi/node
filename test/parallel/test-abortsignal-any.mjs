import * as common from '../common/index.mjs';
import { describe, it } from 'node:test';
import { setTimeout } from 'node:timers/promises';
import assert from 'node:assert';

describe('AbortSignal.any()', { concurrency: true }, () => {
  it('should throw when not receiving an array', () => {
    const regex = /The "signals" argument must be an instance of Array\. Received/;
    assert.throws(() => AbortSignal.any(), regex);
    assert.throws(() => AbortSignal.any(null), regex);
    assert.throws(() => AbortSignal.any(undefined), regex);
  });

  it('should throw when input contains non-signal values', () => {
    try {
      AbortSignal.any([AbortSignal.abort(), undefined]);
      assert.fail('AbortSignal.any() should not accept an input with non-signals');
    } catch (err) {
      assert.strictEqual(
        err.message,
        'The "signals[1]" argument must be an instance of AbortSignal. Received undefined'
      );
    }
  });

  it('creates a non-aborted signal for an empty input', () => {
    const signal = AbortSignal.any([]);
    assert.strictEqual(signal.aborted, false);
    signal.addEventListener('abort', common.mustNotCall());
  });

  it('returns a new signal', () => {
    const originalSignal = new AbortController().signal;
    const signalAny = AbortSignal.any([originalSignal]);
    assert.notStrictEqual(originalSignal, signalAny);
  });

  it('returns an aborted signal if input has an aborted signal', () => {
    const signal = AbortSignal.any([AbortSignal.abort('some reason')]);
    assert.strictEqual(signal.aborted, true);
    assert.strictEqual(signal.reason, 'some reason');
    signal.addEventListener('abort', common.mustNotCall());
  });

  it('returns an aborted signal with the reason of first aborted signal input', () => {
    const signal = AbortSignal.any([AbortSignal.abort('some reason'), AbortSignal.abort('another reason')]);
    assert.strictEqual(signal.aborted, true);
    assert.strictEqual(signal.reason, 'some reason');
    signal.addEventListener('abort', common.mustNotCall());
  });

  it('returns the correct signal in the event target', async () => {
    const signal = AbortSignal.any([AbortSignal.timeout(5)]);
    signal.addEventListener('abort', common.mustCall((e) => {
      assert.strictEqual(e.target, signal);
    }));
    await setTimeout(10);
    assert.ok(signal.aborted);
    assert.strictEqual(signal.reason.name, 'TimeoutError');
    assert.strictEqual(signal.reason.message, 'The operation was aborted due to timeout');
  });

  it('aborts with reason of first aborted signal', () => {
    const controllers = Array.from({ length: 3 }, () => new AbortController());
    const combinedSignal = AbortSignal.any(controllers.map((c) => c.signal));
    controllers[1].abort(1);
    controllers[2].abort(2);
    assert.ok(combinedSignal.aborted);
    assert.strictEqual(combinedSignal.reason, 1);
  });

  it('can accept the same signal more than once', () => {
    const controller = new AbortController();
    const signal = AbortSignal.any([controller.signal, controller.signal]);
    assert.strictEqual(signal.aborted, false);
    controller.abort('reason');
    assert.ok(signal.aborted);
    assert.strictEqual(signal.reason, 'reason');
  });

  it('handles deeply aborted signals', async () => {
    const controllers = Array.from({ length: 2 }, () => new AbortController());
    const composedSignal1 = AbortSignal.any([controllers[0].signal]);
    const composedSignal2 = AbortSignal.any([composedSignal1, controllers[1].signal]);

    composedSignal2.onabort = common.mustCall();
    controllers[0].abort();
    assert.ok(composedSignal2.aborted);
    assert.ok(composedSignal2.reason instanceof DOMException);
    assert.strictEqual(composedSignal2.reason.name, 'AbortError');
  });

  it('executes abort handlers in correct order', () => {
    const controller = new AbortController();
    const signals = [];
    signals.push(controller.signal);
    signals.push(AbortSignal.any([controller.signal]));
    signals.push(AbortSignal.any([controller.signal]));
    signals.push(AbortSignal.any([signals[0]]));
    signals.push(AbortSignal.any([signals[1]]));

    let result = '';
    signals.forEach((signal, i) => signal.addEventListener('abort', () => result += i));
    controller.abort();
    assert.strictEqual(result, '01234');
  });
});
