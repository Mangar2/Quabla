# Qapla Test Versions

## 0.3.015 2025-04-15

- Raiting: 54,4%
- Few fixes on endgame evaluations
- Now using 50% of margin above beta on forewared futility pruning return value (instead of 10%)
- Check extensions only on PV moves

## 0.3.014 2025-04-13

- Reduced ignore count for 50 move rule draw-scoring from 10 to 20
- Reduced king attack values

## 0.3.013 2025-04-13

- Raiting: 50,9%
- 50 moves rule detection included + reducing eval on more than 10 halfmoves without pawn move or capture

## 0.3.011 2025-04-10

- Raiting: 49.6%, 
- Corrected the usage of end game evaluation by adding a boolean flag if there is an endgame function
- Using new pawn evaluation values in computePawnValueNoPieceButPawn function 
- Corrected king distance to 0, 1, 2, ... (was 0, 1, 3, ...) - better correction (b)
- Use king distance now in pawn only endgames too
- Use race algorithm for KPsKPs also for KPs (removed on version b)
- Use current value in endgame evaluation (based on material, pst, pawn) instead of calculating it allone

## 0.3.010 2025-04-10

- Raiting: 51.9% 
- King to pawn distance
- Pawn evaluation with differences between midgame and endgame
- Improved pawn evaluation values
- Pawn hash moved to tt
- Improvements in internal statistic functionaliy

## 0.3.009 2025-04-07

- Removed King Attack as it produced a bad raiting
- Tests for King to pawn distance in endgame evaluation
	- 50.46% on -5 per distance

## 0.3.008 2025-04-07

- Raiting: 47,8
- Updated King Attack, worked well in game against itself
- Fixed bug in clearing memories, tt and history after a move in uci
- Fixed bug with illeagal move in pv due to not resetting pv entry in search nodes

## 0.3.005 2025-03-26

- Raiting: 49.3%; Old Raiting: 65.79
- Knight property values changed to: { {  0,   0}, { 20, 0}, { -43,  0}, {-23,  0} }
- Bishop property values changed to: { {  0,   0}, { 26,  14}, {-10,   0}, { 15,  14} }
- Rook property values changed to:
	- Trapped: -50, -16
	- OpenFile: 18, 6
	- HalfOpenFile: 10, 0
	- ProtectsPP: 25, 0
	- Pinned : -23, 0
	- Row7: { 0,0 }, { 17, 17 }, { 10, 0 }, { 0, 0 }

 ## 0.3.004 2025-03-22

- Optimized eval threat weights, ca. +2.5% against itself (ply 8 search)
- Raiting: 63.18

## 0.3.003 2025-03-21

- More minor bugfixes - all not relevant for playing strength
- Enables the tt to shrink physical memory usage
- Added a debug flag to exclude other threads from whatif
- Implemented multi-threading statistic features to optimize eval weights

## 0.3.002 2025-03-17

- Stabilization bugfixes
- Evaluation refactoring to focus on table lookup
- New version of "eval" command printing a board with pieces and their values

## 0.3.001 2025-03-12

- Stores and retrieves eval from tt.

## 0.3.000 2025-03-10

- Raiting: 60.00
- Some minor bug fixes
- fixed, parameter for alpha/beta on nullmove search - but without effect for non PV moves and nullmove only runs on non PV moves.
- fixed, that root move did not detect fail low correctly in rare caseses
- fixed, that pv search result was not stored in hash for pv nodes in rare cases
- fixed, that ttValue was not initialized on tt miss in search

## 0.2.056 2025-03-09

- Hopefully last version for 0.2
- Fixing pondering for uci
- Implemented rudimentary pondering for xboard
- Slightly changed max-time per move on very short time controls. The last version still had two losses on time.
- Changed the way minimal depth for a move is calcuated on short time control. Should work fine now.

## 0.2.056 2025-03-06

- Raiting: 59.32
- Improvements and fixes for MultiPV
- Activated check extension. It does not harm and improves mate searching

## 0.2.055 2025-03-03

