# API contract, 0.1

- Coordinates: host-normalized display DIPs; timestamps: monotonic report
  seconds, not render-frame time. No physical millimeter calibration is implied.
- `Settings`: defaults enabled, radius 12 DIP, window .120 seconds, cap 4 DIP.
  Accepted analysis ranges: radius (0,100], window (0,1], cap [0,50]. Invalid
  settings throw `std::invalid_argument` before reset changes state. Hosts may
  impose narrower UI ranges. A zero cap or disabled filter passes through points.
- `reset(settings)` starts a new empty open stroke. `append(sample, continuous)`
  consumes one chronological contact sample. Width is an opaque nonnegative
  mapped attribute; it is never changed by the filter.
- Nonfinite position/time/width or negative width returns false without adding
  a sample. `changedBegin()` then equals output size. The caller must record
  that rejection, not pretend every input has an output index. For accepted
  samples, source and output indices are one-to-one, including stationary points.
- Equal/backward time, a gap greater than 50 ms, or `continuous=false` starts a
  new filter run. It does not delete samples. The host must also split continuity
  on invalid clock provenance, device identity or coordinate-transform changes.
- `positions()` and `samples()` are read-only references owned by the instance;
  appends/reset can invalidate element pointers/iterators. Copy data if retaining
  it across calls. Source samples remain unchanged until reset.
- After an accepted append, rebuild geometry starting at `changedBegin()`.
  Indices strictly below `committedPrefix()` will not change on future appends.
  The last accepted contact point remains exact; the tail can revise before
  joining that prefix. The displacement cap bounds position changes, not mesh
  thickness or an application's interpolation overshoot.
- `finish()` and `cancel()` freeze all output and reject further append calls
  until reset. They do not append a lift point or reshape the path. Cancel does
  not automatically discard geometry: that decision belongs to the host.
- No callbacks, global mutable state, or OS/renderer dependencies. Each stroke
  uses its own instance. Concurrent access to one instance needs host locking.
- Allocation failures propagate standard exceptions. After an allocation failure,
  discard/reset the instance before further use. This version does not promise
  fixed memory, hard-real-time timing or valid arithmetic for extreme numeric
  magnitudes outside ordinary display coordinates/report seconds.
- Batches are repeated appends in order; there is no separate batch policy.
  The host need only redraw once after a batch, taking the minimum changed index
  over all its accepted appends, not only the last call's changed index.

The local-normal formula and normal-use output behavior remain revision 1.
The public ranges are wider than InfiniPaint's 4–20/.040–.200/0–6 UI clamps,
allowing PenTraceLab to retain its existing diagnostic parameter range.
