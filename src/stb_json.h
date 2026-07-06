#ifndef STB_JSON_H
#define STB_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <limits.h>
#include <stdint.h>

// Add thread safety attribute for error tracking
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#define STB_JSON_THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER)
#define STB_JSON_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define STB_JSON_THREAD_LOCAL __thread
#else
#define STB_JSON_THREAD_LOCAL
#endif

/**
 * @mainpage STB JSON Library
 * @author Ferki (03.08.2025)
 * @version 1.7.19
 * 
 * Lightweight single-file JSON parser/generator for C/C++
 * 
 * Features:
 * - Single header file implementation
 * - No external dependencies
 * - Thread-safe error tracking
 * - File parsing utilities
 * - Full JSON standard support
 * - Memory management hooks
 * - JSON Pointer (RFC 6901) and Patch (RFC 6902) utilities
 * - Merge Patch (RFC 7386) support
 * - Efficient parsing and serialization
 * 
 * Basic Usage Example:
 * 
 *     #define STB_JSON_IMPLEMENTATION
 *     #include "stb_json.h"
 *     
 *     int main() {
 *         const char* json = "{\"name\":\"John\",\"age\":30,\"cars\":[\"Ford\",\"BMW\"]}";
 *         stb_json* root = stb_json_parse(json);
 *         if (root) {
 *             stb_json* name = stb_json_getobjectitem(root, "name");
 *             printf("Name: %s\n", name->valuestring);
 *             stb_json_delete(root);
 *         }
 *         return 0;
 *     }
 */

typedef struct stb_json stb_json;

#define STB_JSON_VERSION_MAJOR 1
#define STB_JSON_VERSION_MINOR 7
#define STB_JSON_VERSION_PATCH 19

#define STB_JSON_INVALID (0)
#define STB_JSON_FALSE  (1 << 0)
#define STB_JSON_TRUE   (1 << 1)
#define STB_JSON_NULL   (1 << 2)
#define STB_JSON_NUMBER (1 << 3)
#define STB_JSON_STRING (1 << 4)
#define STB_JSON_ARRAY  (1 << 5)
#define STB_JSON_OBJECT (1 << 6)
#define STB_JSON_RAW    (1 << 7)

#define STB_JSON_IS_REFERENCE 256
#define STB_JSON_STRING_IS_CONST 512

#ifndef STB_JSON_NESTING_LIMIT
#define STB_JSON_NESTING_LIMIT 1000
#endif

#ifndef STB_JSON_CIRCULAR_LIMIT
#define STB_JSON_CIRCULAR_LIMIT 10000
#endif

/**
 * @brief JSON value structure
 * 
 * Represents a JSON value with type and data
 */
struct stb_json {
    struct stb_json *next;   /**< Next sibling item */
    struct stb_json *prev;   /**< Previous sibling item */
    struct stb_json *child;  /**< Child item (for arrays/objects) */
    int type;               /**< Type flags (STB_JSON_*) */
    char *valuestring;      /**< String value */
    int valueint;           /**< Integer value */
    double valuedouble;     /**< Double value */
    char *string;           /**< Key name (for objects) */
};

typedef int stb_json_bool;

/**
 * @brief Memory management hooks
 */
typedef struct stb_json_hooks {
    void *(*malloc_fn)(size_t sz);  /**< Custom allocator */
    void (*free_fn)(void *ptr);      /**< Custom deallocator */
} stb_json_hooks;

/* Public API functions */
const char* stb_json_version(void);
void stb_json_inithooks(stb_json_hooks* hooks);
stb_json* stb_json_parse(const char *value);
stb_json* stb_json_parse_file(const char* filename);  // Added file parsing
stb_json* stb_json_parse_withlength(const char *value, size_t buffer_length);
stb_json* stb_json_parse_withopts(const char *value, const char **return_parse_end, stb_json_bool require_null_terminated);
stb_json* stb_json_parse_withlengthopts(const char *value, size_t buffer_length, const char **return_parse_end, stb_json_bool require_null_terminated);
char *stb_json_print(const stb_json *item);
char *stb_json_print_unformatted(const stb_json *item);
char *stb_json_printbuffered(const stb_json *item, int prebuffer, stb_json_bool fmt);
stb_json_bool stb_json_printpreallocated(stb_json *item, char *buffer, const int length, const stb_json_bool format);
void stb_json_delete(stb_json *item);
int stb_json_getarraysize(const stb_json *array);
stb_json *stb_json_getarrayitem(const stb_json *array, int index);
stb_json *stb_json_getobjectitem(const stb_json *object, const char *string);
stb_json *stb_json_getobjectitemcasesensitive(const stb_json *object, const char *string);
stb_json_bool stb_json_hasobjectitem(const stb_json *object, const char *string);
const char *stb_json_geterrorptr(void);
char *stb_json_getstringvalue(const stb_json *item);
double stb_json_getnumbervalue(const stb_json *item);
int stb_json_getnumberint(const stb_json *item);
double stb_json_getnumberdouble(const stb_json *item);
stb_json_bool stb_json_isinvalid(const stb_json *item);
stb_json_bool stb_json_isfalse(const stb_json *item);
stb_json_bool stb_json_istrue(const stb_json *item);
stb_json_bool stb_json_isbool(const stb_json *item);
stb_json_bool stb_json_isnull(const stb_json *item);
stb_json_bool stb_json_isnumber(const stb_json *item);
stb_json_bool stb_json_isstring(const stb_json *item);
stb_json_bool stb_json_isarray(const stb_json *item);
stb_json_bool stb_json_isobject(const stb_json *item);
stb_json_bool stb_json_israw(const stb_json *item);
stb_json *stb_json_createnull(void);
stb_json *stb_json_createtrue(void);
stb_json *stb_json_createfalse(void);
stb_json *stb_json_createbool(stb_json_bool boolean);
stb_json *stb_json_createnumber(double num);
stb_json *stb_json_createstring(const char *string);
stb_json *stb_json_createraw(const char *raw);
stb_json *stb_json_createarray(void);
stb_json *stb_json_createobject(void);
stb_json *stb_json_createstringReference(const char *string);
stb_json *stb_json_createobjectReference(const stb_json *child);
stb_json *stb_json_createarrayReference(const stb_json *child);
stb_json *stb_json_createintarray(const int *numbers, int count);
stb_json *stb_json_createfloatarray(const float *numbers, int count);
stb_json *stb_json_createdoublearray(const double *numbers, int count);
stb_json *stb_json_createstringarray(const char *const *strings, int count);
stb_json_bool stb_json_additemtoarray(stb_json *array, stb_json *item);
stb_json_bool stb_json_additemtoobject(stb_json *object, const char *string, stb_json *item);
stb_json_bool stb_json_additemtoobjectcs(stb_json *object, const char *string, stb_json *item);
stb_json_bool stb_json_additemreferencetoarray(stb_json *array, stb_json *item);
stb_json_bool stb_json_additemreferencetoobject(stb_json *object, const char *string, stb_json *item);
stb_json *stb_json_detachitemviapointer(stb_json *parent, stb_json *item);
stb_json *stb_json_detachitemfromarray(stb_json *array, int which);
void stb_json_deleteitemfromarray(stb_json *array, int which);
stb_json *stb_json_detachitemfromobject(stb_json *object, const char *string);
stb_json *stb_json_detachitemfromobjectcasesensitive(stb_json *object, const char *string);
void stb_json_deleteitemfromobject(stb_json *object, const char *string);
void stb_json_deleteitemfromobjectcasesensitive(stb_json *object, const char *string);
stb_json_bool stb_json_insertiteminarray(stb_json *array, int which, stb_json *newitem);
stb_json_bool stb_json_replaceitemviapointer(stb_json *parent, stb_json *item, stb_json *replacement);
stb_json_bool stb_json_replaceiteminarray(stb_json *array, int which, stb_json *newitem);
stb_json_bool stb_json_replaceiteminobject(stb_json *object,const char *string,stb_json *newitem);
stb_json_bool stb_json_replaceiteminobjectcasesensitive(stb_json *object,const char *string,stb_json *newitem);
stb_json *stb_json_duplicate(const stb_json *item, stb_json_bool recurse);
stb_json_bool stb_json_compare(const stb_json *a, const stb_json *b, const stb_json_bool case_sensitive);
void stb_json_minify(char *json);
stb_json* stb_json_addnulltoobject(stb_json *object, const char *name);
stb_json* stb_json_addtruetoobject(stb_json *object, const char *name);
stb_json* stb_json_addfalsetoobject(stb_json *object, const char *name);
stb_json* stb_json_addbooltoobject(stb_json *object, const char *name, const stb_json_bool boolean);
stb_json* stb_json_addnumbertoobject(stb_json *object, const char *name, const double number);
stb_json* stb_json_addstringtoobject(stb_json *object, const char *name, const char *string);
stb_json* stb_json_addrawtoobject(stb_json *object, const char *name, const char *raw);
stb_json* stb_json_addobjecttoobject(stb_json *object, const char *name);
stb_json* stb_json_addarraytoobject(stb_json *object, const char *name);
#define stb_json_setintvalue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
double stb_json_setnumberhelper(stb_json *object, double number);
#define stb_json_setnumbervalue(object, number) ((object != NULL) ? stb_json_setnumberhelper(object, (double)number) : (number))
char* stb_json_setvaluestring(stb_json *object, const char *valuestring);
#define stb_json_setboolvalue(object, boolValue) ( \
    (object != NULL && ((object)->type & (STB_JSON_FALSE|STB_JSON_TRUE))) ? \
    (object)->type=((object)->type &(~(STB_JSON_FALSE|STB_JSON_TRUE)))|((boolValue)?STB_JSON_TRUE:STB_JSON_FALSE) : \
    STB_JSON_INVALID\
)
#define stb_json_arrayforeach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)
void *stb_json_malloc(size_t size);
void stb_json_free(void *object);

/* JSON Pointer and Patch utilities */
stb_json *stb_json_utils_getpointer(stb_json *object, const char *pointer);
stb_json *stb_json_utils_getpointercasesensitive(stb_json *object, const char *pointer);
stb_json *stb_json_utils_generatepatches(stb_json *from, stb_json *to);
stb_json *stb_json_utils_generatepatchescasesensitive(stb_json *from, stb_json *to);
void stb_json_utils_addpatchtoarray(stb_json *array, const char *operation, const char *path, const stb_json *value);
int stb_json_utils_applypatches(stb_json *object, const stb_json *patches);
int stb_json_utils_applypatchescasesensitive(stb_json *object, const stb_json *patches);
stb_json *stb_json_utils_mergepatch(stb_json *target, const stb_json *patch);
stb_json *stb_json_utils_mergepatchcasesensitive(stb_json *target, const stb_json *patch);
stb_json *stb_json_utils_generatemergepatch(stb_json *from, stb_json *to);
stb_json *stb_json_utils_generatemergepatchcasesensitive(stb_json *from, stb_json *to);
char *stb_json_utils_findpointerfromobjectto(const stb_json *object, const stb_json *target);
void stb_json_utils_sortobject(stb_json *object);
void stb_json_utils_sortobjectcasesensitive(stb_json *object);

/* File utilities */
char* stb_json_read_file(const char* filename);
int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);
int stb_json_main(int argc, char** argv);

#ifdef STB_JSON_IMPLEMENTATION

typedef struct {
    const unsigned char *json;
    size_t position;
} stb_json_error;

// Make error tracking thread-safe
static STB_JSON_THREAD_LOCAL stb_json_error global_error = { NULL, 0 };

/**
 * @brief Get the position of the last parse error
 * @return const char* Pointer to error location
 */
