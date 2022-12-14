// This list was inspired by these sources:
// - https://annevankesteren.nl/2010/8-bit-labels
// - http://l0.cm/encodings/table/

[
  "437",
  "adobe-standard-encoding",
  "armscii-8",
  "bocu-1",
  "cesu-8",
  "cp1025",
  "cp437",
  "cp737",
  "cp851",
  "cp858",
  "cp862",
  "cp864",
  "cp869",
  "cp875",
  "cp950",
  "csiso103t618bit",
  "csiso111ecmacyrillic",
  "cspc8codepage437",
  "csviscii",
  "dos-720",
  "dos-862",
  "ecma-cyrillic",
  "euc-tw",
  "german",
  "geostd8",
  "hp-roman8",
  "ibm-thai",
  "ibm00858",
  "ibm00924",
  "ibm01047",
  "ibm01140",
  "ibm01141",
  "ibm01142",
  "ibm01143",
  "ibm01144",
  "ibm01145",
  "ibm01146",
  "ibm01147",
  "ibm01148",
  "ibm01149",
  "ibm037",
  "ibm1026",
  "ibm1047",
  "ibm273",
  "ibm277",
  "ibm278",
  "ibm280",
  "ibm284",
  "ibm285",
  "ibm290",
  "ibm297",
  "ibm367",
  "ibm420",
  "ibm423",
  "ibm424",
  "ibm437",
  "ibm500",
  "ibm737",
  "ibm775",
  "ibm850",
  "ibm852",
  "ibm855",
  "ibm857",
  "ibm860",
  "ibm861",
  "ibm862",
  "ibm863",
  "ibm864",
  "ibm864i",
  "ibm865",
  "ibm868",
  "ibm869",
  "ibm870",
  "ibm871",
  "ibm880",
  "ibm905",
  "ibm918",
  "iso-2022-jp-1",
  "iso-2022-jp-2",
  "iso-2022-jp-3",
  "iso-8859-8 visual",
  "jis_c6226-1978",
  "jis_x0208-1983",
  "jis_x0208-1990",
  "jis_x0212-1990",
  "johab",
  "latin9",
  "norwegian",
  "sami-ws2",
  "scsu",
  "shift_jis_x0213-2000",
  "swedish",
  "tcvn",
  "tis-620-2533",
  "utf-7",
  "utf-32",
  "viscii",
  "windows-936-2000",
  "windows-sami-2",
  "ws2",
  "x-chinese-cns",
  "x-chinese-eten",
  "x-cp20001",
  "x-cp20003",
  "x-cp20004",
  "x-cp20005",
  "x-cp20261",
  "x-cp20269",
  "x-cp20936",
  "x-cp20949",
  "x-cp21027",
  "x-cp50227",
  "x-cp50229",
  "x-ebcdic-koreanextended",
  "x-europa",
  "x-ia5",
  "x-ia5-german",
  "x-ia5-norwegian",
  "x-ia5-swedish",
  "x-iscii-as",
  "x-iscii-be",
  "x-iscii-de",
  "x-iscii-gu",
  "x-iscii-ka",
  "x-iscii-ma",
  "x-iscii-or",
  "x-iscii-pa",
  "x-iscii-t",
  "x-iscii-ta",
  "x-iscii-te",
  "x-mac-arabic",
  "x-mac-ce",
  "x-mac-centraleurroman",
  "x-mac-chinesesimp",
  "x-mac-chinesetrad",
  "x-mac-croatian",
  "x-mac-devanagari",
  "x-mac-dingbats",
  "x-mac-farsi",
  "x-mac-greek",
  "x-mac-gujarati",
  "x-mac-gurmukhi",
  "x-mac-hebrew",
  "x-mac-icelandic",
  "x-mac-japanese",
  "x-mac-korean",
  "x-mac-roman-latin1",
  "x-mac-romanian",
  "x-mac-symbol",
  "x-mac-thai",
  "x-mac-tibetan",
  "x-mac-turkish",
  "x-mac-vt100",
  "x-nextstep",
  "x-vps",
  "_autodetect",
  "_autodetect_all",
  "_autodetect_kr"
].forEach(label => {
  async_test(t => {
    const frame = document.createElement("iframe");
    t.add_cleanup(() => {
      frame.remove();
    });
    frame.src = "resources/text-plain-charset.py?label=" + label;
    frame.onload = t.step_func_done(() => {
      // If we ever change this default this needs adjusting accordingly.
      assert_equals(frame.contentDocument.characterSet, "windows-1252");
      assert_equals(frame.contentDocument.inputEncoding, "windows-1252");
    });
    document.body.append(frame);
  }, `${label} is not supported by the Encoding Standard`);
});
