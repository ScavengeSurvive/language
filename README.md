# language.inc

Provides a simple API for multi-language systems.

Currently undergoing refactoring after being extracted from the SS core codebase. Not prod-ready.

## Installation

Simply install to your project:

```bash
sampctl package install ScavengeSurvive/language
```

Include in your code and begin using the library:

```pawn
#include <language>
```

## Usage

This package works by loading simple INI files. One file exists for each language in `scriptfiles/languages/` and there is no file extension. So you may have `scriptfiles/languages/English` and `scriptfiles/languages/Espanol` (Pawn has issues with special characters in filenames unfortunately).

These files should have the same keys, see the `scriptfiles/languages` directory in the repo for an example.

### Loading Languages

To load a language from a file, use `InitLanguageFromFile` with just the language name as the parameter, not the full file path. For example `InitLanguageFromFile("English");` loads the `scriptfiles/languages/English` file.

You could also combine this with the fsutil plugin and iterate through the languages directory:

```pawn
new
    Directory:dir = OpenDir("scriptfiles/languages"),
    entry[256],
    ENTRY_TYPE:type;
while(DirNext(dir, type, entry)) {
    if(type == E_REGULAR) {
        InitLanguageFromFile(entry);
    }
}
CloseDir(dir);
```

### Using Language Strings

Now you've loaded languages, you can use strings from each language with the `GetLanguageString(languageid, key[], bool:encode = false)` function. The first parameter is the language ID, which you can obtain via name with `GetLanguageID`.

For example:

```pawn
SendClientMessage(playerid, -1, GetLanguageString(GetLanguageID("English"), "WELCOME"));
```

Would send the string from `English` keyed by `WELCOME`.

### Per-Player Language

You can store a language ID for each player with `SetPlayerLanguage` and retrieve it with `GetPlayerLanguage`. For example, your server could display a list of languages in a dialog with `GetLanguageList` and when the player selects a language, call `SetPlayerLanguage` with their selection and then in future you can use `GetPlayerLanguage` to obtain the language ID that player selected.

There is also a useful macro `@L(playerid, key[])` which you can use to quickly get a string for a player using their assigned language ID:

```pawn
SendClientMessage(playerid, -1, @L(playerid, "WELCOME"));
```

## Testing

To test, simply run the package:

```bash
sampctl package run
```
