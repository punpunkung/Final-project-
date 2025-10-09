#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h> 

#include "unittests.h"
#include "e2e_test.h"

char csv_filename[50] = "test.csv";

void clearInputBuffer();
void ReadCsv();
void AddFineData();
void SearchFineData(); 
void UpdateFineData(); 
void DeleteFineData(); 

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

char* getNextField(char** str, char* field, int size) {
    if (**str == '\0') return NULL;

    char* ptr = field;
    char* end = field + size - 1;

    if (**str == '"') {
        (*str)++; 
        while (**str && ptr < end) {

            if (**str == '"' && *(*str + 1) == '"') {
                *ptr++ = '"';
                *str += 2;
            }

            else if (**str == '"') {
                (*str)++; 

                if (**str == ',') (*str)++;
                break;
            }

            else {
                *ptr++ = *(*str)++;
            }
        }
    }

    else {
        while (**str && **str != ',' && ptr < end) {
            *ptr++ = *(*str)++;
        }

        if (**str == ',') (*str)++;
    }

    *ptr = '\0'; 
    return field;
}

char* trimLeadingSpaces(char* str) {
    if (str == NULL) return NULL;
    while (isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}

void ReadCsv() {
    FILE *Read = fopen(csv_filename, "r");
    if (Read == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ test.csv ได้ หรือไฟล์ยังไม่มีข้อมูล\n\n");
        return;
    }

    char line[1024];

    if (fgets(line, sizeof(line), Read) == NULL) {
        printf("ไฟล์ test.csv ว่างเปล่า ไม่มีข้อมูลที่จะแสดง\n\n");
        fclose(Read);
        return;
    }

    char receiptIds[1000][10];
    char payerNames[1000][100];
    int fineAmounts[1000];
    char paymentDates[1000][11];
    int recordCount = 0;

    while (fgets(line, sizeof(line), Read) && recordCount < 1000) {

        line[strcspn(line, "\r\n")] = 0;

        if (strlen(line) == 0) {
            continue;
        }

        char *id = trimLeadingSpaces(strtok(line, ","));
        char *name = trimLeadingSpaces(strtok(NULL, ","));
        char *fine = trimLeadingSpaces(strtok(NULL, ","));
        char *date = trimLeadingSpaces(strtok(NULL, ",")); 

        if (id && name && fine && date) {
            strcpy(receiptIds[recordCount], id);
            strcpy(payerNames[recordCount], name);
            fineAmounts[recordCount] = atoi(fine);
            strcpy(paymentDates[recordCount], date);
            recordCount++;
        }
    }
    fclose(Read);

    if (recordCount == 0) {
        printf("ไฟล์มีเพียงหัวข้อ ไม่มีข้อมูลที่จะแสดง\n\n");
        return;
    }

    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            if (strcmp(receiptIds[j], receiptIds[j + 1]) > 0) {

                char tempId[10]; strcpy(tempId, receiptIds[j]); strcpy(receiptIds[j], receiptIds[j + 1]); strcpy(receiptIds[j + 1], tempId);
                char tempName[100]; strcpy(tempName, payerNames[j]); strcpy(payerNames[j], payerNames[j + 1]); strcpy(payerNames[j + 1], tempName);
                int tempFine = fineAmounts[j]; fineAmounts[j] = fineAmounts[j + 1]; fineAmounts[j + 1] = tempFine;
                char tempDate[11]; strcpy(tempDate, paymentDates[j]); strcpy(paymentDates[j], paymentDates[j + 1]); strcpy(paymentDates[j + 1], tempDate);
            }
        }
    }

    printf("\n\n+--------------------------------------------------------------------------------+\n");
    printf("|                              ข้อมูลค่าปรับทั้งหมด                             |\n");
    printf("+------------+--------------------------------+--------------+-----------------+\n");
    printf("| ReceiptID  | PayerName                      | FineAmount   | PaymentDate     |\n");
    printf("+------------+--------------------------------+--------------+-----------------+\n");

    for (int i = 0; i < recordCount; i++) {
        printf("| %-10s | %-30s | %-12d | %-15s |\n", 
               receiptIds[i], 
               payerNames[i], 
               fineAmounts[i], 
               paymentDates[i]);
    }

    printf("+------------+--------------------------------+--------------+-----------------+\n");
    printf("| Total Records: %-60d |\n", recordCount);
    printf("+--------------------------------------------------------------------------------+\n\n");
}