- Raiting: 58.29
- IID fixed
- Hash replacement strategy changed. Now better conserving PV values
- Fixed problem skipping pv collection on depth = 0
- Fixed bug on direct mate detection, if remaining search depth not sufficient (alpha/beta window was not set correctly)

## 0.2.054 2025-03-03

- Refactored PV - detect in Search, fixed bugs
	- First lmr search no longer has PV window
	- Fixed not searching PV if window gets null-size by chance
	- LMR fail low no longer researched if result = alpha

## 0.2.053 2025-03-03

- Eval Endgame no longer produces prospecitve mate values
- Aspiration window adaption, keeping alpha on raising, fast change to -max/+max windows on alterating
- Not go to q-search, if in pv and a tt move is left

## 0.2.052 2025-02-25

- aspiration window: max-window on alternating twice
- Always reasearch, in pv nodes, if null-window search results in value above alpha - no fail high anymore (in root and normal search)
- Dont use hash values in pv - also in quiescence search
- Store hash even for "0" values (idicating a repetetive draw) but we do not cut search on it
- Print first pv update on nodes > 2M
- Send current position value to whatif


## 0.2.051 2025-02-24

- Raiting: 52.21
- no lmr cutoff if bestValue < - MIN_MATE_VALUE
- lmr result increases bestValue, if < alpha
- aspiration window increases/decreases on mate value/-mate value
- Set PV of all moves to stack in root move loop
- Promptly return from negaMaxRoot, if skipMoves is larger than available moves

## 0.2.050 2025-02-24

- Raiting: 52.75
- SE fix node type to all 
- Changed window to prevent toggeling

## 0.2.049 2025-02-24

- Raiting: 54.07
- Identical to 48, T5.
- New tournament with little time increase to prevent loss on time. This distracts test results.

## 0.2.048 2025-02-23

- Raiting 52.18
- Improvement in thread management
- Reducing the output amount on low ply to 1
- T1: Added the check having enough material on nullmove again. 51.54
- T2: Removed the check for eval on nullmove, only material is checked. 53.00
- T3: Nullmove remaining depth is now 2 (was 0) 51.11
- T5: T2 mit IID only in PV, 53.93

## 0.2.047 2025-02-23

- Raiting 52.61
- Removed nullmove check having more material 
- Added nullmove check having enough eval with margin reduced by search depth

## 0.2.046 2025-02-22

- T3: only for cut nodes, 50.34
- T2: remaining depth = 0, 53.18
- T1: root case, no verify, 52.25

## 0.2.045 2025-02-21

- Raiting 53.54
- verify only if depth >= 0

## 0.2.044 2025-02-21

- Raiting 52.86
- verify bugfix

## 0.2.043 2025-02-21

- Multi-PV implementation
- Bug detected in verify nullmove. Current "node" is polluted by verify
- To try: nullmove even, if only two plies to go
- To try: nullmove only, if above beta + margin (note Spike has negative margin, stockfish depending on search depth)

## 0.2.042 2025-02-14

- Raiting 55.29
- Search with full window in root move until a bestmove is found.
- This feature will be removed, checking it later again.

## 0.2.041 2025-02-14

- Raiting 54.50 with margin 1 + depth * 4 (this will be in the new version)
- Raiting 53.79 with singular extension margin 10 + depth * 4
- Check for value upper-bound or exact in singular extension

## 0.2.040 2025-02-13

- Raiting: 52.18 (Unclear, of this is an improved version, still keeping it)
- Used improving flag to reduce move count pruning by 100

## 0.2.038 2025-02-12

- Raiting: 51.82 ()
- Futility pruning activated for tt-cut moves
- Changed code to calucate isImproving flag

## 0.2.037 2025-02-10

- Raiting: 52.02
- LMR from first ply (removed ply <= 2 exit for lmr)

## 0.2.036 2025-02-10

- Rating: 53.50
- LMR from second ply (ply <= 1 exit)

## 0.2.035 2025-02-10

- Raiting: 51.14
- Window Size for growing constant on alpha, for dropping, constant on beta.

