# WISumoria TODO

Redesign INT and WIS:
- **WIS (Wisdom)** determines **Mental Capacity**: How many spells/prayers can be learned (spell slots / repertoire).
- **INT (Intelligence)** determines **Mental Energy**: Maximum spell points (mana pool).

---

## 1. Balance & Stat Tables
- [ ] **Rebalance Class Stat Modifiers (`src/data_player.cpp`)**
  - Remove the `-3` INT class penalty for **Priest** and **Paladin**. In vanilla Umoria, INT was a dump stat for them, but with INT determining mana, this penalty causes them to start with 0 mana when combined with racial penalties (e.g. Dwarf, Half-Troll).
  - Review the `-2` WIS penalty for **Rogue** to avoid crippling their spell slot progression when magic unlocks at level 5.
  - Review **Mage** and hybrid classes (**Ranger**, **Paladin**) to ensure Multiple Attribute Dependency (MAD) does not make characters unviable.

## 2. Refactoring & Cleanup
- [ ] **Clean Up `playerGainMana` Signature & Hack (`src/player.h`, `src/player.cpp`)**
  - Remove the `stat` parameter from `void playerGainMana(int stat) -> void playerGainMana()`.
  - Remove the temporary remapping hack:
    ```cpp
    if (stat == PlayerAttr::A_WIS) {
        stat = PlayerAttr::A_INT;
    }
    ```
  - Remove the unused `stat` parameter from `static int newMana(int stat) -> static int newMana()`.
- [ ] **Clean Up `playerCalculateAllowedSpellsCount` (`src/player.h`, `src/player.cpp`)**
  - Remove the `stat` parameter from `void playerCalculateAllowedSpellsCount(int stat) -> void playerCalculateAllowedSpellsCount()`.
  - Remove the `stat` parameter from `static int numberOfSpellsAllowed(int stat) -> static int numberOfSpellsAllowed()`, hardcoding `PlayerAttr::A_WIS`.
- [ ] **Update All Call Sites**
  - Update all calls to `playerGainMana()` and `playerCalculateAllowedSpellsCount()` in:
    - `src/game_run.cpp`
    - `src/player.cpp`
    - `src/player_stats.cpp`
    - `src/spells.cpp`
    - `src/ui.cpp`

## 3. Bug Fixes & Code Quality
- [ ] **Restore Safe `snprintf` in `src/player.cpp`**
  - Revert `sprintf` on line 873 back to `snprintf(tmp_str, MORIA_MESSAGE_SIZE, ...)`.
  - Replace any hard tabs (`\t`) with 4-space indentation to adhere to project code style.
- [ ] **Consolidate Duplicated Caster Logic**
  - Simplify redundant `if (class == SPELL_TYPE_MAGE) ... else if (class == SPELL_TYPE_PRIEST) ...` blocks that execute the exact same logic in:
    - `src/game_run.cpp` (`startMoria`)
    - `src/spells.cpp` (`spellLoseEXP`)
    - `src/ui.cpp` (`playerGainLevel`)
  - Use `if (class_to_use_mage_spells != config::spells::SPELL_TYPE_NONE)` where appropriate.
- [ ] **Synchronize Mana on WIS Loss (`src/player_stats.cpp`)**
  - In `playerSetAndUseStat`, ensure that if WIS changes and drops known spells to 0 (`spells_learnt == 0`), `playerGainMana()` is triggered so mana properly resets to 0.

## 4. Open Design Decisions
- [ ] **Spell Failure Chance (`src/mage_spells.cpp: spellChanceOfSuccess`)**
  - Determine whether spell failure rate is governed by:
    - **INT** (mental energy / focus),
    - **WIS** (mental capacity / comprehension),
    - Retained by class (Mage = INT, Priest = WIS), or
    - A blend of both stats.
- [ ] **Utility for Non-Casters (Warrior)**
  - Validate if INT (Magic Device, Disarm Traps) and WIS (Saving Throws) provide enough distinction for non-caster classes, or if mental attributes should offer additional utility.

