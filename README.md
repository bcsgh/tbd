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


# Bazel/skylark rules to process .tbd files.

## `MODULE.bazel`

```
bazel_dep(
    name = "com_github_bcsgh_tbd",
    version = ...,
)
```

<a id="gen_tbd"></a>

## gen_tbd

<pre>
load("@com_github_bcsgh_tbd//tbd:rule.bzl", "gen_tbd")

gen_tbd(<a href="#gen_tbd-name">name</a>, <a href="#gen_tbd-srcs">srcs</a>, <a href="#gen_tbd-cpp">cpp</a>, <a href="#gen_tbd-dot">dot</a>, <a href="#gen_tbd-out">out</a>, <a href="#gen_tbd-warnings_as_errors">warnings_as_errors</a>)
</pre>

Process a .tbd file.

**PARAMETERS**


| Name  | Description | Default Value |
| :------------- | :------------- | :------------- |
| <a id="gen_tbd-name"></a>name |  The target name.   |  `None` |
| <a id="gen_tbd-srcs"></a>srcs |  The input file.   |  `None` |
| <a id="gen_tbd-cpp"></a>cpp |  If set, generate a C++ implementation at the give location.   |  `None` |
| <a id="gen_tbd-dot"></a>dot |  If set, generate a graphviz depiction at the give location.   |  `None` |
| <a id="gen_tbd-out"></a>out |  Output the resolved values at the give location.   |  `None` |
| <a id="gen_tbd-warnings_as_errors"></a>warnings_as_errors |  Fail on warnings.   |  `False` |


## Setup (for development)
To configure the git hooks, run `./.git_hooks/setup.sh`