## 0.2.034 2025-02-10

- Early stop at 48.00, 400 games
- Window Size for research for rizing/dropping /2 instead of /10. 

## 0.2.033 2025-02-09

- Raiting: 50.86
- No Check extensions

## 0.2.032 2025-02-09

- Raigin: 50.23
- Verify

## 0.2.031 2025-02-09

- Raiting: 50.93
- returns beta + (eval - beta) / 10. Not quite clear, but returning exactly beta increases the node count by multiples.
- 

## 0.2.030 2025-02-09

- Stopped, very bad performance (about 40)
- Bugfix, foreward futility pruning now returns beta instead of eval

## 0.2.029 2025-02-09

- Raiting: 50.86
- Nullmove R: 4 (constant)
- Tested: 4 + depth/3 (49.11); 5 (48.07); 3 + depth/3; 5 + depth/2; 5 + depth /3; Last three stopped due to bad performance.

## 0.2.028 2025-02-08

- Raiting (new): 48.5 (based on a new tournament setup with stronger opponents)
- Raiting (old): 68.7
- Butterfly board fix, moves behind best moves are no longer decreased in pv nodes

## 0.2.027 2025-02-07

- Raiting 63.86
- Butterfly boards

## 0.2.026 2025-02-02

- Raiting 63.61
- se margin 30 + depth * 4

## 0.2.025 2025-02-02

- Raiting 60.46
- se margin 70
- se only, if ttdepth >= seDepth
- dont se if tt shows mate value

## 0.2.024 2025-02-01

- Raiting 58.79
- Singular Extension
- se margin 10 + depth

## 0.2.023 2025-02-01

- Raiting 58,04 -> T1
- Using move count * 3, even, if it performed littel worse in the test set.
- lmr from move number 1, T0, Stopped, very bad
- lmr move count depth * 3, T1
- lmr pv move /2 only, if lmr >= 2, T2, definitely less good.
- All together, T3, Stopped, very bad

## 0.2.022 2025-01-26

- 58,50
- Small speed improvements due to better usage of print templates
- More agressive move count pruning and lmr.

## 0.2.021 2025-01-25

- Raiting 56,18
- No lmr or move count pruning, for checking moves

## 0.2.020 2025-01-21

- Raiting 55,18 
- Kept the hash of version 0.2.019, even, if rating is lower.
- Move count pruning, if depth - lmr < 0, we prune the move

## 0.2.019 2025-01-20

- Raiting 53,50
- Activated hash in quiescence search
- Including using hash move for evades, if available

## 0.2.018 2025-01-20

- Raiting 54,39
- IID out again
- Futility pruning (reversed) now with depth to 10 and with d * 100.
- No futility pruning, if we have a hash move.
- Not implemented: "improving" rule to prune improving moves stronger.

## 0.2.017 2025-01-18

- raiting 52
- IID, seems to be not working for Qapla

## 0.2.016 2025-01-18

- Raiting 53,46 (Classic Eval)
- Undid changes in pawn evaluation creating big slowdown
- Have evades in quiescence search again
- 

## 0.2.013 2025-01-18

- Raiting 49,57
- No IID, IID implemented right
- Compute evades in quiescence search
- 

## 0.2.011 2024-12-29

- Rating 52,96
- IID, 

## 0.2.010 2024-12-28

- NEW Raiting 52,71
- Opponents: Ruffian 1.5, Smarthink 0.17a, Ktulu 7.5, Glaurung 1.21, Abrok 5.0, Quark 2.35, Spike09a, 10a, gaia 3.5, Qapla 0.1.1, Fruit 2.0, Aristach 4.5, SOS-51 Arena
- Nearly 009, but with a completely different set of opponent - stronger.
- Only hindering the engine to use killer/tt-move on root for situation in which search stopped.

## 0.2.009 2024-12-28

