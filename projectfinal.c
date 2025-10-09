#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h> // เพิ่มเข้ามาเพื่อใช้ฟังก์ชันเกี่ยวกับสตริง เช่น strstr, strcmp

// --- Function Prototypes ---
void clearInputBuffer();
void ReadCsv();
void AddFineData();
void SearchFineData(); // ฟังก์ชันค้นหาข้อมูล
void UpdateFineData(); // ฟังก์ชันอัปเดตข้อมูล
void DeleteFineData(); // ฟังก์ชันลบข้อมูล

// ---------------------------
// --- ฟังก์ชันเสริมสำหรับแปลงสตริงเป็นตัวพิมพ์เล็ก ---
void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
// --- ฟังก์ชันเสริมสำหรับตัดช่องว่างนำหน้า ---
char* trimLeadingSpaces(char* str) {
    if (str == NULL) return NULL;
    while (isspace((unsigned char)*str)) {
        str++;
    }
    return str;
}
// ============== 🧾 ฟังก์ชันอ่านข้อมูล (Choice 1) - แก้ไขปัญหาการแสดงผล ==============
void ReadCsv() {
    FILE *Read = fopen("test.csv", "r");
    if (Read == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ test.csv ได้ หรือไฟล์ยังไม่มีข้อมูล\n\n");
        return;
    }

    char line[1024];

    // อ่านและข้ามบรรทัดแรก (Header)
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

    // อ่านข้อมูลจริงจากไฟล์
    while (fgets(line, sizeof(line), Read) && recordCount < 1000) {
        // --- ส่วนที่แก้ไขที่ 1: ลบทั้ง \r และ \n ที่ท้ายบรรทัด ---
        line[strcspn(line, "\r\n")] = 0;

        // ข้ามบรรทัดว่างที่อาจมีในไฟล์
        if (strlen(line) == 0) {
            continue;
        }

        // --- ส่วนที่แก้ไขที่ 2: ปรับปรุงการแยกข้อมูล ---
        char *id = trimLeadingSpaces(strtok(line, ","));
        char *name = trimLeadingSpaces(strtok(NULL, ","));
        char *fine = trimLeadingSpaces(strtok(NULL, ","));
        char *date = trimLeadingSpaces(strtok(NULL, ",")); // อ่านข้อมูลส่วนที่ 4

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

    // จัดเรียงข้อมูล (Sorting) โดยใช้ Bubble Sort (เหมือนเดิม)
    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            if (strcmp(receiptIds[j], receiptIds[j + 1]) > 0) {
                // สลับข้อมูลในทุกๆ Array
                char tempId[10]; strcpy(tempId, receiptIds[j]); strcpy(receiptIds[j], receiptIds[j + 1]); strcpy(receiptIds[j + 1], tempId);
                char tempName[100]; strcpy(tempName, payerNames[j]); strcpy(payerNames[j], payerNames[j + 1]); strcpy(payerNames[j + 1], tempName);
                int tempFine = fineAmounts[j]; fineAmounts[j] = fineAmounts[j + 1]; fineAmounts[j + 1] = tempFine;
                char tempDate[11]; strcpy(tempDate, paymentDates[j]); strcpy(paymentDates[j], paymentDates[j + 1]); strcpy(paymentDates[j + 1], tempDate);
            }
        }
    }

    // แสดงผลข้อมูลที่เรียงแล้วในรูปแบบตาราง
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

    // --- ส่วนที่แก้ไข ---
    // 1. ตรวจสอบรูปแบบ yyyy-mm-dd และความยาว
    if (strlen(dateStr) != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
        return 0; // รูปแบบไม่ถูกต้อง
    }

    // 2. ลองแปลงค่าเป็นตัวเลข (ใช้ sscanf กับ format ใหม่)
    if (sscanf(dateStr, "%d-%d-%d", &year, &month, &day) != 3) {
        return 0; // มีตัวอักษรที่ไม่ใช่ตัวเลข
    }
    // --- สิ้นสุดส่วนที่แก้ไข ---

    // 3. ตรวจสอบขอบเขตของปีและเดือน (เหมือนเดิม)
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;

    // 4. ตรวจสอบวันในแต่ละเดือน (รวมปีอธิกสุรทิน) (เหมือนเดิม)
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[2] = 29;
    }
    if (day < 1 || day > daysInMonth[month]) {
        return 0;
    }

    return 1; // วันที่ถูกต้อง
}
// ----------------------------------------------------


