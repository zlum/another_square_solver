# another_square_solver
Consumer-Produser based solver of quadratic equations

Working Cycle:
--------------------------------------------------------------------------------
QuadReader -> Buffer(Coeffs) -> QuadSolver -> Buffer -> QuadPrinter(Equation)

Build:
--------------------------------------------------------------------------------
Execute next commands in project directory:

* mkdir build
* cd build
* cmake ..
* make

Run:
--------------------------------------------------------------------------------
Program can read any sequence of integer numbers sepparated by space.
Any other symbols or numbers out of boundaries will be ignored.

Input sample:
* ./another_square_solver 4 2 -1

Output sample:
* (4 2 -1) => roots (-0.809017, 0.309017), extremum (-0.25)
