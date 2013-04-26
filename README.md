GRworkbench
===========

Simulation and visualization of general relativistic spacetimes. Old project from department of Physics at The Australian National University. More information: https://sites.google.com/site/andrewjmoylan/grworkbench

This version: Antony Searle & Andrew Moylan

Building
--------

Doesn't build in latest version of boost/c++.

When it does build, something like this should work:

cd lax<br>
make<br>
cd ../lift<br>
make<br>
cd ../grworkbench<br>
make<br>

You need the Boost library in the same directory or on the include path. Good luck.

Running
-------

You should be able to run the windows binary grwb.exe just fine. You need the accompanying layout of .lax files too.

Try running the demo of an interferometer orbiting a Schwarzschild black hole:

interferometer_demo

Or for something fancier, try stepping through the Kerrfest demonstration:

kerrfest<br>
start<br>
walkthrough<br>
s1<br>
s2<br>
etc...<br>

The source .lax files for each of these can be found in the 'work' directory of the layout.
