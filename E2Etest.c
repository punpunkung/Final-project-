#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- ฟังก์ชันสำหรับแสดงผลสี ---
void print_pass(const char* message) {
    printf("\033[0;32m[PASS]\033[0m %s\n", message);
}
void print_fail(const char* message) {
    printf("\033[0;31m[FAIL]\033[0m %s\n", message);
}
void print_info(const char* message) {
    printf("\033[0;34m[INFO]\033[0m %s\n", message);
}

// --- ฟังก์ชันสำหรับคัดลอกไฟล์ (เหมือน cp) ---
int copy_file(const char* src_path, const char* dest_path) {
    FILE *src = fopen(src_path, "r");
    if (src == NULL) return 0;
    FILE *dest = fopen(dest_path, "w");
    if (dest == NULL) {
        fclose(src);
        return 0;
    }

    char ch;
    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);
    return 1;
}

// --- ฟังก์ชันสำหรับเปรียบเทียบไฟล์ (เหมือน diff) ---
int compare_files(const char* file1_path, const char* file2_path) {
    FILE *f1 = fopen(file1_path, "r");
    FILE *f2 = fopen(file2_path, "r");
    if (f1 == NULL || f2 == NULL) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return -1; // Error opening files
    }

    char line1[1024], line2[1024];
    int line_num = 1;
    int are_equal = 1;

    while (fgets(line1, sizeof(line1), f1) && fgets(line2, sizeof(line2), f2)) {
        // ลบ \r \n ท้ายบรรทัดเพื่อให้เปรียบเทียบได้แม่นยำ
        line1[strcspn(line1, "\r\n")] = 0;
        line2[strcspn(line2, "\r\n")] = 0;

        if (strcmp(line1, line2) != 0) {
            printf("       Mismatch found at line %d:\n", line_num);
            printf("       > Expected: \"%s\"\n", line1);
            printf("       > Actual:   \"%s\"\n", line2);
            are_equal = 0;
            break;
        }
        line_num++;
    }

    // เช็คว่าไฟล์ไหนยาวกว่ากัน
    if (are_equal && (fgets(line1, sizeof(line1), f1) || fgets(line2, sizeof(line2), f2))) {
        print_fail("Files have different lengths.");
        are_equal = 0;
    }

    fclose(f1);
    fclose(f2);
    return are_equal;
}


int main() {
    const char* program_name = "./my_program";
    const char* csv_file = "test.csv";
    const char* fixture_file = "test_fixture.csv";
    const char* commands_file = "test_commands.txt";
    const char* expected_output_file = "expected_output.txt";
    const char* actual_output_file = "actual_output.txt";

    print_info("Starting E2E Test written in C...");

    // 1. Setup
    print_info("Setting up the test environment...");
    if (!copy_file(fixture_file, csv_file)) {
        print_fail("Failed to copy fixture file.");
        return 1;
    }
    print_pass("Test environment is ready.");

    // 2. Execute
    print_info("Running the program with test commands...");
    char command[512];
    sprintf(command, "%s < %s > %s", program_name, commands_file, actual_output_file);
    system(command);
    print_pass("Program execution finished.");

    // 3. Verify
    print_info("Comparing actual output with expected output...");
    int result = compare_files(expected_output_file, actual_output_file);

    if (result == 1) {
        print_pass("E2E Test Successful! The output matches the expectation.");
        // 4. Cleanup
        remove(actual_output_file);
    } else {
        print_fail("E2E Test Failed! The output does not match.");
    }
    
    printf("\n");
    return 0;
}