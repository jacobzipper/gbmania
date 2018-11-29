# FOREWORD
Welcome welcome to my horrible weeb game.

Osu!mania is the only game I actually play, so I figured I'd clone it on an esoteric system not designed for rhythm games.

# INSTRUCTIONS
With that said, the rules of osu!mania are super simple, especially if you've played guitar hero.

Just click the buttons on your keyboard when the notes are appearing at your judgment line.

As a convenience, I HIGHLY recommend you configure your emulator to map ABLR to DFJK.

DFJK is the standard four key osu!mania setup, and is what most people get comfortable with.

Note: This is already set up in the mednafen config^

When you play, a good hit gets 300 points, 200 for okay, and 100 for a "you tried" consolation when you basically missed.

# BUILD
### Just run make!

If you want to add anything just clean before running make again to be sure it's caught.

If you want to change up the map, make sure you `npm install` in the utils folder, copy an osu file into the data folder, cd into the utils folder, change the name of the file it's reading, then `node parsemap.js`.

If you want to change up the song as well, you're in for a treat.

You gotta get a wav of the song you want in a format maxmod likes or you're kinda screwed.

I had luck with using sox and turning the osu mp3's into 8 bit 1 channel and 4000 sample rate wavs, but you might not be so lucky.

Also change the SFX_* thing in the logic file to the new name of your song

If you have any questions, concerns, hate mail, etc, let me know in a heated email or something.

# AND

Anyways, have fun no seizures pls.

Jacob Zipper