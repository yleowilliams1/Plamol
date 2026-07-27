# CRPG Engine — Codebase Guide

This document explains the engine's structure to someone who has never seen
the code before. It's organized bottom-up: foundation utilities first, then
the generic engine machinery built on top of them, then the game-specific
systems built on *that*, and finally the entry point that wires everything
together. Read it in order the first time through — nothing here references
a concept before it's been introduced.

Known bugs are called out inline, in a **Known issues** block right where the
relevant function is discussed, rather than in a separate appendix. The idea
is that you hit the warning exactly when you're reading the code it applies
to. (A full standalone bug list also exists in `BUGS.txt` if you want the
severity-ranked version.)

## How the pieces fit together

```
┌─────────────────────────────────────────────────────────────┐
│ main.c                                                       │
│   entry point: init everything, run the game loop            │
└───────────────────────────────┬──────────────────────────────┘
                                 │
┌───────────────────────────────▼──────────────────────────────┐
│ GAME-SPECIFIC SYSTEMS                                        │
│   s_menu / s_combat / s_action   (game states)                │
│   m_map / m_map_tile             (world data)                 │
│   i_inventory / i_items          (item & inventory data)      │
│   p_entity / p_stats             (characters & stats)         │
│   i_input                        (bindings & polling)         │
│   w_window_manager                (resolution-independent UI) │
└───────────────────────────────┬──────────────────────────────┘
                                 │
┌───────────────────────────────▼──────────────────────────────┐
│ CORE SYSTEMS (generic, still engine-facing)                   │
│   l_asset_manager   (generic load/get/free-by-index pattern)  │
│   g_statemachine    (state stack: push/pop/switch)             │
│   f_flags           (global boolean flags)                    │
│   e_engine_settings (path/string config table)                │
│   t_config_tool     (shared .ini parser used by everything)   │
└───────────────────────────────┬──────────────────────────────┘
                                 │
┌───────────────────────────────▼──────────────────────────────┐
│ FOUNDATION (zero game-specific knowledge, depends only on     │
│ libc / raylib)                                                │
│   e_error_handler   (ERR_LOG, XMALLOC/XCALLOC, crash policy)  │
│   t_strings         (safe string helpers, t_atoi)              │
│   t_gindex_tool     (local-index <-> global-index mapping)    │
└─────────────────────────────────────────────────────────────┘
```

Two things worth internalizing before reading further:

- **"gindx" vs "lindx" is a concept you'll see everywhere.** A *global index*
  (gindx) identifies a piece of data conceptually — "item #14," "tile type
  #3" — independent of whether it's currently loaded in memory. A *local
  index* (lindx) is the actual array slot a loaded copy of that data
  currently occupies. Most of the game-data modules (stats, items, tiles,
  inventory) keep a fixed-size array of slots and a parallel `local_indx`
  array that records which gindx (if any) currently occupies each slot. This
  mapping is what `t_gindex_tool.c` manages.

- **Nearly every game-data module follows the exact same load/get/free
  shape**, built on top of `l_asset_manager`. That pattern is documented once,
  in full, in the Core Systems section below. Once you understand it there,
  the per-module sections for stats/items/tiles/inventory only describe what's
  *different* about each one, not the whole pattern again.

---

# Layer 1 — Foundation

These three modules have no knowledge of the game at all. They could be
lifted into an unrelated project unchanged. Everything else in the codebase
is built on top of them, so read this section first.

## `e_error_handler.h` / `.c`

This is the module every other file leans on, via two macros: `ERR_LOG` and
`XMALLOC`/`XCALLOC`. Understanding this file's crash behavior is essential
context for reading almost everything else in the codebase, because a lot of
code that looks like it "falls through" after an error actually doesn't —
the process aborts first.

### Error codes and severity

```c
enum ER_CODE{
    ERR_OK, ERR_ALLOC, ERR_NO_FILE, ERR_PARSE, ERR_INDX,
    ERR_NULL, ERR_RELOAD, ERR_FUCKED, ERR_OUTOFBOUNDS,
};
```

Each code maps to a severity via `severity()`:

| Code | Severity | What happens |
|---|---|---|
| `ERR_ALLOC` | `LVL_CRASH` | Logged, then `abort()` |
| `ERR_FUCKED` | `LVL_CRASH` | Logged, then `abort()` |
| `ERR_NO_FILE` | `LVL_CRITICAL` | Logged, execution continues |
| `ERR_PARSE` | `LVL_CRITICAL` | Logged, execution continues |
| `ERR_OK` | `LVL_WARNING` | Logged, execution continues |
| `ERR_INDX` | `LVL_WARNING` | Logged, execution continues |
| `ERR_NULL` | `LVL_WARNING` | Logged, execution continues |
| `ERR_RELOAD` | `LVL_WARNING` | Logged, execution continues |
| `ERR_OUTOFBOUNDS` | `LVL_WARNING` | Logged, execution continues |

**This is the single most important fact to carry into the rest of the
codebase:** `ERR_LOG(ERR_FUCKED, ...)` and `ERR_LOG(ERR_ALLOC, ...)` are not
"log and continue" calls — they are fatal. A function that calls
`ERR_LOG(ERR_FUCKED, ...)` on a bad path and then keeps writing code below it
is *not* buggy just because there's no explicit `return` — the program dies
inside the macro via `fatal_crash()` -> `abort()`. Every other code
(`ERR_PARSE`, `ERR_INDX`, `ERR_NULL`, `ERR_RELOAD`, `ERR_OUTOFBOUNDS`,
`ERR_OK`, `ERR_NO_FILE`) is non-fatal, so a fall-through after one of *those*
really does continue running, and if the surrounding code assumed otherwise,
that's a genuine bug worth double-checking.

### `err_log` / `ERR_LOG`

```c
void err_log(enum ER_CODE code, const char *file, int line, const char *func, const char *fmt, ...);
#define ERR_LOG(code, fmt, ...) err_log(code, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
```

Always call this through the `ERR_LOG` macro, never `err_log` directly — the
macro fills in file/line/function automatically. It formats the message
(`vsnprintf` into a 512-byte buffer), prints a colorized line to `stderr` if
attached to a terminal, appends a timestamped line to the log file (if one
was opened via `err_init`), and then calls `fatal_crash()` if the severity is
`LVL_CRASH`.

`fatal_crash()` prints a final message, closes the log file, and calls
`abort()`. There is no recovery path once this fires.

### `err_init(const char *path)`

Opens (in append mode) the file at `path` as the persistent log destination.
If this isn't called, or the file fails to open, error messages still print
to `stderr` — they just won't be written to disk. This is genuinely
tolerant of failure (prints a warning to stdout and continues with
`log_file == NULL`), unlike almost every other failure path in this file.

### `XMALLOC(size)` / `XCALLOC(count, size)`

```c
void *xmalloc_impl(size_t size, const char *file, int lie, const char *func);
void *xcalloc_impl(size_t count, size_t size, const char *file, int line, const char *func);
#define XMALLOC(size) xmalloc_impl(size, __FILE__, __LINE__, __func__)
#define XCALLOC(count, size) xcalloc_impl(count, size, __FILE__, __LINE__, __func__)
```

These are the codebase's only sanctioned way to heap-allocate (a comment
elsewhere jokes "NO REALLOC. REALLOC IS FOR COWARDS" — there is genuinely no
`realloc` anywhere in this codebase). Both:
- Log `ERR_INDX` (non-fatal) and return `NULL` if asked to allocate 0 bytes.
- Log `ERR_ALLOC` (**fatal** — this crashes) if the underlying `malloc`/
  `calloc` call fails.

Because `ERR_ALLOC` is fatal, any code that calls `XMALLOC`/`XCALLOC` can
safely treat a non-NULL return as the only reachable outcome from an
allocation-failure standpoint — if the allocator had failed, the process
would already be dead. The only case where you still get `NULL` back from
these functions is the `size == 0` case, which is non-fatal.

**Known issue:** `p_entity.c`'s `p_init_entity` uses raw `calloc()` directly
instead of `XCALLOC`, bypassing this fatal-on-failure guarantee — see the
Known issues note under `p_entity.c` later in this document.

---

## `t_strings.h` / `.c`

Small set of string helpers used throughout the config-parsing code. All of
them lean on `e_error_handler` for logging.

### `char *t_strdup(const char *s)`

