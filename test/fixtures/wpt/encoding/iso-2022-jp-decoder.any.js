function decode(input, output, desc) {
  test(function() {
    var d = new TextDecoder("iso-2022-jp"),
        buffer = new ArrayBuffer(input.length),
        view = new Int8Array(buffer)
    for(var i = 0, l = input.length; i < l; i++) {
      view[i] = input[i]
    }
    assert_equals(d.decode(view), output)
  }, "iso-2022-jp decoder: " + desc)
}
decode([0x1b, 0x24], "�$", "Error ESC")
decode([0x1b, 0x24, 0x50], "�$P", "Error ESC, character")
decode([0x1b, 0x28, 0x42, 0x50], "P", "ASCII ESC, character")
decode([0x1b, 0x28, 0x42, 0x1b, 0x28, 0x42, 0x50], "�P", "Double ASCII ESC, character")
decode([0x50, 0x1b, 0x28, 0x42, 0x50], "PP", "character, ASCII ESC, character")
decode([0x5C, 0x5D, 0x7E], "\\]~", "characters")
decode([0x0D, 0x0E, 0x0F, 0x10], "\x0D��\x10", "SO / SI")

decode([0x1b, 0x28, 0x4A, 0x5C, 0x5D, 0x7E], "¥]‾", "Roman ESC, characters")
decode([0x1b, 0x28, 0x4A, 0x0D, 0x0E, 0x0F, 0x10], "\x0D��\x10", "Roman ESC, SO / SI")
decode([0x1b, 0x28, 0x4A, 0x1b, 0x1b, 0x28, 0x49, 0x50], "�ﾐ", "Roman ESC, error ESC, Katakana ESC")

decode([0x1b, 0x28, 0x49, 0x50], "ﾐ", "Katakana ESC, character")
decode([0x1b, 0x28, 0x49, 0x1b, 0x24, 0x40, 0x50, 0x50], "�佩", "Katakana ESC, multibyte ESC, character")
decode([0x1b, 0x28, 0x49, 0x1b, 0x50], "�ﾐ", "Katakana ESC, error ESC, character")
decode([0x1b, 0x28, 0x49, 0x1b, 0x24, 0x50], "�､ﾐ", "Katakana ESC, error ESC #2, character")
decode([0x1b, 0x28, 0x49, 0x50, 0x1b, 0x28, 0x49, 0x50], "ﾐﾐ", "Katakana ESC, character, Katakana ESC, character")
decode([0x1b, 0x28, 0x49, 0x0D, 0x0E, 0x0F, 0x10], "����", "Katakana ESC, SO / SI")

decode([0x1b, 0x24, 0x40, 0x50, 0x50], "佩", "Multibyte ESC, character")
decode([0x1b, 0x24, 0x42, 0x50, 0x50], "佩", "Multibyte ESC #2, character")
decode([0x1b, 0x24, 0x42, 0x1b, 0x50, 0x50], "�佩", "Multibyte ESC, error ESC, character")
decode([0x1b, 0x24, 0x40, 0x1b, 0x24, 0x40], "�", "Double multibyte ESC")
decode([0x1b, 0x24, 0x40, 0x1b, 0x24, 0x40, 0x50, 0x50], "�佩", "Double multibyte ESC, character")
decode([0x1b, 0x24, 0x40, 0x1b, 0x24, 0x42, 0x50, 0x50], "�佩", "Double multibyte ESC #2, character")
decode([0x1b, 0x24, 0x40, 0x1b, 0x24, 0x50, 0x50], "�ば�", "Multibyte ESC, error ESC #2, character")

decode([0x1b, 0x24, 0x40, 0x50, 0x1b, 0x24, 0x40, 0x50, 0x50], "�佩", "Multibyte ESC, single byte, multibyte ESC, character")
decode([0x1b, 0x24, 0x40, 0x20, 0x50], "��", "Multibyte ESC, lead error byte")
decode([0x1b, 0x24, 0x40, 0x50, 0x20], "�", "Multibyte ESC, trail error byte")

decode([0x50, 0x1b], "P�", "character, error ESC")
decode([0x50, 0x1b, 0x24], "P�$", "character, error ESC #2")
decode([0x50, 0x1b, 0x50], "P�P", "character, error ESC #3")
decode([0x50, 0x1b, 0x28, 0x42], "P", "character, ASCII ESC")
decode([0x50, 0x1b, 0x28, 0x4A], "P", "character, Roman ESC")
decode([0x50, 0x1b, 0x28, 0x49], "P", "character, Katakana ESC")
decode([0x50, 0x1b, 0x24, 0x40], "P", "character, Multibyte ESC")
decode([0x50, 0x1b, 0x24, 0x42], "P", "character, Multibyte ESC #2")
