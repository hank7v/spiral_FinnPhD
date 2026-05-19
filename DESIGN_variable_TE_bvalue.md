# Design: Variable TE and b-value per Repetition (Linear Diffusion Mode)

## Status: DRAFT — timing analysis complete, awaiting confirmation of approach before implementation

---

## Background

`lm_Spiral_MultiSE` is a spiral readout DW-SE IDEA sequence for a Siemens VD11D platform.
The "linear" diffusion mode reads gradient direction vectors `(Gx, Gy, Gz)` from
`CustomerSeq/spiral_FWF/DiffDir_Spiral.txt`, one row per repetition, looping via
`m_mySeqLoop` with a diffusion loop of length `m_lNoDiffDir`.

Currently:
- Gradient **direction** varies per repetition (from file).
- Gradient **magnitude** is constant — set by the user on the special card (`WIP_dDiffGradAmpl_Pos`).
- **TE** is constant — always `rMrProt.te()[0]`.
- **b-value** is constant — it follows from fixed magnitude, fixed timing.

The goal is to acquire a set of measurements where each repetition can have a
**different TE and b-value**, with gradient magnitude derived from b-value and TE.

---

## Requirements

1. Each row in the acquisition table defines one repetition with:
   - Diffusion direction `(Gx, Gy, Gz)` — normalised unit vector
   - Target b-value `b` [s/mm²]
   - Target TE [µs]

2. The sequence derives the required gradient magnitude from b and TE automatically
   (given fixed `SmallDelta` δ and `BigDelta` Δ from the special card).

3. TR is fixed across all repetitions (scanner hardware requirement). It is
   determined by the **longest TE** in the table, which sets the most restrictive timing.

4. Fill times `m_alTEFil[0..4]` become **per-repetition** — computed once per direction
   in `prepare()`, stored, and indexed by `lDir` in `runKernel()`.

5. b-matrix written into MDH reflects the actual per-repetition b-value/direction.

6. The UI interaction changes:
   - **Remove**: user input of gradient amplitude (`WIP_dDiffGradAmpl_Pos`).
   - **Add** (or repurpose): the sequence computes amplitude from the file's b-value
     using the Stejskal-Tanner formula, given δ and Δ.
   - Gradient duration δ (`SmallDelta`) and diffusion time Δ (`BigDelta`) remain
     user-controlled on the special card.
   - The scanner enforces hardware gradient limits; the sequence must check that
     the derived amplitude does not exceed `m_dGradMaxAmpl` for any row.

---

## Table format and external generation (DECIDED)

### Table columns: `(TE, G, δ, Δ)` — not `(TE, b)`

The acquisition table uses `(TE, G, δ, Δ)` directly rather than b-value and TE.
This was decided in agreement with the PhD student developing the optimisation
framework:
- The optimiser works in the physical parameter space `(TE, G, δ, Δ)` and
  applies the constraint equations directly.
- Storing G, δ, Δ explicitly avoids any ambiguity in the Stejskal-Tanner
  inversion (ramp correction, exact b-value formula) between the optimiser and
  the sequence.
- The sequence can still compute and display b-values from these parameters,
  but does not need to invert them.

Each row also carries the diffusion direction `(Gx, Gy, Gz)` (normalised unit
vector). Full table columns:

```
# Gx   Gy   Gz   TE[us]   G[mT/m]   delta[us]   Delta[us]
  1    0    0    60000    45.0      12000       40000
  0    1    0    80000    55.0      12000       50000
  0    0    1    60000    30.0       8000       35000
  0    0    0    60000     0.0      12000       40000
```

### Table is generated externally in MATLAB

The table is produced by the PhD student's MATLAB optimisation framework, not
inside the sequence. The MATLAB code applies the four constraint equations
(using `tasym` and `t180` exported from a reference protocol run) and outputs
a validated table file. The sequence does not need to solve an optimisation
problem — it only needs to read and verify the table.

### Sequence role: soft verification and adaptation, not hard rejection

When running in table mode, `prepare()` reads the table and **double-checks**
each row against the live protocol timing (`TR`, `t180`, `tp`, `tasym`). If a
row is found incompatible (e.g. the protocol was modified after the table was
generated), the sequence **adapts** the row rather than returning `SEQU_ERROR`:
- Clamp δ to `[δmin, δmax]`.
- Clamp Δ to `[Δmin, Δmax]`.
- Recompute b-value from clamped parameters.
- Report the adapted table in the tooltip/WIP display fields so the user can
  see what was changed.