const char *stb_json_geterrorptr(void) {
    if (!global_error.json) return NULL;
    return (const char*)(global_error.json + global_error.position);
}

/**
 * @brief Parse JSON from file
 * @param filename Path to JSON file
 * @return stb_json* Parsed object or NULL on error
 * 
 * Example:
 *     stb_json* root = stb_json_parse_file("data.json");
 *     if (root) {
 *         // Use JSON data
 *         stb_json_delete(root);
 *     }
 */
stb_json* stb_json_parse_file(const char* filename) {
    char* content = stb_json_read_file(filename);
    if (!content) return NULL;
    
    stb_json* result = stb_json_parse(content);
    stb_json_free(content);
    return result;
}

/**
 * @brief Get string value from JSON item
 * @param item JSON item (must be string type)
 * @return char* String value or NULL if invalid
 */
char *stb_json_getstringvalue(const stb_json *item) {
    if (!stb_json_isstring(item)) return NULL;
    return item->valuestring;
}

/**
 * @brief Get number value from JSON item as double
 * @param item JSON item (must be number type)
 * @return double Number value or NAN if invalid
 */
double stb_json_getnumbervalue(const stb_json *item) {
    if (!stb_json_isnumber(item)) return (double)NAN;
    return item->valuedouble;
}

/**
 * @brief Get number value from JSON item as int
 * @param item JSON item (must be number type)
 * @return int Integer value or 0 if invalid
 */
int stb_json_getnumberint(const stb_json *item) {
    if (!stb_json_isnumber(item)) return 0;
    return item->valueint;
}

/**
 * @brief Get number value from JSON item as double
 * @param item JSON item (must be number type)
 * @return double Double value or NAN if invalid
 */
double stb_json_getnumberdouble(const stb_json *item) {
    if (!stb_json_isnumber(item)) return (double)NAN;
    return item->valuedouble;
}

/**
 * @brief Get library version string
 * @return const char* Version string
 */
const char* stb_json_version(void) {
    static char version[15];
    snprintf(version, sizeof(version), "%i.%i.%i", 
             STB_JSON_VERSION_MAJOR, 
             STB_JSON_VERSION_MINOR, 
             STB_JSON_VERSION_PATCH);
    return version;
}

/* UTF-8 and Unicode support functions */
static int case_insensitive_strcmp(const unsigned char *s1, const unsigned char *s2) {
    if (!s1 || !s2) return 1;
    if (s1 == s2) return 0;
    for(; tolower(*s1) == tolower(*s2); s1++, s2++) {
        if (!*s1) return 0;
    }
    return tolower(*s1) - tolower(*s2);
}

static unsigned parse_hex4(const unsigned char *input) {
    unsigned int h = 0;
    size_t i;
    for (i = 0; i < 4; i++) {
        if (input[i] >= '0' && input[i] <= '9') h += (unsigned)input[i] - '0';
        else if (input[i] >= 'A' && input[i] <= 'F') h += (unsigned)10 + input[i] - 'A';
        else if (input[i] >= 'a' && input[i] <= 'f') h += (unsigned)10 + input[i] - 'a';
        else return 0;
        if (i < 3) h = h << 4;
    }
    return h;
}

/**
 * @brief Convert UTF-16 to UTF-8 with full surrogate pairs support
 * @param input UTF-16 input string
 * @param input_end End of input buffer
 * @param output Output buffer for UTF-8
 * @return Number of bytes consumed from input
 */
static unsigned char utf16_to_utf8(const unsigned char *input, const unsigned char *input_end, unsigned char **output) {
    if (input_end - input < 6) return 0;
    
    unsigned first_code = parse_hex4(input + 2);
    if (first_code == 0) return 0;
    
    // Surrogate pairs handling
    if (first_code >= 0xD800 && first_code <= 0xDBFF) {
        if (input_end - input < 12) return 0;
        if (input[6] != '\\' || input[7] != 'u') return 0;
        
        unsigned second_code = parse_hex4(input + 8);
        if (second_code < 0xDC00 || second_code > 0xDFFF) return 0;
        
        long codepoint = 0x10000 + (((first_code & 0x3FF) << 10) | (second_code & 0x3FF);
        
        // Encode as 4-byte UTF-8
        **output = (unsigned char)(0xF0 | (codepoint >> 18));
        *(*output + 1) = (unsigned char)(0x80 | ((codepoint >> 12) & 0x3F));
        *(*output + 2) = (unsigned char)(0x80 | ((codepoint >> 6) & 0x3F));
        *(*output + 3) = (unsigned char)(0x80 | (codepoint & 0x3F));
        *output += 4;
        return 12;
    } 
    else {
        // Direct encoding for BMP
        if (first_code < 0x80) {
            **output = (unsigned char)first_code;
            (*output)++;
        } 
        else if (first_code < 0x800) {
            **output = (unsigned char)(0xC0 | (first_code >> 6));
            *(*output + 1) = (unsigned char)(0x80 | (first_code & 0x3F));
            *output += 2;
        } 
        else {
            **output = (unsigned char)(0xE0 | (first_code >> 12));
            *(*output + 1) = (unsigned char)(0x80 | ((first_code >> 6) & 0x3F));
            *(*output + 2) = (unsigned char)(0x80 | (first_code & 0x3F));
            *output += 3;
        }
        return 6;
    }
}

/**
 * @brief Validate UTF-8 sequence
 * @param s UTF-8 string to validate
 * @return stb_json_bool True if valid UTF-8
 */
static stb_json_bool is_valid_utf8(const unsigned char *s) {
    while (*s) {
        if (*s < 0x80) {
            // 0xxxxxxx
            s++;
        } else if ((*s & 0xE0) == 0xC0) {
            // 110xxxxx 10xxxxxx
            if ((s[1] & 0xC0) != 0x80) return false;
            s += 2;
        } else if ((*s & 0xF0) == 0xE0) {
            // 1110xxxx 10xxxxxx 10xxxxxx
            if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) return false;
            s += 3;
        } else if ((*s & 0xF8) == 0xF0) {
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) return false;
            s += 4;
        } else {
            return false;
        }
    }
    return true;
}

typedef struct internal_hooks {
    void *(*allocate)(size_t size);
    void (*deallocate)(void *pointer);
    void *(*reallocate)(void *pointer, size_t size);
} internal_hooks;

static void *internal_malloc(size_t size) { return malloc(size); }
static void internal_free(void *pointer) { free(pointer); }
static void *internal_realloc(void *pointer, size_t size) { return realloc(pointer, size); }

static internal_hooks global_hooks = { internal_malloc, internal_free, internal_realloc };

static unsigned char* stb_json_strdup(const unsigned char* string, const internal_hooks *hooks) {
    if (!string) return NULL;
    size_t length = strlen((const char*)string) + 1;
    unsigned char *copy = hooks->allocate(length);
    if (copy) memcpy(copy, string, length);
    return copy;
}

/**
 * @brief Initialize memory management hooks
 * @param hooks Custom allocator/deallocator functions
 */
void stb_json_inithooks(stb_json_hooks* hooks) {
    if (!hooks) {
        global_hooks.allocate = malloc;
        global_hooks.deallocate = free;
        global_hooks.reallocate = realloc;
        return;
    }

    global_hooks.allocate = hooks->malloc_fn ? hooks->malloc_fn : malloc;
    global_hooks.deallocate = hooks->free_fn ? hooks->free_fn : free;
    global_hooks.reallocate = (global_hooks.allocate == malloc && global_hooks.deallocate == free) ? realloc : NULL;
}

static stb_json *stb_json_new_item(const internal_hooks *hooks) {
    stb_json* node = hooks->allocate(sizeof(stb_json));
    if (node) memset(node, 0, sizeof(stb_json));
    return node;
}

/**
 * @brief Delete JSON item and all children
 * @param item Item to delete
 */
void stb_json_delete(stb_json *item) {
    if (!item) return;
    
    stb_json *next;
    while (item) {
        next = item->next;
        if (!(item->type & STB_JSON_IS_REFERENCE) && item->child) {
            stb_json_delete(item->child);
        }
        if (!(item->type & STB_JSON_IS_REFERENCE) && item->valuestring) {
            global_hooks.deallocate(item->valuestring);
        }
        if (!(item->type & STB_JSON_STRING_IS_CONST) && item->string) {
            global_hooks.deallocate(item->string);
        }
        global_hooks.deallocate(item);
        item = next;
    }
}

static unsigned char get_decimal_point(void) {
    return '.';
}

typedef struct parse_buffer {
    const unsigned char *content;
    size_t length;
    size_t offset;
    size_t depth;
    internal_hooks hooks;
} parse_buffer;

#define can_read(buffer, size) ((buffer) && ((buffer)->offset + size) <= (buffer)->length)
#define can_access_at_index(buffer, index) ((buffer) && ((buffer)->offset + index) < (buffer)->length)
#define cannot_access_at_index(buffer, index) (!can_access_at_index(buffer, index))
#define buffer_at_offset(buffer) ((buffer)->content + (buffer)->offset)

/**
 * @brief Platform-independent number parser
 */
static double stb_json_strtod(const char *nptr, char **endptr) {
    double sign = 1.0, value = 0.0, fraction = 0.0;
    int exponent = 0, exp_sign = 1;
    double frac_divisor = 1.0;
    const char *p = nptr;

    // Handle sign
    if (*p == '-') {
        sign = -1.0;
        p++;
    } else if (*p == '+') {
        p++;
    }

    // Integer part
    while (*p >= '0' && *p <= '9') {
        value = value * 10.0 + (*p - '0');
        p++;
    }

    // Fraction part
    if (*p == '.') {
        p++;
        while (*p >= '0' && *p <= '9') {
            fraction = fraction * 10.0 + (*p - '0');
            frac_divisor *= 10.0;
            p++;
        }
    }

    // Exponent
    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '-') {
            exp_sign = -1;
            p++;
        } else if (*p == '+') {
            p++;
        }
        while (*p >= '0' && *p <= '9') {
            exponent = exponent * 10 + (*p - '0');
            p++;
        }
    }

    if (endptr) *endptr = (char*)p;

    value += fraction / frac_divisor;
    if (exponent) {
        value *= pow(10.0, exp_sign * exponent);
    }
    return sign * value;
}

static stb_json_bool parse_number(stb_json *item, parse_buffer *input_buffer) {
    if (!input_buffer || !input_buffer->content) return false;

    double number = 0;
    unsigned char *after_end = NULL;
    unsigned char *number_c_string = NULL;
    unsigned char decimal_point = get_decimal_point();
    size_t i = 0, number_string_length = 0;
    stb_json_bool has_decimal = false;

    for (i = 0; can_access_at_index(input_buffer, i); i++) {
        unsigned char c = buffer_at_offset(input_buffer)[i];
        if ((c >= '0' && c <= '9') || c == '+' || c == '-' || c == 'e' || c == 'E') {
            number_string_length++;
        } else if (c == '.') {
            has_decimal = true;
            number_string_length++;
        } else {
            break;
        }
    }

    if (!number_string_length) return false;
    number_c_string = input_buffer->hooks.allocate(number_string_length + 1);
    if (!number_c_string) return false;

    memcpy(number_c_string, buffer_at_offset(input_buffer), number_string_length);
    number_c_string[number_string_length] = '\0';

    if (has_decimal) {
        for (i = 0; i < number_string_length; i++) {
            if (number_c_string[i] == '.') {
                number_c_string[i] = decimal_point;
            }
        }
    }

    number = stb_json_strtod((const char*)number_c_string, (char**)&after_end);
    if (number_c_string == after_end) {
        input_buffer->hooks.deallocate(number_c_string);
        return false;
    }

    item->valuedouble = number;
    if (number >= INT_MAX) item->valueint = INT_MAX;
    else if (number <= (double)INT_MIN) item->valueint = INT_MIN;
    else item->valueint = (int)number;

    item->type = STB_JSON_NUMBER;
    input_buffer->offset += (size_t)(after_end - number_c_string);
    input_buffer->hooks.deallocate(number_c_string);
    return true;
}