int isValidDate(const char *dateStr) {
    int day, month, year;

    if (strlen(dateStr) != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
        return 0; 
    }

    if (sscanf(dateStr, "%d-%d-%d", &year, &month, &day) != 3) {
        return 0; 
    }

    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;

    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[2] = 29;
    }
    if (day < 1 || day > daysInMonth[month]) {
        return 0;
    }

    return 1; 
}

void generateNextId(char* nextId) {
    FILE *file = fopen(csv_filename, "r");
    char line[256];
    char lastId[10] = "F000";

    if (file != NULL) {
        while (fgets(line, sizeof(line), file)) {
            char *token = strtok(line, ",");
            if (token != NULL && token[0] == 'F') {
                strcpy(lastId, token);
            }
        }
        fclose(file);
    }

    int lastNum = atoi(lastId + 1);
    int nextNum = lastNum + 1;
    sprintf(nextId, "F%03d", nextNum);
}

void AddFineData() {
    int Fine;
    char FullName[100], Date[11];
    char newId[10];
    char inputBuffer[100];
    char *endptr;
    long tempFine;

    generateNextId(newId);
    printf("Generated ReceiptID: %s\n", newId);

 printf("PayerName (First Last): ");
    if (fgets(FullName, sizeof(FullName), stdin) != NULL) {

        FullName[strcspn(FullName, "\r\n")] = 0;
    }

    while (1) {
        printf("Fineamount: ");
        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) {
            printf("Error reading input.\n");
            return;
        }
        tempFine = strtol(inputBuffer, &endptr, 10);
        while (*endptr != '\0' && isspace((unsigned char)*endptr)) {
            endptr++;
        }
        if (endptr == inputBuffer || *endptr != '\0') {
            printf("Error: Please enter numbers only.\n");
        } else if (tempFine < 0) {
            printf("Error: Fine amount cannot be negative.\n");
        } else {
            Fine = (int)tempFine;
            break;
        }
    }

    while (1) {

        printf("PaymentDate (yyyy-mm-dd): ");
        scanf("%10s", Date);
        clearInputBuffer();

        if (isValidDate(Date)) {
            break; 
        } else {

            printf("Error: Invalid date or format. Please use yyyy-mm-dd format.\n");
        }
    }

    FILE *Add = fopen(csv_filename, "a");
    if (Add == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์เพื่อเขียนได้\n");
    } else {
        fprintf(Add, "%s,%s,%d,%s\n", newId, FullName, Fine, Date);
        printf("Success: เพิ่มข้อมูลเรียบร้อยแล้ว\n\n");
        fclose(Add);
    }
}