Hard errors (`SEQU_ERROR`) are only returned for truly unrecoverable situations
(e.g. file not found, G exceeds hardware limit after clamping).

### Single-acquisition vs table mode

The sequence will offer two modes selectable on the special card:

| Mode | Behaviour |
|---|---|
| **Single** | Existing logic: user sets TE, G, δ, Δ on the UI; `prepare()` checks and the framework constrains sliders interactively. |
| **Table** | New logic: UI inputs for G, δ, Δ are hidden/disabled; the table file is read; `prepare()` does soft verification and adaptation. |

The UI check of WIP inputs (amplitude, δ, Δ sliders) only occurs in Single mode.

---

## Key sequence logic changes

### In `prepare()` — `lm_Spiral_MultiSE.cpp`

1. **Mode check**: read a WIP boolean flag for table vs single mode.
   In single mode, existing logic runs unchanged.

2. **Read table** from file. Columns: `(Gx, Gy, Gz, TE, G, δ, Δ)`.
   Store per-row vectors in member variables:
   `m_vlTE`, `m_vdG`, `m_vlSmallDelta`, `m_vlBigDelta`.
   Direction factors already handled by `m_myDiff` / `ReadExternalDiffDir`
   (same as today, same file path mechanism).

3. **Soft verification and adaptation** per row — apply the four constraint
   checks using live `tasym`, `t180`, `tp` from the current protocol.
   Clamp δ and Δ into valid ranges if needed. Log any clamped rows to WIP
   tooltip fields.

4. **Determine TR-governing TE**: `lMaxTE = max over all rows of m_vlTE`.
   Use `lMaxTE` everywhere `rMrProt.te()[0]` currently drives `lScanTime`
   and `lSyncScanDuration` (lines 1625, 1702).

5. **Per-row fill times**: for each row `i`, compute `lTimeBeforeRefoc[i]`
   and `lTimeAfterRefoc[i]` from `m_vlTE[i]`, then compute
   `m_alTEFil[0..3][i]` using `m_vlSmallDelta[i]` and `m_vlBigDelta[i]`.
   Store as `std::vector` indexed by direction.

6. **Per-row ramp time**: `lDiffRampTime[i] = m_vdG[i] * minRiseTime`.
   Check `lDiffRampTime[i] ≤ m_vlSmallDelta[i]` (check L1221 equivalent).
   Clamp G downward if needed (soft adaptation).

7. **Gradient check loop** (lines 1325–1347): iterate over all rows using
   per-row `m_vdG[i]`, `m_vlSmallDelta[i]`, `m_vlBigDelta[i]`.

### In `runKernel()` — `lm_Spiral_MultiSE.cpp`

1. Index all per-row quantities by `lDir`: `m_vlTE[lDir]`, `m_vdG[lDir]`,
   `m_vlSmallDelta[lDir]`, `m_vlBigDelta[lDir]`.
2. Use `m_alTEFil[k][lDir]` for all fill time references (lines 2144–2323).
3. Set gradient amplitude from `m_vdG[lDir]` before `prepAmplitude()` calls.
4. b-matrix in MDH is computed from the per-row `(G, δ, Δ)` — correct by
   construction since those values are stored explicitly.

### In `lm_Spiral_MultiSE.h`

- Add `std::vector<long> m_vlTE`
- Add `std::vector<double> m_vdG`
- Add `std::vector<long> m_vlSmallDelta`
- Add `std::vector<long> m_vlBigDelta`
- Change `m_alTEFil[0..3]` from scalar `long` to `std::vector<long>`
  (one element per direction); `m_alTEFil[4..10]` remain scalar.
- Add `bool m_bTableMode`

### UI changes — `lm_Spiral_MultiSE_UI.cpp`

- Add a **mode selector** (Single / Table) on the special card.
- In **Table mode**: hide G, δ, Δ sliders; show table filename field and
  a read-only tooltip displaying the adapted table (any clamped rows flagged).
- In **Single mode**: existing UI unchanged.
- Future: integrate **UIF (UI Factory)** composite pattern to host these
  two sets of parameters as distinct dynamic cards, replacing the current
  vector-element trick. This is an independent improvement that can be added
  after the table logic is validated.