double stb_json_setnumberhelper(stb_json *object, double number) {
    if (!object) return number;
    if (number >= INT_MAX) object->valueint = INT_MAX;
    else if (number <= (double)INT_MIN) object->valueint = INT_MIN;
    else object->valueint = (int)number;
    return object->valuedouble = number;
}

char* stb_json_setvaluestring(stb_json *object, const char *valuestring) {
    if (!object || !(object->type & STB_JSON_STRING) || (object->type & STB_JSON_IS_REFERENCE) || 
        !object->valuestring || !valuestring) return NULL;

    size_t v1_len = strlen(valuestring);
    size_t v2_len = strlen(object->valuestring);
    
    if (v1_len <= v2_len) {
        if (valuestring + v1_len < object->valuestring || object->valuestring + v2_len < valuestring) {
            return NULL;
        }
        strcpy(object->valuestring, valuestring);
        return object->valuestring;
    }
    
    char *copy = (char*)stb_json_strdup((const unsigned char*)valuestring, &global_hooks);
    if (!copy) return NULL;
    stb_json_free(object->valuestring);
    object->valuestring = copy;
    return copy;
}

typedef struct printbuffer {
    unsigned char *buffer;
    size_t length;
    size_t offset;
    size_t depth;
    stb_json_bool noalloc;
    stb_json_bool format;
    internal_hooks hooks;
} printbuffer;

static unsigned char* ensure(printbuffer *p, size_t needed) {
    if (!p || !p->buffer || p->offset >= p->length) return NULL;
    if (needed > INT_MAX) return NULL;
    
    needed += p->offset + 1;
    if (needed <= p->length) return p->buffer + p->offset;
    if (p->noalloc) return NULL;

    size_t newsize = needed > (INT_MAX / 2) ? INT_MAX : needed * 2;
    unsigned char *newbuffer;
    newbuffer = p->hooks.reallocate(p->buffer, newsize);
    
    if (!newbuffer) {
        p->hooks.deallocate(p->buffer);
        p->length = 0;
        p->buffer = NULL;
        return NULL;
    }

    p->length = newsize;
    p->buffer = newbuffer;
    return newbuffer + p->offset;
}

static void update_offset(printbuffer *buffer) {
    if (buffer && buffer->buffer) {
        buffer->offset += strlen((const char*)buffer->buffer + buffer->offset);
    }
}

static stb_json_bool compare_double(double a, double b) {
    double maxVal = fabs(a) > fabs(b) ? fabs(a) : fabs(b);
    return (fabs(a - b) <= maxVal * DBL_EPSILON);
}

static stb_json_bool print_number(const stb_json *item, printbuffer *output_buffer) {
    if (!output_buffer) return false;
    
    double d = item->valuedouble;
    unsigned char number_buffer[26];
    unsigned char decimal_point = get_decimal_point();
    int length = 0;
    size_t i;
    
    if (isnan(d) || isinf(d)) {
        length = sprintf((char*)number_buffer, "null");
    } else if (d == (double)item->valueint) {
        length = sprintf((char*)number_buffer, "%d", item->valueint);
    } else {
        length = sprintf((char*)number_buffer, "%1.15g", d);
        double test;
        if (sscanf((char*)number_buffer, "%lg", &test) != 1 || !compare_double(test, d)) {
            length = sprintf((char*)number_buffer, "%1.17g", d);
        }
    }
    
    if (length < 0 || length > (int)sizeof(number_buffer)-1) return false;
    unsigned char *output_pointer = ensure(output_buffer, (size_t)length);
    if (!output_pointer) return false;
    
    for (i = 0; i < (size_t)length; i++) {
        if (number_buffer[i] == decimal_point) {
            output_pointer[i] = '.';
        } else {
            output_pointer[i] = number_buffer[i];
        }
    }
    output_pointer[i] = '\0';
    output_buffer->offset += (size_t)length;
    return true;
}

/**
 * @brief Minify JSON string with proper escaping support
 */
static void minify_string(char **input, char **output) {
    **output = **input;
    (*input)++;
    (*output)++;
    
    for (; **input && **input != '\"'; (*input)++, (*output)++) {
        if (**input == '\\') {
            **output = **input; (*input)++; (*output)++;
            if (**input) {
                **output = **input;
                // Handle escaped quotes
                if (**input == '\"') (*input)++;
            }
        } else {
            **output = **input;
        }
    }
    
    if (**input == '\"') {
        **output = **input;
        (*input)++;
        (*output)++;
    }
}

static stb_json_bool print_string_ptr(const unsigned char *input, printbuffer *output_buffer) {
    if (!output_buffer) return false;
    if (!input) {
        unsigned char *output = ensure(output_buffer, sizeof("\"\""));
        if (!output) return false;
        strcpy((char*)output, "\"\"");
        return true;
    }
    
    size_t escape_characters = 0;
    const unsigned char *input_pointer;
    for (input_pointer = input; *input_pointer; input_pointer++) {
        unsigned char c = *input_pointer;
        size_t bytes = 0;
        
        // Fast path for ASCII
        if (c < 0x80) {
            switch (c) {
                case '\"': case '\\': case '\b': case '\f': 
                case '\n': case '\r': case '\t': 
                    escape_characters++;
                    break;
                default:
                    if (c < 32) escape_characters += 5;
                    break;
            }
        }
        // Multi-byte UTF-8 sequences
        else {
            bytes = 1;
            if ((c & 0xE0) == 0xC0) bytes = 2;
            else if ((c & 0xF0) == 0xE0) bytes = 3;
            else if ((c & 0xF8) == 0xF0) bytes = 4;
            
            // Validate sequence length
            for (size_t i = 1; i < bytes; i++) {
                if ((input_pointer[i] & 0xC0) != 0x80) {
                    bytes = 1; // Invalid sequence
                    break;
                }
            }
            input_pointer += bytes - 1;
        }
    }
    
    size_t output_length = (size_t)(input_pointer - input) + escape_characters;
    unsigned char *output = ensure(output_buffer, output_length + sizeof("\"\""));
    if (!output) return false;
    
    if (escape_characters == 0) {
        output[0] = '\"';
        memcpy(output + 1, input, output_length);
        output[output_length + 1] = '\"';
        output[output_length + 2] = '\0';
        return true;
    }
    
    output[0] = '\"';
    unsigned char *output_pointer = output + 1;
    for (input_pointer = input; *input_pointer != '\0'; input_pointer++) {
        if ((*input_pointer > 31) && (*input_pointer != '\"') && (*input_pointer != '\\')) {
            *output_pointer++ = *input_pointer;
        } else {
            *output_pointer++ = '\\';
            switch (*input_pointer) {
                case '\\': *output_pointer = '\\'; break;
                case '\"': *output_pointer = '\"'; break;
                case '\b': *output_pointer = 'b'; break;
                case '\f': *output_pointer = 'f'; break;
                case '\n': *output_pointer = 'n'; break;
                case '\r': *output_pointer = 'r'; break;
                case '\t': *output_pointer = 't'; break;
                default: sprintf((char*)output_pointer, "u%04x", *input_pointer); output_pointer += 4; break;
            }
            output_pointer++;
        }
    }
    output[output_length + 1] = '\"';
    output[output_length + 2] = '\0';
    return true;
}

static stb_json_bool print_string(const stb_json *item, printbuffer *p) {
    return print_string_ptr((unsigned char*)item->valuestring, p);
}

static stb_json_bool parse_value(stb_json *item, parse_buffer *input_buffer);
static stb_json_bool print_value(const stb_json *item, printbuffer *output_buffer);
static stb_json_bool parse_array(stb_json *item, parse_buffer *input_buffer);
static stb_json_bool print_array(const stb_json *item, printbuffer *output_buffer);
static stb_json_bool parse_object(stb_json *item, parse_buffer *input_buffer);
static stb_json_bool print_object(const stb_json *item, printbuffer *output_buffer);

static parse_buffer *buffer_skip_whitespace(parse_buffer *buffer) {
    if (!buffer || !buffer->content) return NULL;
    while (can_access_at_index(buffer, 0) && buffer_at_offset(buffer)[0] <= 32) buffer->offset++;
    if (buffer->offset == buffer->length) buffer->offset--;
    return buffer;
}

static parse_buffer *skip_utf8_bom(parse_buffer *buffer) {
    if (!buffer || buffer->offset != 0) return NULL;
    if (can_access_at_index(buffer, 4) && strncmp((const char*)buffer_at_offset(buffer), "\xEF\xBB\xBF", 3) == 0) {
        buffer->offset += 3;
    }
    return buffer;
}

/**
 * @brief Parse JSON string into stb_json object
 * @param value JSON string to parse (must be null-terminated)
 * @return stb_json* Parsed object or NULL on error
 */
stb_json *stb_json_parse(const char *value) {
    return stb_json_parse_withopts(value, 0, 0);
}

stb_json *stb_json_parse_withlength(const char *value, size_t buffer_length) {
    return stb_json_parse_withlengthopts(value, buffer_length, 0, 0);
}

stb_json *stb_json_parse_withopts(const char *value, const char **return_parse_end, stb_json_bool require_null_terminated) {
    size_t buffer_length = value ? strlen(value) + 1 : 0;
    return stb_json_parse_withlengthopts(value, buffer_length, return_parse_end, require_null_terminated);
}

stb_json *stb_json_parse_withlengthopts(const char *value, size_t buffer_length, const char **return_parse_end, stb_json_bool require_null_terminated) {
    parse_buffer buffer = { 0, 0, 0, 0, { 0, 0, 0 } };
    stb_json *item = NULL;
    global_error.json = NULL;
    global_error.position = 0;

    if (!value || !buffer_length) goto fail;
    buffer.content = (const unsigned char*)value;
    buffer.length = buffer_length;
    buffer.offset = 0;
    buffer.hooks = global_hooks;

    item = stb_json_new_item(&global_hooks);
    if (!item) goto fail;

    if (!parse_value(item, buffer_skip_whitespace(skip_utf8_bom(&buffer)))) goto fail;

    if (require_null_terminated) {
        buffer_skip_whitespace(&buffer);
        if (buffer.offset >= buffer.length || buffer_at_offset(&buffer)[0] != '\0') goto fail;
    }
    if (return_parse_end) *return_parse_end = (const char*)buffer_at_offset(&buffer);
    return item;

fail:
    if (item) stb_json_delete(item);
    if (value) {
        stb_json_error local_error = { (const unsigned char*)value, 0 };
        if (buffer.offset < buffer.length) local_error.position = buffer.offset;
        else if (buffer.length > 0) local_error.position = buffer.length - 1;
        if (return_parse_end) *return_parse_end = (const char*)local_error.json + local_error.position;
        global_error = local_error;
    }
    return NULL;
}

