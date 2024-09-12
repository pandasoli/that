# That Language

## Expressions
Everything in **That** is an expression.  
If an expression that doesn't fit in the last one is found, a next one is created.

```lua
local a ptr = 0
read(&a)
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
These operators return `0` or `1`.  
And only values of the same type can be compared.

### Logical operators
```lua
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
In cases the precedence isn't what you want, you can use `{}` to group stuff.

```lua
local a ui32 = {{min + max} / 2} as ui32
```

## Variables
```lua
local imAVariable ui7 = 12 as ui7
imAVariable = 13
```

### Data Types

**That** is statically typed, and very strict,
so that it doesn't understand that an `i8` fits within an `i32`,
so use `as` to change a literal's type.
```lua
local a ui7 = 12 as ui7
```

- Numbers

    Literal integer numbers are `i32`, 
    and floats are `float32`.

- Pointers -- `ptr`

    ```lua
    local a ui7 = 65
    local ptr = &a
    local c ui7 = *b
    ```

    Pointer operations are allowed, it's very useful for structs and arrays implementation.

- Custom types

    You can use this to store new custom types.
    ```lua
    type bool (1, 0)                      -- Boolean

    type i7 (-128...127)                  -- ASCII character
    type ui8 (0...255)                    -- Unsigned 8-bit integer
    type i32 (-2147483648...2147483647)   -- 32-bit integer
    type ui32 (0...4294967295)            -- Unsigned 32-bit integer
    ```


## Control Flow
There's no "if statement", we reuse the control flow structures.
```lua
local a ui7 = 12 as ui7
local b ui7 = 13 as ui7

a == b and {
    -- '%d is equals to %d\n'
    local f ptr = create_arr(37, 100, 32, 105, 115, 32, 101, 113, 117, 97, 108, 115, 32, 116, 111, 32, 37, 100, 10)
    printf(f, a, b)
    free(f)
}
or {
    -- '%d is different from %d\n'
    local f ptr = create_arr(37, 100, 32, 105, 115, 32, 100, 105, 102, 102, 101, 114, 101, 110, 116, 32, 102, 114, 111, 109, 32, 37, 100, 10)
    printf(f, a, b)
    free(f)
}
```

## Functions
```rs
local function fn(i32, i32) i64 = fn(a i32, b i32) i64 {
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
pub type A i32

pub local print = fn {
    -- "a's value is %d\n"
    local f ptr = create_arr(97, 39, 115, 32, 118, 97, 108, 117, 101, 32, 105, 115, 32, 37, 100, 10)
    printf(f, a)
    free(a)
}
```
