# Bug Simulation

A simulation system where different types of bugs move, interact, fight 
and evolve over time on a 10x10 board.

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
# Features

- Abstract `Bug` class with polymorphism.
- Derived Bug types:
  - Crawler
  - Hopper
  - Hunter Bug (tracks scent instead of direct detection) uses scent
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

Optional:
- Weather system affecting scent decay and environment

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

## Stage 10 - Terrain System

- [ ] Add terrain grid
- [ ] Define terrain types (Mud, Rock, Food)
- [ ] Modify movement based on terrain
- [ ] Integrate with all bug types

## Stage 11 - Visualization

- [x] ANSI coloured output
- [x] Board rendering
- [ ] Visit heatmap
- [ ] Fight heatmap
- [ ] Alive bugs graph

## Stage 12 - Optional Weather

- [ ] Add weather states
- [ ] Modify scent decay
- [ ] Affect terrain or bugs