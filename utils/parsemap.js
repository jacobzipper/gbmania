var parser = require('osu-parser');
const fs = require('fs');

function frameToMillis(f) {
    return f * 17;
}

function millisToPos(millis) {
    return 160 - (millis / 4);
}

parser.parseFile('./../data/hime.osu', function (err, beatmap) {
    mp = {'64': 0, '192': 1, '320': 2, '448': 3};
    mpnotes = []
    for (var i = 0; i < beatmap.hitObjects.length; i++) {
        mpnotes.push({'low': -1, 'high': -1, 'note': -1, 'hit': 0});
    }
    ret = '#include \"beatmap.h\"\n\nNote notes[] = {\n';
    var vb = 0;
    var cur = 0;
    while (cur < beatmap.hitObjects.length) {
        var curMillis = frameToMillis(vb);
        for (var j = cur; j < beatmap.hitObjects.length; j++) {
            var pos = millisToPos(beatmap.hitObjects[j].startTime - curMillis);
            if (mpnotes[j].low == -1 && pos > 0) {
                mpnotes[j].low = vb;
                mpnotes[j].note = mp[beatmap.hitObjects[j].position[0] + ''];
            } else if (mpnotes[j].high == -1 && pos > 160) {
                mpnotes[j].high = vb - 1;
                cur += 1;
            }
        }
        vb += 1;
    }
    ret += mpnotes.map(note => '{' + note.low +', ' + note.high + ', ' + note.note + ', ' + note.hit +'}').join(',\n') + '};';
    fs.writeFile('./../source/beatmap.c', ret, function(err) {
        if(err) {
            return console.log(err);
        }
    });
    ret = '#include \"logic.h\"\n\n#define NOTES_LENGTH ' + beatmap.hitObjects.length + '\n\nextern Note notes[];';
    fs.writeFile('./../include/beatmap.h', ret, function(err) {
        if(err) {
            return console.log(err);
        }
    });
});