# TBD

<small>(Note: "TBD" is the actual name, not a placeholder.)</small>

An equation solver that solves for all the **To Be Determined** values in a system of equations.

## Features

- Employs "direct" solutions where possible.
- All values track units.
- Ability to define new units.
- Graphviz integration for for debugging.
- <font color="gray">Find solutions with a minimum number of free variables (coming soon).</font>

## Examples

Values can be pinned, with units:

```
W := 1 [kg];
X := 5 [m];
Y := 7.3 [s];
```

New units can be defined:

```
[furlong]   := 660 [ft];
[firkin]    := 90  [lb];
[fortnight] := 336 [hr];	
```

Equations use (more or less) standard C grammar:

```
V = X / Y;
V / Y = Z;
```

Exponential use the `^` operator and are required to have constant integral exponents.

```
E = W * V^2 / 2;
```

With the exception of unit definitions, the order in which statements are defined is not relevant.

## Limitations/Bugs

I'm still working on things so there are some things that are know to not work fully yet:

- Dependent systems of equations: Currently only direct propagation is well supported.
  A Newton-Raphson solver for systems of equations is implemented, but not well tested.
- Even roots: To avoid dealing with the +/- issue, only odd roots are solved for.

## Future work

- Value hints as starting points.
- Rational exponents.
- Macro instantiation.
- Code generator to allow use in other programs.
- Optimization:
  - Merging of equivalent expression sub trees.
  - Conversion between repeated `+` and `*` and between repeated `*` and `^`.

## Setup (for development)
To configure the git hooks, run `./.git_hooks/setup.sh`