static stb_json_bool parse_value(stb_json *item, parse_buffer *input_buffer) {
    if (!input_buffer || !input_buffer->content) return false;
    
    if (can_read(input_buffer, 4) && strncmp((const char*)buffer_at_offset(input_buffer), "null", 4) == 0) {
        item->type = STB_JSON_NULL;
        input_buffer->offset += 4;
        return true;
    }
    if (can_read(input_buffer, 5) && strncmp((const char*)buffer_at_offset(input_buffer), "false", 5) == 0) {
        item->type = STB_JSON_FALSE;
        input_buffer->offset += 5;
        return true;
    }
    if (can_read(input_buffer, 4) && strncmp((const char*)buffer_at_offset(input_buffer), "true", 4) == 0) {
        item->type = STB_JSON_TRUE;
        item->valueint = 1;
        input_buffer->offset += 4;
        return true;
    }
    if (can_access_at_index(input_buffer, 0) && buffer_at_offset(input_buffer)[0] == '\"') {
        return parse_string(item, input_buffer);
    }
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '-' || (buffer_at_offset(input_buffer)[0] >= '0' && buffer_at_offset(input_buffer)[0] <= '9'))) {
        return parse_number(item, input_buffer);
    }
    if (can_access_at_index(input_buffer, 0) && buffer_at_offset(input_buffer)[0] == '[') {
        return parse_array(item, input_buffer);
    }
    if (can_access_at_index(input_buffer, 0) && buffer_at_offset(input_buffer)[0] == '{') {
        return parse_object(item, input_buffer);
    }
    return false;
}

static stb_json_bool print_value(const stb_json *item, printbuffer *output_buffer) {
    if (!item || !output_buffer) return false;
    
    unsigned char *output = NULL;
    switch (item->type & 0xFF) {
        case STB_JSON_NULL:
            output = ensure(output_buffer, 5);
            if (output) strcpy((char*)output, "null");
            break;
        case STB_JSON_FALSE:
            output = ensure(output_buffer, 6);
            if (output) strcpy((char*)output, "false");
            break;
        case STB_JSON_TRUE:
            output = ensure(output_buffer, 5);
            if (output) strcpy((char*)output, "true");
            break;
        case STB_JSON_NUMBER:
            return print_number(item, output_buffer);
        case STB_JSON_RAW:
            if (!item->valuestring) return false;
            size_t raw_length = strlen(item->valuestring) + 1;
            output = ensure(output_buffer, raw_length);
            if (output) memcpy(output, item->valuestring, raw_length);
            break;
        case STB_JSON_STRING:
            return print_string(item, output_buffer);
        case STB_JSON_ARRAY:
            return print_array(item, output_buffer);
        case STB_JSON_OBJECT:
            return print_object(item, output_buffer);
        default:
            return false;
    }
    return output != NULL;
}

static stb_json_bool parse_array(stb_json *item, parse_buffer *input_buffer) {
    stb_json *head = NULL;
    stb_json *current_item = NULL;

    if (input_buffer->depth >= STB_JSON_NESTING_LIMIT) return false;
    input_buffer->depth++;

    if (buffer_at_offset(input_buffer)[0] != '[') return false;
    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    
    if (can_access_at_index(input_buffer, 0) && buffer_at_offset(input_buffer)[0] == ']') goto success;
    if (cannot_access_at_index(input_buffer, 0)) {
        input_buffer->offset--;
        return false;
    }
    
    input_buffer->offset--;
    do {
        stb_json *new_item = stb_json_new_item(&input_buffer->hooks);
        if (!new_item) return false;
        
        if (!head) {
            current_item = head = new_item;
        } else {
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }
        
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value(current_item, input_buffer)) return false;
        buffer_skip_whitespace(input_buffer);
    } while (can_access_at_index(input_buffer, 0) && buffer_at_offset(input_buffer)[0] == ',');
    
    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != ']') return false;

success:
    input_buffer->depth--;
    if (head) head->prev = current_item;
    item->type = STB_JSON_ARRAY;
    item->child = head;
    input_buffer->offset++;
    return true;
}

static stb_json_bool print_array(const stb_json *item, printbuffer *output_buffer) {
    if (!output_buffer) return false;
    
    unsigned char *output_pointer = ensure(output_buffer, 1);
    if (!output_pointer) return false;
    *output_pointer = '[';
    output_buffer->offset++;
    output_buffer->depth++;
    
    stb_json *current_element = item->child;
    while (current_element) {
        if (!print_value(current_element, output_buffer)) return false;
        update_offset(output_buffer);
        if (current_element->next) {
            size_t length = output_buffer->format ? 2 : 1;
            output_pointer = ensure(output_buffer, length + 1);
            if (!output_pointer) return false;
            *output_pointer++ = ',';
            if (output_buffer->format) *output_pointer++ = ' ';
            *output_pointer = '\0';
            output_buffer->offset += length;
        }
        current_element = current_element->next;
    }
    
    output_pointer = ensure(output_buffer, 2);
    if (!output_pointer) return false;
    *output_pointer++ = ']';
    *output_pointer = '\0';
    output_buffer->depth--;
    return true;
}

static stb_json_bool parse_object(stb_json *item, parse_buffer *input_buffer) {
    stb_json *head = NULL;
    stb_json *current_item = NULL;

    if (input_buffer->depth >= STB_JSON_NESTING_LIMIT) return false;
    input_buffer->depth++;

    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != '{') return false;
    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    
    if (can_access_at_index(input_buffer, 0) && buffer_at_offset(input_buffer)[0] == '}') goto success;
    if (cannot_access_at_index(input_buffer, 0)) {
        input_buffer->offset--;
        return false;
    }
    
    input_buffer->offset--;
    do {
        stb_json *new_item = stb_json_new_item(&input_buffer->hooks);
        if (!new_item) return false;
        
        if (!head) {
            current_item = head = new_item;
        } else {
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }
        
        if (cannot_access_at_index(input_buffer, 1)) return false;
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_string(current_item, input_buffer)) return false;
        buffer_skip_whitespace(input_buffer);
        
        current_item->string = current_item->valuestring;
        current_item->valuestring = NULL;
        
        if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != ':') return false;
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value(current_item, input_buffer)) return false;
        buffer_skip_whitespace(input_buffer);
    } while (can_access_at_index(input_buffer, 0) && buffer_at_offset(input_buffer)[0] == ',');
    
    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != '}') return false;

success:
    input_buffer->depth--;
    if (head) head->prev = current_item;
    item->type = STB_JSON_OBJECT;
    item->child = head;
    input_buffer->offset++;
    return true;
}

static stb_json_bool print_object(const stb_json *item, printbuffer *output_buffer) {
    if (!output_buffer) return false;
    
    size_t length = output_buffer->format ? 2 : 1;
    unsigned char *output_pointer = ensure(output_buffer, length + 1);
    if (!output_pointer) return false;
    
    *output_pointer++ = '{';
    if (output_buffer->format) *output_pointer++ = '\n';
    output_buffer->offset += length;
    output_buffer->depth++;
    
    stb_json *current_item = item->child;
    while (current_item) {
        if (output_buffer->format) {
            size_t i;
            output_pointer = ensure(output_buffer, output_buffer->depth);
            if (!output_pointer) return false;
            for (i = 0; i < output_buffer->depth; i++) *output_pointer++ = '\t';
            output_buffer->offset += output_buffer->depth;
        }
        
        if (!print_string_ptr((unsigned char*)current_item->string, output_buffer)) return false;
        update_offset(output_buffer);
        
        length = output_buffer->format ? 2 : 1;
        output_pointer = ensure(output_buffer, length);
        if (!output_pointer) return false;
        *output_pointer++ = ':';
        if (output_buffer->format) *output_pointer++ = '\t';
        output_buffer->offset += length;
        
        if (!print_value(current_item, output_buffer)) return false;
        update_offset(output_buffer);
        
        length = (size_t)((current_item->next ? 1 : 0) + (output_buffer->format ? 1 : 0));
        output_pointer = ensure(output_buffer, length + 1);
        if (!output_pointer) return false;
        if (current_item->next) *output_pointer++ = ',';
        if (output_buffer->format) *output_pointer++ = '\n';
        *output_pointer = '\0';
        output_buffer->offset += length;
        
        current_item = current_item->next;
    }
    
    output_pointer = ensure(output_buffer, output_buffer->format ? (output_buffer->depth + 1) : 2);
    if (!output_pointer) return false;
    if (output_buffer->format) {
        size_t i;
        for (i = 0; i < (output_buffer->depth - 1); i++) *output_pointer++ = '\t';
    }
    *output_pointer++ = '}';
    *output_pointer = '\0';
    output_buffer->depth--;
    return true;
}

int stb_json_getarraysize(const stb_json *array) {
    if (!array) return 0;
    stb_json *child = array->child;
    size_t size = 0;
    while (child) { size++; child = child->next; }
    return (int)size;
}

static stb_json* get_array_item(const stb_json *array, size_t index) {
    stb_json *child = array ? array->child : NULL;
    while (child && index > 0) { index--; child = child->next; }
    return child;
}

stb_json *stb_json_getarrayitem(const stb_json *array, int index) {
    if (index < 0) return NULL;
    return get_array_item(array, (size_t)index);
}

static stb_json *get_object_item(const stb_json *object, const char *name, stb_json_bool case_sensitive) {
    if (!object || !name) return NULL;
    stb_json *current_element = object->child;
    if (case_sensitive) {
        while (current_element && current_element->string && strcmp(name, current_element->string) != 0) {
            current_element = current_element->next;
        }
    } else {
        while (current_element && current_element->string && case_insensitive_strcmp((unsigned char*)name, (unsigned char*)current_element->string) != 0) {
            current_element = current_element->next;
        }
    }
    return (current_element && current_element->string) ? current_element : NULL;
}

stb_json *stb_json_getobjectitem(const stb_json *object, const char *string) {
    return get_object_item(object, string, false);
}

stb_json *stb_json_getobjectitemcasesensitive(const stb_json *object, const char *string) {
    return get_object_item(object, string, true);
}

stb_json_bool stb_json_hasobjectitem(const stb_json *object, const char *string) {
    return stb_json_getobjectitem(object, string) ? 1 : 0;
}

static void suffix_object(stb_json *prev, stb_json *item) {
    prev->next = item;
    item->prev = prev;
}

static stb_json *create_reference(const stb_json *item, const internal_hooks *hooks) {
    if (!item) return NULL;
    stb_json *reference = stb_json_new_item(hooks);
    if (reference) {
        memcpy(reference, item, sizeof(stb_json));
        reference->string = NULL;
        reference->type |= STB_JSON_IS_REFERENCE;
        reference->next = reference->prev = NULL;
    }
    return reference;
}

static stb_json_bool add_item_to_array(stb_json *array, stb_json *item) {
    if (!item || !array || array == item) return false;
    
    stb_json *child = array->child;
    if (!child) {
        array->child = item;
        item->prev = item;
    } else {
        if (child->prev) {
            suffix_object(child->prev, item);
            array->child->prev = item;
        }
    }
    return true;
}

stb_json_bool stb_json_additemtoarray(stb_json *array, stb_json *item) {
    return add_item_to_array(array, item);
}

static void* cast_away_const(const void* string) {
    return (void*)string;
}

static stb_json_bool add_item_to_object(stb_json *object, const char *string, stb_json *item, const internal_hooks *hooks, stb_json_bool constant_key) {
    if (!object || !string || !item || object == item) return false;
    
    char *new_key;
    int new_type;
    if (constant_key) {
        new_key = (char*)cast_away_const(string);
        new_type = item->type | STB_JSON_STRING_IS_CONST;
    } else {
        new_key = (char*)stb_json_strdup((const unsigned char*)string, hooks);
        if (!new_key) return false;
        new_type = item->type & ~STB_JSON_STRING_IS_CONST;
    }
    
    if (!(item->type & STB_JSON_STRING_IS_CONST) && item->string) {
        hooks->deallocate(item->string);
    }
    item->string = new_key;
    item->type = new_type;
    return add_item_to_array(object, item);
}