- Rating 68,82
- Detected a bug. Qabla played g3g4 - a silly loosing move in 8/8/8/5p1p/4kP1P/6P1/5K2/8 w - - 38 99. The problem was never reproducable.
- No longer writing anything to the transposition table, if search was aborted. Hopefully this fixed the bug. 
- Identified, that Qabla does not evaluate differences in material between mid- and endgame well. 
- Updated few material values now: 
-- PAWN_VALUE_MG = 80; (75)
-- PAWN_VALUE_EG = 95; (90)
-- KNIGHT_VALUE_MG = 360; (375)
-- KNIGHT_VALUE_EG = 325; (375)
-- BISHOP_VALUE_MG = 360; (375)
-- BISHOP_VALUE_EG = 330; (375)
-- ROOK_VALUE_MG = 550; (550)
-- ROOK_VALUE_EG = 550; (550)
-- QUEEN_VALUE_MG = 975; (1000)
-- QUEEN_VALUE_EG = 1025; (1000)

## 0.2.008 2024-12-26

- Re-implemented stopping search, if time is low. Was removed in refactoring process. This lead to few losses due to time forefeight.
- Everything else is identical to 0.2.006. This version shall double check the rating of 0.2.006.
- Rating 64,32

## 0.2.007 2024-12-26

- Changed king-attack-threat evaluation to use "midgame-v2-variable" instead "midgame (v1)". This leads to less king-attack weight near end-game. This was not successful and is thus switched back.

## 0.2.006 2024-12-26

- Added Rook-on-row-7 bonus
- Rating 65,48

## 0.2.005 2024-12-26

- Changed pawn material value PAWN_VALUE_MG 65 -> 75; PAWN_VALUE_EG = 85 -> 90;
- Rating 64,96

## 0.2.004 2024-12-25

- Improvement in Whatif
- Refactoring root-move
- Prepared to remove the "search-state" from the search variables, the search state principle hinders to implement a multi-processor version
- Implemented lmr
- Rating: 63,23 (+50 Elo, exactly what was expected)

## 0.2.003 2024-12-23

- Cleanup without functional differece
- Increased pawn material to 65/85 (from 50/70)
- Increased bishop and knight material to 375 (from 325)
- Increased rook material to 550 (from 500)
- Increased queen material to 1050 (from 975)
- Raiting: 56,39
- Rook mobility now:
```
static constexpr value_t ROOK_MOBILITY_MAP[15][2] = { 
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 8, 8 }, { 12, 12 }, { 16, 16 },
	{ 20, 20 }, { 25, 25 }, { 25, 25 }, { 25, 25 }, { 25, 25 }, { 25, 25 }, { 25, 25 } };
```
- Former:
```
static constexpr value_t ROOK_MOBILITY_MAP[15][2] = { 
	{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 10, 10 }, { 15, 15 }, { 20, 20 },
	{ 25, 25 }, { 30, 30 }, { 30, 30 }, { 30, 30 }, { 30, 30 }, { 30, 30 }, { 30, 30 } };
```

- Bishop mobility now:
```
static constexpr value_t BISHOP_MOBILITY_MAP[15][2] = {
	{ -15, -25 }, { -10, -15 }, { 0, 0 }, { 5, 5 }, { 8, 8 }, { 13, 13 }, { 16, 16 }, { 18, 18 },
	{ 20, 20 }, { 22, 22 }, { 24, 24 }, { 25, 25 }, { 25, 25 }, { 25, 25 }, { 25, 25 }
};

```
- Former:
```
static constexpr value_t BISHOP_MOBILITY_MAP[15][2] = {
	{ -15, -25 }, { -10, -15 }, { 0, 0 }, { 5, 5 }, { 10, 10 }, { 15, 15 }, { 20, 20 }, { 22, 22 },
	{ 24, 24 }, { 26, 26 }, { 28, 28 }, { 30, 30 }, { 30, 30 }, { 30, 30 }, { 30, 30 }
};

```



## 0.2.002 2024-12-21

- Refactored version with clearer search
- Same functionality as last 0.1. version
- Rating: 54.71 - same as non refactored version

## 0.2.001 2024-12-19

- Refactoring with missing features
- Rating: 27,32, test set "Fruit 2.0" 

