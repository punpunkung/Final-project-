// File: e2e_test.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ทำการ include header file ของตัวเอง
#include "e2e_test.h"

// =================================================================
// ============== 🧪 E2E TEST IMPLEMENTATION ==============
// =================================================================

// --- นิยามสีสำหรับแสดงผลใน Console ---
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define RED     "\033[31m"

/**
 * @brief Utility function to parse CSV fields, handling quoted fields.
 * (คัดลอกมาจาก projectfinal.c เพื่อให้ไฟล์นี้ทำงานได้สมบูรณ์)
 */
static char* getNextField(char** str, char* field, int size) {
    if (**str == '\0') return NULL;

    char* ptr = field;
    char* end = field + size - 1;

    if (**str == '"') {
        (*str)++;
        while (**str && ptr < end) {
            if (**str == '"') {
                (*str)++;
                if (**str != '"') {
                    (*str)++; // ข้าม comma
                    break;
                }
            }
            *ptr++ = *(*str)++;
        }
    } else {
        while (**str && **str != ',' && ptr < end) {
            *ptr++ = *(*str)++;
        }
        if (**str == ',') (*str)++;
    }
    *ptr = '\0';
    return field;
}


// --- ฟังก์ชันช่วยเหลือสำหรับการทดสอบ (Test Helpers) ---

int file_exists(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

void assert_equal_int(int expected, int actual, const char* message) {
    if (expected == actual) {
        printf(GREEN "  ✔ PASS: " RESET "%s (Expected: %d, Got: %d)\n", message, expected, actual);
    } else {
        printf(RED "  ❌ FAIL: " RESET "%s (Expected: %d, Got: %d)\n", message, expected, actual);
    }
}

void assert_equal_string(const char* expected, const char* actual, const char* message) {
    if (strcmp(expected, actual) == 0) {
        printf(GREEN "  ✔ PASS: " RESET "%s (Expected: '%s', Got: '%s')\n", message, expected, actual);
    } else {
        printf(RED "  ❌ FAIL: " RESET "%s (Expected: '%s', Got: '%s')\n", message, expected, actual);
    }
}

void add_fine_data_direct(const char* filename, const char* id, const char* name, int fine, const char* date) {
    FILE *file = fopen(filename, "a");
    if (file != NULL) {
        fprintf(file, "%s,\"%s\",%d,%s\n", id, name, fine, date);
        fclose(file);
    }
}

int load_data_for_verification(const char* filename, char ids[][10], char names[][100], int fines[], char dates[][11]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return 0;

    char line[1024];
    int count = 0;

    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 0;
    }

    while (fgets(line, sizeof(line), file) && count < 1000) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char *linePtr = line;
        char temp_id[10], temp_name[100], temp_fine[20], temp_date[11];

        if (getNextField(&linePtr, temp_id, sizeof(temp_id)) &&
            getNextField(&linePtr, temp_name, sizeof(temp_name)) &&
            getNextField(&linePtr, temp_fine, sizeof(temp_fine)) &&
            getNextField(&linePtr, temp_date, sizeof(temp_date)))
        {
            strcpy(ids[count], temp_id);
            strcpy(names[count], temp_name);
            fines[count] = atoi(temp_fine);
            strcpy(dates[count], temp_date);
            count++;
        }
    }
    fclose(file);
    return count;
}


// --- ฟังก์ชันหลักสำหรับ End-to-End Test ---

void run_e2e_test() {
    printf(BLUE "\n=== STARTING END-TO-END TEST: Complete Fine Management Workflow ===\n" RESET);
    printf("  Scenario: Add Data -> Save to File -> Read from File -> Verify Data\n\n");

    printf("  ARRANGE: Preparing test environment...\n");
    const char* test_filename = "e2e_test_fines.csv";
    remove(test_filename);

    struct FineRecord {
        char id[10];
        char name[100];
        int fine;
        char date[11];
    } test_data[] = {
        {"F001", "John Doe", 500, "2025-10-01"},
        {"F002", "Jane Smith", 1000, "2025-10-02"},
        {"F003", "Peter Jones", 750, "2025-10-03"}
    };
    int expected_record_count = 3;
    printf("  - Test file: %s\n", test_filename);
    printf("  - Records to add: %d\n", expected_record_count);

    printf("\n  ACT: Executing the end-to-end workflow...\n");
    printf("  - Step 1 & 2: Adding records and saving to CSV file...\n");
    FILE *tempFile = fopen(test_filename, "w");
    if(tempFile) {
        fprintf(tempFile, "ReceiptID,PayerName,FineAmount,PaymentDate\n");
        fclose(tempFile);
    }
    for (int i = 0; i < expected_record_count; i++) {
        add_fine_data_direct(test_filename, test_data[i].id, test_data[i].name, test_data[i].fine, test_data[i].date);
    }

    printf("  - Step 3: Simulating memory reset...\n");
    char read_ids[1000][10];
    char read_names[1000][100];
    int read_fines[1000];
    char read_dates[1000][11];

    printf("  - Step 4: Reading data back from the file...\n");
    int actual_record_count = load_data_for_verification(test_filename, read_ids, read_names, read_fines, read_dates);
    printf("    -> Read %d records successfully.\n", actual_record_count);


    printf("\n  ASSERT: Verifying the results...\n");
    assert_equal_int(1, file_exists(test_filename), "Test CSV file was created");
    assert_equal_int(expected_record_count, actual_record_count, "Record count after reading from file");

    for (int i = 0; i < expected_record_count; i++) {
        char test_message[100];
        sprintf(test_message, "Record #%d ID", i + 1);
        assert_equal_string(test_data[i].id, read_ids[i], test_message);
        sprintf(test_message, "Record #%d PayerName", i + 1);
        assert_equal_string(test_data[i].name, read_names[i], test_message);
        sprintf(test_message, "Record #%d FineAmount", i + 1);
        assert_equal_int(test_data[i].fine, read_fines[i], test_message);
        sprintf(test_message, "Record #%d PaymentDate", i + 1);
        assert_equal_string(test_data[i].date, read_dates[i], test_message);
    }
    
    remove(test_filename);
    printf(BLUE "\n=== END-TO-END TEST: COMPLETED ===\n\n" RESET);
}