A `strdup` that goes through `XMALLOC` instead of `malloc`, and logs
`ERR_NULL` (non-fatal) and returns `NULL` if `s` is `NULL`. Copies
`strlen(s) + 1` bytes so the null terminator is included.

### `void t_cpy(char **str, char *val)`

Frees whatever `*str` currently points to (if anything), then duplicates
`val` into it via `t_strdup`. This is the standard way string fields on
structs (like item names/descriptions) get assigned during config parsing —
see `i_items.c`'s `item_parser`, which calls `t_cpy(&item->strs[i], p.value)`.
Logs `ERR_NULL` (non-fatal) if `str` itself is `NULL`, or if the duplication
failed for a non-NULL `val`.

### `void t_atoi(const char *str, int *result)`

A hand-written, careful string-to-int parser (not the libc `atoi`, despite
the name). It:
- Skips leading whitespace.
- Accepts an optional leading `+`/`-`.
- Requires at least one digit.
- Accumulates digits into a `long long` and checks for `INT_MAX`/`INT_MIN`
  overflow/underflow as it goes, logging `ERR_PARSE` (non-fatal) and
  bailing out early if it overflows.
- Skips trailing whitespace, then requires the string to be fully consumed
  - trailing garbage after that logs `ERR_PARSE` and returns without writing
  `*result`.
- On success, writes the parsed value into `*result`.

This is the function every `.ini` config parser in the codebase (stats,
items, tiles, map, input, flags) calls to turn a config value string into an
integer.

> **Known issue - the `NULL_ATOI` easter egg is a live data-corruption bug,
> not just a joke.** After a successful parse:
> ```c
> *result = (int)(sign * value);
> if(*result == NULL_ATOI){   // NULL_ATOI = 0x55AA = 21930
>     ERR_LOG(ERR_PARSE, "...");
>     *result = *result + 1;
> }
> ```
> `ERR_PARSE` is non-fatal, so this code actually runs: **any config value
> that legitimately parses to exactly 21930 gets silently rewritten to
> 21931.** If that number ever shows up as a real stat value, damage number,
> or index in a data file, the corruption will be silent and very hard to
> trace back to this function. Either keep it and document it loudly next to
> `NULL_ATOI`'s definition in `t_strings.h`, or remove it.

### `bool t_snprintf(char *buf, size_t bufsize, size_t *out_len, const char *fmt, ...)`

