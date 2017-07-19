'use strict';

/* The following tests are copied from WPT, modifications to them should be
   upstreamed first. Refs:
   https://github.com/w3c/web-platform-tests/blob/4839a0a804/url/toascii.json
   License: http://www.w3.org/Consortium/Legal/2008/04-testsuite-copyright.html
*/
module.exports =
[
  "This resource is focused on highlighting issues with UTS #46 ToASCII",
  {
    "comment": "Label with hyphens in 3rd and 4th position",
    "input": "aa--",
    "output": "aa--"
  },
  {
    "input": "a†--",
    "output": "xn--a---kp0a"
  },
  {
    "input": "ab--c",
    "output": "ab--c"
  },
  {
    "comment": "Label with leading hyphen",
    "input": "-x",
    "output": "-x"
  },
  {
    "input": "-†",
    "output": "xn----xhn"
  },
  {
    "input": "-x.xn--nxa",
    "output": "-x.xn--nxa"
  },
  {
    "input": "-x.β",
    "output": "-x.xn--nxa"
  },
  {
    "comment": "Label with trailing hyphen",
    "input": "x-.xn--nxa",
    "output": "x-.xn--nxa"
  },
  {
    "input": "x-.β",
    "output": "x-.xn--nxa"
  },
  {
    "comment": "Empty labels",
    "input": "x..xn--nxa",
    "output": "x..xn--nxa"
  },
  {
    "input": "x..β",
    "output": "x..xn--nxa"
  },
  {
    "comment": "Invalid Punycode",
    "input": "xn--a",
    "output": null
  },
  {
    "input": "xn--a.xn--nxa",
    "output": null
  },
  {
    "input": "xn--a.β",
    "output": null
  },
  {
    "comment": "Valid Punycode",
    "input": "xn--nxa.xn--nxa",
    "output": "xn--nxa.xn--nxa"
  },
  {
    "comment": "Mixed",
    "input": "xn--nxa.β",
    "output": "xn--nxa.xn--nxa"
  },
  {
    "input": "ab--c.xn--nxa",
    "output": "ab--c.xn--nxa"
  },
  {
    "input": "ab--c.β",
    "output": "ab--c.xn--nxa"
  },
  {
    "comment": "CheckJoiners is true",
    "input": "\u200D.example",
    "output": null
  },
  {
    "input": "xn--1ug.example",
    "output": null
  },
  {
    "comment": "CheckBidi is true",
    "input": "يa",
    "output": null
  },
  {
    "input": "xn--a-yoc",
    "output": null
  },
  {
    "comment": "processing_option is Nontransitional_Processing",
    "input": "ශ්‍රී",
    "output": "xn--10cl1a0b660p"
  },
  {
    "input": "نامه‌ای",
    "output": "xn--mgba3gch31f060k"
  },
  {
    "comment": "U+FFFD",
    "input": "\uFFFD.com",
    "output": null
  },
  {
    "comment": "U+FFFD character encoded in Punycode",
    "input": "xn--zn7c.com",
    "output": null
  },
  {
    "comment": "Label longer than 63 code points",
    "input": "x01234567890123456789012345678901234567890123456789012345678901x",
    "output": "x01234567890123456789012345678901234567890123456789012345678901x"
  },
  {
    "input": "x01234567890123456789012345678901234567890123456789012345678901†",
    "output": "xn--x01234567890123456789012345678901234567890123456789012345678901-6963b"
  },
  {
    "input": "x01234567890123456789012345678901234567890123456789012345678901x.xn--nxa",
    "output": "x01234567890123456789012345678901234567890123456789012345678901x.xn--nxa"
  },
  {
    "input": "x01234567890123456789012345678901234567890123456789012345678901x.β",
    "output": "x01234567890123456789012345678901234567890123456789012345678901x.xn--nxa"
  },
  {
    "comment": "Domain excluding TLD longer than 253 code points",
    "input": "01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678.x",
    "output": "01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678.x"
  },
  {
    "input": "01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678.xn--nxa",
    "output": "01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678.xn--nxa"
  },
  {
    "input": "01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678.β",
    "output": "01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678.xn--nxa"
  }
]
