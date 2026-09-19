# WISumoria TODO

Redesign INT and WIS:
- **WIS (Wisdom)** determines **Mental Capacity**: How many spells/prayers can be learned (spell slots / repertoire).
- **INT (Intelligence)** determines **Mental Energy**: Maximum spell points (mana pool).

---

## 1. Balance & Stat Tables
- [x] **Rebalance Class Stat Modifiers (`src/data_player.cpp`)**
  - Removed the `-3` INT class penalty for **Priest** and **Paladin** (set to `0`), preventing 0-mana lockouts on races like Dwarf and Half-Troll.
  - Removed the `-2` WIS penalty for **Rogue** (set to `0`) so spell slot progression is not crippled when magic unlocks at level 5.
  - Symmetrized baseline attributes: Mage (+3 INT, 0 WIS) mirrors Priest (0 INT, +3 WIS); Ranger (+2 INT, 0 WIS) mirrors Paladin (0 INT, +1 WIS).

## 2. Refactoring & Cleanup
- [x] **Clean Up `playerGainMana` Signature & Hack (`src/player.h`, `src/player.cpp`)**
  - Removed the `stat` parameter: `void playerGainMana()`.
  - Removed the temporary `stat = PlayerAttr::A_INT` remapping hack.
  - Removed the unused `stat` parameter from `static int newMana()`.
- [x] **Clean Up `playerCalculateAllowedSpellsCount` (`src/player.h`, `src/player.cpp`)**
  - Removed the `stat` parameter: `void playerCalculateAllowedSpellsCount()`.
  - Removed the `stat` parameter from `static int numberOfSpellsAllowed()`, hardcoding `PlayerAttr::A_WIS`.
- [x] **Update All Call Sites**
  - Updated all calls to `playerGainMana()` and `playerCalculateAllowedSpellsCount()` across:
    - `src/game_run.cpp`
    - `src/player.cpp`
    - `src/player_stats.cpp`
    - `src/spells.cpp`
    - `src/ui.cpp`

## 3. Bug Fixes & Code Quality
- [x] **Restore Safe `snprintf` in `src/player.cpp`**
  - Reverted `sprintf` on line 873 back to `snprintf(tmp_str, MORIA_MESSAGE_SIZE, ...)`.
  - Replaced hard tabs (`\t`) with 4-space indentation to adhere to project code style.
- [x] **Consolidate Duplicated Caster Logic**
  - Replaced redundant `if (class == SPELL_TYPE_MAGE) ... else if (class == SPELL_TYPE_PRIEST) ...` blocks with unified `if (class_to_use_mage_spells != config::spells::SPELL_TYPE_NONE)` checks in:
    - `src/spells.cpp` (`spellLoseEXP`)
    - `src/ui.cpp` (`playerGainLevel`)
- [x] **Synchronize Mana on WIS Loss (`src/player_stats.cpp`)**
  - In `playerSetAndUseStat`, added check ensuring that if WIS loss drops known spells to 0 (`spells_learnt == 0`), `playerGainMana()` is called to reset mana to 0.

## 4. Open Design Decisions
- [ ] **Spell Failure Chance (`src/mage_spells.cpp: spellChanceOfSuccess`)**
  - Determine whether spell failure rate is governed by:
    - **INT** (mental energy / focus),
    - **WIS** (mental capacity / comprehension),
    - Retained by class (Mage = INT, Priest = WIS), or
    - A blend of both stats.
- [ ] **Utility for Non-Casters (Warrior)**
  - Validate if INT (Magic Device, Disarm Traps) and WIS (Saving Throws) provide enough distinction for non-caster classes, or if mental attributes should offer additional utility.
