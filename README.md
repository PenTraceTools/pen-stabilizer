# pen-stabilizer

[PenTraceTools organization](https://github.com/PenTraceTools) · [Shared versioning workflow](https://github.com/PenTraceTools/.github/blob/main/VERSIONING.md)

Experimental, source-first C++17 local-normal pen position correction.
Package **0.1.0**, algorithm behavior revision **1**. MIT licensed.

This extracts the streaming filter previously embedded in InfiniPaint, derived
from PenTraceLab 0.4.0 (`ef6555a6defd12b8dde5afc408df4975eb4492b2`). It is not a
new pen driver, Windows service, DLL requirement, executable dependency, or Rust
rewrite. Compile the header into your application. No runtime downloads.

## Real-pen comparison

![Same recorded diagonal: original and corrected positions with measured cross-track RMS](https://raw.githubusercontent.com/PenTraceTools/pen-trace-lab/main/docs/comparisons/test6/diagonal-analysis.png)

On this selected Surface Pro 11 / Metapen M2 stroke, whole-stroke straightness RMS
is **13.1% lower** (2.313 to 2.009 DIP). Small ripples are visibly reduced; broad
curvature remains. The vertical scale is expanded. This is batch replay evidence,
not a hardware-accuracy or live-latency claim.
[View the image gallery, exact settings and measurement method](https://github.com/PenTraceTools/pen-trace-lab/tree/main/docs/comparisons/test6).
Only images and aggregate results are public, not the original recording.

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

## Consumers

[PenTraceLab](https://github.com/PenTraceTools/pen-trace-lab) uses the batch source API
for its local-correction comparison. The [InfiniPaint fork](https://github.com/alexiokay/infinipaint-Custom/tree/graphite-ui)
and [upstream proposal #98](https://github.com/ErrorAtLine0/infinipaint/pull/98)
use the streaming API. Both currently pin the v0.1.0 source commit; their app
versions and releases are independent. A proposal is not an upstream merge.

[Real-pen comparison gallery](https://github.com/PenTraceTools/pen-trace-lab/tree/main/docs/comparisons/test6):
selected same-input raw/corrected examples, pressure/renderer variants and original SVGs.
This is supplementary replay evidence, not physical ground truth or full-app validation.
