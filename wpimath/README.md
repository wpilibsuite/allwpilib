# wpimath

wpimath contains utilities for robot control (feedforward/feedback), state estimation (filters, Kalman and otherwise), 2D/3D geometry, kinematics, trajectory generation, and trajectory optimization.

## Implementation guidelines

A lot of wpimath features directly implement equations from books or papers. The following guidelines make that code easier to maintain and audit for correctness.

### Citations

Cite source books/papers at the top of the function (e.g., `See section 5.6 of "book name".`). If multiple items from a given work are referenced, write a bibliography entry for the work to reference later.

Cite the equation numbers each line of code implements, if applicable. For example, `See equation (#.#) of [1].` where `[1]` is a bibliography reference number.

### Comments

Comment each line of code with its pretty-printed math equivalent.
```cpp
// xₖ₊₁ = Axₖ + Buₖ
x = A * x + B * u;
```

Link to explanatory material where appropriate to explain background knowledge and/or notation choice.

### Variable naming

Follow established mathematical convention where possible (e.g., use A, B, C, D for state-space notation instead of `stateTransitionMatrix`).

Use math symbols in variable names (see [Unicodeit](#Unicodeit)) to match source papers. This usually entails some Greek letters (α), but diacritics, superscripts, and subscripts need to be spelled out (`ẋ` → `x_dot`, `αₖ²` → `α_k_sq`) since compilers reject them, and the small features make variable names difficult to read.

### Derivations

Put small derivations in comments within the function. Put large derivations in algorithms.md and link to them.

## Unicodeit

When writing math expressions in documentation, use https://www.unicodeit.net/ to convert LaTeX to a Unicode equivalent that's easier to read. Not all expressions will translate (e.g., superscripts of superscripts) so focus on making it readable by someone who isn't familiar with LaTeX. If content on multiple lines needs to be aligned in Doxygen/Javadoc comments (e.g., integration/summation limits, matrices packed with square brackets and superscripts for them), put them in @verbatim/@endverbatim blocks in Doxygen or `<pre>` tags in Javadoc so they render with monospace font.

The LaTeX to Unicode conversions can also be done locally via the unicodeit Python package. To install it, execute:
```bash
pip install --user unicodeit
```

Here's example usage:
```bash
$ python -m unicodeit.cli 'x_{k+1} = Ax_k + Bu_k'
xₖ₊₁ = Axₖ + Buₖ
```

On Linux, this process can be streamlined further by adding the following Bash function to your .bashrc (requires `wl-clipboard` on Wayland or `xclip` on X11):
```bash
# Converts LaTeX to Unicode, prints the result, and copies it to the clipboard
uc() {
  if [ $WAYLAND_DISPLAY ]; then
    python -m unicodeit.cli $@ | tee >(wl-copy -n)
  else
    python -m unicodeit.cli $@ | tee >(xclip -sel)
  fi
}
```

Here's example usage:
```bash
$ uc 'x_{k+1} = Ax_k + Bu_k'
xₖ₊₁ = Axₖ + Buₖ
```