void generateNextId(char* nextId) {
    FILE *file = fopen("test.csv", "r");
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
    char FirstName[50], LastName[50], Date[11];
    char newId[10];
    char inputBuffer[100];
    char *endptr;
    long tempFine;

    generateNextId(newId);
    printf("Generated ReceiptID: %s\n", newId);

    printf("PayerName (First Last): ");
    scanf("%49s %49s", FirstName, LastName);
    clearInputBuffer();

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

    // --- ส่วนที่แก้ไข: ลูปสำหรับรับและตรวจสอบวันที่ ---
    while (1) {
        // แก้ไขข้อความแนะนำให้ผู้ใช้
        printf("PaymentDate (yyyy-mm-dd): ");
        scanf("%10s", Date);
        clearInputBuffer();

        if (isValidDate(Date)) {
            break; // วันที่ถูกต้อง ออกจากลูป
        } else {
            // แก้ไขข้อความ Error
            printf("Error: Invalid date or format. Please use yyyy-mm-dd format.\n");
        }
    }
    // ---------------------------------------------

    FILE *Add = fopen("test.csv", "a");
    if (Add == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์เพื่อเขียนได้\n");
    } else {
        fprintf(Add, "%s,%s %s,%d,%s\n", newId, FirstName, LastName, Fine, Date);
        printf("Success: เพิ่มข้อมูลเรียบร้อยแล้ว\n\n");
        fclose(Add);
    }
}

