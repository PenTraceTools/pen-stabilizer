# pen-stabilizer

Experimental, source-first C++17 local-normal pen position correction.
Package **0.1.0**, algorithm behavior revision **1**. MIT licensed.

This extracts the streaming filter previously embedded in InfiniPaint, derived
from PenTraceLab 0.4.0 (`ef6555a6defd12b8dde5afc408df4975eb4492b2`). It is not a
new pen driver, Windows service, DLL requirement, executable dependency, or Rust
rewrite. Compile the header into your application. No runtime downloads.

## Native integration

Pin this repository as a Git submodule or vendor a specific released source
revision (including LICENSE). Then:

```cmake
add_subdirectory(deps/pen-stabilizer)
target_link_libraries(your_app PRIVATE pen_stabilizer::pen_stabilizer)
```

Include `<pen_stabilizer/stabilizer.hpp>`. See [minimal.cpp](examples/minimal.cpp)
and the [API contract](docs/API.md). The CMake target is an INTERFACE target: it
does not generate a library binary. Including the header with an include path
also works without CMake. Consumers choose and lock updates; do not use a moving
branch as the dependency version.

## Behavior and limits

The core computes a symmetric arc-length-weighted local mean, projects its
offset onto the local normal, tapers at corners and endpoints, and caps movement.
It keeps the newest reported contact point exact and revises only a recent tail.
Older points freeze. Width attributes remain unchanged. No prediction, One Euro
filter, forced straight lines, or post-lift catch-up is used.

It cannot infer true physical motion or distinguish every intentional curve
from hardware wobble. More correction can soften detail. A 120 ms revision
window is not zero-delay recovery: the live tip is raw while recent geometry
can change. The current core retains whole-stroke arrays; memory grows with
stroke length, and time-window bounds do not bound sample count or rendering
cost. Benchmark in your own renderer before adopting it for long strokes.

The host owns pen input, clock/coordinate conversion, width mapping, rendering,
undo, document storage and cancel policy. An append-only renderer must support
a temporary revisable tail. Destructive erasing is not a safe drop-in consumer.

## Tests and provenance

CI runs the same live-prefix cases against the **independent pinned original**
PenTraceLab implementation at 60/120/240/672 Hz, plus invariant tests and an
integration example. Sanitizers run on Linux. These are synthetic regression
tests, not evidence of physical-pen accuracy. No user recordings are included.

To run on your build machine:

```sh
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

The oracle comparison additionally requires the pinned PenTraceLab source and
`-DPENTRACE_REFERENCE=/absolute/path/to/reference`. Core geometry derives from
the MIT PenTraceLab implementation; InfiniPaint-specific GPL UI/rendering code
is not included. Preserve [LICENSE](LICENSE) when redistributing source.

## Roadmap

First stabilize native API and measured performance with InfiniPaint and
PenTraceLab consuming the same source. Future bindings may use a narrow C ABI;
Rust wrappers or a Rust implementation are optional later decisions, not
prerequisites. This is a library with an API, not a universal protocol/framework.

Version API/package changes separately from behavior changes. Record both
`package_version` and `algorithm_revision`, parameters and coordinate units in
diagnostic results. See [CHANGELOG.md](CHANGELOG.md).