A `snprintf` wrapper with defensive error handling: rejects a `NULL` format
string, rejects a `NULL` buffer paired with a nonzero `bufsize`, checks
`vsnprintf`'s return value for encoding errors, defensively null-terminates
the buffer if some exotic libc didn't, and returns `false` (logging
`ERR_PARSE`, non-fatal) if the output was truncated. `out_len`, if non-NULL,
receives the number of bytes the *full* formatted string would have needed
(matching `vsnprintf`'s contract) - this lets a caller measure the required
buffer size first by calling with `buf = NULL, bufsize = 0`, then allocate
exactly that much. `t_config_tool.c`'s `t_ini_plus_indx` uses exactly this
two-call pattern to build a path string of the right size.

---

## `t_gindex_tool.h` / `.c`

This module manages the mapping between **global indexes** (gindx - "which
conceptual item/tile/stat-block is this") and **local indexes** (lindx -
"which array slot is it currently sitting in"). Every game-data module that
follows the load/free asset pattern (stats, items, tiles, inventory) is built
on this.

### `struct local_indx`

```c
struct local_indx{
    bool active;
    int gindx;
};
```

A module keeps a fixed-size array of these - one entry per possible array
slot - parallel to its actual data array. `active` says whether the slot is
currently occupied; `gindx` says which global index occupies it when it is.

### `bool t_indxvalid(int size, int lindx)`

```c
bool t_indxvalid(int size, int lindx){
    return (lindx < 0 || lindx >= size);
}
```

> **KNOWN ISSUE - this is the most serious bug in the codebase, and it
> affects nearly every module below this one.** Read this carefully before
> touching any code that calls `t_indxvalid`.
>
> The function's name and every call site's usage implies "returns true when
> the index IS valid." The actual body returns `true` when the index is
> **out of bounds** - i.e., the opposite. Nearly every call site in the
> codebase (`t_find_free_lindx` and `h_gchecker`/`h_lchecker` in this same
> file; `l_getter_checks`, `l_load_asset`, `t_free_asset` in
> `l_asset_manager.c`; `i_get_pckitemdata`/`i_get_pckitemstrs` in
> `i_items.c`; `i_get_inv_proto` in `i_inventory.c`; `p_get_dev` in
> `p_stats.c`) uses it as `if(!t_indxvalid(...)) { treat as bad index }`,
> which - given the function's real behavior - actually means "if this index
> IS good, treat it as bad." Concretely, this currently makes
> `l_load_asset` (the function every single asset type in the game loads
> through) abort the program via `ERR_FUCKED` on every *successful* load,
> because a good lindx makes `t_indxvalid` return `false`, and `!false` is
> `true`, which triggers the "array is full" fatal error path.
>
> The one place that's internally consistent with the function's *literal*
> current behavior is `t_lindx_to_gindx` (below) - but that's only because
> that single call site happens to interpret the return value the opposite
> way from every other call site, not because it's "correct" in some
> absolute sense. The bug is the *inconsistency*, and it needs to be fixed by
> picking one meaning and auditing every caller listed above.
>
> Until this is fixed, treat the entire asset-loading system described later
> in this document as **not currently functional as written**, even though
> the design around it (described below) is sound.

### `int t_find_free_lindx(struct local_indx *arr, int size)`

Scans `arr` for the first slot where `active` is false and returns its
index, or `NULL_INDX` (`-1`) if `arr` is `NULL`, `size` is negative, or no
free slot exists. This is how a module finds an empty array slot to load new
data into.

### `bool h_gchecker(...)` / `bool h_lchecker(...)`

Internal helper predicates used by the `t_g*`/`t_l*` functions below to
validate a gindx (by converting it to a lindx first) or a lindx directly.
Not part of the public interface (no declaration in the header), but visible
at file scope, so technically callable from other translation units if
someone includes the `.c` file directly - don't do that.

### `bool t_gset_lindx(...)` / `bool t_lset_lindx(...)`

Mark a slot as active and record which gindx it now holds.
`t_gset_lindx` looks the slot up *by gindx* (so you'd use this when you
already know which gindx you're claiming a slot for and want the tool to
find/validate the slot). `t_lset_lindx` is given the lindx directly and just
stamps the gindx into it - this is the one actually used by the asset-loading
path (`l_asset_manager.c` calls `t_lset_lindx`, not `t_gset_lindx`, right
after finding a free slot with `t_find_free_lindx`).

### `bool t_lfree_lindx(...)` / `bool t_gfree_lindx(...)`

Zero out a slot's `local_indx` entry (marking it inactive), looked up by
lindx or gindx respectively.

### `int t_gindx_to_lindx(struct local_indx *arr, int size, int gindx)`

Linear scan over `arr` looking for an entry whose `.gindx` matches. Returns
`NULL_INDX` if not found (or if `size < 0`). This is the "is this gindx
currently loaded, and if so where" lookup - called constantly throughout the
game-data modules before deciding whether to autoload something.

### `int t_lindx_to_gindx(struct local_indx *arr, int size, int lindx)`

The reverse lookup - given a slot index, what gindx does it currently hold.
Given `t_indxvalid`'s literal (bugged) behavior described above, this
function currently returns the *out-of-bounds* sentinel `NULL_INDX` when the
lindx is genuinely bad, and indexes into `arr[lindx]` when the lindx is
genuinely good - which happens to be the behavior you'd want, but only
because this one function's author used the function's literal return value
rather than its intended one. Once `t_indxvalid` is fixed to match its name
and its other callers, this function will need its condition flipped to
match.

---

# Layer 2 — Core systems

Everything in this layer is still generic engine machinery — none of it
knows what a "tile" or "item" is — but it depends on Layer 1, and Layer 3
(the actual game data) depends on it.

## `t_config_tool.h` / `.c`

This is the shared `.ini`-style config file parser used by every game-data
module in the codebase (stats, items, tiles, map, inventory, input, flags).
Understanding this file means understanding how *all* game data gets loaded.

### The `config_pack` handed to every parser

```c
struct config_pack{
    char line[256];
    char current_section[64];
    char key[64];
    char value[128];
};
typedef void (*ParserType)(struct config_pack, void *ptr);
```

Every module that reads a config file writes its own `ParserType` function
(e.g. `stat_parser` in `p_stats.c`, `tile_parser` in `m_map_tile.c`,
`item_parser` in `i_items.c`). `t_config` calls that function once per
`key = value` line it encounters, passing the current section name, key,
and value, plus an opaque `void *ptr` the caller supplied — almost always a
pointer to the specific struct being populated (a `struct BaseStats *`, a
`struct Item *`, etc). The parser function's job is just to look at
`p.current_section` and `p.key` and, if it recognizes them, write into
`*ptr` accordingly. This callback shape is what lets one generic file-reading
loop serve every different data type in the game.

### `bool t_check(char *line, char *arg)`

A `NULL`-safe `strcmp(line, arg) == 0`. Nearly every parser function in the
codebase is built out of chained `t_check` calls comparing `p.current_section`
or `p.key` against expected string literals. Logs `ERR_PARSE` (non-fatal)
and returns `false` if either argument is `NULL`.

### `bool t_config(void *ptr, char *path, ParserType func)`

The actual line-by-line `.ini` reader. For each line read via `fgets`:
- Blank lines, and lines starting with `#` or `;`, are skipped (comments).
- A line starting with `[` is parsed as a section header:
  `current_section` is updated via `sscanf(line, "[%63[^]]]", ...)`.
- Any other line is matched against `key=value` via
  `sscanf(line, "%63[^=]=%127[^\n]", key, value)`. If that succeeds, the
  value is trimmed of leading/trailing whitespace (including `\r`, so
  Windows-style line endings are tolerated), and `func(pack, ptr)` is called.

If `path` is `NULL` or the file fails to open, this logs `ERR_FUCKED` — which,
per the error-handler section above, is **fatal**: the process aborts. In
other words, a missing or unreadable config file is not a recoverable
condition anywhere in this codebase; it takes the whole program down. Treat
every call to `t_config` (directly, or indirectly via `t_loader`/
`l_load_asset`) as something that will crash the game if the underlying file
is missing, not something that returns `false` for you to check.

### `char *t_ini_plus_indx(char *base, int indx)`

Builds a path like `base/3.ini` (inserting a `/` if `base` doesn't already
end in one) using the measure-then-allocate `t_snprintf` pattern described in
the `t_strings.c` section above. Returns a heap-allocated string the caller
must `free()`. This is how the codebase maps a global index onto an actual
per-item config file — item #3 lives at `<items_path>/3.ini`, tile #7 at
`<tiles_path>/7.ini`, and so on.

### `bool t_loader(int gindx, struct local_indx *iman, ParserType func, char *path, void *ptr, int lindx)`

Combines the above two: builds the `gindx.ini` path under `path`, then calls
`t_config` on it. Returns `false` (logging `ERR_PARSE`, non-fatal) if
`t_config` returns `false` — though per the note above, `t_config` itself
usually crashes before returning `false` in most failure cases, since its
own failure paths go through `ERR_FUCKED`. This is the function
`l_asset_manager.c`'s `l_load_asset` calls to actually do the parsing, once
it's found a free array slot to parse the data into.

---

## `l_asset_manager.h` / `.c` — the shared load/get/free pattern

**This is the most important section to understand before reading any of
the game-data modules** (`p_stats`, `i_items`, `m_map_tile`, `i_inventory`).
Every one of them is a thin wrapper around this file's three functions. Once
you understand the pattern here, you can skim the per-module sections later
and just note what's different about each one.

### The idea in one paragraph

Each game-data module keeps two parallel fixed-size arrays: one holding the
actual struct data (`struct Item items[MAX_ITEMS]`, `struct BaseStats
bstats[STAT_CAP]`, etc.) and one holding `struct local_indx` bookkeeping
(which slots are occupied and by which global index — see `t_gindex_tool.c`
above). "Loading" a gindx means: find a free slot in the local-index array,
parse that gindx's `.ini` file directly into the corresponding data-array
slot, and mark the slot as occupied. "Freeing" a gindx means: find its slot,
zero it out, and mark it free again. Everything else (looking a gindx up
by whether it's currently loaded, autoloading on demand) is built out of
those two primitives.

### `struct AssetLoadPackage` / `struct AssetFreePackage`

```c
struct AssetLoadPackage{
    int gindx;
    struct local_indx *index_manager;
    int arr_cap;
    void *arr;
    size_t element_size;
    ParserType function;
    char *path;
};

struct AssetFreePackage{
    int gindx;
    struct local_indx *index_manager;
    int arr_cap;
    void *arr;
    size_t element_size;
};
```

These are just parameter bundles — every game-data module builds one of
these structs (as a compound literal, typically) and hands it to
`l_load_asset` or `t_free_asset`. `arr` is passed as `void *` and indexed
using `element_size`, which is how one generic function can operate on
arrays of any struct type without templates or macros. For example, from
`p_stats.c`:

```c
struct AssetLoadPackage pckg = {
    .gindx = gindx,
    .index_manager = indx_man,
    .arr_cap = STAT_CAP,
    .arr = bstats,
    .element_size = sizeof(struct BaseStats),
    .function = stat_parser,
    .path = e_grab_str(STATS_PATH),
};
return l_load_asset(pckg);
```

Every other module's load function (`t_load_item`, `t_load_tile`,
`i_load_inventory`) looks structurally identical to this, just with
different array/size/parser/path values.

### `bool l_load_asset(struct AssetLoadPackage pckg)`

1. Finds a free slot via `t_find_free_lindx(pckg.index_manager, pckg.gindx)`
   — note this passes `pckg.gindx` as the `size` argument, not
   `pckg.arr_cap`; see Known issues below.
2. Computes the byte address of that slot within `pckg.arr` using
   `pckg.element_size` (manual pointer arithmetic on a `void *`, cast to
   `char *` first since you can't do arithmetic on `void *` directly).
3. Calls `t_loader(...)` to parse the gindx's `.ini` file straight into that
   slot.
4. Calls `t_lset_lindx(...)` to mark the slot occupied.
5. Returns `true` (the return value doesn't currently reflect whether
   loading/setting actually succeeded — see Known issues).

> **Known issue:** Step 1 passes `pckg.gindx` where `t_find_free_lindx`
> expects a `size` (the capacity of the index-manager array to search). This
> means the search range depends on which gindx you're loading rather than
> the actual array capacity (`pckg.arr_cap`), which is almost certainly not
> intended — it should be `t_find_free_lindx(pckg.index_manager, pckg.arr_cap)`.
>
> **Known issue:** As covered in the `t_gindex_tool.c` section above, the
> `!t_indxvalid(...)` check immediately after step 1 currently treats a
> *successful* slot-find as a fatal "array is full" error, because of the
> `t_indxvalid` inversion bug. This function cannot currently complete a
> successful load without the codebase-wide `t_indxvalid` fix described
> earlier.
>
> **Known issue:** `l_load_asset` always returns `true` at the end,
> regardless of whether `t_loader` or `t_lset_lindx` actually reported
> success — it logs a fatal error if either failed (which would have already
> aborted the process), but if a future version of those functions ever
> became non-fatal on failure, this return value would silently lie.

### `bool t_free_asset(struct AssetFreePackage pckg)`

Looks up the gindx's current slot via `t_gindx_to_lindx`, zeroes that slot's
bytes with `memset`, and frees it in the index manager via `t_lfree_lindx`.
Note the naming inconsistency — this is declared as `t_free_asset` (with a
`t_` prefix matching the `t_*` tools) in `l_asset_manager.h`, even though
it's implemented in `l_asset_manager.c` alongside the `l_`-prefixed
functions. Not a bug, just something that can be confusing when searching
the codebase by prefix.

**Important:** modules whose data includes heap-allocated pointers (notably
`i_items.c`, whose `struct Item` holds `char *strs[ISTR_COUNT]`) do **not**
use `t_free_asset` for freeing, specifically because a plain `memset` would
leak those strings rather than freeing them first. `i_items.c`'s
`t_free_item` frees each string explicitly before zeroing the slot. Keep this
in mind if you add a new data type with owned pointers — `t_free_asset`
alone is not sufficient for it.

### `int l_getter_checks(int gindx, bool autoload, int cap, struct local_indx *iman, Loader ldr)`

This is the "get me the slot for this gindx, autoloading it first if
necessary and allowed" helper that every module's public getter function
(`i_get_pckitemdata`, `i_get_inv_proto`, `p_get_dev`, etc.) calls before
touching its data array. Given a `Loader` function pointer (matching
`typedef bool (*Loader)(int gindx)` — e.g. `t_load_item`, `i_load_inventory`)
it will:

- If `autoload` is true: check whether the gindx is already loaded. If the
  (buggy, see above) `t_indxvalid` check says it's *not* already loaded, call
  `ldr(gindx)` to load it, then look up the resulting lindx again.
- If `autoload` is false: just look up the gindx's current lindx. If it
  isn't loaded, log `ERR_INDX` (non-fatal) and **recursively call itself
  with `autoload = true`** — so passing `autoload = false` really means "try
  to find it already loaded, but load it anyway if not," not "never load
  it." Every module's public getter functions (see Layer 3) offer an
  `autoload` parameter with this same recursive-fallback behavior.

Returns the resolved lindx (or `NULL_INDX` on failure, subject to the same
`t_indxvalid` bug described earlier affecting whether "failure" is even
detected correctly right now).

---

## `g_statemachine.h` / `.c`

A small fixed-depth state stack, used to drive which game state (menu,
combat, exploration, etc.) is currently receiving update/render calls.

### `struct GameStateNode`

```c
struct GameStateNode{
    StateFunc on_enter;
    StateFunc on_exit;
    StateFunc on_update;
    StateFunc on_render;
    StateFunc on_pause;
    StateFunc on_resume;
};
```

Each game state (see `s_menu.c`, `s_combat.c` in Layer 3) is just a bundle of
six function pointers (`typedef void (*StateFunc)(void);`), built and
returned by a constructor function like `menu_state()`. The state machine
itself doesn't know or care what a "menu" or "combat" state actually does —
it just calls whichever of these six pointers is relevant at the right time.

Note `on_pause`/`on_resume` are part of the struct but nothing in either
batch of files (`sm_push`/`sm_pop`/`sm_switch` included) currently calls
them — they're wired into the data structure but not yet invoked anywhere in
the state machine logic itself. Worth confirming that's intentional
(perhaps meant for a future feature) rather than an oversight.

### `void sm_init(struct GameStateNode node)`

Just calls `sm_push(node)`. This is what `main.c` calls once at startup with
`menu_state()` to seed the stack.

### `void sm_push(struct GameStateNode node)` / `void sm_pop(void)`

Push adds a new state on top of the stack (calling its `on_enter`), up to
`MAX_STATE_STACK` (8) deep — logging `ERR_FUCKED` (**fatal**) if the stack is
already full. Pop calls the current top state's `on_exit`, then removes it.
Popping an empty stack is a silent no-op (not an error).

### `void sm_switch(struct GameStateNode node)`

Pops the current state and pushes a new one — used for transitions where
you don't want the old state reachable again (the header comment gives
"explore → combat" as the example: ending combat shouldn't return to a
stale explore state below it, since there wasn't one pushed underneath).

### `void sm_update(void)` / `void sm_draw(void)`

Called once per frame from `main.c`'s game loop. Each calls the
`on_update`/`on_render` function pointer of whichever state is currently on
top of the stack (a no-op if the stack is empty, or if that particular
pointer is `NULL`).

---

## `f_flags.h` / `.c`

A simple global boolean flag store, loaded once from a config file at
startup and queried by name for the rest of the program's life.

### `struct FlagManager`

```c
struct Flag{
    uint32_t hash;
    bool     value;
};
struct FlagManager{
    struct Flag flags[MAX_FLAGS];
    int count;
};
```

Flags are stored as (FNV-1a hash of the name, value) pairs in a flat array,
searched linearly by hash rather than by string comparison — a small
optimization over storing/comparing the flag name strings directly, at the
cost of (extremely unlikely) hash collisions being silently treated as the
same flag.

### `void f_init_flag(void)`

Allocates the single global `struct FlagManager *fm` via `XCALLOC`, then
parses the flag config file (`e_grab_str(FLAG_PATH)`) via `t_config`, using
`flag_parser` as the callback. `flag_parser` treats a value of `"1"` or
`"true"` as `true` and anything else as `false`, then calls `flag_set` for
every key in the file. If `fm` is already initialized when this is called,
it logs `ERR_RELOAD` (non-fatal) and calls `f_free_flag()` first before
re-initializing — so calling `f_init_flag()` twice in a row is tolerated,
just logged.

### `void f_free_flag(void)`

Frees `fm` and sets it back to `NULL`. Logs `ERR_RELOAD` (non-fatal, not
fatal despite the name) if called when `fm` is already `NULL` — this is a
"tried to double free" warning, not a crash.

### `bool flag_get(const char *name)` / `void flag_set(const char *name, bool value)`

`flag_set` either updates an existing flag's value (if its hash matches one
already stored) or appends a new entry (silently doing nothing if
`MAX_FLAGS` is already full — no error logged in that specific case, which
is worth knowing since it means a flag can silently fail to register if the
game ever grows past 256 distinct flags). `flag_get` returns `false` for any
name it doesn't recognize, without logging anything in that case. Both log
`ERR_FUCKED` (**fatal**) if called before `f_init_flag()` has run.

---

## `e_engine_settings.h`

Only the header was provided in these batches (no `e_engine_settings.c`),
but its shape is important since almost every game-data module calls
`e_grab_str(...)` to find out where its config files live:

```c
enum EngStrings{
    MAP_PATH, TILE_PATH, PORTRAIT_PATH, FLAG_PATH, ITEMS_PATH,
    STATS_PATH, INVENTORY_PATH, INPUT_PATH, ENG_STR_COUNT,
};
struct EngineSettings{
    char *strings[ENG_STR_COUNT];
};

char *e_grab_str(enum EngStrings type);
void e_free_setting();
bool e_load_engine_settings();
```

`e_grab_str` is presumably a simple bounds-checked accessor into a global
`struct EngineSettings`, returning the path string associated with a given
`EngStrings` enum value (e.g. `e_grab_str(ITEMS_PATH)` returns the directory
where per-item `.ini` files live, which `i_items.c`'s `t_load_item` then
passes to `l_load_asset`). Several comments elsewhere in the codebase (e.g.
in `f_flags.c` and `i_input.c`) assert that `e_grab_str` "cannot return
NULL" for a valid enum value — worth confirming that guarantee once
`e_engine_settings.c` is available, since several other modules currently
rely on it without checking.

---

# Layer 3 — Game-specific systems

Everything from here on is specific to this particular game, but the
modules that manage loaded data (`p_stats`, `i_items`, `m_map_tile`,
`i_inventory`) all reuse the load/get/free pattern documented in the
`l_asset_manager.c` section above — if something here looks unfamiliar,
that's the section to go back to.

## `p_stats.h` / `.c`

Defines the six base stats and a larger set of derived stats/attributes
computed from them.

### `enum Stats` and `enum Dev`

```c
enum Stats{ STR, DEX, CON, SOC, INT, WIS, BSTAT_COUNT };
```
The six base attributes: strength, dexterity, constitution, social,
intelligence, wisdom.

```c
enum Dev{
    PHYSICAL_COORDINATION, WORD, PROB_ANALYSIS, SPATIAL, MUSICAL, NATURAL,
    INTERPERSONAL, INTRAPERSONAL, INNOCENCE, HEROISM, LOVE, AUTHORITARIAN,
    AC, DR, MAX_HP, INITIATIVE, DERV_CAP
};
```
"Dev" (derived) values are computed on the fly from combinations of base
stats — things like `AC` (armor class), `DR` (damage reduction), `MAX_HP`,
plus a set of "personality"/skill-flavored derived values
(`PHYSICAL_COORDINATION`, `WORD`, `HEROISM`, etc). None of these are stored
directly; they're all calculated by `p_get_dev` from whichever base stats
are currently loaded for a given gindx.

### `struct BaseStats` and the asset pattern

```c
struct BaseStats{
    int basestats[BSTAT_COUNT];
};
static struct BaseStats bstats[STAT_CAP] = {0};
static struct local_indx indx_man[STAT_CAP] = {0};
```

This follows the standard pattern from `l_asset_manager.c`: `t_load_stat`
and `t_free_stat` build an `AssetLoadPackage`/`AssetFreePackage` and delegate
straight to `l_load_asset`/`t_free_asset`. `stat_parser` is the `ParserType`
callback — it only looks at the `[stats]` section of a stat `.ini` file and
matches keys against `base_lookup[]` (`"strength"`, `"dexterity"`, etc.) via
`t_atoi`.

### `int p_get_dev(int gindx, enum Dev d, bool autoload)`

The one function outside the standard pattern that does real work. Given a
gindx and which derived value you want, it:
1. Resolves the gindx to a `struct BaseStats *` — following the same
   "look it up, autoload if requested/needed" logic as
   `l_getter_checks` (written out longhand here rather than calling that
   shared helper directly, so it's slightly duplicated compared to
   `i_items.c`/`i_inventory.c`, which do call it).
2. Runs a `switch` over `enum Dev` computing each derived value from a
   small formula over the base stats, e.g.:
   ```c
   case PHYSICAL_COORDINATION:
       return (int)ceil(((b[STR] + b[DEX] - b[INT]) / 2.0) / 4.0);
   ...
   case MAX_HP:
       return (int)ceil((b[CON] + b[STR]) - b[DEX]);
   ```

This is the formulas file — if you're trying to understand or rebalance
character math, this `switch` statement is the entire ruleset.

---

## `p_entity.h` / `.c`

The minimal "a thing that exists in the world at a position" type.

### `struct Entity`

```c
enum EntityData{ D_STAT, D_INV, D_COUNT };
struct Entity{
    int x;
    int y;
    int data_gindx[D_COUNT];
};
```

An entity is just a position plus an array of gindxes pointing at its
associated data — `data_gindx[D_STAT]` is the gindx of this entity's stat
block (loadable via `p_stats.c`), `data_gindx[D_INV]` is presumably meant
similarly for an inventory gindx (`i_inventory.c`). Entities don't own or
embed their stats/inventory directly — they just reference them by gindx,
which is the same "loaded elsewhere, referenced by index" philosophy as the
rest of the codebase.

### `struct Entity *p_init_entity(int tx, int ty, int stat)`

> **Known issue — this function does not compile as written.**
> ```c
> struct Entity *p_init_entity(int tx, int ty, int stat){
>     struct Entity *e = calloc(1, sizeof(struct Entity));
>     e->x = tx;
>     e->y = ty;
>     e->stats = stat;   // struct Entity has no member named "stats"
>     return e;
> }
> ```
> `struct Entity` has `x`, `y`, and `data_gindx[D_COUNT]` — there is no
> `stats` member. This almost certainly should be
> `e->data_gindx[D_STAT] = stat;` to match the struct's actual shape.
>
> **Also note:** this function calls raw `calloc()` instead of `XCALLOC`,
> which is the only allocation in either batch that doesn't go through the
> project's standard allocator. That means it doesn't get the fatal
> out-of-memory check every other allocation in the codebase gets — if
> `calloc` returns `NULL` here, the very next line (`e->x = tx`) dereferences
> a null pointer instead of cleanly aborting with a logged message.

### `void p_free(struct Entity *e)`

Just `free(e)`. Since entities don't own their stat/inventory data directly
(only reference it by gindx), this doesn't need to cascade-free anything —
but it also means nothing frees the referenced stat/inventory gindx
automatically when an entity goes away; that's presumably the caller's
responsibility (via `t_free_stat`/`i_free_inventory`) and worth keeping in
mind to avoid leaking loaded slots.

### `void p_move_x(struct Entity *e, bool can_move, int dir)` / `p_move_y(...)`

Straightforward: if `can_move` is true, nudge `e->x`/`e->y` by
`dir * SPEED` (`SPEED` is `1`). If `can_move` is false, print a message to
stdout and do nothing. `dir` is presumably expected to be `-1`, `0`, or `1`
(no validation currently enforces this).

---

## `i_items.h` / `.c`

Item definitions, including a packed 32-bit encoding for "this item adds N
to stat X" style effects.

### The packed dataset format

```c
enum ItemData{ S_ADD, S_HIT, S_DAMAGE, S_CONSUME, S_FLAG, S_RANGE, IDATA_COUNT };
struct Item{
    uint32_t dataset[IDATA_COUNT];
    char *strs[ISTR_COUNT];
};
```

Most entries in `dataset[]` are **packed**: the high 16 bits store a "stat"
value, the low 16 bits store an "amount" value, via:

```c
uint32_t pack_dataset(uint16_t a, uint16_t b){
    return((uint32_t) a << 16) | b;
}
struct ItemDataSet unpack(uint32_t packed){
    struct ItemDataSet set = {0};
    set.stat = (packed >> 16) & 0xFFFF;
    set.amount = packed & 0xFFFF;
    return set;
}
```

So, for example, `dataset[S_ADD]` for a "+5 strength" item would have `stat
= STR` packed into its top 16 bits and `amount = 5` packed into its bottom
16 bits. `i_inventory.c`'s `i_get_bonus_matrx` (documented below) is the
main consumer of this — it unpacks each hotbar item's `S_ADD` entry to
figure out which derived stat to bump and by how much.

`dataset[S_FLAG]` is the one exception — it's used as a plain bitmask
(`enum ItemFlags`: `FLAG_THROWABLE`, `FLAG_CONSUMEABLE`) rather than a
packed stat/amount pair. `dataset[S_RANGE]` is also handled specially in the
parser (a plain integer, `range_per_tiles`, not a packed pair) despite being
declared as part of the same `uint32_t dataset[]` array as the packed
entries — worth knowing so you don't accidentally try to `unpack()` it.

### Config sections

An item `.ini` file has one section per `ItemData` entry name (`[add]`,
`[hit]`, `[damage]`, `[consume]`, `[range]`), each expecting `Stat=` and
`Amount=` keys (packed into the corresponding `dataset[]` slot) — except
`[range]`, which instead expects a `range_per_tiles=` key read as a plain
integer, and `[flags]` (matching `itemdata_lokup[S_FLAG]`, i.e. `"flags"`),
which expects boolean-ish keys matching `itemflag_lokup[]`
(`"throwable"`, `"consumeable"`). There's also a `[Strings]` section for
`name=`/`description=`, copied via `t_cpy` into `item->strs[]`.

### `bool t_load_item(int gindx)` / the asset pattern

Standard pattern via `l_load_asset`, same as stats — nothing unusual here.

### `bool t_free_item(int gindx)`

> **Note (not a bug, a deliberate and correct deviation):** unlike
> `t_free_stat`/`t_free_tile` (which just call `t_free_asset` directly),
> `t_free_item` does **not** use `t_free_asset`. It explicitly frees each
> non-NULL entry in `item->strs[]` first, *then* zeroes the slot and frees it
> from the index manager by hand. This is because `t_free_asset`'s generic
> `memset`-based free would leak the heap-allocated name/description strings
> rather than freeing them. This is the example referenced in the
> `l_asset_manager.c` section above — any future data type that owns heap
> pointers needs this same treatment, not the generic `t_free_asset` path.

### `uint32_t i_get_pckitemdata(int gindx, enum ItemData d, bool autoload)` / `char *i_get_pckitemstrs(...)`

Public getters — both go through `l_getter_checks` (the shared
autoload-or-fail helper from `l_asset_manager.c`) to resolve the gindx to a
lindx, then index into `items[lindx].dataset[d]` or `items[lindx].strs[d]`
respectively. Both log `ERR_FUCKED` (**fatal**) if the gindx couldn't be
resolved, and both log `ERR_FUCKED` if `d` is out of range for its
respective enum — so passing a bad enum value crashes rather than returning
a sentinel.

---

## `i_inventory.h` / `.c`

Inventory slots (hotbar + backpack) as arrays of item gindxes, plus the
logic for turning equipped hotbar items into a matrix of derived-stat
bonuses.

### `struct Inventory`

```c
#define HOTBAR_SIZE 8
#define INVENTORY_SIZE 32
struct Inventory{
    int hotbar_items[HOTBAR_SIZE];
    int inventory[INVENTORY_SIZE];
};
```

Both arrays just hold item gindxes (`0` presumably meaning "empty slot,"
though nothing explicitly enforces that convention in this file — worth
confirming against whatever gindx numbering scheme the item data actually
uses, since `0` could also be a legitimate item's gindx).

### `i_load_inventory` / `i_free_inventory` / the asset pattern

Standard `l_asset_manager` pattern again — `inv_parser` reads a `[hotbar]`
section and an `[inventory]` section, where each recognized key is a
stringified index (`"0"`, `"1"`, …) built at parse time via `t_snprintf`
and compared against `p.key`. This means an inventory `.ini` file's hotbar
section looks like:
```ini
[hotbar]
0=12
1=45
```
— key `0` maps to `hotbar_items[0]`, and so on.

### `struct Inventory i_get_inv_proto(int gindx, bool autoload)`

Resolves and returns a **copy** of the loaded `struct Inventory` for a
gindx (via `l_getter_checks`), or a zeroed-out `struct Inventory` if
resolution failed (logging `ERR_NULL`, non-fatal, in that case — this
function does not crash on failure, unlike most of the item getters above).

### `struct DervBonusMatrix i_get_bonus_matrx(int gindx, bool autoload)`

The interesting one. Given an inventory gindx, this:
1. Loads that inventory via `i_get_inv_proto`.
2. For each of the `HOTBAR_SIZE` hotbar slots, treats the stored value as an
   **item gindx** and calls `i_get_pckitemdata(inv.hotbar_items[i], S_ADD,
   true)` — i.e., autoloads that item and reads its packed "add" dataset.
3. Unpacks that into `{stat, amount}` via `unpack()`.
4. Writes `matrx.derv[data.stat] = data.amount` — note this is a plain
   assignment, not `+=`, so if two hotbar items both wanted to boost the
   same derived stat, the second one processed would overwrite the first
   rather than stacking. Worth confirming whether that's intended (only one
   item per stat bonus allowed) or should actually accumulate.

The comment in the source flags this function's biggest operational
consequence directly: calling it autoloads **both** the inventory and every
item referenced in its hotbar into memory, and the caller is responsible for
freeing them afterward if desired — this function doesn't do any cleanup of
what it loads.

---

## `m_map_tile.h` / `.c`

Defines what a single *tile type* is capable of — its content indexes
(portrait, text, combat encounter, loot table) and a bitmask of interaction
flags.

### `struct TileData`

```c
enum Flags{ FLAG_LOOK, FLAG_INTERACT, FLAG_LOOTABLE, FLAG_DIALOGUE, FLAG_COMBAT, FLAG_COUNT };
enum TileInfo{ T_PORTRAIT, T_TEXT, T_COMBAT, T_LOOT, T_COMBAT_LOOT, T_COUNT };
struct TileData{
    int indexes[T_COUNT];
    uint16_t tile_flags;
};
```

`indexes[]` holds gindxes pointing at other loadable data (a portrait
asset, dialogue text, a combat encounter definition, a loot table) depending
on which `TileInfo` slot. `tile_flags` is a bitmask built from `enum Flags`
— `FLAG_INTERACT`, `FLAG_LOOTABLE`, etc. — describing what kinds of player
interaction this tile type supports.

### Config sections

A tile `.ini` has a `[DataIndexes]` section (keys matching
`tiledata_lokup[]`: `"portrait"`, `"text"`, `"combat"`, `"loot"`,
`"combat_loot"`) and a `[Flags]` section (keys matching `flag_lookup[]`:
`"look"`, `"interact"`, `"lootable"`, `"dialogue"`, `"combat"`). Each flag
key's value is parsed as an integer and treated as a boolean — `> 0` sets
the corresponding bit in `tile_flags`.

`t_load_tile` / `t_free_tile` follow the standard `l_asset_manager` pattern
exactly like `p_stats.c` — nothing unusual here.

---

## `m_map.h` / `.c`

Assembles an actual playable map out of a grid of tile references, using a
two-pass parse: first read map-wide metadata to know how big to allocate,
then parse the actual tile grid into that allocation.

### `struct MapData` — a flexible array member

```c
struct MapTileData{
    int  tile_indx;
    bool is_passable;
    int  tile_texture_index;
    int  start_x;
    int  start_y;
    int  end_x;
    int  end_y;
};
struct MapData{
    int tileset;
    int count;
    struct MapTileData data[];   // flexible array member
};
```

`data[]` is a C99 flexible array member — `struct MapData` is allocated with
exactly enough trailing space for `count` `MapTileData` entries, computed as
`sizeof(struct MapData) + count * sizeof(struct MapTileData)`. This is why
map loading needs two passes: you can't allocate `struct MapData` until you
know `count`, and you don't know `count` until you've read the metadata
section of the map file.

### The two-pass load in `m_init(void)`

```c
struct MetadataTemp *meta = XMALLOC(sizeof(struct MetadataTemp));
bool win = t_config(meta, filepath, parse_meta);      // pass 1: just get count/tileset
size_t size = sizeof(struct MapData) + meta->count * sizeof(struct MapTileData);
m_free();
map_ptr = XCALLOC(1, size);                            // now we know how big to allocate
map_ptr->count = meta->count;
map_ptr->tileset = meta->tileset;
bool win_2 = t_config(map_ptr, (char *)filepath, parse_map);  // pass 2: fill in the tile grid
free(meta);
```

Pass 1 (`parse_meta`) only looks at the `[metadata]` section for `count=`
and `tileset=`. Pass 2 (`parse_map`) re-reads the *same file* from the
start, this time populating the actual `data[]` array, and also handles a
separate concept, "palettes" (see below).

### Palettes: `g_pallete[]`

```c
struct MapTileData g_pallete[MAX_STRUCTS] = {0};   // MAX_STRUCTS = 512
```

A map file can define named tile "palette" presets under `[palletes.N]`
sections (note: spelled "pallete"/"palletes" throughout the source, not
"palette"/"palettes" — this is consistent internally, just worth knowing
when grepping) — each one sets `tile_indx` and `tile_texture_index` for
palette slot `N`. Then, in a `[function.N]` section (representing one
placed tile on the map grid), a `pallete=` key looks up
`g_pallete[pallete_index]` and copies its `tile_indx`/`tile_texture_index`
into that grid cell — a shorthand so map authors don't need to repeat the
same tile/texture pair for every placed tile of the same type. `start_x`,
`start_y`, `end_x`, `end_y`, and `passable` are set directly per-`function.N`
section rather than via the palette.

> **Known issue — heap buffer overflow via `function.N` sections.**
> ```c
> if(sscanf(p.current_section, "function.%d", &func_indx) == 1){
>     if(func_indx < 0 || func_indx >= MAX_FUNCTIONS){return;}  // MAX_FUNCTIONS = 512
>     ...
>     m->data[func_indx].start_x = ...
> ```
> `m->data` was allocated to hold exactly `meta->count` entries — not 512.
> The bounds check here uses the fixed constant `MAX_FUNCTIONS` instead of
> `m->count` (or `meta->count`, same value). Any map `.ini` with
> `meta.count` smaller than 512 but a `function.N` section where `N` falls
> between `meta->count` and 511 writes past the end of the allocation. This
> is driven entirely by map file content — any hand-edited or malformed map
> file can trigger it, not just a contrived edge case. **Fix: bound-check
> against `m->count` instead of `MAX_FUNCTIONS`.**

> **Known issue — unchecked palette index.**
> ```c
> } else if(t_check(p.key, "pallete")){
>     int pallete;
>     t_atoi(p.value, &pallete);
>     m->data[func_indx].tile_indx = g_pallete[pallete].tile_indx;
> ```
> The parallel `[palletes.N]`-parsing branch just above this one *does*
> bounds-check its index against `MAX_STRUCTS`. This branch doesn't check
> `pallete` at all before indexing `g_pallete[pallete]` — a negative or
> oversized value from the map file reads out of bounds directly. **Fix:**
> add the same `< 0 || >= MAX_STRUCTS` guard used in the palette-definition
> branch.

> **Known issue — spurious "double free" warning on the very first map
> load.**
> ```c
> void m_init(void){
>     ...
>     m_free();   // called unconditionally, even before map_ptr was ever set
>     map_ptr = XCALLOC(1, size);
> ```
> `map_ptr` starts `NULL`. On the very first call to `m_init()`, this
> unconditional `m_free()` call hits `m_free`'s `else` branch:
> ```c
> void m_free(){
>     if(map_ptr){ free(map_ptr); return; }
>     ERR_LOG(ERR_NULL, "Tried to double free map");  // non-fatal, so it prints
> }
> ```
> `ERR_NULL` is non-fatal, so this doesn't crash — but it *does* log a
> "double free" warning every single time the game starts, even though
> nothing has actually been double-freed. This will read as an alarming
> false positive to anyone checking logs. **Fix:** only call `m_free()` from
> `m_init()` if `map_ptr` is already non-NULL.

### `void m_free(void)`

Frees `map_ptr` and — notably — does **not** set it back to `NULL`
afterward (compare with `f_free_flag`, which does reset its global pointer
to `NULL`). Combined with the unconditional `m_free()` call inside `m_init`
above, this is a secondary concern worth flagging: after a normal (non-first)
`m_init()` call sequence, `map_ptr` is freed and then immediately reassigned
by `XCALLOC`, so this doesn't cause an immediate dangling-pointer read in
practice — but if `m_free()` is ever called from somewhere else in the
program without an immediate reassignment, `map_ptr` would be left as a
dangling pointer rather than `NULL`, and a subsequent `m_free()` call would
then double-free instead of hitting the "already NULL" branch.

---

## `i_input.h` / `.c`

Config-driven keybinding: reads an `.ini` file mapping actions to physical
keys/buttons, then exposes simple `pressed`/`held`/`released` queries by
`enum Action`.

### The binding model

```c
enum Action{ A_WALK_LEFT, A_WALK_RIGHT, A_WALK_UP, A_WALK_DOWN, A_ACTION, A_COUNT };
enum KeyType{ KEYBOARD, MOUSE, GAMEPAD, TYPE_COUNT };
#define MAX_KEYS 4
struct KeySet{
    struct InputKey keys[MAX_KEYS];   // up to 4 physical keys per action
    bool is_bound, is_pressed, is_held, is_released;
};
static struct KeySet input[A_COUNT] = {0};
```

Each `Action` can have up to `MAX_KEYS` (4) physical keys bound to it
simultaneously (e.g. both `W` and the up-arrow bound to `A_WALK_UP`) — any
one of them being pressed/held/released satisfies the corresponding query
for that action, via bitwise-OR accumulation in `i_update_input`.

### Loading bindings: `i_init_input(void)`

Parses `.ini` sections named `bind.N`, each expecting `Action=`,
`Input_Type=`, and `Key=` keys, via the `input_parser` callback and a
temporary `struct parser_set` array sized `MAX_KEYS * A_COUNT`. After
parsing, it walks that temporary array and calls `i_set_binding(...)` for
every entry that ended up with a fully-specified action/type/key —
partially-specified or malformed `bind.N` entries are skipped with a logged
`ERR_PARSE` (non-fatal).

`ac_lokup`/`ty_lokup` are the string→enum lookup functions for the `.ini`
file's `Action=`/`Input_Type=` values (`"walk_left"` → `A_WALK_LEFT`,
`"Keyboard"` → `KEYBOARD`, etc).