void SearchFineData() {
    int searchChoice;
    printf("\n--- ค้นหาข้อมูล ---\n");
    printf("1) ค้นหาจาก ReceiptID\n");
    printf("2) ค้นหาจาก ชื่อ-นามสกุล (PayerName)\n");
    printf("เลือกวิธีการค้นหา: ");

    if (scanf("%d", &searchChoice) != 1) {
        printf("กรุณาป้อนข้อมูลเป็นตัวเลขเท่านั้น\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    char searchTerm[100];
    char line[1024];
    int found = 0;

    FILE *Search = fopen(csv_filename, "r");
    if (Search == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ได้\n");
        return;
    }

    if (searchChoice == 1) {
        printf("กรอก ReceiptID ที่ต้องการค้นหา(FXXX): ");
        scanf("%9s", searchTerm);
        clearInputBuffer();
        printf("\n--- ผลการค้นหาสำหรับ ReceiptID: \"%s\" ---\n", searchTerm);
    } else if (searchChoice == 2) {
        printf("กรอก ชื่อ-นามสกุล ที่ต้องการค้นหา: ");
        if (fgets(searchTerm, sizeof(searchTerm), stdin) != NULL) {
            searchTerm[strcspn(searchTerm, "\r\n")] = 0;
        }
        printf("\n--- ผลการค้นหาสำหรับชื่อ: \"%s\" ---\n", searchTerm);
    } else {
        printf("ตัวเลือกไม่ถูกต้อง\n");
        fclose(Search);
        return;
    }

    printf("+------------+--------------------------------+--------------+-----------------+\n");
    printf("| ReceiptID  | PayerName                      | FineAmount   | PaymentDate     |\n");
    printf("+------------+--------------------------------+--------------+-----------------+\n");

    fgets(line, sizeof(line), Search); 

    while (fgets(line, sizeof(line), Search)) {
        line[strcspn(line, "\r\n")] = 0;
        char tempLine[1024];
        strcpy(tempLine, line);

        char *linePtr = tempLine;
        char id[10], name[100], fineStr[20], date[11];

        if (getNextField(&linePtr, id, sizeof(id)) &&
            getNextField(&linePtr, name, sizeof(name)) &&
            getNextField(&linePtr, fineStr, sizeof(fineStr)) &&
            getNextField(&linePtr, date, sizeof(date)))
        {
            int isMatch = 0;
            if (searchChoice == 1) { 

                if (strcmp(id, searchTerm) == 0) {
                    isMatch = 1;
                }
            } else { 
                char lowerNameFromFile[100];
                char lowerSearchTerm[100];
                strcpy(lowerNameFromFile, name);
                strcpy(lowerSearchTerm, searchTerm);
                toLowerCase(lowerNameFromFile);
                toLowerCase(lowerSearchTerm);

                if (strstr(lowerNameFromFile, lowerSearchTerm) != NULL) {
                    isMatch = 1;
                }
            }

            if (isMatch) {
                printf("| %-10s | %-30s | %-12d | %-15s |\n", id, name, atoi(fineStr), date);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("| %-78s |\n", "ไม่พบข้อมูลที่ตรงกัน");
    }
    printf("+------------+--------------------------------+--------------+-----------------+\n\n");

    fclose(Search);
}

void UpdateFineData() {
    char updateId[10];
    char line[1024];
    int found = 0;

    while(1) {
        printf("กรอก ReceiptID ของข้อมูลที่ต้องการอัปเดต(FXXX) (หรือพิมพ์ 'exit' เพื่อกลับเมนูหลัก): ");
        scanf("%9s", updateId);
        clearInputBuffer();

        if (strcmp(updateId, "exit") == 0) {
            printf("\n");
            return; 
        }

        FILE *originalFile = fopen(csv_filename, "r");
        if (originalFile == NULL) {
            printf("Error: ไม่สามารถเปิดไฟล์ต้นฉบับได้\n");
            return;
        }

        char oldPayerName[100], oldPaymentDate[11];
        int oldFineAmount;

        fgets(line, sizeof(line), originalFile); 

        while (fgets(line, sizeof(line), originalFile)) {
            line[strcspn(line, "\r\n")] = 0;
            char tempLine[1024];
            strcpy(tempLine, line);

            char *linePtr = tempLine;
            char id[10], name[100], fineStr[20], date[11];

            getNextField(&linePtr, id, sizeof(id));

            if (id != NULL && strcmp(id, updateId) == 0) {
                found = 1;

                getNextField(&linePtr, name, sizeof(name));
                getNextField(&linePtr, fineStr, sizeof(fineStr));
                getNextField(&linePtr, date, sizeof(date));

                strcpy(oldPayerName, name);
                oldFineAmount = atoi(fineStr);
                strcpy(oldPaymentDate, date);
                break; 
            }
        }
        fclose(originalFile);

        if (found) {

            printf("\n--- พบข้อมูลปัจจุบัน ---\n");
            printf("+------------+--------------------------------+--------------+-----------------+\n");
            printf("| ReceiptID  | PayerName                      | FineAmount   | PaymentDate     |\n");
            printf("+------------+--------------------------------+--------------+-----------------+\n");
            printf("| %-10s | %-30s | %-12d | %-15s |\n", updateId, oldPayerName, oldFineAmount, oldPaymentDate);
            printf("+------------+--------------------------------+--------------+-----------------+\n\n");

            printf("--- กรุณากรอกข้อมูลใหม่ ---\n");
            int newFine;

            char newFullName[100], newDate[11];
            char inputBuffer[100];
            char *endptr;
            long tempFine;

            printf("New PayerName (First Last): ");
            if (fgets(newFullName, sizeof(newFullName), stdin) != NULL) {
                newFullName[strcspn(newFullName, "\r\n")] = 0; 
            }

            while (1) {
                printf("New FineAmount: ");
                if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) { return; }
                tempFine = strtol(inputBuffer, &endptr, 10);
                while (*endptr != '\0' && isspace((unsigned char)*endptr)) { endptr++; }
                if (endptr == inputBuffer || *endptr != '\0' || tempFine < 0) {
                    printf("Error: Please enter a valid non-negative number.\n");
                } else {
                    newFine = (int)tempFine;
                    break;
                }
            }

            while (1) {
                printf("New PaymentDate (yyyy-mm-dd): ");
                scanf("%10s", newDate);
                clearInputBuffer();
                if (isValidDate(newDate)) { break; } 
                else { printf("Error: Invalid date or format.\n"); }
            }

            printf("\n\n--- กรุณาตรวจสอบและยืนยันการเปลี่ยนแปลง ---\n");
            printf("+-----------------+--------------------------------+--------------------------------+\n");
            printf("| Field           | Old Data                       | New Data                       |\n");
            printf("+-----------------+--------------------------------+--------------------------------+\n");
            printf("| PayerName       | %-30s | %-30s |\n", oldPayerName, newFullName);
            printf("| FineAmount      | %-30d | %-30d |\n", oldFineAmount, newFine);
            printf("| PaymentDate     | %-30s | %-30s |\n", oldPaymentDate, newDate);
            printf("+-----------------+--------------------------------+--------------------------------+\n");

            char confirm;
            printf("คุณต้องการยืนยันการอัปเดตหรือไม่? (y/n): ");
            scanf(" %c", &confirm);
            clearInputBuffer();

            if (confirm == 'y' || confirm == 'Y') {

                FILE *origFile = fopen(csv_filename, "r");
                FILE *tempFile = fopen("temp.csv", "w");

                fgets(line, sizeof(line), origFile);
                fprintf(tempFile, "%s", line);

                while (fgets(line, sizeof(line), origFile)) {
                    char tempLine[1024];
                    strcpy(tempLine, line);

                    char *p = tempLine;
                    char id_field[10];
                    getNextField(&p, id_field, sizeof(id_field));

                    if (strcmp(id_field, updateId) == 0) {

                        fprintf(tempFile, "%s,\"%s\",%d,%s\n", updateId, newFullName, newFine, newDate);
                    } else {

                        fprintf(tempFile, "%s", line);
                    }
                }
                fclose(origFile);
                fclose(tempFile);

                remove("test.csv");
                rename("temp.csv", "test.csv");
                printf("\nSuccess: อัปเดตข้อมูลเรียบร้อยแล้ว\n\n");
            } else {
                printf("\nCancel: การอัปเดตถูกยกเลิก\n\n");
            }
            break; 
        } else {
             printf("\nไม่พบข้อมูล ReceiptID: %s, กรุณาลองใหม่อีกครั้ง\n\n", updateId);
        }
    }
}

void DeleteFineData() {
    char deleteId[10];
    char line[1024];
    int found = 0;
    char confirm;

    while(1) {
        printf("กรอก ReceiptID ของข้อมูลที่ต้องการลบ(FXXX) (หรือพิมพ์ 'exit' เพื่อกลับ): ");
        scanf("%9s", deleteId);
        clearInputBuffer();

        if (strcmp(deleteId, "exit") == 0) {
            printf("\n");
            return;
        }

        FILE *originalFile = fopen(csv_filename, "r");
        if (originalFile == NULL) {
            printf("Error: ไม่สามารถเปิดไฟล์ต้นฉบับได้\n");
            return;
        }

        char payerNameToDelete[100];
        fgets(line, sizeof(line), originalFile); 

        while (fgets(line, sizeof(line), originalFile)) {
            char tempLine[1024];
            strcpy(tempLine, line);

            char *linePtr = tempLine;
            char id_field[10];
            getNextField(&linePtr, id_field, sizeof(id_field));

            if (strcmp(id_field, deleteId) == 0) {
                found = 1;

                getNextField(&linePtr, payerNameToDelete, sizeof(payerNameToDelete));
                break;
            }
        }
        fclose(originalFile);

        if (found) {
            printf("\nคุณต้องการลบข้อมูลของ '%s' (ReceiptID: %s) ใช่หรือไม่? (y/n): ", payerNameToDelete, deleteId);
            scanf(" %c", &confirm);
            clearInputBuffer();

            if (confirm == 'y' || confirm == 'Y') {

                FILE *origFile = fopen(csv_filename, "r");
                FILE *tempFile = fopen("temp.csv", "w");

                fgets(line, sizeof(line), origFile); 
                fprintf(tempFile, "%s", line); 

                while (fgets(line, sizeof(line), origFile)) {
                    char tempLine[1024];
                    strcpy(tempLine, line);

                    char *p = tempLine;
                    char id_field[10];
                    getNextField(&p, id_field, sizeof(id_field));

                    if (strcmp(id_field, deleteId) != 0) {
                        fprintf(tempFile, "%s", line);
                    }
                }
                fclose(origFile);
                fclose(tempFile);

                remove("test.csv");
                rename("temp.csv", "test.csv");
                printf("\nSuccess: ลบข้อมูลเรียบร้อยแล้ว\n\n");
            } else {
                printf("\nCancel: การลบข้อมูลถูกยกเลิก\n\n");
            }
            break; 
        } else {
            printf("\nไม่พบข้อมูล ReceiptID: %s สำหรับการลบ, กรุณาลองใหม่อีกครั้ง\n\n", deleteId);
        }
    }
}

int main() {
    int choice;
    do {
        printf("1) อ่านข้อมูลการจ่ายค่าปรับ\n");
        printf("2) เพิ่มข้อมูลการจ่ายค่าปรับ\n");
        printf("3) ค้นหาข้อมูลการจ่ายค่าปรับ\n");
        printf("4) อัพเดทข้อมูลการจ่ายค่าปรับ\n");
        printf("5) ลบข้อมูลการจ่ายค่าปรับ\n");
        printf("6) ออกจากโปรแกรม\n");
        printf("7) Run Unit Tests\n");
        printf("8) Run E2E Test\n");
        printf("เลือกดำเนินการ: ");

        if (scanf("%d", &choice) != 1) {
            printf("กรุณาป้อนข้อมูลเป็นตัวเลขเท่านั้น\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                ReadCsv();
                break; 

            case 2:
                AddFineData();
                break;

            case 3:
                SearchFineData();
                break;

            case 4:
                UpdateFineData();
                break;

            case 5:
                DeleteFineData();
                break;

            case 6:
                printf("ออกจากโปรแกรม\n");
                break;

            case 7:
                run_all_tests();
                break;
            case 8:
                run_e2e_test();
                break;

            default: 
                printf("ตัวเลือกไม่ถูกต้อง กรุณาลองใหม่อีกครั้ง\n");
                break;
        }

    } while (choice != 6);

    return 0;
}