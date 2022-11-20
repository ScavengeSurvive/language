#pragma dynamic 16384

#include "language.inc"

#define RUN_TESTS
#include <YSI_Core\y_testing>
#include <YSI_Coding\y_hooks>


hook OnScriptInit() {
	Logger_ToggleDebug("language", true);
	Logger_Dbg("language", "language debugging activated");
}

Test:InitLanguages() {
	InitLanguages();
}

// Test:AddLanguageEntry() {
// 	new
// 		languageid,
// 		ret;

// 	languageid = InitLanguage("TestLanguage1");
// 	ASSERT(languageid != -1);

// 	ret = AddLanguageEntry(languageid, "L1KEY01", "value");
// 	ASSERT(ret == 0);

// 	new result[MAX_LANGUAGE_ENTRY_LENGTH];
// 	ret = _language_stringFromKey(languageid, "L1KEY01", result);
// 	ASSERT(ret == 0);
// 	ASSERT(!strcmp(result, "value"));
// 	printf("L1KEY01='%s'", result);

// 	ret = _language_stringFromKey(languageid, "L1KEY02", result);
// 	ASSERT(ret == 1);
// }
