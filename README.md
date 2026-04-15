# Bug Simulation

A simulation system where different types of bugs move, interact, fight 
and evolve over time on a 10x10 board.

# Features

- Abstract `Bug` class with polymorphism.
- Derived Bug types:
  - Crawler
  - Hopper
- Movement system with direction and boundary handling
- Bug interactions
- File input
- Life history tracking for each bug
- Cell occupancy tracking
- Output file generation

# Innovative Features

- ANSI coloured terminal output
- Hunter Bug (tracks scent instead of direct detection)
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

## Stage 1 - Core Architecture

**Goal:** Build foundation correctly
- [ ] Create `Bug` abstract base class
- [ ] Implement fields
- [ ] Add virtual `move()` method
- [ ] Implement `Crawler` class
- [ ] Implement `Hopper` class
- [ ] Create `Board` class
- [ ] Store bugs using `vector<Bug*>`

## Stage 2 - File Input & Basic Features

**Goal:** Get a working system
- [ ] Read `bugs.txt`
- [ ] Dynamically allocate bugs
- [ ] Display all bugs
- [ ] Implement "Find Bug by ID"
- [ ] Basic menu system

## Stage 3 - Movement System

**Goal:** Correct simulation behaviour
- [ ] Implement `move()` for Crawler
- [ ] Implement `move()` for Hopper
- [ ] Handle board edges properly
- [ ] Implement random direction logic
- [ ] Record path history

## Stage 4 - Board Mechanics

**Goal:** Make simulation meaningful
- [ ] Implement Tap (move all bugs)
- [ ] Freeze one random bug per tap
- [ ] Track cell occupancy
- [ ] Display all cells and their bugs

## Stage 5 - Combat System

**Goal:** Bug interaction
- [ ] Detect bugs in same cell
- [ ] Pair bugs for fights
- [ ] Implement 3 round fight system
- [ ] Apply random damage (0–5)
- [ ] Mark dead bugs
- [ ] Track "who killed who"

## Stage 6 - Simulation Loop

**Goal:** Automation
- [ ] Implement “Run Simulation”
- [ ] Tap every second
- [ ] Display live updates
- [ ] Stop when one bug remains

## Stage 7 - Output & History

**Goal:** Data persistence
- [ ] Display life history
- [ ] Save to output file with timestamp
- [ ] Include paths + death info

## Innovative Development

## Stage 8 - Scent System

- [ ] Add smell grid to Board
- [ ] Bugs deposit scent when moving
- [ ] Fights create stronger scent
- [ ] Implement scent decay each turn

## Stage 9 - Hunter Bug

- [ ] Create new derived class `HunterBug`
- [ ] Follow the strongest scent direction
- [ ] Use tolerance (±0.5) for decision-making
- [ ] Implement path memory
- [ ] Backtracking when scent disappears

## Stage 10 - Terrain System

- [ ] Add terrain grid
- [ ] Define terrain types (Mud, Rock, Food)
- [ ] Modify movement based on terrain
- [ ] Integrate with all bug types

## Stage 11 - Visualization

- [ ] ANSI coloured output
- [ ] Board rendering
- [ ] Visit heatmap
- [ ] Fight heatmap
- [ ] Alive bugs graph

## Stage 12 - Optional Weather

- [ ] Add weather states
- [ ] Modify scent decay
- [ ] Affect terrain or bugs