> **Known issue — `"action"` binds to the wrong enum value.**
> ```c
> static enum Action ac_lokup(char *str){
>     if(t_check(str, "walk_left")){return A_WALK_LEFT;}
>     if(t_check(str, "walk_right")){return A_WALK_RIGHT;}
>     if(t_check(str, "walk_up")){return A_WALK_UP;}
>     if(t_check(str, "walk_down")){return A_WALK_DOWN;}
>     if(t_check(str, "action")){return A_WALK_DOWN;}   // should be A_ACTION
>     return INVALID_ENUM;
> }
> ```
> Binding `Action=action` in the config currently resolves to `A_WALK_DOWN`
> instead of `A_ACTION`. Two consequences: `A_ACTION` (the game's generic
> "interact/use" action) can never be bound from config as written, and
> binding `"action"` will silently clobber whatever key was meant for
> walking down. **Fix:** `if(t_check(str, "action")){return A_ACTION;}`

> **Known issue — `i_init_input()` is never called.** Nothing in `main.c`
> (or anywhere else in either batch) calls this function. Since
> `input[].is_bound` starts `false` and only `i_set_binding` (called from
> inside `i_init_input`) ever sets it `true`, every call to
> `i_input_pressed`/`i_input_held`/`i_input_released` will return `false`
> unconditionally until this is wired into startup — likely alongside
> `f_init_flag()` and `sm_init()` in `main.c`. This may simply be a missing
> line rather than a design gap, but it's the kind of thing that reads as
> "nothing responds to input" with no obvious cause if you don't know to
> check here.

