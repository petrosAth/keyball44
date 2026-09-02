# Host test guidance

## Test model

- Tests are standalone C11 programs built and run by `scripts/test.sh`. They
  exercise platform-independent firmware modules without mise, an Arm
  toolchain, or a Vial-QMK checkout.
- Use standard `assert` checks and small, behavior-named `static void` test
  functions called from `main`. Keep tests deterministic and independent of
  hardware, wall-clock time, networking, and filesystem state.
- Compile production `.c` files directly rather than duplicating their logic.
  Add only the smallest QMK API surface needed under `tests/stubs/`.

## Coverage and registration

- Test public behavior and invariants, not private implementation details.
  Cover zero and invalid inputs, lower and upper bounds, exact transition
  timestamps, timer wraparound when relevant, normalization, saturation, and
  independent simultaneous state.
- Wire every new test program into `scripts/test.sh`. Retain the strict
  `-std=c11 -Wall -Wextra -Werror -pedantic` compilation contract and include
  the exact production sources the test needs.
- When changing a packed or cross-half structure, test its size and a byte-copy
  round trip as well as semantic fields.

Run `make test` after any firmware engine, diagnostic state, stub, test, or test
runner change. A passing test added outside `scripts/test.sh` does not count as
part of the suite.