---

## What stays the same

- The diffusion loop structure via `m_mySeqLoop` — same counter, same length.
- RF pulses, slice selection, readout (spiral), ADC — unchanged.
- Free-waveform (`m_bIsFreeDiffusion`) path — untouched by this change.
- `myDiffusionNumbers`, `ReadExternalDiffDir` — unchanged in Option A.

---

## File format proposal (Option A)

`DiffDir_Spiral.txt` — unchanged:
```
# Normalised diffusion directions (Gx Gy Gz in PRS)
1 0 0
0 1 0
0 0 1
0 0 0
```

New `TEb_Spiral.txt` — same number of rows:
```
# TE [us]   b [s/mm2]
60000       1000
80000       2000
100000      3000
60000       0
```

Row order must match between the two files. Row with b=0 is the b0 image
(TE still applied, gradient amplitude will be 0 or near-zero).

---

---

## Timing analysis: where TE couples into the sequence

### The two root variables (lines 1197–1200 of `prepare()`)

All fill times and feasibility checks flow from splitting TE in half around the
180° refocusing pulse:

```
lTimeBeforeRefoc = TE/2 - GradSliSel/2 - GradSliSelReph - GradSliRefoc/2
lTimeAfterRefoc  = TE/2 - GradSliRefoc/2 - m_lReadoutBeforeEcho
lTimeRefoc       = GradSliRefoc  (hardware only, TE-independent)
```

### The five TE-dependent fill times

These are computed from `lTimeBeforeRefoc` / `lTimeAfterRefoc` and the user
gradient parameters δ, Δ, `lDiffRampTime`:

| Fill time | Formula | Physical role |
|---|---|---|
| `m_alTEFil[0]` | `lTimeBeforeRefoc - δ - lDiffRamp - TEFil[1]` | gap between slice-rephase end and 1st diff grad start |
| `m_alTEFil[1]` | `Δ - δ - lDiffRamp - lTimeRefoc - TEFil[2] - lCrush` | gap between 1st diff grad end and 180° pulse start |
| `m_alTEFil[2]` | `max(lCrush, Δ - lTimeBeforeRefoc - lTimeRefoc - lCrush)` | gap after 180° pulse before 2nd diff grad start |
| `m_alTEFil[3]` | `lTimeAfterRefoc - δ₂ - lDiffRamp - TEFil[2]` | gap after 2nd diff grad end before readout |
| `m_alTEFil[10]`| vibration gradient fill (TE-derived, optional path only) | — |

`m_alTEFil[4..8]` are **multi-echo spacing** fills — **TE-independent**, unchanged.

### Feasibility checks that gate `prepare()` return values

| Line | Condition | TE-dependent? | Error returned |
|---|---|---|---|
| 1221 | `lDiffRampTime > δ` | No (amplitude/rise-time only) | `SEQU_ERROR` |
| 1227 | `Δ < lTimeRefoc + δ + lDiffRamp` | No | `SEQU_ERROR` |
| 1234 | `δ + lDiffRamp + lCrush > lTimeBeforeRefoc` | **Yes** (∝ TE/2) | `SBB_NEGATIV_TEFILL` |
| 1241 | `δ₂ + lDiffRamp + max(lCrush, Δ−…) + cameraDelay > lTimeAfterRefoc` | **Yes** | `SBB_NEGATIV_TEFILL` |
| 1248 | `Δ + δ₂ + lDiffRamp > lTimeBeforeRefoc + lTimeRefoc + lTimeAfterRefoc` | **Yes** (right side = TE − hardware) | `SBB_NEGATIV_TEFILL` |
| 1256 | `lCrushTime > δ + lDiffRamp` | No | `SEQU_ERROR` |

Checks at lines 1234, 1241, 1248 must be run **per row** with each row's TE.
If any row fails, `prepare()` must return the appropriate error.

### Additional coupling: `lDiffRampTime` depends on amplitude

```
lDiffRampTime = G × minRiseTime
```

Since G varies per row (derived from b-value), `lDiffRampTime` also varies.
Check L1221 (`lDiffRampTime > δ`) must therefore be evaluated **per row** too.

### Where `rMrProt.te()[0]` is used directly in `prepare()`

