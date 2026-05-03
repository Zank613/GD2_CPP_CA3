# Bug Simulation

A simulation system where different types of bugs move, interact, fight 
and evolve over time on a 10x10 board.

# For SFML branch [click here](https://github.com/Zank613/GD2_CPP_CA3/tree/sfml)

# Run

I suggest using Terminal that supports ANSI colours for normal usage. (e.g. Windows Terminal)

**Run Normally**
```bash
.\GD2_CPP_CA3.exe
```

**Run Normally with Debug messages**
```bash
.\GD2_CPP_CA3.exe --debug
```

**Run with no colour**
```bash
.\GD2_CPP_CA3.exe --nocolour
```

**Run with no board rendering**
```bash
.\GD2_CPP_CA3.exe --novisual
```

**Run Normally with a defined seed**
```bash
.\GD2_CPP_CA3.exe --seed N
```

**Run Faster (no board rendering and doesn't wait for 1 second)**
```bash
.\GD2_CPP_CA3.exe --fast
```

**These commands can be combined as well.**
```bash
.\GD2_CPP_CA3.exe --seed N --fast --debug
```
# Features

- Abstract `Bug` class with polymorphism.
- Derived Bug types:
  - Crawler
  - Hopper
  - Hunter Bug (tracks scent instead of direct detection)
- Movement system with direction and boundary handling
- Bug interactions
- File input
- Life history tracking for each bug
- Cell occupancy tracking
- Output file generation

# Innovative Features

- ANSI coloured terminal output
- Scent system with dissipation over time
- Terrain system with traversal effects
- ASCII based heatmaps and graphs (simulation analytics)

# Simulation Systems

## Scent System

- Bugs leave trails when moving
- Scent intensity decays over time
- Fights create strong scent hotspots
- Hunter Bug follows scent instead of tracking bugs directly

## Terrain System

- Grid based terrain affecting movement:
  - Mud ->  slows bugs
  - Rock -> blocks movement
  - Food -> heals bugs
- Adds environmental interaction to simulation

# Visualization

- ASCII board rendering
- Cell visit heatmaps
- Fight hotspot maps
- Alive bugs over time graph
- Optional filtered views (by bug type / id)

# Roadmap

## ~~Stage 1 - Core Architecture~~

**Goal:** Build foundation correctly
- [x] Create `Bug` abstract base class
- [x] Implement fields
- [x] Add virtual `move()` method
- [x] Implement `Crawler` class
- [x] Implement `Hopper` class
- [x] Create `Board` class
- [x] Store bugs using `vector<Bug*>`

## ~~Stage 2 - File Input & Basic Features~~

**Goal:** Get a working system
- [x] Read `bugs.txt`
- [x] Dynamically allocate bugs
- [x] Display all bugs
- [x] Implement "Find Bug by ID"
- [x] Basic menu system

## ~~Stage 3 - Movement System~~

**Goal:** Correct simulation behaviour
- [x] Implement `move()` for Crawler
- [x] Implement `move()` for Hopper
- [x] Handle board edges properly
- [x] Implement random direction logic
- [x] Record path history

## ~~Stage 4 - Board Mechanics~~

**Goal:** Make simulation meaningful
- [x] Implement Tap (move all bugs)
- [x] Freeze one random bug per tap
- [x] Track cell occupancy
- [x] Display all cells and their bugs

## ~~Stage 5 - Combat System~~

**Goal:** Bug interaction
- [x] Detect bugs in same cell
- [x] Pair bugs for fights
- [x] Implement 3 round fight system
- [x] Apply random damage (0–5)
- [x] Mark dead bugs
- [x] Track "who killed who"

## ~~Stage 6 - Simulation Loop~~

**Goal:** Automation
- [x] Implement “Run Simulation”
- [x] Tap every second
- [x] Display live updates
- [x] Stop when one bug remains

## ~~Stage 7 - Output & History~~

**Goal:** Data persistence
- [x] Display life history
- [x] Save to output file with timestamp
- [x] Include paths + death info

## Innovative Development

## ~~Stage 8 - Scent System~~

- [x] Add smell grid to Board
- [x] Bugs deposit scent when moving
- [x] Fights create stronger scent
- [x] Implement scent decay each turn

## ~~Stage 9 - Hunter Bug~~

- [x] Create new derived bug class `Hunter`
- [x] Follow the strongest scent direction
- [x] Use tolerance (±0.5) for decision-making
- [x] Implement path memory
- [x] Backtracking when scent disappears

## ~~Stage 10 - Terrain System~~

- [x] Add terrain grid
- [x] Define terrain types (Mud, Rock, Food)
- [x] Modify movement based on terrain
- [x] Integrate with all bug types

## ~~Stage 11 - Visualization~~

- [x] ANSI coloured output
- [x] Board rendering
- [x] Visit heatmap
- [x] Fight heatmap

~~Alive bugs graph~~ _Removed due to being unnecessary._

## Stage 12 - Optional Weather

- [ ] Discuss with lecturer about feasibility and feature creep problem
- [ ] Add weather states
- [ ] Modify scent decay
- [ ] Affect terrain or bugs

# Miscellaneous
## Performance Optimization

For performance optimization I will repeatedly check for performance using GCC's
own performance profiling build options.

Then I will try to figure out which parts can be optimized and fixed. I will also
include some graphs here if I can for results and comparison.

CMake Profile Flags used for information:
```cmake
-DENABLE_GCC_TIME_REPORT=ON -DENABLE_GCC_INCLUDE_TREE=ON
```

### Build Comparison

| File                  | Before Optimization | After Optimization | Change       |
|-----------------------|---------------------|--------------------|--------------|
| `Bug.cpp`             | 0.21s               | 0.18s              | 0.03s faster |
| `Utils.cpp`           | 0.27s               | 0.25s              | 0.02s faster |
| `Crawler.cpp`         | 0.33s               | 0.30s              | 0.03s faster |
| `Hopper.cpp`          | 0.34s               | 0.30s              | 0.04s faster |
| `ConsoleRenderer.cpp` | 0.37s               | 0.33s              | 0.04s faster |
| `main.cpp`            | 0.43s               | 0.37s              | 0.06s faster |
| `Hunter.cpp`          | 0.49s               | 0.43s              | 0.06s faster |
| `Board.cpp`           | 0.77s               | 0.64s              | 0.13s faster |

The optimization pass improved measured build performance by approximately 12.8%. Across the compared source files, 
total compile time decreased from 3.21 seconds before optimization to 2.80 seconds after optimization, 
meaning the optimised build used around 87.2% of the original compile time.

### Runtime Comparison

After downloading [Intel VTune](https://www.intel.com/content/www/us/en/developer/tools/oneapi/vtune-profiler.html) and painstakingly learning how to read the logs from it,
I tested runtime performance of the game.

Parameters for both tests:

```bash
.\GD2_CPP_CA3.exe --seed 446056276
```
This seed was chosen because Tap counts were at a rather small count: 84.

| Metric | Before optimisation | After optimisation |               Change | Interpretation |
|---|---:|---:|---------------------:|---|
| IPC | 0.656 | 0.715 |                +9.0% | Better instruction throughput overall |
| P-core IPC | 0.649 | 0.808 |               +24.5% | Significant improvement on performance cores |
| E-core IPC | 0.670 | 0.549 |               -18.1% | E-core efficiency dropped, but P-core IPC improved strongly |
| Average CPU frequency | 4.1 GHz | 4.2 GHz |                +2.4% | Slightly higher average clock speed |
| Logical core utilisation | 1.8% | 1.3% |                -0.5% | Program used less total CPU time overall |
| Average logical cores used | 0.427 / 24 | 0.310 / 24 |               -27.4% | Less CPU occupancy overall |
| Microarchitecture usage | 8.7% | 10.0% |                +1.3% | CPU pipeline was slightly more active |
| P-core retiring | 8.7% | 11.1% |                +2.4% | More useful work was completed by the CPU |
| P-core front-end bound | 21.3% | 23.6% |                +2.3% | Slightly more instruction-fetch/decode limitation |
| P-core back-end bound | 71.0% | 72.7% |                +1.7% | Still heavily back-end bound |
| P-core memory bound | 56.1% | 48.9% |                -7.2% | Less memory/cache waiting after optimisation |
| P-core cache bound | 47.3% | 38.5% |                -8.8% | Fewer cache-related stalls |
| P-core L1 bound | 9.9% | 11.1% |                +1.2% | Slightly more L1 cache pressure |
| P-core L2 bound | 28.1% | 19.1% |                -9.0% | Strong reduction in L2-related stalls |
| P-core L3 bound | 9.3% | 8.4% |                -0.9% | Small reduction in L3-related stalls |
| P-core DRAM bound | 10.0% | 13.2% |                +3.2% | Slightly more main-memory waiting |
| P-core core bound | 14.9% | 23.8% |                +8.9% | More stalls are now core-side rather than memory-side |
| Average DRAM bandwidth | 1.130 GB/s | 1.053 GB/s |                -6.8% | Slightly less memory bandwidth used |
| Vectorization | 86.5% | 88.5% |                +2.0% | Slight improvement, though the program is not vector-heavy |
| GPU active time | 0.1% | 0.1% | No meaningful change | Expected for a console-based program |

I say this is rather went well optimization applied to the game. Even though
my optimizations were simpler the report suggest there were some meaningful performance
optimization going on.

Note 1: Most of the performance can be significantly improved if
the game was launched with `--fast` command as there will be no `std::cout`
every single line and game will not wait and finish quickly as possible.

Note 2: Your CPU might not have E core and P core specifications as mine
is one of the latest generation Intel based CPU. ([Intel Core Ultra 9 275HX](https://www.intel.com/content/www/us/en/products/sku/242293/intel-core-ultra-9-processor-275hx-36m-cache-up-to-5-40-ghz/specifications.html))

Note 3: These comparisons have been made with Debug build profile chosen. As such if
Release build was chosen the results would be even more significant as I hope CLion uses `-O2`
compile optimization flag or even `-O3`