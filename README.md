GRworkbench
===========

Simulation and visualization of general relativistic spacetimes

Original version: Antony Searle & Andrew Moylan

Description: https://sites.google.com/site/andrewjmoylan/grworkbench

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
