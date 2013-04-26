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
