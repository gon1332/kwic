# A Case Study of Architectural Styles

Based on Chapter 3 of _Software Architecture: Perspectives on an Emerging Discipline, Mary Shaw, David Garlan_
this repository is using the KWIC to showcase various architectural styles.

## The many faces of KWIC

### Solution 1: Main Program/Subroutine with Shared Data

Input, Circular Shift, Alphabetize, and Output are subroutines invoked in sequence by a single main
program, communicating only through file-scope shared data (the raw text, the line/word index, and the
alphabetized index) rather than through parameters or return values. No subroutine's signature declares
what it reads or writes - that information is only visible by reading its body. This is precisely the
weakness the style is meant to demonstrate: control coupling is loose (a simple call sequence), but data
coupling is tight, since every subroutine must agree on the shared representation and any change to it
ripples through all four.

### Solution 2: Abstract Data Types

Representation is now fully hidden behind named operations, nothing external can reach into internal storage by
reference or by assumption, and the class boundaries map cleanly onto the book's stated modules.
The remaining coupling - output() needing to know both ADTs' public interfaces, alpha_shift's constructor
needing circular_shift's interface - is exactly the control coupling that Solution 2 is supposed to still have,
per the book, rather than something to fix here.


## References

Parnas, D. L. (1972). On the criteria to be used in decomposing systems into modules. *Communications of
the ACM*, 15(12), 1053–1058.

Shaw, M., & Garlan, D. (1996). *Software Architecture: Perspectives on an Emerging Discipline*. Prentice Hall.