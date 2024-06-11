## That Lang

### Build

```sh
mkdir build && cd $_ && \
cmake .. && make all test
```

Check `cat Makefile | grep \.PHONY` for more options.

### TODO

- [ ] Read as needed

	Instead of lexing the text in the memory,
	receive a stream and read char by char, storin' in memory
	only what is needed with a string builder.
