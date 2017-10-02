// Copyright (c) 1989-2008 James E. Wilson, Robert A. Koeneke, David J. Grabiner
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

// Staff code

#include "headers.h"
#include "externs.h"

static bool staffPlayerIsCarrying(int &item_pos_start, int &item_pos_end) {
    if (inventory_count == 0) {
        printMessage("But you are not carrying anything.");
        return false;
    }

    if (!inventoryFindRange(TV_STAFF, TV_NEVER, item_pos_start, item_pos_end)) {
        printMessage("You are not carrying any staffs.");
        return false;
    }

    return true;
}

static bool staffPlayerCanUse(Inventory_t &item) {
    int chance = py.misc.saving_throw;
    chance += playerStatAdjustmentWisdomIntelligence(A_INT);
    chance -= item.depth_first_found - 5;
    chance += class_level_adj[py.misc.class_id][CLASS_DEVICE] * py.misc.level / 3;

    if (py.flags.confused > 0) {
        chance = chance / 2;
    }

    // Give everyone a slight chance
    if (chance < PLAYER_USE_DEVICE_DIFFICULTY && randomNumber(PLAYER_USE_DEVICE_DIFFICULTY - chance + 1) == 1) {
        chance = PLAYER_USE_DEVICE_DIFFICULTY;
    }

    if (chance < 1) {
        chance = 1;
    }

    if (randomNumber(chance) < PLAYER_USE_DEVICE_DIFFICULTY) {
        printMessage("You failed to use the staff properly.");
        return false;
    }

    if (item.misc_use < 1) {
        printMessage("The staff has no charges left.");
        if (!spellItemIdentified(item)) {
            itemAppendToInscription(item, ID_EMPTY);
        }
        return false;
    }

    return true;
}

static bool staffDischarge(Inventory_t *item) {
    bool identified = false;

    item->misc_use--;

    uint32_t flags = item->flags;
    while (flags != 0) {
        int staff_type = getAndClearFirstBit(flags) + 1;

        switch (staff_type) {
            case 1:
                identified = spellLightArea(char_row, char_col);
                break;
            case 2:
                identified = dungeonDetectSecretDoorsOnPanel();
                break;
            case 3:
                identified = dungeonDetectTrapOnPanel();
                break;
            case 4:
                identified = dungeonDetectTreasureOnPanel();
                break;
            case 5:
                identified = dungeonDetectObjectOnPanel();
                break;
            case 6:
                playerTeleport(100);
                identified = true;
                break;
            case 7:
                identified = true;
                dungeonEarthquake();
                break;
            case 8:
                identified = false;

                for (int i = 0; i < randomNumber(4); i++) {
                    int y = char_row;
                    int x = char_col;
                    identified |= monsterSummon(y, x, false);
                }
                break;
            case 10:
                identified = true;
                spellDestroyArea(char_row, char_col);
                break;
            case 11:
                identified = true;
                spellStarlite(char_row, char_col);
                break;
            case 12:
                identified = spellSpeedAllMonsters(1);
                break;
            case 13:
                identified = spellSpeedAllMonsters(-1);
                break;
            case 14:
                identified = spellSleepAllMonsters();
                break;
            case 15:
                identified = spellChangePlayerHitPoints(randomNumber(8));
                break;
            case 16:
                identified = spellDetectInvisibleCreaturesOnPanel();
                break;
            case 17:
                if (py.flags.fast == 0) {
                    identified = true;
                }
                py.flags.fast += randomNumber(30) + 15;
                break;
            case 18:
                if (py.flags.slow == 0) {
                    identified = true;
                }
                py.flags.slow += randomNumber(30) + 15;
                break;
            case 19:
                identified = spellMassPolymorph();
                break;
            case 20:
                if (spellRemoveCurseFromAllItems()) {
                    if (py.flags.blind < 1) {
                        printMessage("The staff glows blue for a moment..");
                    }
                    identified = true;
                }
                break;
            case 21:
                identified = spellDetectEvil();
                break;
            case 22:
                if (playerCureBlindness() || playerCurePoison() || playerCureConfusion()) {
                    identified = true;
                }
                break;
            case 23:
                identified = spellDispelCreature(CD_EVIL, 60);
                break;
            case 25:
                identified = spellDarkenArea(char_row, char_col);
                break;
            case 32:
                // store bought flag
                break;
            default:
                printMessage("Internal error in staffs()");
                break;
        }
    }

    return identified;
}

// Use a staff. -RAK-
void useStaff() {
    player_free_turn = true;

    int item_pos_start, item_pos_end;
    if (!staffPlayerIsCarrying(item_pos_start, item_pos_end)) {
        return;
    }

    int item_id;
    if (!inventoryGetInputForItemId(item_id, "Use which staff?", item_pos_start, item_pos_end, CNIL, CNIL)) {
        return;
    }

    // From here on player uses up a turn
    player_free_turn = false;

    Inventory_t &item = inventory[item_id];

    if (!staffPlayerCanUse(item)) {
        return;
    }

    bool identified = staffDischarge(&item);

    if (identified) {
        if (!itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
            // round half-way case up
            py.misc.exp += (item.depth_first_found + (py.misc.level >> 1)) / py.misc.level;

            displayCharacterExperience();

            itemIdentify(item_id);
        }
    } else if (!itemSetColorlessAsIdentified(item.category_id, item.sub_category_id, item.identification)) {
        itemSetAsTried(item);
    }

    itemChargesRemainingDescription(item_id);
}
