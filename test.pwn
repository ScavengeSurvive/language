#include "language.inc"

main() {
	DefineLanguageReplacement("KEY", "VALUE");
	LoadLanguage("", "");
	GetLanguageString(0, "key[]");
}
