#PCCS unit/integration test executable
## Introduction
This folder contains the unit/integration tests for libPCCS

We are not particularly striving for 100% test coverage,
but it's useful to have this when reworking/refactoring/debugging code.

It's based on the [Catch2](https://github.com/catchorg/Catch2) framework

# Building
make

# Usage
See help message:
```./pccs_test --help```

Run every test:

```./pccs_test```

Run tests with a specific tag (for example "gen1"):

```./pccs_test \[gen1\]

Show checks with results even when tests passed:
```./pccs_test --success
