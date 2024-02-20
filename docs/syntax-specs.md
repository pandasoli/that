# That Language

## Expressions
Everything in **That** is an expression, there are no statements.  
If an expression that doesn't fit in the last one is found, a next one is created.

```lua
local a *ui7 | nil = nil read(&a)
```
For example, this code can be divided into _variable assignment_ and _function call_.

You can use `{}` to group multiple expressions, just like in **C** with statements.

### Arithmetic
```lua
add + me
subtract - me
divide / me
```
More complex operations are made up from these.

The `-` operator is the only one that can also be a prefix talking about negative numbers.
```lua
-negateMe
```

### Comparison and equality
```lua
less < than
greather > than
equals == to
different != from
```
These operators only return `0` (false) or `1` (true).  
And only values of the same type can be compared.

### Logical operators
```
not 0   -- 1
not 1   -- 0
```
The `not` operator returns `1` if the operand is `0`, and vice versa.

The other two logical operators really are control flow constructs.
```lua
1 and 14   -- 14
0 and 14   -- 0

0 or 12    -- 12
14 or 12   -- 14
```
- `and` returns the left operand if it's false, or the right operand otherwise.
- `or` returns the left operand if it's true and the right operand otherwise.

### Precedence
All of these operators have the same precedence that you'd expect from C.
In cases the precedence isn't what you want, you can use `()` to group stuff.

```lua
local a ui32 = ((min + max) / 2) as ui32
```

## Variables
```lua
local imAVariable ui7 = 12 as ui7
imAVariable = 13
```

### Data Types

**That** is statically typed, and very strict,
but even so it doesn't understand that an `i8` fits within an `i32`,
so use `as` to change a literal's type.
```lua
local a ui7 = 12 as ui7
```

- Numbers

    Literal numbers are always `i32`.  
    And floats are also valid.

- Nil -- `nil`

    Use this when you don't want the value to be anything.
    ```lua
    local b *ui8 | nil = nil
    ```

- Unions -- `|`

    Use this to give many times to a variable.
    ```lua
    local a ui7 = 2 as ui7

    local name *ui7 | ui8 | nil = nil
    name = &a
    name = a          -- Invalid
    name = a as ui8
    ```

- Pointers -- `*`

    ```lua
    local a ui7 = 65
    local b *ui7 = &a
    local c ui7 = *b
    ```

    Pointer operations are allowed, it's very useful for structs and arrays implementation.

- Tuples -- `(<type>, <type>, ...)`

    Tuples unlike arrays, are imutable,
    and were made with a way of returning multiple values of a function in mind.
    ```lua
    fn divide(a i32, b i32) (i32 | nil, error | nil) {
        a == 0 or b == 0 and {
            return (nil, 1)
        }

        (a / b, nil)
    }
    ```

- Custom types

    You can use this to store new custom types.  
    Create a range of numbers with `...` inside a tuple.
    ```lua
    type bool (1, 0)                      -- Boolean
    type bool 1 | 0                       -- also works

    type i7 (-128...127)                  -- ASCII character
    type ui8 (0...255)                    -- Unsigned 8-bit integer
    type i32 (-2147483648...2147483647)   -- 32-bit integer
    type ui32 (0...4294967295)            -- Unsigned 32-bit integer
    ```
    Types are not overwritable!

- Unknown -- `unknown`

    An unknown type holds a data a function needs but it doesn't know about.
    Like the function `free` that expects a pointer, but it doesn't care about the data type.

## Control Flow
There's no "if statement", we reuse the control flow structures.
```lua
local a ui7 = 12 as ui7
local b ui7 = 13 as ui7

a == b and {
    -- '%d is equals to %d\n'
    local f *ui7 = create_arr(ui7, 37, 100, 32, 105, 115, 32, 101, 113, 117, 97, 108, 115, 32, 116, 111, 32, 37, 100, 10)
    printf(f, a, b)
    free(f as *unknown)
}
or {
    -- '%d is different from %d\n'
    local f *ui7 = create_arr(ui7, 37, 100, 32, 105, 115, 32, 100, 105, 102, 102, 101, 114, 101, 110, 116, 32, 102, 114, 111, 109, 32, 37, 100, 10)
    printf(f, a, b)
    free(f as *unknown)
}
```

## Functions
```lua
fn function(a i32, b i32) i32
```
Just like in **C** functions can be declared before its definition.

```lua
fn function(a i32, b i32) i64 {
    return a + b
}
```

## Include other files
```lua
import greetings
import greetings (hello, hi)
```

- `import greetings` Imports all public declarations
- `import greetings (hello, hi)` Imports `hello`'s and `hi`'s declarations

Use `pub` to give external access to local declarations.
```lua
pub local a ui7 = 0
pub type A i32 | nil

pub fn print() {
    -- "a's value is %d\n"
    local f *ui7 = create_arr(ui7, 97, 39, 115, 32, 118, 97, 108, 117, 101, 32, 105, 115, 32, 37, 100, 10)
    printf(f, a)
    free(a as *unknown)
}
```
