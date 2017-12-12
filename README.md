# language.inc

Provides a simple API for multi-language systems.

Currently undergoing refactoring after being extracted from the SS core codebase. Not prod-ready.

## Usage

Simply add to your `pawn.json` and include:

```json
{
    "dependencies": ["ScavengeSurvive/language"]
}
```

```pawn
#include <language>
```

## Testing

To test, simply run the package:

```bash
sampctl package run
```
