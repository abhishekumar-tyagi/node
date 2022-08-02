import assert from 'node:assert';

function random() {
  return `Random Value: ${Math.random()}`;
}
function transform(value) {
  return value.replaceAll(/Random Value: \d+\.+\d+/g, 'Random Value: *');
}

await assert.snapshot(transform(random()));
await assert.snapshot(transform(random()));
