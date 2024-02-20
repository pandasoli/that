## Parsing ensurance
When the project has a stable release the CLI should compile only with an ensurance file
to prevent unexpected parsing issues if it's not sure the parsing it stable yet.

The project dev should generate an ensurance file for each file in the project making sure the AST is correct.
This will be done with the CLI's help (I'll still have to figure out how).

## Static or shared
You can define if you want a library to be static or shared linked.