// ============== 🔎 ฟังก์ชันค้นหาข้อมูล (Choice 3) - ปรับปรุงการแสดงผล ==============
void SearchFineData() {
    int choice;
    printf("\n---------- Search Menu ----------\n");
    printf("1) ค้นหาด้วย ReceiptID\n");
    printf("2) ค้นหาด้วยชื่อผู้จ่าย (Payer Name)\n");
    printf("3) กลับเมนูหลัก\n");
    printf("---------------------------------\n");
    printf("เลือกวิธีการค้นหา: ");

    if (scanf("%d", &choice) != 1) {
        printf("Error: กรุณาป้อนเป็นตัวเลขเท่านั้น\n\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    char line[1024];
    int foundCount = 0;

    // --- ค้นหาด้วย ReceiptID ---
    if (choice == 1) {
        char searchId[10];
        printf("กรอก ReceiptID ที่ต้องการค้นหา: ");
        scanf("%9s", searchId);
        clearInputBuffer();

        FILE *file = fopen("test.csv", "r");
        if (file == NULL) { printf("Error: ไม่สามารถเปิดไฟล์ได้\n"); return; }
        
        // ข้าม Header
        fgets(line, sizeof(line), file);

        // --- ส่วนที่แก้ไข: แสดงผลเป็นตาราง ---
        printf("\n\n+--------------------------------------------------------------------------------+\n");
        printf("|                        ผลการค้นหาสำหรับ ReceiptID: %-10s                     |\n", searchId);
        printf("+------------+--------------------------------+--------------+-----------------+\n");
        printf("| ReceiptID  | PayerName                      | FineAmount   | PaymentDate     |\n");
        printf("+------------+--------------------------------+--------------+-----------------+\n");

        while (fgets(line, sizeof(line), file)) {
            char tempLine[1024];
            strcpy(tempLine, line);
            line[strcspn(line, "\r\n")] = 0;

            char *id = strtok(tempLine, ",");
            if (id != NULL && strcmp(id, searchId) == 0) {
                char *name = strtok(NULL, ",");
                char *fine = strtok(NULL, ",");
                char *date = strtok(NULL, ",");
                printf("| %-10s | %-30s | %-12d | %-15s |\n", id, name, atoi(fine), date);
                foundCount++;
            }
        }
        fclose(file);

        if (foundCount == 0) {
            printf("| %-78s |\n", "ไม่พบข้อมูล");
        }
        
        printf("+------------+--------------------------------+--------------+-----------------+\n");
        printf("| Total Found: %-63d |\n", foundCount);
        printf("+--------------------------------------------------------------------------------+\n\n");

    }
    // --- ค้นหาด้วยชื่อ ---
    else if (choice == 2) {
        char searchQuery[100];
        printf("กรอกชื่อบางส่วนที่ต้องการค้นหา: ");
        if (fgets(searchQuery, sizeof(searchQuery), stdin) == NULL) return;
        searchQuery[strcspn(searchQuery, "\r\n")] = 0;

        FILE *file = fopen("test.csv", "r");
        if (file == NULL) { printf("Error: ไม่สามารถเปิดไฟล์ได้\n"); return; }
        
        // ข้าม Header
        fgets(line, sizeof(line), file);
        
        char lowerSearchQuery[100];
        strcpy(lowerSearchQuery, searchQuery);
        toLowerCase(lowerSearchQuery);

        // --- ส่วนที่แก้ไข: แสดงผลเป็นตาราง ---
        printf("\n\n+--------------------------------------------------------------------------------+\n");
        printf("|                     ผลการค้นหาสำหรับชื่อ: '%-20s'                      |\n", searchQuery);
        printf("+------------+--------------------------------+--------------+-----------------+\n");
        printf("| ReceiptID  | PayerName                      | FineAmount   | PaymentDate     |\n");
        printf("+------------+--------------------------------+--------------+-----------------+\n");
        
        while (fgets(line, sizeof(line), file)) {
            char originalLine[1024];
            strcpy(originalLine, line); // เก็บข้อมูลบรรทัดเดิมไว้แสดงผล
            originalLine[strcspn(originalLine, "\r\n")] = 0;

            char *id_token = strtok(line, ",");
            char *name_token = strtok(NULL, ",");

            if (name_token != NULL) {
                char lowerNameFromFile[100];
                strcpy(lowerNameFromFile, name_token);
                toLowerCase(lowerNameFromFile);

                if (strstr(lowerNameFromFile, lowerSearchQuery) != NULL) {
                    char *id_p = strtok(originalLine, ",");
                    char *name_p = strtok(NULL, ",");
                    char *fine_p = strtok(NULL, ",");
                    char *date_p = strtok(NULL, ",");
                    printf("| %-10s | %-30s | %-12d | %-15s |\n", id_p, name_p, atoi(fine_p), date_p);
                    foundCount++;
                }
            }
        }
        fclose(file);
        
        if (foundCount == 0) {
            printf("| %-78s |\n", "ไม่พบข้อมูล");
        }

        printf("+------------+--------------------------------+--------------+-----------------+\n");
        printf("| Total Found: %-63d |\n", foundCount);
        printf("+--------------------------------------------------------------------------------+\n\n");
    }
    else if (choice == 3) {
        printf("\n");
        return;
    }
    else {
        printf("Error: ตัวเลือกไม่ถูกต้อง\n\n");
    }
}

// ============== 📝 ฟังก์ชันอัปเดตข้อมูล (Choice 4) - ปรับปรุงใหม่ทั้งหมด ==============
void UpdateFineData() {
    char updateId[10];
    char line[1024];
    int found = 0;

    // Loop สำหรับรับ ReceiptID จนกว่าจะเจอ หรือผู้ใช้ยกเลิก
    while(1) {
        printf("กรอก ReceiptID ของข้อมูลที่ต้องการอัปเดต (หรือพิมพ์ 'exit' เพื่อกลับเมนูหลัก): ");
        scanf("%9s", updateId);
        clearInputBuffer();
        
        if (strcmp(updateId, "exit") == 0) {
            printf("\n");
            return; // กลับเมนูหลัก
        }

        FILE *originalFile = fopen("test.csv", "r");
        if (originalFile == NULL) {
            printf("Error: ไม่สามารถเปิดไฟล์ต้นฉบับได้\n");
            return;
        }

        //--- ค้นหาข้อมูลและเก็บข้อมูลเก่าไว้ ---
        char oldPayerName[100], oldPaymentDate[11];
        int oldFineAmount;
        
        fgets(line, sizeof(line), originalFile); // ข้าม Header

        while (fgets(line, sizeof(line), originalFile)) {
            line[strcspn(line, "\r\n")] = 0;
            char tempLine[1024];
            strcpy(tempLine, line);
            char *id = strtok(tempLine, ",");
            if (id != NULL && strcmp(id, updateId) == 0) {
                found = 1;
                strcpy(oldPayerName, strtok(NULL, ","));
                oldFineAmount = atoi(strtok(NULL, ","));
                strcpy(oldPaymentDate, strtok(NULL, ","));
                break; // เจอแล้ว ออกจาก loop ค้นหา
            }
        }
        fclose(originalFile);

        if (found) {
            // --- 1. แสดงข้อมูลปัจจุบันที่พบ ---
            printf("\n--- พบข้อมูลปัจจุบัน ---\n");
            printf("+------------+--------------------------------+--------------+-----------------+\n");
            printf("| ReceiptID  | PayerName                      | FineAmount   | PaymentDate     |\n");
            printf("+------------+--------------------------------+--------------+-----------------+\n");
            printf("| %-10s | %-30s | %-12d | %-15s |\n", updateId, oldPayerName, oldFineAmount, oldPaymentDate);
            printf("+------------+--------------------------------+--------------+-----------------+\n\n");

            // --- 2. รับข้อมูลใหม่จากผู้ใช้ ---
            printf("--- กรุณากรอกข้อมูลใหม่ ---\n");
            int newFine;
            char newFirstName[50], newLastName[50], newDate[11], newFullName[100];
            char inputBuffer[100];
            char *endptr;
            long tempFine;

            printf("New PayerName (First Last): ");
            scanf("%49s %49s", newFirstName, newLastName);
            sprintf(newFullName, "%s %s", newFirstName, newLastName); // รวมชื่อ-นามสกุล
            clearInputBuffer();

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

            // --- 3. แสดงตารางเปรียบเทียบข้อมูล ---
            printf("\n\n--- กรุณาตรวจสอบและยืนยันการเปลี่ยนแปลง ---\n");
            printf("+-----------------+--------------------------------+--------------------------------+\n");
            printf("| Field           | Old Data                       | New Data                       |\n");
            printf("+-----------------+--------------------------------+--------------------------------+\n");
            printf("| PayerName       | %-30s | %-30s |\n", oldPayerName, newFullName);
            printf("| FineAmount      | %-30d | %-30d |\n", oldFineAmount, newFine);
            printf("| PaymentDate     | %-30s | %-30s |\n", oldPaymentDate, newDate);
            printf("+-----------------+--------------------------------+--------------------------------+\n");

            // --- 4. ขอการยืนยัน ---
            char confirm;
            printf("คุณต้องการยืนยันการอัปเดตหรือไม่? (y/n): ");
            scanf(" %c", &confirm);
            clearInputBuffer();

            if (confirm == 'y' || confirm == 'Y') {
                // --- 5. เริ่มกระบวนการเขียนไฟล์ ---
                FILE *origFile = fopen("test.csv", "r");
                FILE *tempFile = fopen("temp.csv", "w");

                // คัดลอก Header ไปยังไฟล์ temp
                fgets(line, sizeof(line), origFile);
                fprintf(tempFile, "%s", line);

                // เขียนข้อมูลทีละบรรทัด
                while (fgets(line, sizeof(line), origFile)) {
                    char tempLine[1024];
                    strcpy(tempLine, line);
                    char *id = strtok(tempLine, ",");
                    if (id != NULL && strcmp(id, updateId) == 0) {
                        // เขียนข้อมูลใหม่สำหรับ ID ที่ตรงกัน
                        fprintf(tempFile, "%s,%s,%d,%s\n", updateId, newFullName, newFine, newDate);
                    } else {
                        // คัดลอกข้อมูลเดิมที่ไม่เปลี่ยนแปลง
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
            break; // ออกจาก loop while(1) หลัก
        } else {
             printf("\nไม่พบข้อมูล ReceiptID: %s, กรุณาลองใหม่อีกครั้ง\n\n", updateId);
        }
    }
}

// ============== 🗑️ ฟังก์ชันลบข้อมูล (Choice 5) ==============
void DeleteFineData() {
    char deleteId[10];
    char line[1024];
    int found = 0;

    // --- ส่วนที่แก้ไข: เพิ่ม Loop ---
    while(1) {
        printf("กรอก ReceiptID ของข้อมูลที่ต้องการลบ (หรือพิมพ์ 'exit' เพื่อกลับเมนูหลัก): ");
        scanf("%9s", deleteId);
        clearInputBuffer();

        if (strcmp(deleteId, "exit") == 0) {
            printf("\n");
            return;
        }

        FILE *originalFile = fopen("test.csv", "r");
        if (originalFile == NULL) {
            printf("Error: ไม่สามารถเปิดไฟล์ต้นฉบับได้\n");
            return;
        }

        // ตรวจสอบว่ามี ID นี้ในไฟล์หรือไม่ก่อน
        found = 0;
        while (fgets(line, sizeof(line), originalFile)) {
            char tempLine[1024];
            strcpy(tempLine, line);
            char *id = strtok(tempLine, ",");
            if (id != NULL && strcmp(id, deleteId) == 0) {
                found = 1;
                break;
            }
        }
        fclose(originalFile);

        if (found) {
            // ถ้าเจอ ID ค่อยเริ่มกระบวนการลบ
            FILE *origFile = fopen("test.csv", "r");
            FILE *tempFile = fopen("temp.csv", "w");

            if (origFile == NULL || tempFile == NULL) {
                printf("Error: เกิดข้อผิดพลาดในการเปิดไฟล์\n");
                if (origFile) fclose(origFile);
                if (tempFile) fclose(tempFile);
                return;
            }

            while (fgets(line, sizeof(line), origFile)) {
                char tempLine[1024];
                strcpy(tempLine, line);
                char *id = strtok(tempLine, ",");
                if (id == NULL || strcmp(id, deleteId) != 0) {
                    fprintf(tempFile, "%s", line);
                }
            }

            fclose(origFile);
            fclose(tempFile);

            remove("test.csv");
            rename("temp.csv", "test.csv");
            
            printf("Success: ลบข้อมูล ReceiptID: %s เรียบร้อยแล้ว\n\n", deleteId);
            break; // ออกจาก loop while(1)
        } else {
            printf("ไม่พบข้อมูล ReceiptID: %s เพื่อทำการลบ, กรุณาลองใหม่อีกครั้ง\n\n", deleteId);
        }
    }
}


// ============== Main Program ==============
int main() {
    int choice;
    do {
        printf("1) อ่านข้อมูลการจ่ายค่าปรับ\n");
        printf("2) เพิ่มข้อมูลการจ่ายค่าปรับ\n");
        printf("3) ค้นหาข้อมูลการจ่ายค่าปรับ\n");
        printf("4) อัพเดทข้อมูลการจ่ายค่าปรับ\n");
        printf("5) ลบข้อมูลการจ่ายค่าปรับ\n");
        printf("6) ออกจากโปรแกรม\n");
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
                break; // อย่าลืม break; เพื่อออกจาก switch

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

            default: // ทำงานเมื่อไม่มี case ไหนตรงกับเงื่อนไข (เหมือนกับ else ตัวสุดท้าย)
                printf("ตัวเลือกไม่ถูกต้อง กรุณาลองใหม่อีกครั้ง\n");
                break;
        }

    } while (choice != 6);
    
    return 0;
}