stb_json_bool stb_json_additemtoobject(stb_json *object, const char *string, stb_json *item) {
    return add_item_to_object(object, string, item, &global_hooks, false);
}

stb_json_bool stb_json_additemtoobjectcs(stb_json *object, const char *string, stb_json *item) {
    return add_item_to_object(object, string, item, &global_hooks, true);
}

stb_json_bool stb_json_additemreferencetoarray(stb_json *array, stb_json *item) {
    return array ? add_item_to_array(array, create_reference(item, &global_hooks)) : false;
}

stb_json_bool stb_json_additemreferencetoobject(stb_json *object, const char *string, stb_json *item) {
    return (object && string) ? add_item_to_object(object, string, create_reference(item, &global_hooks), &global_hooks, false) : false;
}

stb_json* stb_json_addnulltoobject(stb_json *object, const char *name) {
    stb_json *null = stb_json_createnull();
    if (add_item_to_object(object, name, null, &global_hooks, false)) return null;
    stb_json_delete(null);
    return NULL;
}

stb_json* stb_json_addtruetoobject(stb_json *object, const char *name) {
    stb_json *true_item = stb_json_createtrue();
    if (add_item_to_object(object, name, true_item, &global_hooks, false)) return true_item;
    stb_json_delete(true_item);
    return NULL;
}

stb_json* stb_json_addfalsetoobject(stb_json *object, const char *name) {
    stb_json *false_item = stb_json_createfalse();
    if (add_item_to_object(object, name, false_item, &global_hooks, false)) return false_item;
    stb_json_delete(false_item);
    return NULL;
}

stb_json* stb_json_addbooltoobject(stb_json *object, const char *name, stb_json_bool boolean) {
    stb_json *bool_item = stb_json_createbool(boolean);
    if (add_item_to_object(object, name, bool_item, &global_hooks, false)) return bool_item;
    stb_json_delete(bool_item);
    return NULL;
}

stb_json* stb_json_addnumbertoobject(stb_json *object, const char *name, double number) {
    stb_json *number_item = stb_json_createnumber(number);
    if (add_item_to_object(object, name, number_item, &global_hooks, false)) return number_item;
    stb_json_delete(number_item);
    return NULL;
}

stb_json* stb_json_addstringtoobject(stb_json *object, const char *name, const char *string) {
    stb_json *string_item = stb_json_createstring(string);
    if (add_item_to_object(object, name, string_item, &global_hooks, false)) return string_item;
    stb_json_delete(string_item);
    return NULL;
}

stb_json* stb_json_addrawtoobject(stb_json *object, const char *name, const char *raw) {
    stb_json *raw_item = stb_json_createraw(raw);
    if (add_item_to_object(object, name, raw_item, &global_hooks, false)) return raw_item;
    stb_json_delete(raw_item);
    return NULL;
}

stb_json* stb_json_addobjecttoobject(stb_json *object, const char *name) {
    stb_json *object_item = stb_json_createobject();
    if (add_item_to_object(object, name, object_item, &global_hooks, false)) return object_item;
    stb_json_delete(object_item);
    return NULL;
}

stb_json* stb_json_addarraytoobject(stb_json *object, const char *name) {
    stb_json *array = stb_json_createarray();
    if (add_item_to_object(object, name, array, &global_hooks, false)) return array;
    stb_json_delete(array);
    return NULL;
}

stb_json *stb_json_detachitemviapointer(stb_json *parent, stb_json *item) {
    if (!parent || !item || (item != parent->child && !item->prev)) return NULL;
    
    if (item != parent->child) item->prev->next = item->next;
    if (item->next) item->next->prev = item->prev;
    if (item == parent->child) parent->child = item->next;
    else if (!item->next) parent->child->prev = item->prev;
    
    item->prev = item->next = NULL;
    return item;
}

stb_json *stb_json_detachitemfromarray(stb_json *array, int which) {
    if (which < 0) return NULL;
    return stb_json_detachitemviapointer(array, get_array_item(array, (size_t)which));
}

void stb_json_deleteitemfromarray(stb_json *array, int which) {
    stb_json_delete(stb_json_detachitemfromarray(array, which));
}

stb_json *stb_json_detachitemfromobject(stb_json *object, const char *string) {
    stb_json *item = stb_json_getobjectitem(object, string);
    return stb_json_detachitemviapointer(object, item);
}

stb_json *stb_json_detachitemfromobjectcasesensitive(stb_json *object, const char *string) {
    stb_json *item = stb_json_getobjectitemcasesensitive(object, string);
    return stb_json_detachitemviapointer(object, item);
}

void stb_json_deleteitemfromobject(stb_json *object, const char *string) {
    stb_json_delete(stb_json_detachitemfromobject(object, string));
}

void stb_json_deleteitemfromobjectcasesensitive(stb_json *object, const char *string) {
    stb_json_delete(stb_json_detachitemfromobjectcasesensitive(object, string));
}

stb_json_bool stb_json_insertiteminarray(stb_json *array, int which, stb_json *newitem) {
    if (which < 0 || !newitem) return false;
    stb_json *after_inserted = get_array_item(array, (size_t)which);
    if (!after_inserted) return add_item_to_array(array, newitem);
    
    newitem->next = after_inserted;
    newitem->prev = after_inserted->prev;
    after_inserted->prev = newitem;
    if (after_inserted == array->child) array->child = newitem;
    else newitem->prev->next = newitem;
    return true;
}

stb_json_bool stb_json_replaceitemviapointer(stb_json *parent, stb_json *item, stb_json *replacement) {
    if (!parent || !parent->child || !replacement || !item) return false;
    if (replacement == item) return true;
    
    replacement->next = item->next;
    replacement->prev = item->prev;
    if (replacement->next) replacement->next->prev = replacement;
    if (item == parent->child) {
        if (parent->child->prev == parent->child) replacement->prev = replacement;
        parent->child = replacement;
    } else {
        if (replacement->prev) replacement->prev->next = replacement;
        if (!replacement->next) parent->child->prev = replacement;
    }
    
    item->next = item->prev = NULL;
    stb_json_delete(item);
    return true;
}

stb_json_bool stb_json_replaceiteminarray(stb_json *array, int which, stb_json *newitem) {
    if (which < 0) return false;
    return stb_json_replaceitemviapointer(array, get_array_item(array, (size_t)which), newitem);
}

static stb_json_bool replace_item_in_object(stb_json *object, const char *string, stb_json *replacement, stb_json_bool case_sensitive) {
    if (!replacement || !string) return false;
    
    if (!(replacement->type & STB_JSON_STRING_IS_CONST) && replacement->string) {
        stb_json_free(replacement->string);
    }
    replacement->string = (char*)stb_json_strdup((const unsigned char*)string, &global_hooks);
    if (!replacement->string) return false;
    replacement->type &= ~STB_JSON_STRING_IS_CONST;
    
    return stb_json_replaceitemviapointer(object, get_object_item(object, string, case_sensitive), replacement);
}

stb_json_bool stb_json_replaceiteminobject(stb_json *object, const char *string, stb_json *newitem) {
    return replace_item_in_object(object, string, newitem, false);
}

stb_json_bool stb_json_replaceiteminobjectcasesensitive(stb_json *object, const char *string, stb_json *newitem) {
    return replace_item_in_object(object, string, newitem, true);
}

stb_json *stb_json_createnull(void) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) item->type = STB_JSON_NULL;
    return item;
}

stb_json *stb_json_createtrue(void) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) item->type = STB_JSON_TRUE;
    return item;
}

stb_json *stb_json_createfalse(void) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) item->type = STB_JSON_FALSE;
    return item;
}

stb_json *stb_json_createbool(stb_json_bool boolean) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) item->type = boolean ? STB_JSON_TRUE : STB_JSON_FALSE;
    return item;
}

stb_json *stb_json_createnumber(double num) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) {
        item->type = STB_JSON_NUMBER;
        item->valuedouble = num;
        if (num >= INT_MAX) item->valueint = INT_MAX;
        else if (num <= (double)INT_MIN) item->valueint = INT_MIN;
        else item->valueint = (int)num;
    }
    return item;
}

stb_json *stb_json_createstring(const char *string) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) {
        item->type = STB_JSON_STRING;
        item->valuestring = (char*)stb_json_strdup((const unsigned char*)string, &global_hooks);
        if (!item->valuestring) {
            stb_json_delete(item);
            return NULL;
        }
    }
    return item;
}

stb_json *stb_json_createstringReference(const char *string) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) {
        item->type = STB_JSON_STRING | STB_JSON_IS_REFERENCE;
        item->valuestring = (char*)cast_away_const(string);
    }
    return item;
}

stb_json *stb_json_createobjectReference(const stb_json *child) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) {
        item->type = STB_JSON_OBJECT | STB_JSON_IS_REFERENCE;
        item->child = (stb_json*)cast_away_const(child);
    }
    return item;
}

stb_json *stb_json_createarrayReference(const stb_json *child) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) {
        item->type = STB_JSON_ARRAY | STB_JSON_IS_REFERENCE;
        item->child = (stb_json*)cast_away_const(child);
    }
    return item;
}

stb_json *stb_json_createraw(const char *raw) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) {
        item->type = STB_JSON_RAW;
        item->valuestring = (char*)stb_json_strdup((const unsigned char*)raw, &global_hooks);
        if (!item->valuestring) {
            stb_json_delete(item);
            return NULL;
        }
    }
    return item;
}

stb_json *stb_json_createarray(void) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) item->type = STB_JSON_ARRAY;
    return item;
}

stb_json *stb_json_createobject(void) {
    stb_json *item = stb_json_new_item(&global_hooks);
    if (item) item->type = STB_JSON_OBJECT;
    return item;
}

stb_json *stb_json_createintarray(const int *numbers, int count) {
    if (count < 0 || !numbers) return NULL;
    stb_json *a = stb_json_createarray();
    if (!a) return NULL;
    for (size_t i = 0; i < (size_t)count && a; i++) {
        stb_json *n = stb_json_createnumber(numbers[i]);
        if (!n) { stb_json_delete(a); return NULL; }
        if (!i) a->child = n;
        else suffix_object(a->child->prev, n);
        a->child->prev = n;
    }
    return a;
}

stb_json *stb_json_createfloatarray(const float *numbers, int count) {
    if (count < 0 || !numbers) return NULL;
    stb_json *a = stb_json_createarray();
    if (!a) return NULL;
    for (size_t i = 0; i < (size_t)count && a; i++) {
        stb_json *n = stb_json_createnumber((double)numbers[i]);
        if (!n) { stb_json_delete(a); return NULL; }
        if (!i) a->child = n;
        else suffix_object(a->child->prev, n);
        a->child->prev = n;
    }
    return a;
}

stb_json *stb_json_createdoublearray(const double *numbers, int count) {
    if (count < 0 || !numbers) return NULL;
    stb_json *a = stb_json_createarray();
    if (!a) return NULL;
    for (size_t i = 0; i < (size_t)count && a; i++) {
        stb_json *n = stb_json_createnumber(numbers[i]);
        if (!n) { stb_json_delete(a); return NULL; }
        if (!i) a->child = n;
        else suffix_object(a->child->prev, n);
        a->child->prev = n;
    }
    return a;
}

