# pixel-sorting-c

A command line tool for pixel sorting your images, written in C.

## Getting Started
You'll need [libjpeg](http://libjpeg.sourceforge.net/) and GCC 4.7 to compile
the binaries. Once you've handled the dependencies and successfully run `make`,
you can pixel sort you images! Note that libjpeg is under the GPL, so I'm not
including it directly in the repo. 


## CLI Tool Usage
``usage: pixelsort [source.jpg] [destination.jpg] <query>``

## Query Syntax
A query takes the following form:

```SORT [ROWS|COLS] [ASC|DESC] BY [AVG|MUL|MIN|MAX|XOR] WITH [FULL|FIXED <k>|DARK <k>|LIGHT <k>] RUNS```

+ The `[ROWS|COLS]` distinction sets the "run" type (sort row-wise or col-wise).
+ The `[ASC|DESC]` distinction sets the ordering direction for the comparator.
+ The `BY [...]` clause states how a numeric value is extracted from a pixel.
+ The `WITH [...] RUNS` clause states how run boundaries are computed.

Note that multiple queries can be strung together using the `THEN` keyword. This enables easy chaining of operations without having to write the buffers to disk between each run.

## Examples
+ [pixelsort'd Van Gogh](http://imgur.com/a/kmtxm)

## Next Steps
This tool was written hastily, so it isn't pure C. However, it's not that far off, so priority number 1 is moving off of g++ and onto gcc. From there, the goal is to add on as many interesting, exciting pixel sorting algorithms as can be conjured up from the internets' collective imagination!

## Contributors
Max Seiden <140dbs@gmail.com>

## License
The MIT License (MIT)

Copyright (c) <2014> Max Seiden <140dbs@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