| Line | Context | Treatment in new design |
|---|---|---|
| 386 | WIP display write (informational) | Write max TE or leave as-is |
| 1197/1199 | **Core**: compute `lTimeBeforeRefoc`, `lTimeAfterRefoc` | Replace with `lTE[i]` inside per-row loop |
| 1204 | Debug print | Update print |
| **1625** | **`lScanTime`** — passed to `TrTiFillTimes()`, sets TR minimum | Use `lMaxTE` (longest TE in file) |
| 1685 | Debug print | Update print |
| **1702** | **`lSyncScanDuration`** — sync scan timing | Use `lMaxTE` |

### `lScanTime` and TR

```cpp
lScanTime = lScanTimeSBBs + GradSliSel/2 + rMrProt.te()[0] + m_alTEFil[4]
            + readout_terms + spoiler   // line 1625
```

`lScanTime` is **linearly proportional to TE**. The framework passes this to
`TrTiFillTimes()` to enforce TR ≥ TRmin. With variable TE, this must use
`lMaxTE = max over all rows`, so TR is sized for the worst case and each
shorter-TE row gets extra `m_alTEFil[3]` fill to pad to the same TR.

### `runKernel()` — no direct `rMrProt.te()[0]`

`runKernel` never calls `rMrProt.te()[0]` for timing. It only consumes
pre-computed `m_alTEFil[]` values at lines 2144, 2165, 2193, 2233, 2243, 2323.
This is the key architectural advantage: once `m_alTEFil[k]` becomes a
per-direction array, `runKernel` just indexes `m_alTEFil[k][lDir]` with no
other changes needed to the kernel timing logic.

### What is TE-independent (computed once)

- Hardware durations: `GradSliSel`, `GradSliSelReph`, `GradSliRefoc` (180° pulse)
- `lTimeRefoc` (the 180° pulse window)
- δ (`wiplSmallDelta`), Δ (`wiplCapitalDelta`) — user card inputs, same for all rows
- Feasibility checks L1221, L1227, L1256
- `m_alTEFil[4..8]` (multi-echo spacing)
- Gradient shape/timing setup (same δ, Δ for all rows)

### What is TE-dependent (per row from file)

- `lTimeBeforeRefoc`, `lTimeAfterRefoc` → from `TE[i]/2`
- `m_alTEFil[0]`, `[1]`, `[2]`, `[3]`, `[10]` — stored as per-direction arrays
- Feasibility checks L1234, L1241, L1248
- `lScanTime` / `lSyncScanDuration` → use `lMaxTE`
- Gradient amplitude G → from b-value inversion (Stejskal-Tanner)
- `lDiffRampTime` → from G × minRiseTime (per row)

---

---

## Sequence timing diagram and constraint derivation

The diagram below (also in `timing_diagram_EPI.md`) shows the two readout
variants side by side. Both share the same RF line and the same diffusion
gradient structure — only the readout gradient waveform and its position
relative to TE differ.

```
               |<────────────────── TE/2 ─────────────>|<──────────── TE/2 ─────────────────>|

                 90°                               180°
                                                                                  ADC / readout
RF             /\                                     /\                |‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|
              /  \                                   /  \               |                                    |
             /    \                                 /    \              |                                    |
────────────/      \───────────────────────────────/      \─────────────|────────────────────────────────────|


            |‾‾‾‾‾‾| EPI prep                     |‾‾‾‾‾‾‾|
            | slice|   /\                         | slice |
GRAD  ──────| sel  |──/  \────────────────────────| refoc |────────────────────── /‾\ /‾\ /‾\ /‾\ /‾\ /‾\ /‾\ /‾\ /‾\ /‾\
                   |_|    |‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|     |       |‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|
                  rephase | diff grad 1 (G) |     |       |  diff grad 2 (G)|              EPI readout
                          |_________________|     |       |_________________|
                <---tp--->|<──── δmax + tasym ───>|       |<────── δmax ────────>|<-- tepi -->
                          |<──── δ ──────────>|           |<───── δ ────────>|
                          |<──────── Δ ──────────────────>|
                                                 ->| t180 |<-


                                                                                                    /\        /\
            |‾‾‾‾‾‾|                              |‾‾‾‾‾‾‾|                               /\      /  \      /  \
            | slice|                              | slice |                              /  \    /    \    /
GRAD  ──────| sel  |──────────────────────────────| refoc |─────────────────────────────/    \  /      \  /
                   |_|    |‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|             |‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|                \/        \/
                  rephase |  diff grad 1 (G)|             | diff grad 2 (G) |          spiral readout (growing ampl)
                          |_________________|             |_________________|
                <-tp-><──────── δmax ────────────>        |<────── δmax + tasym ─────────────>
                          |<──── δ ────────>|             |<──────── δ ────────>|
                          |<─────────────────── Δ ───────>|
```

