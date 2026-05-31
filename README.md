# Plamol — Game Design Document

---

## Table of Contents

- [Interface](#interface)
  - [Graphics Mode](#graphics-mode)
  - [Action Mode](#action-mode)
  - [Mode Sequencing](#mode-sequencing)
- [Combat](#combat)
  - [Initiating Combat](#initiating-combat)
  - [Turn Order](#turn-order)
  - [Actions](#actions)
  - [Attack Rolls & Damage](#attack-rolls--damage)
  - [Opportunity Attacks](#opportunity-attacks)
  - [Ending Combat](#ending-combat)
- [Stats & Character Creation](#stats--character-creation)
  - [Base Stats](#base-stats)
  - [Character Creation](#character-creation)
  - [Leveling Up](#leveling-up)
  - [Hotbar & Inventory](#hotbar--inventory)
  - [Derived Skills](#derived-skills)
  - [Combat-Derived Stats](#combat-derived-stats)

---

## Interface

Plamol operates across two sequential modes: **Graphics Mode** and **Action Mode**.

### Graphics Mode

Graphics Mode is the default state of play. From here, the player can:

- Navigate the map
- Position their character
- Initiate Action Mode on any object or tile

Every tile in the game world has an associated data structure — sand, doors, walls, NPCs — anything can be initiated upon.

### Action Mode

Action Mode is initiated from within Graphics Mode by the player interacting with an object. Once active, the player can:

- **Look** at the object
- **Interact** with it
- **Use items** from their inventory or hotbar
- **Engage** with available actions specific to that object

The icon of the initiated object is displayed within Action Mode.

> **Core Design Pillar:** The player should never be *forced* into Action Mode. Random encounters and NPC-initiated interactions are strictly prohibited. The player is the sole actor in initiating Action Mode — the world does not act upon them.

### Mode Sequencing

Modes follow a strict sequential order. Each mode may only have a single child:

```
Graphics Mode → Action Mode → Combat
```

- Graphics Mode cannot initiate Combat directly
- Combat cannot initiate Graphics Mode
- This order must never be broken

---

## Combat

### Initiating Combat

Combat can only be initiated through hostile actions within Action Mode. **Any action can potentially be hostile** — hostile actions are clearly marked in the UI.

### Turn Order

Combat is turn-based with a twist:

- **Enemy turn order** is fixed at initiation
- **Player turn order** defaults to their Initiative value
- The player may shift their *following* turn forward or backward toward their default value, or place it before the end of the current round

### Actions

Each combatant has a pool of **100 action points** per turn. Sub-actions (movement, attacks, etc.) subtract from this pool based on their base cost, modified by relevant player skills.

- **Remaining action points carry over** to the next turn
- However, leftover points increase the chance of a successful opponent attack:

```
Attack Roll Bonus = remaining_action / 4
Final Attack Roll = attack_roll + (remaining_action / 4)
```

- **For each full action point spent**, the spender gains **+2 to Armor Class** for that round (stacked on top of base AC)

### Attack Rolls & Damage

Sub-actions that require an attack (e.g. swinging a sword) use the following resolution:

**Attack Roll:**
```
d20 + applicable stat bonuses  vs.  Defender's Armor Class
```

**Damage Roll (on hit):**
```
Weapon dice roll + applicable stat bonuses − Defender's Damage Reduction
```

Damage dice and applicable bonuses are defined per item.

**Ranged Weapons:**
```
Attack Roll −10 per tile beyond the weapon's optimal range
```

### Opportunity Attacks

Two combatants in neighboring tiles trigger an opportunity attack if one moves away:

- Functions identically to a normal attack
- Action points are expended from the **previous round's** action pool
- A combatant who fully expended their action pool last round **cannot** make opportunity attacks

### Ending Combat

Combat ends automatically when all enemies are dead, or when the combat state is set to **Intimidate**.

To attempt an **Intimidate end**, a player may spend their entire action (receiving the +2 AC bonus) on an end-combat roll:

```
Both sides roll: 1d20 + chosen skill   (player chooses their skill, enemy uses a matching skill)
Higher roll wins
```

*Example: The player wagers Spatial against the enemy's Innocence — both roll 1d20, add the respective skill, and compare.*

Once combat ends, the player is placed into Action Mode over the battlefield. From here:
- **Interact** → loot bodies
- **Look** → receive descriptions

---

## Stats & Character Creation

### Base Stats

Base stats are **never used directly**. All derived skills divide the result by 4 (rounded up).

Base stats can be positive or negative. They may be increased or decreased on level up — decreasing costs the same level-up points as increasing.

| Stat | Abbreviation |
|---|---|
| Strength | STR |
| Dexterity | DEX |
| Constitution | CON |
| Social | SOC |
| Intelligence | INT |
| Wisdom | WIS |

### Character Creation

**Step 1 — Roll Stats**
Each base stat is rolled with **3d6**. A total points value is shown and the player may redistribute freely.

**Step 2 — Tag Skills**
Players receive **4 tags**, each adding **+5** to a chosen skill.

**Step 3 — Choose Perks**
Up to **5 perks** may be selected (none are required). Perks are risk/reward trade-offs designed to encourage min-maxing.

*Examples:*
- `+2 STR / −4 DEX` — *"Lifted weights as a child and can't lose the weight"*
- `+4 INT / −8 SOC` — *"Spent your years reading instead of socializing"*

**Starting Equipment:** None. All items must be purchased.

### Leveling Up

Experience is gained through kills, interactions, and quests. Each threshold is **1.33× the previous** (starting at 100).

| Level | Reward |
|---|---|
| Every level | Tag one skill |
| Every 2nd level | Choose two randomized perks **or** tag two skills |
| Every 3rd level | Choose one of four randomized perks **or** increase a base stat by 1 |

### Hotbar & Inventory

**Hotbar (8 slots):** Items in the hotbar actively apply their stat modifiers. Weapons must be placed here to be used. An item with `+2 STR` has no effect unless it is in the hotbar.

**Inventory:** Uses the Arcanum inventory system.

### Derived Skills

All skill values are: `formula result ÷ 4`, rounded up.

| Skill | Formula |
|---|---|
| Physical Coordination | `(STR + DEX − INT) / 2` |
| Word | `(SOC + INT − CON) / 2` |
| Prob Analysis | `(INT + WIS − STR) / 2` |
| Spatial | `(INT + DEX − SOC) / 2` |
| Musical | `(WIS + DEX − SOC) / 2` |
| Natural | `(WIS + CON − DEX) / 2` |
| Interpersonal | `(SOC + WIS − STR) / 2` |
| Intrapersonal | `(INT + CON − WIS) / 2` |
| Innocence | `(CON + DEX − INT) / 2` |
| Heroism | `(STR + SOC − INT) / 2` |
| Love | `(SOC + CON − DEX) / 2` |
| Author | `(STR + INT − SOC) / 2` |

### Combat-Derived Stats

| Stat | Formula |
|---|---|
| Armor Class | `(DEX + WIS) − STR` |
| Damage Reduction | `(STR + CON) − DEX` |
| Max Hit Points | `(CON + STR) − DEX` |
| Initiative | `((DEX + WIS) − INT) / 4` |
