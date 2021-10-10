// Copyright 2021 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Flags: --harmony-temporal

let bigint_nano = 567890123456789000000n;
let bigint_milli = 567890123456789n;
let inst1 = new Temporal.Instant(bigint_nano);
let inst2 = Temporal.Instant.fromEpochMilliseconds(bigint_milli);
assertEquals(inst1, inst2);

let just_fit_neg_bigint = -8640000000000000n;
let just_fit_pos_bigint = 8640000000000000n;
let too_big_bigint = 8640000000000001n;
let too_small_bigint = -8640000000000001n;

assertThrows(() =>
    {let inst = Temporal.Instant.fromEpochMilliseconds(too_small_bigint)},
    RangeError);
assertThrows(() =>
    {let inst = Temporal.Instant.fromEpochMilliseconds(too_big_bigint)},
    RangeError);
assertEquals(just_fit_neg_bigint,
    (Temporal.Instant.fromEpochMilliseconds(
        just_fit_neg_bigint)).epochMilliseconds);
assertEquals(just_fit_pos_bigint,
    (Temporal.Instant.fromEpochMilliseconds(
        just_fit_pos_bigint)).epochMilliseconds);
