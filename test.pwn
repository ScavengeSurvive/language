#define MAX_LANGUAGE 2

#include "language.inc"

#define RUN_TESTS
#include <YSI\y_testing>


TestInit:main() {
	logger_debug("language", true);
}

Test:InitLanguageFromFile() {
	new ret;

	ret = InitLanguageFromFile("English");
	ASSERT(ret == 1);

	ret = InitLanguageFromFile("NonExistent");
	ASSERT(ret == -2);

	ret = InitLanguageFromFile("Empty");
	ASSERT(ret == -3);

	ret = InitLanguageFromFile("Espanol");
	ASSERT(ret == 1);

	ret = InitLanguageFromFile("English");
	ASSERT(ret == -1);
}


// DefineLanguageReplacement("KEY", "VALUE");
// GetLanguageString(0, "key[]");
