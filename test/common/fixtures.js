'use strict';

const path = require('path');
const fs = require('fs');
const { pathToFileURL } = require('url');

const fixturesDir = path.join(__dirname, '..', 'fixtures');

function fixturesPath(...args) {
  return path.join(fixturesDir, ...args);
}

function fixturesFileURL(...args) {
  return pathToFileURL(fixturesPath(...args));
}

function readFixtureSync(args, enc) {
  if (Array.isArray(args))
    return fs.readFileSync(fixturesPath(...args), enc);
  return fs.readFileSync(fixturesPath(args), enc);
}

function readFixtureKey(name, enc) {
  return fs.readFileSync(fixturesPath('keys', name), enc);
}

function readFixtureKeys(enc, ...names) {
  return names.map((name) => readFixtureKey(name, enc));
}

// This should be in sync with test/fixtures/utf8_test_text.txt.
// We copy them here as a string because this is supposed to be used
// in fs API tests.
const utf8TestText = '永和九年，嵗在癸丑，暮春之初，會於會稽山隂之蘭亭，脩稧事也。' +
                     '羣賢畢至，少長咸集。此地有崇山峻領，茂林脩竹；又有清流激湍，' +
                     '暎帶左右。引以為流觴曲水，列坐其次。雖無絲竹管弦之盛，一觴一詠，' +
                     '亦足以暢敘幽情。是日也，天朗氣清，恵風和暢；仰觀宇宙之大，' +
                     '俯察品類之盛；所以遊目騁懐，足以極視聽之娛，信可樂也。夫人之相與，' +
                     '俯仰一世，或取諸懐抱，悟言一室之內，或因寄所託，放浪形骸之外。' +
                     '雖趣舎萬殊，靜躁不同，當其欣扵所遇，暫得扵己，怏然自足，' +
                     '不知老之將至。及其所之既惓，情隨事遷，感慨係之矣。向之所欣，' +
                     '俛仰之閒以為陳跡，猶不能不以之興懐；況脩短隨化，終期扵盡。' +
                     '古人云：「死生亦大矣。」豈不痛哉！每攬昔人興感之由，若合一契，' +
                     '未嘗不臨文嗟悼，不能喻之扵懐。固知一死生為虛誕，齊彭殤為妄作。' +
                     '後之視今，亦由今之視昔，悲夫！故列敘時人，錄其所述，雖世殊事異，' +
                     '所以興懐，其致一也。後之攬者，亦將有感扵斯文。';

module.exports = {
  fixturesDir,
  path: fixturesPath,
  fileURL: fixturesFileURL,
  readSync: readFixtureSync,
  readKey: readFixtureKey,
  readKeys: readFixtureKeys,
  utf8TestText,
  get utf8TestTextPath() {
    return fixturesPath('utf8_test_text.txt');
  },
};
