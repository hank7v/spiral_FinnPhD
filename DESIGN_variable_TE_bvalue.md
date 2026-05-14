# Design: Variable TE and b-value per Repetition (Linear Diffusion Mode)

## Status: DRAFT — awaiting analysis of new UI/loop logic before implementation

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

## Open question: how to provide (b, TE) per repetition

Two approaches discussed:

### Option A — Separate TE/b file (safer, minimal impact on existing classes)

- Keep `DiffDir_Spiral.txt` with 3 columns `(Gx, Gy, Gz)` unchanged.
- Add a second file, e.g. `TEb_Spiral.txt`, with 2 columns `(TE_us, b_value)`,
  same number of rows as `DiffDir_Spiral.txt`.
- Read the second file inside `lm_Spiral_MultiSE.cpp` only, using a simple
  `ReadSingleColumn`-style reader or a new minimal class. No changes to
  `ReadExternalDiffDir`, `myDiffusionNumbers`, or any helper class.
- **Pro:** zero risk of corrupting existing logic. All changes stay in the sequence file.
- **Con:** two files must stay in sync; more files for the user to manage.

### Option B — Derive a new class from `ReadExternalDiffDir`

- `ReadExternalDiffDir` is constructed with `FileReadBase(3)` — fixed 3-column reader.
- `FileReadBase` supports a constructor `FileReadBase(long lColumns)` and a
  `setNoColumns()` method, so the infrastructure for more columns already exists.
- A derived class (e.g. `ReadDiffDirWithTE`) would override `readFile()` to use
  4 columns and expose `getTE(long lDir)` and `getBValue(long lDir)`.
- `ReadExternalDiffDir` uses `virtual readFile()` and `virtual interpretHeader()`,
  so derivation is supported.
- **Pro:** single file, clean OO extension, consistent with existing architecture.
- **Con:** requires changes to `ReadExternalDiffDir.h`, `myDiffusionNumbers.h/.cpp`
  to thread the new class through, or a parallel member in the sequence.

**Preferred approach (to confirm):** Option A for a first working version,
keeping all new logic inside `lm_Spiral_MultiSE.cpp` and a new lightweight reader.
Option B can be a later refactor once the logic is validated on the scanner.

---

## Key sequence logic changes (once approach is confirmed)

### In `prepare()` — `lm_Spiral_MultiSE.cpp`

1. **Read TE/b table** from file (Option A) or extended direction file (Option B).
   Store as `std::vector<long> m_vlTE` and `std::vector<double> m_vdBTarget`.

2. **Determine TR-governing TE**: `lMaxTE = *std::max_element(m_vlTE.begin(), m_vlTE.end())`.
   All timing calculations that currently use `rMrProt.te()[0]` for TR sizing use `lMaxTE`.

3. **Per-direction fill times**: replace scalar `m_alTEFil[0..4]` with
   2D arrays `m_alTEFil[K_NO_TIME_ELEMENTS][MAX_DIFF_DIR]` (or `std::vector<std::array>`).
   Loop `for(lI = 0; lI < m_lNoDiffDir; lI++)` computes fill times for `m_vlTE[lI]`.

4. **Derive gradient amplitude per direction**: from Stejskal-Tanner:
   ```
   b = gamma^2 * G^2 * delta^2 * (Delta - delta/3)   [simplified, no ramp]
   G = sqrt(b / (gamma^2 * delta^2 * (Delta - delta/3)))
   ```
   With ramp correction already handled by `bValueMono()`. Store as
   `std::vector<double> m_vdDiffAmpl`.
   Check each against `m_dGradMaxAmpl` — return `SEQU_ERROR` if exceeded.

5. **Gradient check loop** (lines 1325–1347): use `m_vdDiffAmpl[lI]` instead of
   `dDiffGradAmpl1`.

### In `runKernel()` — `lm_Spiral_MultiSE.cpp`

1. Replace `rMrProt.te()[0]` lookups for timing with `m_vlTE[lDir]`.
2. Use `m_alTEFil[...][lDir]` per-direction fill times.
3. Use `m_vdDiffAmpl[lDir]` for gradient amplitude.
4. b-matrix written to MDH reflects the per-direction b-value — already works
   if amplitude is set correctly before `writeBMatrix()`.

### In `lm_Spiral_MultiSE.h`

- Add `std::vector<long> m_vlTE`
- Add `std::vector<double> m_vdBTarget`
- Add `std::vector<double> m_vdDiffAmpl`
- Change `m_alTEFil` from `long[K_NO_TIME_ELEMENTS]` to
  `std::vector<std::array<long, K_NO_TIME_ELEMENTS>>` (or equivalent 2D structure)

### UI changes — `lm_Spiral_MultiSE_UI.cpp`

- Hide or repurpose `WIP_dDiffGradAmpl_Pos` (amplitude slider):
  in the new logic the sequence computes amplitude — the user should not set it.
- δ (`SmallDelta`) and Δ (`BigDelta`) WIP parameters remain.
- Consider adding a read-only display or console output of the derived amplitude
  range for the user's information.

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

## Next steps (waiting for user)

- [ ] User to analyse how the new b→amplitude inversion fits the existing
      `bValueMono()` / `setGradients()` logic and whether δ/Δ constraints
      are sufficient to guarantee a feasible solution for all rows.
- [ ] Confirm Option A vs Option B.
- [ ] Confirm TE unit in file (µs assumed above).
- [ ] Clarify whether TE in the file must respect `rSeqLim.setTE()` bounds,
      or whether those bounds should be relaxed / removed when variable-TE mode is active.
- [ ] Decide how `Check()` iterates — currently only checks 1 direction
      (`m_lDiffLoopCounterForCheck < 1`); with variable TE it may need to check all.
