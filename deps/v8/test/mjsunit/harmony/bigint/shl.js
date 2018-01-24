// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Generated by tools/bigint-tester.py.

// Flags: --harmony-bigint

var data = [{
  a: "-9a6d035348727045f6abf7d59056d30e9ce885e87f5f8438347bfcda0a1f9b",
  b: "-2",
  r: "-269b40d4d21c9c117daafdf56415b4c3a73a217a1fd7e10e0d1eff368287e7"
}, {
  a: "615f9676062ea7a1b89396ce4208712f279475490829",
  b: "ff",
  r: "30afcb3b031753d0dc49cb672104389793ca3aa484148000000000000000000000000000000000000000000000000000000000000000"
}, {
  a: "-9b6131d8b806543fce32b4c2ca2038ffa956929848a61b5eb7f",
  b: "-e7",
  r: "-1"
}, {
  a: "-331d9e",
  b: "0",
  r: "-331d9e"
}, {
  a: "cb79696d3a6f5d5d034e9d2",
  b: "-d33",
  r: "0"
}, {
  a: "ca99",
  b: "10",
  r: "ca990000"
}, {
  a: "6f97833d5",
  b: "0",
  r: "6f97833d5"
}, {
  a: "67d36e7948d18af35f0823c0d58ba47ca0846cdfaa7a7407f09d44747275532681b343",
  b: "f",
  r: "33e9b73ca468c579af8411e06ac5d23e5042366fd53d3a03f84ea23a393aa99340d9a18000"
}, {
  a: "f4896",
  b: "-7",
  r: "1e91"
}, {
  a: "996ce2a9e0f7d65e0523204c9c469bfd14821efe571ac59cdc01",
  b: "1d",
  r: "132d9c553c1efacbc0a464099388d37fa29043dfcae358b39b8020000000"
}, {
  a: "-f8f",
  b: "f1",
  r: "-1f1e000000000000000000000000000000000000000000000000000000000000"
}, {
  a: "-b685bbcd953ba9c5973ae523dc81d7b35e0cf2b9b51026d4ba1ac21bd5c3c18f9c13",
  b: "0",
  r: "-b685bbcd953ba9c5973ae523dc81d7b35e0cf2b9b51026d4ba1ac21bd5c3c18f9c13"
}, {
  a: "e2295b362b7048fb163d1272178ed441517fc689e5ec5ea40f29",
  b: "-30",
  r: "e2295b362b7048fb163d1272178ed441517fc689"
}, {
  a: "-b322e816b014448f44e60b418582390d2a3ad95",
  b: "0",
  r: "-b322e816b014448f44e60b418582390d2a3ad95"
}, {
  a: "4c135e4d7",
  b: "0",
  r: "4c135e4d7"
}, {
  a: "-d5b694",
  b: "f1",
  r: "-1ab6d28000000000000000000000000000000000000000000000000000000000000"
}, {
  a: "-7994be7",
  b: "-d",
  r: "-3ccb"
}, {
  a: "a6443add555ea15af90092e8",
  b: "42",
  r: "29910eb75557a856be4024ba00000000000000000"
}, {
  a: "9385ed",
  b: "e5",
  r: "1270bda000000000000000000000000000000000000000000000000000000000"
}, {
  a: "-531",
  b: "7d",
  r: "-a620000000000000000000000000000000"
}];

var error_count = 0;
for (var i = 0; i < data.length; i++) {
  var d = data[i];
  var a = BigInt.parseInt(d.a, 16);
  var b = BigInt.parseInt(d.b, 16);
  var r = a << b;
  if (d.r !== r.toString(16)) {
    print("Input A:  " + a.toString(16));
    print("Input B:  " + b.toString(16));
    print("Result:   " + r.toString(16));
    print("Expected: " + d.r);
    print("Op: <<");
    error_count++;
  }
}
if (error_count !== 0) {
  print("Finished with " + error_count + " errors.")
  quit(1);
}