stb_json *stb_json_createstringarray(const char *const *strings, int count) {
    if (count < 0 || !strings) return NULL;
    stb_json *a = stb_json_createarray();
    if (!a) return NULL;
    for (size_t i = 0; i < (size_t)count && a; i++) {
        stb_json *n = stb_json_createstring(strings[i]);
        if (!n) { stb_json_delete(a); return NULL; }
        if (!i) a->child = n;
        else suffix_object(a->child->prev, n);
        a->child->prev = n;
    }
    return a;
}

static stb_json *stb_json_duplicate_rec(const stb_json *item, size_t depth, stb_json_bool recurse) {
    if (!item) return NULL;
    
    stb_json *newitem = stb_json_new_item(&global_hooks);
    if (!newitem) return NULL;
    memcpy(newitem, item, sizeof(stb_json));
    newitem->string = NULL;
    newitem->valuestring = NULL;
    newitem->child = NULL;
    newitem->next = newitem->prev = NULL;
    
    if (item->string && !(item->type & STB_JSON_STRING_IS_CONST)) {
        newitem->string = (char*)stb_json_strdup((unsigned char*)item->string, &global_hooks);
        if (!newitem->string) goto fail;
    }
    if (item->valuestring) {
        newitem->valuestring = (char*)stb_json_strdup((unsigned char*)item->valuestring, &global_hooks);
        if (!newitem->valuestring) goto fail;
    }
    
    if (!recurse) return newitem;
    if (depth >= STB_JSON_CIRCULAR_LIMIT) goto fail;
    
    stb_json *child = item->child;
    stb_json *next = NULL;
    while (child) {
        stb_json *newchild = stb_json_duplicate_rec(child, depth + 1, true);
        if (!newchild) goto fail;
        if (next) {
            next->next = newchild;
            newchild->prev = next;
            next = newchild;
        } else {
            newitem->child = next = newchild;
        }
        child = child->next;
    }
    if (newitem->child) newitem->child->prev = next;
    return newitem;
    
fail:
    if (newitem) stb_json_delete(newitem);
    return NULL;
}

stb_json *stb_json_duplicate(const stb_json *item, stb_json_bool recurse) {
    return stb_json_duplicate_rec(item, 0, recurse);
}

static void skip_oneline_comment(char **input) {
    *input += 2;
    for (; **input; (*input)++) if (**input == '\n') { (*input)++; return; }
}

static void skip_multiline_comment(char **input) {
    *input += 2;
    for (; **input; (*input)++) {
        if (**input == '*' && (*input)[1] == '/') {
            *input += 2;
            return;
        }
    }
}

/**
 * @brief Minify JSON string
 * @param json JSON string to minify (modified in-place)
 */
void stb_json_minify(char *json) {
    char *into = json;
    if (!json) return;
    
    while (*json) {
        switch (*json) {
            case ' ': case '\t': case '\r': case '\n': json++; break;
            case '/':
                if (json[1] == '/') skip_oneline_comment(&json);
                else if (json[1] == '*') skip_multiline_comment(&json);
                else json++;
                break;
            case '\"': minify_string(&json, &into); break;
            default: *into++ = *json++; break;
        }
    }
    *into = '\0';
}

stb_json_bool stb_json_isinvalid(const stb_json *item) { return item && (item->type & 0xFF) == STB_JSON_INVALID; }
stb_json_bool stb_json_isfalse(const stb_json *item) { return item && (item->type & 0xFF) == STB_JSON_FALSE; }
stb_json_bool stb_json_istrue(const stb_json *item) { return item && (item->type & 0xFF) == STB_JSON_TRUE; }
stb_json_bool stb_json_isbool(const stb_json *item) { return item && (item->type & (STB_JSON_FALSE|STB_JSON_TRUE)); }
stb_json_bool stb_json_isnull(const stb_json *item) { return item && (item->type & 0xFF) == STB_JSON_NULL; }
stb_json_bool stb_json_isnumber(const stb_json *item) { return item && (item->type & 0xFF) == STB_JSON_NUMBER; }
stb_json_bool stb_json_isstring(const stb_json *item) { return item && (item->type & 0xFF) == STB_JSON_STRING; }
stb_json_bool stb_json_isarray(const stb_json *item) { return item && (item->type & 0xFF) == STB_JSON_ARRAY; }
stb_json_bool stb_json_isobject(const stb_json *item) { return item && (item->type & 0xFF) == STB_JSON_OBJECT; }
stb_json_bool stb_json_israw(const stb_json *item) { return item && (item->type & 0xFF) == STB_JSON_RAW; }

stb_json_bool stb_json_compare(const stb_json *a, const stb_json *b, stb_json_bool case_sensitive) {
    if (!a || !b || (a->type & 0xFF) != (b->type & 0xFF)) return false;
    if (a == b) return true;
    
    switch (a->type & 0xFF) {
        case STB_JSON_FALSE: case STB_JSON_TRUE: case STB_JSON_NULL: return true;
        case STB_JSON_NUMBER: return compare_double(a->valuedouble, b->valuedouble);
        case STB_JSON_STRING: case STB_JSON_RAW: 
            return a->valuestring && b->valuestring && strcmp(a->valuestring, b->valuestring) == 0;
        case STB_JSON_ARRAY: {
            stb_json *a_element = a->child;
            stb_json *b_element = b->child;
            while (a_element && b_element) {
                if (!stb_json_compare(a_element, b_element, case_sensitive)) return false;
                a_element = a_element->next;
                b_element = b_element->next;
            }
            return !a_element && !b_element;
        }
        case STB_JSON_OBJECT: {
            stb_json *a_element = a->child;
            stb_json *b_element = b->child;
            while (a_element && b_element) {
                if (case_sensitive ? strcmp(a_element->string, b_element->string) != 0 : 
                    case_insensitive_strcmp((unsigned char*)a_element->string, (unsigned char*)b_element->string) != 0) {
                    return false;
                }
                if (!stb_json_compare(a_element, b_element, case_sensitive)) return false;
                a_element = a_element->next;
                b_element = b_element->next;
            }
            return !a_element && !b_element;
        }
        default: return false;
    }
}

static unsigned char* stb_json_utils_strdup(const unsigned char* string) {
    if (!string) return NULL;
    size_t length = strlen((const char*)string) + 1;
    unsigned char *copy = stb_json_malloc(length);
    if (copy) memcpy(copy, string, length);
    return copy;
}

static int compare_strings(const unsigned char *s1, const unsigned char *s2, stb_json_bool case_sensitive) {
    if (!s1 || !s2) return 1;
    if (s1 == s2) return 0;
    return case_sensitive ? strcmp((const char*)s1, (const char*)s2) : case_insensitive_strcmp(s1, s2);
}

static stb_json_bool compare_pointers(const unsigned char *name, const unsigned char *pointer, stb_json_bool case_sensitive) {
    if (!name || !pointer) return false;
    for (; *name && *pointer && *pointer != '/'; name++, pointer++) {
        if (*pointer == '~') {
            if ((pointer[1] != '0' || *name != '~') && (pointer[1] != '1' || *name != '/')) return false;
            pointer++;
        } else if ((case_sensitive && *name != *pointer) || (!case_sensitive && tolower(*name) != tolower(*pointer))) {
            return false;
        }
    }
    return (*name == '\0') && (*pointer == '\0' || *pointer == '/');
}

static size_t pointer_encoded_length(const unsigned char *string) {
    size_t length = 0;
    for (; *string; string++, length++) if (*string == '~' || *string == '/') length++;
    return length;
}

static void encode_string_as_pointer(unsigned char *dest, const unsigned char *src) {
    for (; *src; src++, dest++) {
        if (*src == '/') {
            *dest++ = '~';
            *dest = '1';
        } else if (*src == '~') {
            *dest++ = '~';
            *dest = '0';
        } else {
            *dest = *src;
        }
    }
    *dest = '\0';
}

/**
 * @brief Find JSON Pointer path from object to target
 * @param object Root object
 * @param target Target item
 * @return char* JSON Pointer string (must be freed) or NULL
 */
