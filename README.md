# chttpserver

Minimal standalone HTTP server application framework implemented in C

## Requirements

List of essential packages to use and/or develop the project:

- make
- gcc
- bash

## Thoughts

The very idea of this repository is to build a minimal foundation of code that serves as a bootstrap kit for those unfortunate enough to do web programming in C.

Reasons and motivations for this choice completely left in the hands of the receiving developer. Project aims to mature into a compact and programmable web server/proxy with minimal overhead.

Regarding the make tools: in case of very easy and compatible integration, `cmake` might be introduced as an alternative to `make`

`gcc` version requirements are loose, project hasn't anchored upon a specific version and standard of C

`gcc` can be interchanged with another compiler but then `Makefile` adjustments are required

## How to

### Build the project
The build (compilation) directive of Makefile sets up additional metadata (in form of neighbouring .d file) for the source files so recompiling of unchanged source code can be avoided
```bash
make        # runs the first directive "all" which builds the project
```

### Run the project
```bash
make run    # runs the "run" directive which depends on the "all" directive
```