### `i_update_input` / `i_input_pressed` / `i_input_held` / `i_input_released`

Each public query function checks `is_bound` first (returning `false`
immediately if the action has no bindings at all), then calls
`i_update_input(action)` to refresh the current frame's pressed/held/
released state by polling raylib (`IsKeyPressed`/`IsGamepadButtonPressed`/
etc. — note mouse buttons are declared in the `KeyType` enum and the
`InputKey` union, but `i_update_input`'s polling loop only actually branches
on `KEYBOARD` and `GAMEPAD`, not `MOUSE` — so a binding of type `MOUSE`
would never update `is_pressed`/`is_held`/`is_released`, worth confirming
that's intentional or an oversight if mouse bindings are meant to work),
then returns the relevant field. Calling `i_update_input` redundantly for
every query (rather than once per frame) is slightly wasteful but not
incorrect, since it's idempotent within a frame.

### `i_input_get_mouse_x/y`

Thin wrappers over raylib's `GetMouseX`/`GetMouseY`. Note the header
declares these as returning `float`, but raylib's `GetMouseX`/`GetMouseY`
return `int` — the implicit conversion is harmless here, just worth knowing
if you're grepping for float-returning mouse functions elsewhere.

---

## `w_window_manager.h` / `.c`

Resolution-independence helpers: the game is designed against a fixed
"base" resolution (1280×720) and these functions scale arbitrary pixel
values up or down to match whatever the actual window size currently is.

### The scale factors

```c
#define BASE_X 1280
#define BASE_Y 720
float w_relative_scale_x = 1.0f;
float w_relative_scale_y = 1.0f;

void w_update_relative_scale(){
    w_relative_scale_x = (float)GetScreenWidth()/w_base_res_x;
    w_relative_scale_y = (float)GetScreenHeight()/w_base_res_y;
}
```

`main.c`'s game loop calls `w_update_relative_scale()` once per frame,
before anything else, so these two globals always reflect the current
window size relative to the 1280×720 baseline. Everything else in this file
is just a convenience wrapper multiplying some input value by one or both
of these factors.

### The `wcn*` family ("window convert")

- `wcnfx(num)` / `wcnfy(num)` — scale a `float` by the X or Y factor
  respectively.
- `wcnix(num)` / `wcniy(num)` — same, but for `int`, casting the result back
  to `int`.
- `wcnv(Vector2)` — scales both components of a raylib `Vector2` at once
  (by X and Y respectively).
- `wcnf(num)` / `wcni(num)` — a variant that doesn't pick a single axis;
  instead it picks *whichever* scale factor is larger (`x > y ? x : y`) and
  applies that one. This is presumably meant for things like font sizes or
  UI element dimensions that should scale uniformly rather than stretching
  independently per-axis on a non-square aspect-ratio change.

> **Known issue — `wcnfx` scales by the wrong axis.**
> ```c
> float wcnfx(float num){
>     float x = w_relative_scale_y;   // should be w_relative_scale_x
>     return num * x;
> }
> ```
> The "x" variant multiplies by `w_relative_scale_y` instead of
> `w_relative_scale_x`. Compare with `wcniy` (the integer Y variant), which
> correctly uses `w_relative_scale_y`, and `wcnix`, which correctly uses
> `w_relative_scale_x` — `wcnfx` alone looks like a copy-paste slip.
> Anything scaled horizontally through `wcnfx` currently scales by the
> vertical ratio instead, which will look visibly wrong the moment the
> window's aspect ratio differs from 16:9. **Fix:**
> `float x = w_relative_scale_x;`

---

## Game states: `s_menu`, `s_combat`, `s_action`, `s_graphics`

Each of these builds and returns a `struct GameStateNode` (see
`g_statemachine.h` above) — a bundle of six function pointers the state
machine calls at the appropriate time. All four headers follow the exact
same one-function shape:

```c
struct GameStateNode menu_state(void);
struct GameStateNode combat_state(void);
struct GameStateNode action_state(void);
struct GameStateNode graphics_state(void);
```

### `s_menu.c` — the only fully-implemented state in either batch

```c
void menu_render(){
    DrawRectangle(0,0,GetScreenWidth(), GetScreenHeight(), PL_LIGHT_BROWN);
    DrawText("Background Texture Here",wcnf(10.f),wcnf(10.f), wcnf(20.f), BLACK);
}
```

Everything else (`menu_enter`, `menu_exit`, `menu_update`, `menu_pause`,
`menu_resume`) is currently an empty stub. `menu_render` is a placeholder
screen — note it does use `wcnf(...)` (the window-scaling helper from
`w_window_manager.c`) for its position/size values, which is the intended
pattern for any future render code in other states too, so screen elements
stay correctly positioned across different window sizes.

### `s_combat.c`

All six functions (`combat_enter/exit/update/render/pause/resume`) are
present but empty — this is scaffolding for a state that hasn't been
implemented yet, not a bug.

### `s_action.h`, `s_graphics.h`

> **Known issue — no corresponding `.c` file was included in either batch.**
> Both headers declare a state-constructor function (`action_state()`,
> `graphics_state()`) with no definition provided. If these definitions
> genuinely don't exist yet anywhere in the project, anything that calls
> them (or a build that tries to link a binary requiring them) will fail at
> link time. If they simply weren't included in this documentation pass,
> disregard this note — but it's worth confirming one way or the other.

---

# Layer 4 — Entry point

## `main.c`

This is the shortest file in the codebase and the one place that ties every
layer together. Reading it after everything above should feel like
recognizing old friends rather than encountering new concepts.

```c
int main(){
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(w_base_res_x, w_base_res_y, "Game");
    SetTargetFPS(TARGET_FPS);
    srand(time(NULL));

    bool parsed = e_load_engine_settings();
        if(!parsed){ERR_LOG(ERR_FUCKED, "Engine prase failed!");}
    f_init_flag();
    sm_init(menu_state());

    while (!WindowShouldClose()){
        w_update_relative_scale();
        sm_update();

        BeginDrawing();
            ClearBackground(DARKGRAY);
            sm_draw();
        EndDrawing();
    }

    e_free_setting();
    f_free_flag();
    r_cleanup_textures();
    CloseWindow();
    return 0;
}
```

Walking through it against everything documented above:

1. **Window setup** — raylib's own `InitWindow`/`SetTargetFPS`, using the
   base resolution constants from `w_window_manager.h` as the *initial*
   window size (the window is resizable afterward, hence
   `FLAG_WINDOW_RESIZABLE` and the per-frame `w_update_relative_scale()`
   call later).
2. **Engine settings** — `e_load_engine_settings()` populates the path table
   (`e_engine_settings.h`) that every game-data module's loader depends on
   (`e_grab_str(ITEMS_PATH)`, etc). If this fails, `ERR_LOG(ERR_FUCKED, ...)`
   is fatal, so the "if failed" check here is really just for the log
   message on the way down, not a recoverable branch.
3. **Flags** — `f_init_flag()` loads the global boolean flag table
   (`f_flags.c`).
4. **State machine seed** — `sm_init(menu_state())` pushes the menu state
   (`s_menu.c`) as the initial (and, at program start, only) entry on the
   state stack (`g_statemachine.c`).
5. **The game loop** — once per frame: refresh the window-scale factors
   (`w_window_manager.c`), update whichever state is on top of the stack
   (`sm_update()`), then draw it (`sm_draw()`) between raylib's
   `BeginDrawing()`/`EndDrawing()` pair.
6. **Shutdown** — free engine settings, free the flag table, clean up
   textures, close the window.

> **Known issue — missing include/dependency.** `main.c` includes
> `"r_asset_manager.h"` and calls `r_cleanup_textures()` at shutdown, but
> neither `r_asset_manager.h` nor `r_asset_manager.c` was included in either
> uploaded batch. If this file genuinely doesn't exist yet, `main.c` won't
> compile or link. If it just wasn't part of this documentation pass, it
> should be included for a complete picture — flagging here so it isn't
> missed.

> **Known issue — `i_init_input()` is not called here.** As noted in the
> `i_input.c` section, nothing currently initializes the input-binding
> system. Given the shape of the rest of this function, the natural place
> to add it would be right alongside `f_init_flag()` and `sm_init(...)`.

> **Known issue — `t_indxvalid` inversion blocks everything downstream in
> practice.** Worth restating here at the very end: because
> `e_load_engine_settings()` and `f_init_flag()` both succeed independently
> of the asset-loading system, `main.c` itself will run and open a window
> even with the `t_indxvalid` bug described in the `t_gindex_tool.c` section
> present. The game will *appear* to start. It's only the moment any code
> path tries to load a stat, item, tile, or inventory gindx — which,
> currently, nothing in either batch actually does yet, since no game state
> beyond the menu placeholder is implemented — that the bug would manifest
> as an immediate crash. This is worth keeping in mind: the bug is silent
> until asset loading is actually exercised, so it's easy to believe the
> engine "works" during early testing right up until the first real gameplay
> code tries to load something.

---

# Appendix — file quick-reference

A flat index if you already understand the architecture and just need to
find where something lives.

| File | Layer | What it's for |
|---|---|---|
| `e_error_handler.h/.c` | Foundation | `ERR_LOG`, `XMALLOC`/`XCALLOC`, crash policy |
| `t_strings.h/.c` | Foundation | `t_strdup`, `t_cpy`, `t_atoi`, `t_snprintf` |
| `t_gindex_tool.h/.c` | Foundation | local-index ↔ global-index mapping (`t_indxvalid` bug lives here) |
| `t_config_tool.h/.c` | Core | shared `.ini` parser (`t_config`, `t_loader`) |
| `l_asset_manager.h/.c` | Core | generic load/get/free-by-gindx pattern |
| `g_statemachine.h/.c` | Core | state stack (push/pop/switch/update/draw) |
| `f_flags.h/.c` | Core | global boolean flags, loaded from config |
| `e_engine_settings.h` | Core | path/string config table (`e_grab_str`) |
| `p_stats.h/.c` | Game data | base stats + derived-stat formulas |
| `p_entity.h/.c` | Game data | position + data-gindx references |
| `i_items.h/.c` | Game data | item definitions, packed stat/amount encoding |
| `i_inventory.h/.c` | Game data | hotbar/backpack slots, bonus-matrix computation |
| `m_map_tile.h/.c` | Game data | tile-type definitions (flags, content indexes) |
| `m_map.h/.c` | Game data | assembled map grid (flexible array member) |
| `i_input.h/.c` | Presentation | keybinding config + pressed/held/released polling |
| `w_window_manager.h/.c` | Presentation | resolution-independent scaling (`wcn*` functions) |
| `s_menu.h/.c` | Presentation | menu game state (only fully-stubbed-out state) |
| `s_combat.h/.c` | Presentation | combat game state (empty stubs) |
| `s_action.h` | Presentation | action game state (`.c` missing from batches) |
| `s_graphics.h` | Presentation | graphics game state (`.c` missing from batches) |
| `main.c` | Entry point | startup sequence + game loop |

## Known issues index

For the full severity-ranked writeup of every bug mentioned inline above,
see the standalone `BUGS.txt`. Quick pointers back into this document:

- `t_indxvalid` inversion — see **`t_gindex_tool.h` / `.c`**, Layer 1
- `t_atoi` / `NULL_ATOI` corruption — see **`t_strings.h` / `.c`**, Layer 1
- `l_load_asset` wrong-argument + always-`true`-return issues — see
  **`l_asset_manager.h` / `.c`**, Layer 2
- `p_init_entity` compile error + raw `calloc` — see **`p_entity.h` / `.c`**,
  Layer 3
- `m_map.c` buffer overflow + unchecked palette index + spurious double-free
  warning — see **`m_map.h` / `.c`**, Layer 3
- `i_input.c` `"action"` mis-binding + never-called `i_init_input` — see
  **`i_input.h` / `.c`**, Layer 3
- `w_window_manager.c` `wcnfx` wrong axis — see **`w_window_manager.h` /
  `.c`**, Layer 3
- `main.c` missing `r_asset_manager` dependency — see **Layer 4**

---

*End of guide.*