char *stb_json_utils_findpointerfromobjectto(const stb_json *object, const stb_json *target) {
    if (!object || !target) return NULL;
    if (object == target) return (char*)stb_json_utils_strdup((const unsigned char*)""");
    
    size_t child_index = 0;
    stb_json *current_child = object->child;
    while (current_child) {
        char *target_pointer = stb_json_utils_findpointerfromobjectto(current_child, target);
        if (target_pointer) {
            unsigned char *full_pointer = NULL;
            size_t len = 0;
            
            if (stb_json_isarray(object)) {
                len = strlen(target_pointer) + 32;
                full_pointer = stb_json_malloc(len);
                if (!full_pointer) {
                    stb_json_free(target_pointer);
                    return NULL;
                }
                snprintf((char*)full_pointer, len, "/%zu%s", child_index, target_pointer);
            } 
            else if (stb_json_isobject(object)) {
                len = pointer_encoded_length((unsigned char*)current_child->string) + 
                      strlen(target_pointer) + 2;
                full_pointer = stb_json_malloc(len);
                if (!full_pointer) {
                    stb_json_free(target_pointer);
                    return NULL;
                }
                full_pointer[0] = '/';
                encode_string_as_pointer(full_pointer + 1, (unsigned char*)current_child->string);
                strcat((char*)full_pointer, (char*)target_pointer);
            }
            
            stb_json_free(target_pointer);
            return (char*)full_pointer;
        }
        child_index++;
        current_child = current_child->next;
    }
    return NULL;
}

static stb_json *get_array_item(const stb_json *array, size_t item) {
    stb_json *child = array ? array->child : NULL;
    while (child && item > 0) { item--; child = child->next; }
    return child;
}

static stb_json_bool decode_array_index_from_pointer(const unsigned char *pointer, size_t *index) {
    if (pointer[0] == '0' && pointer[1] && pointer[1] != '/') return false;
    size_t parsed_index = 0;
    size_t position = 0;
    for (; pointer[position] >= '0' && pointer[position] <= '9'; position++) {
        parsed_index = 10 * parsed_index + (size_t)(pointer[position] - '0');
    }
    if ((pointer[position] && pointer[position] != '/') || position == 0) return false;
    *index = parsed_index;
    return true;
}

static stb_json *get_item_from_pointer(stb_json *object, const char *pointer, stb_json_bool case_sensitive) {
    if (!pointer) return NULL;
    stb_json *current_element = object;
    while (*pointer == '/' && current_element) {
        pointer++;
        if (stb_json_isarray(current_element)) {
            size_t index;
            if (!decode_array_index_from_pointer((const unsigned char*)pointer, &index)) return NULL;
            current_element = get_array_item(current_element, index);
        } else if (stb_json_isobject(current_element)) {
            current_element = current_element->child;
            while (current_element && !compare_pointers((unsigned char*)current_element->string, (const unsigned char*)pointer, case_sensitive)) {
                current_element = current_element->next;
            }
        } else {
            return NULL;
        }
        while (*pointer && *pointer != '/') pointer++;
    }
    return current_element;
}

stb_json *stb_json_utils_getpointer(stb_json *object, const char *pointer) {
    return get_item_from_pointer(object, pointer, false);
}

stb_json *stb_json_utils_getpointercasesensitive(stb_json *object, const char *pointer) {
    return get_item_from_pointer(object, pointer, true);
}

static void decode_pointer_inplace(unsigned char *string) {
    if (!string) return;
    unsigned char *decoded_string = string;
    for (; *string; decoded_string++, string++) {
        if (*string == '~') {
            if (string[1] == '0') *decoded_string = '~';
            else if (string[1] == '1') *decoded_string = '/';
            else return;
            string++;
        } else {
            *decoded_string = *string;
        }
    }
    *decoded_string = '\0';
}

static stb_json *detach_item_from_array(stb_json *array, size_t which) {
    stb_json *c = array->child;
    while (c && which > 0) { c = c->next; which--; }
    if (!c) return NULL;
    
    if (c != array->child) c->prev->next = c->next;
    if (c->next) c->next->prev = c->prev;
    if (c == array->child) array->child = c->next;
    else if (!c->next) array->child->prev = c->prev;
    
    c->prev = c->next = NULL;
    return c;
}

static stb_json *detach_path(stb_json *object, const unsigned char *path, stb_json_bool case_sensitive) {
    unsigned char *parent_pointer = stb_json_utils_strdup(path);
    if (!parent_pointer) return NULL;
    
    unsigned char *child_pointer = (unsigned char*)strrchr((char*)parent_pointer, '/');
    if (!child_pointer) { stb_json_free(parent_pointer); return NULL; }
    *child_pointer = '\0';
    child_pointer++;
    
    stb_json *parent = get_item_from_pointer(object, (char*)parent_pointer, case_sensitive);
    decode_pointer_inplace(child_pointer);
    stb_json *detached_item = NULL;
    
    if (stb_json_isarray(parent)) {
        size_t index;
        if (!decode_array_index_from_pointer(child_pointer, &index)) goto cleanup;
        detached_item = detach_item_from_array(parent, index);
    } else if (stb_json_isobject(parent)) {
        detached_item = case_sensitive ? 
            stb_json_detachitemfromobjectcasesensitive(parent, (char*)child_pointer) :
            stb_json_detachitemfromobject(parent, (char*)child_pointer);
    }
    
cleanup:
    stb_json_free(parent_pointer);
    return detached_item;
}

static stb_json *sort_list(stb_json *list, stb_json_bool case_sensitive) {
    if (!list || !list->next) return list;
    
    stb_json *first = list;
    stb_json *second = list;
    stb_json *current = list;
    while (current && current->next) {
        second = second->next;
        current = current->next->next;
    }
    if (!second || !second->prev) return list;
    second->prev->next = NULL;
    second->prev = NULL;
    
    first = sort_list(first, case_sensitive);
    second = sort_list(second, case_sensitive);
    stb_json *result = NULL;
    stb_json *result_tail = NULL;
    
    while (first && second) {
        stb_json *smaller = compare_strings((unsigned char*)first->string, (unsigned char*)second->string, case_sensitive) < 0 ? first : second;
        if (!result) {
            result = result_tail = smaller;
        } else {
            result_tail->next = smaller;
            smaller->prev = result_tail;
            result_tail = smaller;
        }
        if (smaller == first) first = first->next;
        else second = second->next;
    }
    
    if (first) {
        if (!result) return first;
        result_tail->next = first;
        first->prev = result_tail;
    }
    if (second) {
        if (!result) return second;
        result_tail->next = second;
        second->prev = result_tail;
    }
    return result;
}

static void sort_object(stb_json *object, stb_json_bool case_sensitive) {
    if (object) object->child = sort_list(object->child, case_sensitive);
}

static stb_json_bool compare_json(stb_json *a, stb_json *b, stb_json_bool case_sensitive) {
    if (!a || !b || (a->type & 0xFF) != (b->type & 0xFF)) return false;
    switch (a->type & 0xFF) {
        case STB_JSON_NUMBER: 
            return a->valueint == b->valueint && compare_double(a->valuedouble, b->valuedouble);
        case STB_JSON_STRING: case STB_JSON_RAW:
            return a->valuestring && b->valuestring && strcmp(a->valuestring, b->valuestring) == 0;
        case STB_JSON_ARRAY: {
            stb_json *a_child = a->child;
            stb_json *b_child = b->child;
            while (a_child && b_child) {
                if (!compare_json(a_child, b_child, case_sensitive)) return false;
                a_child = a_child->next;
                b_child = b_child->next;
            }
            return !a_child && !b_child;
        }
        case STB_JSON_OBJECT: {
            sort_object(a, case_sensitive);
            sort_object(b, case_sensitive);
            stb_json *a_child = a->child;
            stb_json *b_child = b->child;
            while (a_child && b_child) {
                if (compare_strings((unsigned char*)a_child->string, (unsigned char*)b_child->string, case_sensitive) != 0) return false;
                if (!compare_json(a_child, b_child, case_sensitive)) return false;
                a_child = a_child->next;
                b_child = b_child->next;
            }
            return !a_child && !b_child;
        }
        default: return true;
    }
}

static stb_json_bool insert_item_in_array(stb_json *array, size_t which, stb_json *newitem) {
    stb_json *child = array->child;
    while (child && which > 0) { child = child->next; which--; }
    if (!child) return add_item_to_array(array, newitem);
    
    newitem->next = child;
    newitem->prev = child->prev;
    child->prev = newitem;
    if (child == array->child) array->child = newitem;
    else newitem->prev->next = newitem;
    return true;
}

static stb_json *get_object_item(const stb_json *object, const char* name, stb_json_bool case_sensitive) {
    return case_sensitive ? 
        stb_json_getobjectitemcasesensitive(object, name) : 
        stb_json_getobjectitem(object, name);
}

enum patch_operation { INVALID, ADD, REMOVE, REPLACE, MOVE, COPY, TEST };

static enum patch_operation decode_patch_operation(const stb_json *patch, stb_json_bool case_sensitive) {
    stb_json *operation = get_object_item(patch, "op", case_sensitive);
    if (!stb_json_isstring(operation)) return INVALID;
    const char *op = operation->valuestring;
    if (strcmp(op, "add") == 0) return ADD;
    if (strcmp(op, "remove") == 0) return REMOVE;
    if (strcmp(op, "replace") == 0) return REPLACE;
    if (strcmp(op, "move") == 0) return MOVE;
    if (strcmp(op, "copy") == 0) return COPY;
    if (strcmp(op, "test") == 0) return TEST;
    return INVALID;
}

static void overwrite_item(stb_json *root, const stb_json replacement) {
    if (!root) return;
    if (root->string) stb_json_free(root->string);
    if (root->valuestring) stb_json_free(root->valuestring);
    if (root->child) stb_json_delete(root->child);
    memcpy(root, &replacement, sizeof(stb_json));
}

static int apply_patch(stb_json *object, const stb_json *patch, stb_json_bool case_sensitive) {
    stb_json *path = get_object_item(patch, "path", case_sensitive);
    if (!stb_json_isstring(path)) return 2;
    
    enum patch_operation opcode = decode_patch_operation(patch, case_sensitive);
    if (opcode == INVALID) return 3;
    if (opcode == TEST) {
        return !compare_json(get_item_from_pointer(object, path->valuestring, case_sensitive), 
                            get_object_item(patch, "value", case_sensitive), case_sensitive);
    }
    
    if (path->valuestring[0] == '\0') {
        if (opcode == REMOVE) {
            static const stb_json invalid = {0};
            overwrite_item(object, invalid);
            return 0;
        }
        if (opcode == REPLACE || opcode == ADD) {
            stb_json *value = get_object_item(patch, "value", case_sensitive);
            if (!value) return 7;
            stb_json *duplicate = stb_json_duplicate(value, 1);
            if (!duplicate) return 8;
            overwrite_item(object, *duplicate);
            stb_json_free(duplicate);
            if (object->string) { stb_json_free(object->string); object->string = NULL; }
            return 0;
        }
    }
    
    if (opcode == REMOVE || opcode == REPLACE) {
        stb_json *old_item = detach_path(object, (unsigned char*)path->valuestring, case_sensitive);
        if (!old_item) return 13;
        stb_json_delete(old_item);
        if (opcode == REMOVE) return 0;
    }
    
    stb_json *value = NULL;
    if (opcode == MOVE || opcode == COPY) {
        stb_json *from = get_object_item(patch, "from", case_sensitive);
        if (!from) return 4;
        if (opcode == MOVE) value = detach_path(object, (unsigned char*)from->valuestring, case_sensitive);
        else value = get_item_from_pointer(object, from->valuestring, case_sensitive);
        if (!value) return 5;
        if (opcode == COPY) value = stb_json_duplicate(value, 1);
        if (!value) return 6;
    } else {
        value = get_object_item(patch, "value", case_sensitive);
        if (!value) return 7;
        value = stb_json_duplicate(value, 1);
        if (!value) return 8;
    }
    
    unsigned char *parent_pointer = stb_json_utils_strdup((unsigned char*)path->valuestring);
    if (!parent_pointer) { stb_json_delete(value); return 9; }
    unsigned char *child_pointer = (unsigned char*)strrchr((char*)parent_pointer, '/');
    if (!child_pointer) { stb_json_free(parent_pointer); stb_json_delete(value); return 9; }
    *child_pointer = '\0';
    child_pointer++;
    stb_json *parent = get_item_from_pointer(object, (char*)parent_pointer, case_sensitive);
    decode_pointer_inplace(child_pointer);
    
    if (!parent || !child_pointer) { stb_json_free(parent_pointer); stb_json_delete(value); return 9; }
    if (stb_json_isarray(parent)) {
        if (strcmp((char*)child_pointer, "-") == 0) {
            stb_json_additemtoarray(parent, value);
        } else {
            size_t index;
            if (!decode_array_index_from_pointer(child_pointer, &index)) { stb_json_free(parent_pointer); stb_json_delete(value); return 11; }
            if (!insert_item_in_array(parent, index, value)) { stb_json_free(parent_pointer); stb_json_delete(value); return 10; }
        }
    } else if (stb_json_isobject(parent)) {
        if (case_sensitive) stb_json_deleteitemfromobjectcasesensitive(parent, (char*)child_pointer);
        else stb_json_deleteitemfromobject(parent, (char*)child_pointer);
        stb_json_additemtoobject(parent, (char*)child_pointer, value);
    } else {
        stb_json_free(parent_pointer);
        stb_json_delete(value);
        return 9;
    }
    
    stb_json_free(parent_pointer);
    return 0;
}

int stb_json_utils_applypatches(stb_json *object, const stb_json *patches) {
    if (!stb_json_isarray(patches)) return 1;
    stb_json *current_patch = patches->child;
    int status = 0;
    while (current_patch && !status) {
        status = apply_patch(object, current_patch, false);
        current_patch = current_patch->next;
    }
    return status;
}

int stb_json_utils_applypatchescasesensitive(stb_json *object, const stb_json *patches) {
    if (!stb_json_isarray(patches)) return 1;
    stb_json *current_patch = patches->child;
    int status = 0;
    while (current_patch && !status) {
        status = apply_patch(object, current_patch, true);
        current_patch = current_patch->next;
    }
    return status;
}

static void compose_patch(stb_json *patches, const unsigned char *operation, const unsigned char *path, const unsigned char *suffix, const stb_json *value) {
    if (!patches || !operation || !path) return;
    stb_json *patch = stb_json_createobject();
    if (!patch) return;
    stb_json_additemtoobject(patch, "op", stb_json_createstring((const char*)operation));
    
    if (!suffix) {
        stb_json_additemtoobject(patch, "path", stb_json_createstring((const char*)path));
    } else {
        size_t path_length = strlen((const char*)path);
        size_t suffix_length = pointer_encoded_length(suffix);
        unsigned char *full_path = stb_json_malloc(path_length + suffix_length + 2);
        sprintf((char*)full_path, "%s/", (const char*)path);
        encode_string_as_pointer(full_path + path_length + 1, suffix);
        stb_json_additemtoobject(patch, "path", stb_json_createstring((const char*)full_path));
        stb_json_free(full_path);
    }
    
    if (value) stb_json_additemtoobject(patch, "value", stb_json_duplicate(value, 1));
    stb_json_additemtoarray(patches, patch);
}

void stb_json_utils_addpatchtoarray(stb_json *array, const char *operation, const char *path, const stb_json *value) {
    compose_patch(array, (const unsigned char*)operation, (const unsigned char*)path, NULL, value);
}

static void create_patches(stb_json *patches, const unsigned char *path, stb_json *from, stb_json *to, stb_json_bool case_sensitive) {
    if (!from || !to) return;
    if ((from->type & 0xFF) != (to->type & 0xFF)) {
        compose_patch(patches, (const unsigned char*)"replace", path, 0, to);
        return;
    }
    
    switch (from->type & 0xFF) {
        case STB_JSON_NUMBER:
            if (from->valueint != to->valueint || !compare_double(from->valuedouble, to->valuedouble)) {
                compose_patch(patches, (const unsigned char*)"replace", path, NULL, to);
            }
            return;
        case STB_JSON_STRING:
            if (strcmp(from->valuestring, to->valuestring) != 0) {
                compose_patch(patches, (const unsigned char*)"replace", path, NULL, to);
            }
            return;
        case STB_JSON_ARRAY: {
            stb_json *from_child = from->child;
            stb_json *to_child = to->child;
            size_t index = 0;
            unsigned char *new_path = stb_json_malloc(strlen((const char*)path) + 20 + 2);
            if (!new_path) return;
            
            for (index = 0; from_child && to_child; index++, from_child = from_child->next, to_child = to_child->next) {
                sprintf((char*)new_path, "%s/%lu", path, (unsigned long)index);
                create_patches(patches, new_path, from_child, to_child, case_sensitive);
            }
            for (; from_child; from_child = from_child->next, index++) {
                sprintf((char*)new_path, "%lu", (unsigned long)index);
                compose_patch(patches, (const unsigned char*)"remove", path, new_path, NULL);
            }
            for (; to_child; to_child = to_child->next) {
                compose_patch(patches, (const unsigned char*)"add", path, (const unsigned char*)"-", to_child);
            }
            stb_json_free(new_path);
            return;
        }
        case STB_JSON_OBJECT: {
            sort_object(from, case_sensitive);
            sort_object(to, case_sensitive);
            stb_json *from_child = from->child;
            stb_json *to_child = to->child;
            while (from_child || to_child) {
                int diff;
                if (!from_child) diff = 1;
                else if (!to_child) diff = -1;
                else diff = compare_strings((unsigned char*)from_child->string, (unsigned char*)to_child->string, case_sensitive);
                
                if (diff == 0) {
                    size_t path_length = strlen((const char*)path);
                    size_t from_child_name_length = pointer_encoded_length((unsigned char*)from_child->string);
                    unsigned char *new_path = stb_json_malloc(path_length + from_child_name_length + 2);
                    sprintf((char*)new_path, "%s/", path);
                    encode_string_as_pointer(new_path + path_length + 1, (unsigned char*)from_child->string);
                    create_patches(patches, new_path, from_child, to_child, case_sensitive);
                    stb_json_free(new_path);
                    from_child = from_child->next;
                    to_child = to_child->next;
                } else if (diff < 0) {
                    compose_patch(patches, (const unsigned char*)"remove", path, (unsigned char*)from_child->string, NULL);
                    from_child = from_child->next;
                } else {
                    compose_patch(patches, (const unsigned char*)"add", path, (unsigned char*)to_child->string, to_child);
                    to_child = to_child->next;
                }
            }
            return;
        }
        default: return;
    }
}

stb_json *stb_json_utils_generatepatches(stb_json *from, stb_json *to) {
    if (!from || !to) return NULL;
    stb_json *patches = stb_json_createarray();
    create_patches(patches, (const unsigned char*)"", from, to, false);
    return patches;
}

stb_json *stb_json_utils_generatepatchescasesensitive(stb_json *from, stb_json *to) {
    if (!from || !to) return NULL;
    stb_json *patches = stb_json_createarray();
    create_patches(patches, (const unsigned char*)"", from, to, true);
    return patches;
}

void stb_json_utils_sortobject(stb_json *object) {
    sort_object(object, false);
}

void stb_json_utils_sortobjectcasesensitive(stb_json *object) {
    sort_object(object, true);
}

static stb_json *merge_patch(stb_json *target, const stb_json *patch, stb_json_bool case_sensitive) {
    if (!stb_json_isobject(patch)) {
        stb_json_delete(target);
        return stb_json_duplicate(patch, 1);
    }
    if (!stb_json_isobject(target)) {
        stb_json_delete(target);
        target = stb_json_createobject();
    }
    
    stb_json *patch_child = patch->child;
    while (patch_child) {
        if (stb_json_isnull(patch_child)) {
            if (case_sensitive) stb_json_deleteitemfromobjectcasesensitive(target, patch_child->string);
            else stb_json_deleteitemfromobject(target, patch_child->string);
        } else {
            stb_json *replace_me = case_sensitive ? 
                stb_json_detachitemfromobjectcasesensitive(target, patch_child->string) :
                stb_json_detachitemfromobject(target, patch_child->string);
            stb_json *replacement = merge_patch(replace_me, patch_child, case_sensitive);
            if (!replacement) { stb_json_delete(target); return NULL; }
            stb_json_additemtoobject(target, patch_child->string, replacement);
        }
        patch_child = patch_child->next;
    }
    return target;
}

stb_json *stb_json_utils_mergepatch(stb_json *target, const stb_json *patch) {
    return merge_patch(target, patch, false);
}

stb_json *stb_json_utils_mergepatchcasesensitive(stb_json *target, const stb_json *patch) {
    return merge_patch(target, patch, true);
}

static stb_json *generate_merge_patch(stb_json *from, stb_json *to, stb_json_bool case_sensitive) {
    if (!to) return stb_json_createnull();
    if (!stb_json_isobject(to) || !stb_json_isobject(from)) return stb_json_duplicate(to, 1);
    
    sort_object(from, case_sensitive);
    sort_object(to, case_sensitive);
    stb_json *patch = stb_json_createobject();
    if (!patch) return NULL;
    
    stb_json *from_child = from->child;
    stb_json *to_child = to->child;
    while (from_child || to_child) {
        int diff;
        if (!from_child) diff = 1;
        else if (!to_child) diff = -1;
        else diff = compare_strings((unsigned char*)from_child->string, (unsigned char*)to_child->string, case_sensitive);
        
        if (diff < 0) {
            stb_json_additemtoobject(patch, from_child->string, stb_json_createnull());
            from_child = from_child->next;
        } else if (diff > 0) {
            stb_json_additemtoobject(patch, to_child->string, stb_json_duplicate(to_child, 1));
            to_child = to_child->next;
        } else {
            if (!compare_json(from_child, to_child, case_sensitive)) {
                stb_json_additemtoobject(patch, to_child->string, stb_json_utils_generatemergepatch(from_child, to_child));
            }
            from_child = from_child->next;
            to_child = to_child->next;
        }
    }
    if (!patch->child) { stb_json_delete(patch); return NULL; }
    return patch;
}

stb_json *stb_json_utils_generatemergepatch(stb_json *from, stb_json *to) {
    return generate_merge_patch(from, to, false);
}

stb_json *stb_json_utils_generatemergepatchcasesensitive(stb_json *from, stb_json *to) {
    return generate_merge_patch(from, to, true);
}

static unsigned char *print(const stb_json *item, stb_json_bool format, const internal_hooks *hooks) {
    static const size_t default_buffer_size = 256;
    printbuffer buffer = {0};
    buffer.buffer = hooks->allocate(default_buffer_size);
    if (!buffer.buffer) return NULL;
    buffer.length = default_buffer_size;
    buffer.format = format;
    buffer.hooks = *hooks;
    
    if (!print_value(item, &buffer)) goto fail;
    update_offset(&buffer);
    
    unsigned char *printed;
    printed = hooks->reallocate(buffer.buffer, buffer.offset + 1);
    if (printed) printed[buffer.offset] = '\0';
    return printed;
    
fail:
    if (buffer.buffer) hooks->deallocate(buffer.buffer);
    return NULL;
}

char *stb_json_print(const stb_json *item) {
    return (char*)print(item, true, &global_hooks);
}

char *stb_json_print_unformatted(const stb_json *item) {
    return (char*)print(item, false, &global_hooks);
}

char *stb_json_printbuffered(const stb_json *item, int prebuffer, stb_json_bool fmt) {
    if (prebuffer < 0) return NULL;
    printbuffer p = {0};
    p.buffer = global_hooks.allocate((size_t)prebuffer);
    if (!p.buffer) return NULL;
    p.length = (size_t)prebuffer;
    p.format = fmt;
    p.hooks = global_hooks;
    
    if (!print_value(item, &p)) {
        global_hooks.deallocate(p.buffer);
        return NULL;
    }
    return (char*)p.buffer;
}

stb_json_bool stb_json_printpreallocated(stb_json *item, char *buffer, const int length, const stb_json_bool format) {
    if (length < 0 || !buffer) return false;
    printbuffer p = {0};
    p.buffer = (unsigned char*)buffer;
    p.length = (size_t)length;
    p.noalloc = true;
    p.format = format;
    p.hooks = global_hooks;
    return print_value(item, &p);
}

void *stb_json_malloc(size_t size) {
    return global_hooks.allocate(size);
}

void stb_json_free(void *object) {
    global_hooks.deallocate(object);
}

/**
 * @brief Read file contents into memory
 * @param filename File to read
 * @return char* File contents (must be freed) or NULL
 */
char* stb_json_read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* content = (char*)malloc(length + 1);
    if (!content) { fclose(file); return NULL; }
    if (fread(content, 1, length, file) != (size_t)length) {
        free(content);
        fclose(file);
        return NULL;
    }
    content[length] = '\0';
    fclose(file);
    return content;
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size <= 4) return 0;
    if (data[size-1] != '\0') return 0;
    if (data[0] != '1' && data[0] != '0') return 0;
    if (data[1] != '1' && data[1] != '0') return 0;
    if (data[2] != '1' && data[2] != '0') return 0;
    if (data[3] != '1' && data[3] != '0') return 0;

    int minify = data[0] == '1';
    int require_termination = data[1] == '1';
    int formatted = data[2] == '1';
    int buffered = data[3] == '1';

    stb_json* json = stb_json_parse_withopts((const char*)data + 4, NULL, require_termination);
    if (!json) return 0;

    char* printed_json = buffered 
        ? stb_json_printbuffered(json, 1, formatted)
        : (formatted ? stb_json_print(json) : stb_json_print_unformatted(json));
    
    if (printed_json) free(printed_json);

    if (minify) {
        unsigned char* copied = (unsigned char*)malloc(size);
        if (copied) {
            memcpy(copied, data, size);
            stb_json_minify((char*)copied + 4);
            free(copied);
        }
    }

    stb_json_delete(json);
    return 0;
}

int stb_json_main(int argc, char** argv) {
    if (argc < 2) return EXIT_FAILURE;

    FILE* f = fopen(argv[1], "rb");
    if (!f) return EXIT_FAILURE;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    
    if (size <= 0) {
        fclose(f);
        return EXIT_FAILURE;
    }

    char* buf = (char*)malloc(size);
    if (!buf) {
        fclose(f);
        return EXIT_FAILURE;
    }

    if (fread(buf, 1, size, f) != (size_t)size) {
        free(buf);
        fclose(f);
        return EXIT_FAILURE;
    }
    fclose(f);

    LLVMFuzzerTestOneInput((uint8_t*)buf, size);
    free(buf);
    return EXIT_SUCCESS;
}

#endif /* STB_JSON_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* STB_JSON_H */