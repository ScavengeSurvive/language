// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_language
	#undef _inc_language
#endif
// custom include-guard to ensure we don't duplicate
#if defined _language_included
	#endinput
#endif
#define _language_included

#include <string>
#include <logger>
#include <map>
#include <fsutil>
#include <YSI_Coding\y_hooks>


// the directory language data is stored
#if !defined DIRECTORY_LANGUAGES
	#define DIRECTORY_LANGUAGES "languages/"
#endif

// the default language to fall back to on missing keys
#if !defined DEFAULT_LANGUAGE
	#define DEFAULT_LANGUAGE "English"
#endif

// the total amount of languages
#if !defined MAX_LANGUAGE
	#define MAX_LANGUAGE (12)
#endif

// the total amount of entries per language
#if !defined MAX_LANGUAGE_ENTRIES
	#define MAX_LANGUAGE_ENTRIES (1024)
#endif

// the maximum length of a language key
#if !defined MAX_LANGUAGE_KEY_LEN
	#define MAX_LANGUAGE_KEY_LEN (32)
#endif

// the maximum length of a language text entry
#if !defined MAX_LANGUAGE_ENTRY_LENGTH
	#define MAX_LANGUAGE_ENTRY_LENGTH (768)
#endif

// name limit for a language
#if !defined MAX_LANGUAGE_NAME
	#define MAX_LANGUAGE_NAME (32)
#endif

// maximum amount of string aliases
#if !defined MAX_LANGUAGE_ALIASES
	#define MAX_LANGUAGE_ALIASES (48)
#endif


// lengths for alias key/value
#define MAX_LANGUAGE_ALIAS_KEY_LEN (32)
#define MAX_LANGUAGE_ALIAS_VAL_LEN (32)

// delimiter for language data files
#define DELIMITER_CHAR '='

#define INVALID_LANGUAGE_ID (-1)

// @L is shorthand for getting a language string for a given player
// parameters are: %0:playerid, %1:key
#define @L(%0,%1) GetLanguageString(GetPlayerLanguage(%0), %1)


enum e_LANGUAGE_ENTRY_DATA {
	LANG_KEY[MAX_LANGUAGE_KEY_LEN],
	LANG_VALUE[MAX_LANGUAGE_ENTRY_LENGTH]
}

enum e_LANGUAGE_TAG_ALIAS_DATA {
	lang_alias_key[MAX_LANGUAGE_ALIAS_KEY_LEN],
	lang_alias_val[MAX_LANGUAGE_ALIAS_VAL_LEN]
}


static
	lang_Name[MAX_LANGUAGE][MAX_LANGUAGE_NAME],
	Map:lang_Map[MAX_LANGUAGE],
	lang_Total,

	lang_Replacements[MAX_LANGUAGE_ALIASES][e_LANGUAGE_TAG_ALIAS_DATA],
	lang_TotalReplacements,

	lang_PlayerLanguage[MAX_PLAYERS];


hook OnPlayerConnect(playerid) {
	lang_PlayerLanguage[playerid] = 0;
	return 1;
}

stock DefineLanguageReplacement(key[], val[]) {
	strcat(lang_Replacements[lang_TotalReplacements][lang_alias_key], key, MAX_LANGUAGE_ALIAS_KEY_LEN);
	strcat(lang_Replacements[lang_TotalReplacements][lang_alias_val], val, MAX_LANGUAGE_ALIAS_VAL_LEN);

	lang_TotalReplacements++;
}

stock InitLanguages() {
	new
		Directory:dir = OpenDir("./scriptfiles/"DIRECTORY_LANGUAGES),
		entry[256],
		ENTRY_TYPE:type,
		name[64];

	while(DirNext(dir, type, entry)) {
		if(type == E_REGULAR) {
			PathBase(entry, name);

			Logger_Dbg("language", "loading language file",
				Logger_S("entry", entry),
				Logger_S("name", name));

			new id = InitLanguageFromFile(name);
			if(id == -1) {
				Logger_Err("failed to load language",
					Logger_S("name", name));
			}
		}
	}

	CloseDir(dir);
}

stock InitLanguage(name[]) {
	for(new languageid ; languageid < lang_Total; ++languageid) {
		if(!strcmp(lang_Name[languageid], name)) {
			return languageid;
		}
	}

	if(lang_Total == MAX_LANGUAGE) {
		Logger_Err("lang_Total reached MAX_LANGUAGE",
			Logger_I("max", MAX_LANGUAGE));
		return INVALID_LANGUAGE_ID;
	}

	strcat(lang_Name[lang_Total], name, MAX_LANGUAGE_NAME);

	Logger_Dbg("language", "initialised language",
		Logger_S("name", name),
		Logger_I("id", lang_Total));

	return lang_Total++;
}

stock InitLanguageFromFile(langname[]) {
	new ret = initLanguageFromFile(langname);
	// if we had a failure that wasn't caused by InitLanguage
	// roll back lang_Total so a slot isn't used up.
	if(ret < -1) {
		lang_Total--;
	}

	return ret;
}

