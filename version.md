# Quabla Test Versions

## 0.2.010 2024-12-28

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
