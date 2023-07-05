// Copyright 2019 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Throws only once during construction.
// Check for all getters to prevent regression.
// Preserve the order of getter initialization.
let getCount = 0;
let weekday = new Array();
let year = new Array();
let month = new Array();
let day = new Array();
let hour = new Array();
let minute = new Array();
let second = new Array();
let localeMatcher = new Array();
let hour12 = new Array();
let hourCycle = new Array();
let dateStyle = new Array();
let timeStyle = new Array();
let timeZone = new Array();
let era = new Array();
let timeZoneName = new Array();
let formatMatcher = new Array();

new Intl.DateTimeFormat(['en-US'], {
  get weekday() {
    weekday.push(++getCount);
  },
  get year() {
    year.push(++getCount);
  },
  get month() {
    month.push(++getCount);
  },
  get day() {
    day.push(++getCount);
  },
  get hour() {
    hour.push(++getCount);
  },
  get minute() {
    minute.push(++getCount);
  },
  get second() {
    second.push(++getCount);
  },
  get localeMatcher() {
    localeMatcher.push(++getCount);
  },
  get hour12() {
    hour12.push(++getCount);
  },
  get hourCycle() {
    hourCycle.push(++getCount);
  },
  get timeZone() {
    timeZone.push(++getCount);
  },
  get dateStyle() {
    dateStyle.push(++getCount);
  },
  get timeStyle() {
    timeStyle.push(++getCount);
  },
  get era() {
    era.push(++getCount);
  },
  get timeZoneName() {
    timeZoneName.push(++getCount);
  },
  get formatMatcher() {
    formatMatcher.push(++getCount);
  }
});

// Read by ToDateTimeOptions and also in Table 1
// https://tc39.es/proposal-intl-datetime-style/#table-datetimeformat-components
assertEquals(2, weekday.length);
assertEquals(2, hour.length);
assertEquals(2, minute.length);
assertEquals(2, second.length);

// Read by ToDateTimeOptions and also overwrite with a default 'numeric'
// https://tc39.es/proposal-intl-datetime-style/#table-datetimeformat-components
assertEquals(1, year.length);
assertEquals(1, month.length);
assertEquals(1, day.length);

// In Table 1
// https://tc39.es/proposal-intl-datetime-style/#table-datetimeformat-components
assertEquals(1, era.length);
assertEquals(1, timeZoneName.length);

// Read by ToDateTimeOptions and InitializeDateTimeFormat
assertEquals(2, dateStyle.length);
assertEquals(2, timeStyle.length);

// Only read by InitializeDateTimeFormat
assertEquals(1, localeMatcher.length);
assertEquals(1, hour12.length);
assertEquals(1, hourCycle.length);
assertEquals(1, timeZone.length);
assertEquals(1, formatMatcher.length);

// ToDateTimeOptions
assertEquals(1, weekday[0]);
assertEquals(2, year[0]);
assertEquals(3, month[0]);
assertEquals(4, day[0]);
assertEquals(5, hour[0]);
assertEquals(6, minute[0]);
assertEquals(7, second[0]);
assertEquals(8, dateStyle[0]);
assertEquals(9, timeStyle[0]);

// InitializeDateTimeFormat
assertEquals(10, localeMatcher[0]);
assertEquals(11, hour12[0]);
assertEquals(12, hourCycle[0]);
assertEquals(13, timeZone[0]);

// Table 1 loop in InitializeDateTimeFormat
assertEquals(14, weekday[1]);
assertEquals(15, era[0]);
assertEquals(16, hour[1]);
assertEquals(17, minute[1]);
assertEquals(18, second[1]);
assertEquals(19, timeZoneName[0]);

// After the Table 1 loop in InitializeDateTimeFormat
assertEquals(20, formatMatcher[0]);
assertEquals(21, dateStyle[1]);
assertEquals(22, timeStyle[1]);

assertEquals(22, getCount);
