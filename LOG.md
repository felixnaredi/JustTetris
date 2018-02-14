## Log

#### 2018-02-13 22:28:16 +0100

It should either be possible to, at build time or runtime, decide if
the programs interface should use GLFW+OpenGL or ncurses (possible
something else also). The common **node** is the header *interface*,
i.e there will be one implementation of the interface header using
GLFW and one using ncurses.

#### 2018-02-10 00:56:37 +0100

I do believe that the benefits of providing safe and debug builds will
be worth the effort. However, don't sacrifice to much readability on
it, and perhaps most important - don't get stuck because of them. If
they manage to catch five or more otherwise untracable bugs they will
probably make up for the time implementing them.

#### 2018-02-09 16:56:02 +0100

There is a GNU library for linear algebra (and allot of other math
related stuff) called GNU Scientific Library. I don't think that much
math needed for the program but it can be good to know.