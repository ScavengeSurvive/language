#define MAX_LANGUAGE 2

#include "language.inc"

#define RUN_TESTS
#include <YSI\y_testing>


TestInit:main() {
	logger_debug("language", true);
}

Test:LoadLanguage() {
	new ret;

	ret = LoadLanguage("English");
	ASSERT(ret == 1);

	ret = LoadLanguage("NonExistent");
	ASSERT(ret == -2);

	ret = LoadLanguage("Empty");
	ASSERT(ret == -3);

	ret = LoadLanguage("Espanol");
	ASSERT(ret == 1);

	ret = LoadLanguage("English");
	ASSERT(ret == -1);
}


// DefineLanguageReplacement("KEY", "VALUE");
// GetLanguageString(0, "key[]");