stock AddLanguageEntry(languageid, key[], value[]) {
	if(!(0 <= languageid < lang_Total)) {
		Logger_Err("attempt to add entry to invalid language",
			Logger_I("languageid", languageid),
			Logger_S("key", key),
			Logger_S("value", value));
		return 1;
	}

	if(isnull(key)) {
		Logger_Err("null key specified");
		return 2;
	}

	if(isnull(value)) {
		Logger_Err("null value specified",
			Logger_S("key", key));
		return 3;
	}

	if(MAP_count(lang_Map[languageid]) >= MAX_LANGUAGE_ENTRIES) {
		Logger_Err("MAX_LANGUAGE_ENTRIES limit reached",
			Logger_I("languageid", languageid),
			Logger_S("key", key),
			Logger_S("value", value));
		return 4;
	}

	new replaced[MAX_LANGUAGE_ENTRY_LENGTH];
	_language_doReplace(value, replaced);
	MAP_insert_str_str(lang_Map[languageid], key, replaced);

	Logger_Dbg("language", "Added language key",
		Logger_I("languageid", languageid),
		Logger_S("key", key),
		Logger_S("value", value));

	return 0;
}

stock GetLanguageString(languageid, const key[], bool:encode = false) {
	new
		result[MAX_LANGUAGE_ENTRY_LENGTH],
		ret;

	if(!(0 <= languageid < lang_Total)) {
		Logger_Err("invalid language",
			Logger_I("languageid", languageid));
		return result;
	}

	ret = _language_stringFromKey(languageid, key, result, sizeof(result), encode);

	switch(ret) {
		case 1: {
			Logger_Err("invalid key",
				Logger_S("key", key));
		}
		case 2: {
			Logger_Err("key not found",
				Logger_S("key", key),
				Logger_I("language", languageid));

			// return english if key not found
			if(languageid != 0) {
				strcat(result, GetLanguageString(0, key, encode), MAX_LANGUAGE_ENTRY_LENGTH);
			}
		}
	}

	return result;
}

stock GetLanguageList(list[][]) {
	for(new i; i < lang_Total; i++) {
		list[i][0] = EOS;
		strcat(list[i], lang_Name[i], MAX_LANGUAGE_NAME);
	}

	return lang_Total;
}

stock GetLanguageName(languageid, name[]) {
	if(!(0 <= languageid < lang_Total)) {
		return 1;
	}

	name[0] = EOS;
	strcat(name, lang_Name[languageid], MAX_LANGUAGE_NAME);

	return 0;
}

stock GetLanguageID(name[]) {
	for(new i; i < lang_Total; i++) {
		if(!strcmp(name, lang_Name[i])) {
			return i;
		}
	}
	return -1;
}

stock GetPlayerLanguage(playerid) {
	if(!IsPlayerConnected(playerid)) {
		return -1;
	}

	return lang_PlayerLanguage[playerid];
}

stock SetPlayerLanguage(playerid, language) {
	if(!IsPlayerConnected(playerid)) {
		return 1;
	}

	lang_PlayerLanguage[playerid] = language;

	return 0;
}


// -
// Internals
// -


stock _language_stringFromKey(languageid, const key[], result[], len = sizeof(result), bool:encode = false) {
	if(!MAP_contains_str(lang_Map[languageid], key)) {
		return 1;
	}

	MAP_get_str_arr(lang_Map[languageid], key, result, len);

	if(encode) {
		ConvertEncoding(result);
	}

	return 0;
}

_language_doReplace(input[], output[]) {
	new
		bool:in_tag = false,
		tag_start = -1,
		output_idx;

	for(new i = 0; input[i] != EOS; ++i) {
		if(in_tag) {
			if(input[i] == '}') {
				for(new j; j < lang_TotalReplacements; ++j) {
					if(!strcmp(input[tag_start], lang_Replacements[j][lang_alias_key], false, i - tag_start)) {
						for(new k; lang_Replacements[j][lang_alias_val][k] != 0 && output_idx < MAX_LANGUAGE_ENTRY_LENGTH; ++k)
							output[output_idx++] = lang_Replacements[j][lang_alias_val][k];

						break;
					}
				}

				in_tag = false;
				continue;
			}
		} else {
			if(input[i] == '{') {
				tag_start = i + 1;
				in_tag = true;
				continue;
			} else if(input[i] == '\\') {
				if(input[i + 1] == 'n') {
					output[output_idx++] = '\n';
					i += 1;
				} else if(input[i + 1] == 't') {
					output[output_idx++] = '\t';
					i += 1;
				}
			} else {
				output[output_idx++] = input[i];
			}
		}
	}
}

_language_quickSort(array[][], left, right) {
	new
		tempLeft = left,
		tempRight = right,
		pivot = array[(left + right) / 2][0];

	while(tempLeft <= tempRight) {
		while(array[tempLeft][0] < pivot)
			tempLeft++;

		while(array[tempRight][0] > pivot)
			tempRight--;
		
		if(tempLeft <= tempRight) {
			_language_swapStrings(array[tempLeft][LANG_KEY], array[tempRight][LANG_KEY]);
			_language_swapStrings(array[tempLeft][LANG_VALUE], array[tempRight][LANG_VALUE]);

			tempLeft++;
			tempRight--;
		}
	}

	if(left < tempRight)
		_language_quickSort(array, left, tempRight);

	if(tempLeft < right)
		_language_quickSort(array, tempLeft, right);
}

