## Recursion optimization
**Stack shrink** is a mechanism implemented in the binary creation process that
reduces the size of a function's stack frame by removing the values that will no longer be used
and reallocating others (the references to them are also changed).
It is used right before a function calls itself.

## Dead functionalities
If a function's return value is never used in its calls, why return it?  
Or, why create this function if it doesn't make any side effect?
