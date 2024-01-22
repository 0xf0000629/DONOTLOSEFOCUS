# DONOTLOSEFOCUS
The repository of my game written on C and nothing else!<br>
If you haven't seen what this whole thing looks like in action, here's a video of it:<br>
https://www.youtube.com/watch?v=-OXcGB75o08

File list:
- DoNotLoseFocus.c - the game itself
- screenshottest.c - changes color and shifts the screen around
- invoker.c - spawns random webpages and windows
- distractor.c - draws random images on screen
- cursorgen.c - draws copies of your cursor on screen
- icongen.c - draws random Windows icons on screen
- font.dat - textures

This project has a small story to it.<br>
Back in school I was learning C++ and obviously got obsessed with WinAPI and the things it was capable of.<br>
Because I wasn't nearly skilled enough I couldn't make anything interesting or impressive. So - I told myself that when I will eventually be good enough I will attempt making two things.<br>
One - I will make a C++ game with ASCII graphics that uses either ray-casting or parallax as base.<br>
Two - I will make a malware akin to the stuff that Leurak and other people were making years ago (except for maybe the whole your PC is dead now thing).<br>
<br>
So here I am now, with two things I promised myself I'll make, and I am given a programming language project.<br>
Chaos ensues.<br>
<br>
<br>
So, okay. I need to make a game that somehow continuously generates obstacles and ALSO uses a parallax effect. How would I do that?<br>
I came up with a fun idea. What if I generate sections equal to the size of the camera and then conveyor belt them towards the player?<br>
There we go - that's how the whole engine works. It generates a chunk and then waits until the chunk is fully visible on camera.<br>
Then it will generate another one, put it directly to the right of that chunk and then wait until the player reaches the position again.<br>
Swap the old chunk with the most recent one, generate another chunk, rinse and repeat.<br>
And now - the same exact strategy with the background! <br> 
But since we want to create the parallax effect, we need more layers and a slower rolling speed, to create the illusion of depth. <br>
I decided on generating blocks of different sizes, attached to the floor and ceiling. To make them easy enough to avoid, I set the max Y sizes to half the camera Y size - 1.<br>
<br>
After I finished making the random generation and gave it to people to playtest, I realized a problem - the game is very easy. Just stay in the center and you're good. That's no good.<br>
I tweaked the generation to make the blocks generate with Y-sizes bigger than half the screen, so that the player has to avoid them.<br>
Then I sprinkled in some small blocks, and now the game is too hard. Awesome. I needed a fresh idea.<br>
Then it hit me - what if I had invincibility frames that I could collect, but they were limited to a certain amount? That should work, right?<br>
And it did. It worked out hilariously well. Most walls weren't that thick, so they were easy to bust through, but if you wanted to take on a thick wall, you would need to save them, and you couldn't save too many.<br>
<br>
<br>
And that's the game itself, but only half the issue. I needed to make it more exciting.<br>
I started looking through the awful things WinAPI could do to bitmaps and what exactly it could make into a bitmap.<br>
The answer was clear - almost everything. But I didn't need much for the payloads.<br>
Screen tearing was the hardest to make because I had to tweak the math a lot in order for it to look cool.<br>
When I got it to work, I figured that I should make it interesting by using both AND and OR blending. Later I made the game activate the latter on the second half.<br>
Everything else, icon generation, image generation, cursor spam, etc. etc. was easy to make, you just need to figure out where to take the bitmaps from.<br>
<br>
<br>
After I was done, I had a thought that maybe it would be cool if I was to make the payloads into parts of the main code and not separate programs.<br>
I could. But to me, having them as separate programs that you could launch separately was cooler. Like, after you beat the main game, the payloads are here.<br>
Go send them to your friend as a prank or something. It would be funny. <br>
<br>
<br>
<br>
That's about it. Thanks for reading, the one singular person to reach this point (or even find this text wall, for that matter).<br>
Have a good one.<br>
<br>
<br>
<br>
P.S. you may be wondering, X Future, why haven't you chosen ray-casting instead?<br>
Answer - I did write it and then decided against using it because I didn't like what it looked like with low resolution and ASCII graphics.<br>
and to be fair and honest - I've always enjoyed parallax 2.5D more than 3D graphics.
