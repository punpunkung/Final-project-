#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "unittests.h" // Include "สารบัญ" ของตัวเอง

// Forward declaration of functions from the main file that we need to test
void toLowerCase(char *str);
int isValidDate(const char *dateStr);
void generateNextId(char *nextId);
char* getNextField(char** str, char* field, int size);

// ================================================================
// 1. "Micro" Testing Framework
// ================================================================
int total_tests = 0;
int tests_passed = 0;

// (คัดลอก Macro ทั้งหมด ASSERT_EQUALS_STRING, ASSERT_TRUE, ASSERT_FALSE มาไว้ที่นี่)
#define ASSERT_EQUALS_STRING(expected, actual, test_name) do { \
    total_tests++; \
    if (strcmp(expected, actual) == 0) { \
        tests_passed++; \
        printf("  [PASS] %s\n", test_name); \
    } else { \
        printf("  [FAIL] %s \n\t Expected: \"%s\" \n\t Got:      \"%s\"\n", test_name, expected, actual); \
    } \
} while (0)

#define ASSERT_TRUE(condition, test_name) do { \
    total_tests++; \
    if (condition) { \
        tests_passed++; \
        printf("  [PASS] %s\n", test_name); \
    } else { \
        printf("  [FAIL] %s \n\t Expected: true (1) \n\t Got:      false (0)\n", test_name); \
    } \
} while (0)

#define ASSERT_FALSE(condition, test_name) do { \
    total_tests++; \
    if (!(condition)) { \
        tests_passed++; \
        printf("  [PASS] %s\n", test_name); \
    } else { \
        printf("  [FAIL] %s \n\t Expected: false (0) \n\t Got:      true (1)\n", test_name); \
    } \
} while (0)

// ================================================================
// 2. Test Suites (กลุ่มฟังก์ชันสำหรับเทส)
// ================================================================

// (คัดลอกฟังก์ชัน test_suite_... ทั้งหมดมาไว้ที่นี่)
void test_suite_toLowerCase() {
    printf("\n--- Testing toLowerCase() ---\n");
    char str1[] = "Hello World";
    toLowerCase(str1);
    ASSERT_EQUALS_STRING("hello world", str1, "Mixed case to lower");
    // ... tests ...
}

void test_suite_isValidDate() {
    printf("\n--- Testing isValidDate() ---\n");
    ASSERT_TRUE(isValidDate("2025-10-09"), "Valid date");
    // ... tests ...
}

void test_suite_getNextField() {
    printf("\n--- Testing getNextField() ---\n");
    char line1[] = "F001,John Doe,500,2025-10-09";
    char* linePtr1 = line1;
    char field[100];
    getNextField(&linePtr1, field, 100);
    ASSERT_EQUALS_STRING("F001", field, "Simple CSV - First field");
    // ... tests ...
}

void test_suite_generateNextId() {
    printf("\n--- Testing generateNextId() ---\n");
    char nextId[10];
    strcpy(csv_filename, "non_existent_file.csv");
    generateNextId(nextId);
    ASSERT_EQUALS_STRING("F001", nextId, "File does not exist");
    // ... tests ...
    strcpy(csv_filename, "test.csv"); // คืนค่าชื่อไฟล์กลับ
}

// ================================================================
// 3. Test Runner
// ================================================================

void run_all_tests() {
    total_tests = 0;
    tests_passed = 0;
    printf("\n======================================\n");
    printf("         RUNNING UNIT TESTS\n");
    printf("======================================\n");
    
    test_suite_toLowerCase();
    test_suite_isValidDate();
    test_suite_getNextField();
    test_suite_generateNextId();

    printf("\n--------------------------------------\n");
    printf("Test Results: %d/%d tests passed.\n", tests_passed, total_tests);
    printf("======================================\n\n");
}