### Constraints (same form for both EPI and Spiral)

```
δmin = 2 · risetime · G          — ramp must fit inside flat top
δmax = TE/2 - t180/2 - tasym     — flat top must fit in available half-TE window
Δmin = t180 + δ                  — Δ must span at least the 180° pulse plus one δ
Δmax = TE - tasym - δ            — Δ cannot push grad 2 past the readout start
```

### The asymmetry parameter `tasym`

`tasym` captures the net timing asymmetry between the left and right windows
around the 180° pulse, regardless of which side is larger. The constraint
equations are identical in form for both readout types — only the numerical
value of `tasym` differs. Once `tasym` is known for a given readout type and
protocol, the four constraint equations apply unchanged. The direction of the
asymmetry (which side has more room) does not affect the formal dependency.

### Consequence for table validation

Given a table row with `(TE, G, δ, Δ)` (see Table Format below):

1. Check `δ ≥ δmin(G)` — ramp fits inside flat top.
2. Check `δ ≤ δmax(TE)` — flat top fits in the available half-TE window.
3. Check `Δ ≥ Δmin(δ)` — Δ spans at least the 180° pulse plus one δ.
4. Check `Δ ≤ Δmax(TE, δ)` — Δ does not push grad 2 past the readout start.

All four checks are scalar inequalities in `(TE, G, δ, Δ)` — one row at a time.

---

## The prepare() validation challenge

### Current logic recap

In the current implementation:
- The user selects a single TE on the UI.
- The user is offered a range of compatible Δ, δ, and gradient amplitude G
  (noting that G is not purely hardware-limited — `lDiffRampTime = G × minRiseTime`
  must fit inside δ, so amplitude and timing are coupled).
- `prepare()` checks feasibility once for this single combination and returns
  errors that constrain the UI sliders interactively.
- The diffusion loop iterates only over direction, with all timing fixed.

### What we want

An outer loop (new dimension in the acquisition table) where each row can have
its own TE, Δ, δ, and G (derived from b-value). The table replaces the single
UI-selected combination with a list of N combinations.

### The core challenge

`prepare()` is called once per protocol update and performs a single feasibility
check. With a table of N rows, there is no longer one combination to check —
there are N, and they may impose different constraints. The IDEA framework's
interactive UI feedback mechanism (return error → slider range narrows) cannot
operate row-by-row.

Three strategies were considered:

---

### Strategy 1 — Check only the most restrictive TE

Run the existing feasibility checks using only `min(TE)` from the table (shortest
TE is the hardest to satisfy). If δ and Δ pass for the shortest TE, they will
pass for all longer TEs.

**Problem:** gradient amplitude G (and therefore `lDiffRampTime`) also varies
per row, driven by b-value. A row with a short TE may require a modest b-value
and therefore a modest G — while a row with a long TE may require a high b-value
and a large G with a long ramp. Checking only the shortest-TE row does not catch
violations in other rows. The checks are not monotone in TE alone.

**Verdict:** insufficient without also knowing the worst-case G.

---

### Strategy 2 — Expose all per-row Δ, δ on the UI

Allow the user to set Δ and δ independently for each row via the WIP parameter
block, so `prepare()` can check each combination.

**Problem:** the IDEA WIP parameter block has a hard limit on the number of
parameters. With N rows (potentially 20–100), exposing Δ and δ per row is not
feasible. The UI cannot support it.

**Verdict:** not implementable.

---

### Strategy 3 — User defines the worst-case combination; table must be consistent (Recommended)

The user specifies on the UI a single "governing" combination:
- **Shortest TE** they intend to use in the table.
- **Highest b-value** they intend to use in the table.

`prepare()` checks feasibility for this worst-case combination:
- Shortest TE → most restrictive `lTimeBeforeRefoc` and `lTimeAfterRefoc`.
- Highest b-value → largest G → longest `lDiffRampTime` → hardest to fit in δ.

