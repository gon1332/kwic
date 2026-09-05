# A Case Study of Architectural Styles

Based on Chapter 3 of _Software Architecture: Perspectives on an Emerging Discipline, Mary Shaw, David Garlan_
this repository is using the KWIC to showcase various architectural styles.

Example output:
```
> ./cmake-build-debug-system/kwic --method shared_data --file tests/small --width 40
                       implementation of KWIC implementation of                       
                                 text to Small text to                                 
                                 of KWIC implementation of KWIC                                 
                     KWIC implementation of KWIC implementation                     
                                     the test the                                     
                                to Small text to Small                                
                                    test the test                                    
                              Small text to Small text
```

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

### Solution 3: Implicit Invocation

Modules no longer call each other directly. Instead, a producer announces that something happened (a
`subject<Payload>` firing a strongly-typed event), and any number of interested modules react to it
(`observer<Payload>` instances registered against that specific event). `circular_shift` never calls
`alphabetizer`, and neither know of each other's existence — both are wired together only by what events
they've each subscribed to. This buys loose *control* coupling: reading the code no longer tells you what
runs next, only what event was fired.

Data coupling is a different story. `lines_tokenized` still holds shared state (`m_words`) that both
`circular_shift` and `alphabetizer` read directly, matching what Garlan & Shaw note about their own version
of this solution: implicit invocation loosens control coupling, not data coupling. What *is* fully hidden,
per module, is anything each module doesn't need to expose — alphabetical order, in particular, is private
state owned only by `alphabetizer`, exposed solely through `getalphaindex()`, so `lines_tokenized` has no
sorting-related dependency and stays reusable by any future consumer of tokenized lines that doesn't care
about alphabetical order.

Alphabetization is also incremental rather than batch: each new-line event carries only the newly created
shift entries, and `alphabetizer` inserts them into its own already-sorted index (`std::ranges::upper_bound`
+ `insert`) instead of re-sorting everything from scratch on every line.

**Assumptions made:**
- Input is simulated as a stream of discrete "new line" events (one per line of the source text), rather
  than modeling a real interactive/asynchronous input source.
- Event dispatch (`notify`) is synchronous and single-threaded - there is no concurrency, queueing, or
  ordering concern between observers of the same event.
- Events are strongly typed per subject (`subject<Payload>` templated on a concrete payload type) rather
  than routed through one generic `Event` base class, so there's a distinct `subject`/`observer` pair per
  kind of event rather than a single dispatcher for all event kinds.
- Global objects (`g_sub_new_input`, `g_sub_new_line`, `g_lines_tokenized`) are process-lifetime singletons
  with no reset; the solution isn't designed to be safely re-run more than once per process.
- Observer lifetime is tied to RAII: attaching happens in the constructor, detaching in the destructor, so
  an observer is "subscribed" for exactly as long as the corresponding C++ object is alive.

### Solution 4: Pipes and Filters

Input, Circular Shift, Alphabetize, and Output are independent functions chained together, each consuming
only what its immediate predecessor produced and returning a new value for the next stage — no shared
state, no globals, no reaching back past an adjacent stage. `circular_shift`'s output (`shifted_lines`,
bundling word content and shift indices into one payload) is the pipe that both `alphabetize` and `output`
depend on, kept explicit as a single named type rather than threaded loosely through every signature.

This gets simplicity, an intuitive top-to-bottom flow, and easy reuse of individual filters - but it's
strictly batch: control only ever flows one way, so there's no way to add "delete a line" or any interactive
capability without introducing persistent shared storage, which breaks the model. It's also inherently
less space-efficient, since data conceptually flows forward through the pipeline rather than being
mutated in place.


## References

Parnas, D. L. (1972). On the criteria to be used in decomposing systems into modules. *Communications of
the ACM*, 15(12), 1053–1058.

Shaw, M., & Garlan, D. (1996). *Software Architecture: Perspectives on an Emerging Discipline*. Prentice Hall.