_language_swapStrings(str1[], str2[]) {
	new tmp;

	for(new i; str1[i] != '\0' || str2[i] != '\0'; i++) {
		tmp = str1[i];
		str1[i] = str2[i];
		str2[i] = tmp;
	}
}

/*
	Credit for this function goes to Y_Less:
	http://forum.sa-mp.com/showpost.php?p=3015480&postcount=6
*/
stock ConvertEncoding(string[]) {
	static const
		real[256] = {
			  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
			 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
			 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
			 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
			 64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
			 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
			 96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
			112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
			128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
			144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
			160,  94, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
			124, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 175,
			128, 129, 130, 195, 131, 197, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141,
			208, 173, 142, 143, 144, 213, 145, 215, 216, 146, 147, 148, 149, 221, 222, 150,
			151, 152, 153, 227, 154, 229, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
			240, 174, 165, 166, 167, 245, 168, 247, 248, 169, 170, 171, 172, 253, 254, 255
		};

	for(new i = 0, len = strlen(string), ch; i != len; ++i) {
		// Check if this character is in our reduced range.
		if(0 <= (ch = string[i]) < 256) {
			string[i] = real[ch];
		}
	}
}

/*
	Not sure where this code came from... random pastebin link!

stock ConvertEncoding(string[]) {
	for(new i = 0, len = strlen(string); i != len; ++i) {
		switch(string[i]) {
			case 0xC0 .. 0xC3: string[i] -= 0x40;
			case 0xC7 .. 0xC9: string[i] -= 0x42;
			case 0xD2 .. 0xD5: string[i] -= 0x44;
			case 0xD9 .. 0xDC: string[i] -= 0x47;
			case 0xE0 .. 0xE3: string[i] -= 0x49;
			case 0xE7 .. 0xEF: string[i] -= 0x4B;
			case 0xF2 .. 0xF5: string[i] -= 0x4D;
			case 0xF9 .. 0xFC: string[i] -= 0x50;
			case 0xC4, 0xE4: string[i] = 0x83;
			case 0xC6, 0xE6: string[i] = 0x84;
			case 0xD6, 0xF6: string[i] = 0x91;
			case 0xD1, 0xF1: string[i] = 0xEC;
			case 0xDF: string[i] = 0x96;
			case 0xBF: string[i] = 0xAF;
		}
	}
}
*/


stock initLanguageFromFile(langname[]) {
	new languageid = InitLanguage(langname);
	if(languageid == INVALID_LANGUAGE_ID) {
		return -1;
	}

	new
		filename[256],
		File:f,
		line[MAX_LANGUAGE_KEY_LEN + 1 + MAX_LANGUAGE_ENTRY_LENGTH],
		linenumber = 1,
		bool:skip,
		length,
		delimiter,
		index;

	format(filename, sizeof(filename), DIRECTORY_LANGUAGES"%s", langname);
	f = fopen(filename, io_read);

	if(!f) {
		Logger_Err("unable to open file",
			Logger_S("filename", filename));
		return -2;
	}

	new
		key[MAX_LANGUAGE_KEY_LEN],
		val[MAX_LANGUAGE_ENTRY_LENGTH];

	while(fread(f, line)) {
		length = strlen(line);

		if(length < 4) {
			continue;
		}

		delimiter = 0;

		// search for the delimiter
		while(line[delimiter] != DELIMITER_CHAR) {
			if(!(32 <= line[delimiter] < 127)) {
				Logger_Err("language key contains non-alphabetic character",
					Logger_I("line", linenumber),
					Logger_S("file", filename),
					Logger_I("character", line[delimiter]));
				skip = true;
				break;
			}

			if(delimiter >= MAX_LANGUAGE_KEY_LEN) {
				Logger_Err("language key length too long",
					Logger_I("line", linenumber),
					Logger_S("file", filename),
					Logger_I("delimiter", delimiter));
				skip = true;
				break;
			}

			key[delimiter] = line[delimiter];
			delimiter++;
		}

		if(skip) {
			skip = false;
			continue;
		}

		if(delimiter < 4) {
			Logger_Err("language key length too short",
				Logger_I("line", linenumber),
				Logger_S("file", filename),
				Logger_I("delimiter", delimiter));
			continue;
		}

		if(delimiter >= length - 1) {
			Logger_Err("line contains no delimiter",
				Logger_I("line", linenumber),
				Logger_S("file", filename),
				Logger_I("delimiter", delimiter));
			continue;
		}

		key[delimiter] = EOS;

		strmid(key, line, 0, delimiter, MAX_LANGUAGE_ENTRY_LENGTH);
		strmid(val, line, delimiter + 1, length - 1, MAX_LANGUAGE_ENTRY_LENGTH);

		AddLanguageEntry(languageid, key, val);

		linenumber++;
	}

	fclose(f);

	return index + 1;
}