If this combination is feasible, all other rows in the table are guaranteed to
be feasible too, because:
- Longer TEs only increase `lTimeBeforeRefoc` and `lTimeAfterRefoc` (more room).
- Lower b-values only decrease G and `lDiffRampTime` (easier to fit in δ).

The table file is then loaded and each row's G is derived from its b-value.
At load time, a runtime check verifies that no row's G exceeds `m_dGradMaxAmpl`
and that each row's `lDiffRampTime` fits inside the δ for that row (if δ is
fixed, this is guaranteed by the UI check on max b; if δ varies per row,
the max-b row must use the minimum δ of the table).

**Important constraint on the table:** if δ and Δ are fixed (same for all rows,
set on the UI), then the user only needs to ensure the (shortest TE, highest b)
pair is feasible. The sequence enforces this at `prepare()` time. Any table row
that violates a hardware limit at runtime would cause a `SEQU_ERROR` in
`runKernel()`, which is a safety net but not a user-friendly one — so the
documentation / user guide for the table format must state the constraint clearly.

**UI inputs required under Strategy 3:**
- Δ (`WIP_lBigDelta`) — single value, same for all rows.
- δ (`WIP_lSmallDelta`) — single value, same for all rows.
- Shortest TE in table — either read automatically from the file and used
  internally, or entered manually as a UI field for the check.
- Highest b-value in table — same: read from file or entered manually.

Reading both automatically from the file is cleaner and removes the risk of
user entry errors. `prepare()` would scan the loaded table for `min(TE)` and
`max(b)`, then run all feasibility checks against that combination.

**Verdict: recommended approach.**

---

### Consequence for `m_alTEFil` storage

Under Strategy 3, δ and Δ are fixed scalars (same for all rows). Only TE and G
vary per row. This simplifies the per-row computation:
- `lTimeBeforeRefoc[i]` and `lTimeAfterRefoc[i]` — computed from `TE[i]`.
- `m_alTEFil[0..3][i]` — computed per row from `lTimeBeforeRefoc[i]`,
  `lTimeAfterRefoc[i]`, and the fixed δ, Δ.
- `lDiffRampTime[i]` — computed from `G[i] × minRiseTime`.
- `G[i]` — derived from `b[i]` via Stejskal-Tanner inversion with fixed δ, Δ.

The check `lDiffRampTime[i] ≤ δ` (L1221 analogue) is guaranteed for all rows
by the `prepare()` check on `max(b)`, provided δ is fixed.

---

---

## Implementation plan

### Step 1 — New file reader for the acquisition table

Create a new lightweight reader class (or free functions inside
`lm_Spiral_MultiSE.cpp` for now) that reads the table file.

**Filename (fixed for now):** `CustomerSeq/spiral_FWF/AcqTable_Spiral.txt`

**File format:**
```
# Gx   Gy   Gz   TE[us]   G[mT/m]   delta[us]   Delta[us]
  1    0    0    60000    45.0       12000        40000
  0    1    0    80000    55.0       12000        50000
  0    0    1    60000    30.0        8000        35000
  0    0    0    60000     0.0       12000        40000
```

**Reader design:** Use `FileReadBase` (7 columns) as the base — it already
handles `#` comment lines and whitespace-separated values. A thin wrapper or
standalone function reads the file and populates:
- `std::vector<double> vGx, vGy, vGz` — direction unit vector components
- `std::vector<long>   vTE` — echo time [µs]
- `std::vector<double> vG` — gradient amplitude [mT/m]
- `std::vector<long>   vDelta` — δ, gradient duration [µs]
- `std::vector<long>   vBigDelta` — Δ, diffusion time [µs]

Note: the direction columns `(Gx, Gy, Gz)` may alternatively come from the
existing `DiffDir_Spiral.txt` file (keeping the current mechanism) if the user
wants to repeat a full direction set at each `(TE, G, δ, Δ)` combination.
This choice can be deferred — the reader should handle both cases.

---

### Step 2 — Per-row compatibility check chain

This is the core validation function, called once per table row during
`prepare()`. It maps the four constraint equations from the timing diagram
onto the sequence's concrete variables.

#### Sequence constants (computed once before the row loop)

From the current code, the hardware-fixed quantities are:

| Symbol | Code expression | Meaning |
|---|---|---|
| `t180` | `m_sGSliRefoc.getTotalTime()` | Full duration of 180° refoc pulse (`lTimeRefoc`) |
| `tp` (spiral) | `m_sGSliSel.getTotalTime()/2 + m_sGSliSelReph.getTotalTime()` | Slice sel + rephase = left-side fixed time |
| `tasym` (spiral)| `tp` = above | Spiral asymmetry = only the prep time |
| `tasym` (EPI) | `tepi - tp` | EPI asymmetry = readout duration minus prep |
| `minRiseTime` | `wipdDiffRiseTime1` (user card, [µs/mT/m]) | Slew-rate-limited rise time per unit amplitude |
| `lFieldCameraDelay` | `510` [µs] | Fixed hardware delay for field camera trigger |

**Concrete expressions for `lTimeBeforeRefoc` and `lTimeAfterRefoc` in terms of
the diagram symbols** (spiral case, from lines 1197–1200):

```
lTimeBeforeRefoc = TE/2 - t180/2 - tp
                 = TE/2 - m_sGSliRefoc.getTotalTime()/2
                         - m_sGSliSel.getTotalTime()/2
                         - m_sGSliSelReph.getTotalTime()
                 = TE/2 - t180/2 - tasym      [diagram notation]

lTimeAfterRefoc  = TE/2 - t180/2 - m_lReadoutBeforeEcho
                 = TE/2 - t180/2 - tasym      [tasym = m_lReadoutBeforeEcho for spiral]
```

Note: `m_lReadoutBeforeEcho` is the spiral-specific `tasym` on the right side.
`tp` (slice prep) is the `tasym` on the left side. In general `tasym` may differ
left vs right — the constraint equations use whichever side is relevant.

#### Per-row check chain (pseudocode)

```cpp
struct TableRow {
    double Gx, Gy, Gz;   // direction (unit vector)
    long   TE;            // [us]
    double G;             // [mT/m]
    long   delta;         // δ [us]
    long   bigDelta;      // Δ [us]
};

struct SequenceConstants {
    long t180;            // m_sGSliRefoc.getTotalTime()
    long tp;              // m_sGSliSel.getTotalTime()/2 + m_sGSliSelReph.getTotalTime()
    long tReadoutBefore;  // m_lReadoutBeforeEcho  (tasym right side)
    double minRiseTime;   // wipdDiffRiseTime1  [us/(mT/m)]
    double gradMaxAmpl;   // m_dGradMaxAmpl  [mT/m]
    long fieldCamDelay;   // 510 us
};

bool checkRow(const TableRow& row, const SequenceConstants& sc,
              TableRow& adapted, std::string& report)
{
    long lDiffRampTime = fSDSRoundUpGRT(row.G * sc.minRiseTime);   // [us]

    // --- Compute available windows ---
    long lTimeBeforeRefoc = row.TE/2 - sc.t180/2 - sc.tp;
    long lTimeAfterRefoc  = row.TE/2 - sc.t180/2 - sc.tReadoutBefore;

    // --- Check 1: ramp fits inside δ ---
    // δmin = 2 * rampTime  (ramp up + ramp down, flat top ≥ 0)
    // Equivalent: lDiffRampTime ≤ δ
    if (lDiffRampTime > row.delta) {
        // clamp: reduce G until ramp fits, recompute G from clamped delta
        adapted.delta = row.delta;  // keep delta, reduce G
        adapted.G = row.delta / sc.minRiseTime;  // max G for this delta
        report += "Row: G clamped from ramp constraint; ";
    }

    // --- Check 2: δ fits in available half-TE window (left side) ---
    // δmax = lTimeBeforeRefoc   (left window)
    if (row.delta + lDiffRampTime > lTimeBeforeRefoc) {
        adapted.delta = lTimeBeforeRefoc - lDiffRampTime;
        if (adapted.delta < lDiffRampTime) adapted.delta = lDiffRampTime; // minimum
        report += "Row: δ clamped to left window; ";
    }

    // --- Check 3: Δmin — Δ must span 180° pulse plus δ ---
    // Δmin = t180 + δ
    long lDeltaMin = sc.t180 + row.delta + lDiffRampTime;
    if (row.bigDelta < lDeltaMin) {
        adapted.bigDelta = lDeltaMin;
        report += "Row: Δ increased to Δmin; ";
    }

    // --- Check 4: δ fits in right window (after 180°) ---
    // Equivalent to lTimeAfterRefoc constraint (line 1241):
    // δ + ramp + max(crush, Δ-lTimeBeforeRefoc-t180-crush) + camDelay ≤ lTimeAfterRefoc
    long lOvershoot = std::max(0L, row.bigDelta - lTimeBeforeRefoc - sc.t180);
    if (row.delta + lDiffRampTime + lOvershoot + sc.fieldCamDelay > lTimeAfterRefoc) {
        // Reduce Δ to remove overshoot
        adapted.bigDelta = lTimeBeforeRefoc + sc.t180;  // Δmax before overshoot
        report += "Row: Δ reduced to fit right window; ";
    }

    // --- Check 5: Δmax — Δ + δ must not exceed total TE window ---
    // (line 1248): Δ + δ + ramp ≤ lTimeBeforeRefoc + t180 + lTimeAfterRefoc = TE - hardware
    long lTETotal = lTimeBeforeRefoc + sc.t180 + lTimeAfterRefoc;
    if (row.bigDelta + row.delta + lDiffRampTime > lTETotal) {
        adapted.bigDelta = lTETotal - row.delta - lDiffRampTime;
        report += "Row: Δ clamped to TE window; ";
    }

    // --- Check 6: hardware amplitude limit ---
    if (adapted.G > sc.gradMaxAmpl) {
        adapted.G = sc.gradMaxAmpl;
        report += "Row: G clamped to hardware max; ";
    }

    // return false if any adaptation was needed (caller logs to tooltip)
    return report.empty();
}
```

#### Fill time computation per row (after checks pass)

```cpp
// These are the m_alTEFil values for this direction, stored in vectors:
long lTEFil2 = std::max(0L, row.bigDelta - lTimeBeforeRefoc - sc.t180);
long lTEFil1 = row.bigDelta - row.delta - lDiffRampTime - sc.t180 - lTEFil2;
long lTEFil0 = lTimeBeforeRefoc - row.delta - lDiffRampTime - lTEFil1;
long lTEFil3 = lTimeAfterRefoc  - row.delta - lDiffRampTime - lTEFil2;
```

These replace the scalar `m_alTEFil[0..3]` in the current code.

#### Integration into `prepare()`

```
1. Compute SequenceConstants (once, after all gradient/pulse prep calls)
2. Read table file → vector of TableRow
3. lMaxTE = max(row.TE) across all rows  → use for lScanTime, lSyncScanDuration
4. For each row i:
      checkRow(row[i], sc, adapted[i], report[i])
      compute lTEFil[0..3][i] from adapted[i]
      compute lDiffRampTime[i] from adapted[i].G
5. Write adapted table summary to WIP tooltip field
6. Store m_vlTE, m_vdG, m_vlDelta, m_vlBigDelta, m_vlTEFil as member vectors
7. Continue with existing prepare() logic using lMaxTE where rMrProt.te()[0] was used
```

---

## Next steps

### Decided
- [x] Table columns: `(Gx, Gy, Gz, TE, G, δ, Δ)` — not `(TE, b)`.
- [x] Table generated externally in MATLAB by the optimisation framework.
- [x] Sequence does soft adaptation (clamping), not hard rejection, on table rows.
- [x] Single vs Table mode selector on the special card.
- [x] `tasym` is the single sequence-specific parameter for constraint equations.
- [x] TE unit: µs (matching `rMrProt.te()[0]`).

### Still open
- [ ] Exact table file format details: separator (space/tab/comma), header
      comment syntax, units for G (mT/m assumed), column order.
- [ ] How clamped rows are reported to the user — tooltip field indices to use.
- [ ] `rSeqLim.setTE()` bounds: should they be widened or bypassed in table mode
      (since TE comes from the file, not the UI slider)?
- [ ] `Check()` iteration: currently checks only 1 direction. In table mode it
      should check at minimum the row with the smallest TE and the row with the
      largest G (highest `lDiffRampTime`).
- [ ] UIF (UI Factory) integration: composite card pattern to cleanly separate
      Single and Table parameter groups. Planned as a follow-on after table
      logic is validated on the scanner.
