#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define FILENAME "test.csv"
#define TEMP_FILENAME "temp.csv"
#define MAX_RECORDS 1000
#define MAX_LINE_LENGTH 1024

// --- สร้าง Struct เพื่อจัดการข้อมูลให้ง่ายขึ้น ---
typedef struct {
    char id[50];
    char payerName[100];
    int fineAmount;
    char paymentDate[20];
} FineData;

// --- Function Prototypes ---
// Helper Functions
void clearInputBuffer();
char* trimWhitespace(char* str);
int getIntegerInput(const char* prompt);
void toLowerCase(char *str);

// Core Functions
void readAndDisplayFines();
void addFineData();
void searchFineData();
void updateFineData();
void deleteFineData();

// Sorting Functions (for qsort)
int compareByID(const void* a, const void* b);
int compareByName(const void* a, const void* b);
int compareByFine(const void* a, const void* b);
int compareByDate(const void* a, const void* b);

// ======================= Main Program =======================
int main() {
    int choice;
    do {
        printf("\n======= Fine Management System =======\n");
        printf("1) อ่านข้อมูลค่าปรับทั้งหมด (และเรียงลำดับ)\n");
        printf("2) เพิ่มข้อมูลค่าปรับ\n");
        printf("3) ค้นหาข้อมูลค่าปรับ\n");
        printf("4) อัพเดทข้อมูลค่าปรับ\n");
        printf("5) ลบข้อมูลค่าปรับ\n");
        printf("6) ออกจากโปรแกรม\n");
        printf("======================================\n");

        choice = getIntegerInput("เลือกดำเนินการ: ");

        switch (choice) {
            case 1: readAndDisplayFines(); break;
            case 2: addFineData(); break;
            case 3: searchFineData(); break;
            case 4: updateFineData(); break;
            case 5: deleteFineData(); break;
            case 6: printf("กำลังออกจากโปรแกรม...\n"); break;
            default: printf("ตัวเลือกไม่ถูกต้อง กรุณาลองใหม่อีกครั้ง\n"); break;
        }
    } while (choice != 6);

    return 0;
}

// ======================= Helper Functions =======================

// ฟังก์ชันล้าง Input Buffer ที่ปลอดภัย
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ฟังก์ชันตัดช่องว่าง (Whitespace) หน้า-หลังสตริง
char* trimWhitespace(char* str) {
    if (!str) return NULL;
    char *end;
    // Trim leading space
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

// ฟังก์ชันรับ Input ที่เป็นตัวเลขเท่านั้น (ป้องกันโปรแกรมแครช)
int getIntegerInput(const char* prompt) {
    char buffer[100];
    int value;
    printf("%s", prompt);
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            return -1; // End of file
        }
        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        } else {
            printf("กรุณาป้อนข้อมูลเป็นตัวเลขเท่านั้น: ");
        }
    }
}

// ฟังก์ชันแปลงสตริงเป็นตัวพิมพ์เล็ก
void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// ======================= Core Functions =======================

// --- 🧾 ฟังก์ชันอ่านข้อมูล (Choice 1) ---
// ปรับปรุง: ใช้ Struct, เพิ่มการเรียงข้อมูลด้วย qsort
void readAndDisplayFines() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ %s ได้\n\n", FILENAME);
        return;
    }

    FineData records[MAX_RECORDS];
    int count = 0;
    char line[MAX_LINE_LENGTH];

    fgets(line, sizeof(line), file); // ข้าม Header

    while (count < MAX_RECORDS && fgets(line, sizeof(line), file)) {
        char *id = trimWhitespace(strtok(line, ","));
        char *name = trimWhitespace(strtok(NULL, ","));
        char *fine = trimWhitespace(strtok(NULL, ","));
        char *date = trimWhitespace(strtok(NULL, "\r\n"));

        if (id && name && fine && date) {
            strcpy(records[count].id, id);
            strcpy(records[count].payerName, name);
            records[count].fineAmount = atoi(fine);
            strcpy(records[count].paymentDate, date);
            count++;
        }
    }
    fclose(file);

    if (count == 0) {
        printf("ไม่พบข้อมูลค่าปรับในไฟล์\n");
        return;
    }

    int sortChoice = 0;
    printf("\n--- เลือกรูปแบบการเรียงข้อมูล ---\n");
    printf("1) เรียงตาม ReceiptID (ค่าเริ่มต้น)\n");
    printf("2) เรียงตาม PayerName\n");
    printf("3) เรียงตาม FineAmount\n");
    printf("4) เรียงตาม PaymentDate\n");
    sortChoice = getIntegerInput("เลือก: ");

    switch (sortChoice) {
        case 2: qsort(records, count, sizeof(FineData), compareByName); break;
        case 3: qsort(records, count, sizeof(FineData), compareByFine); break;
        case 4: qsort(records, count, sizeof(FineData), compareByDate); break;
        default: qsort(records, count, sizeof(FineData), compareByID); break;
    }

    printf("\n+--------------------------------------------------------------------------------+\n");
    printf("|                              ข้อมูลค่าปรับทั้งหมด                               |\n");
    printf("+------------+--------------------------------+--------------+-----------------+\n");
    printf("| ReceiptID  | PayerName                      | FineAmount   | PaymentDate     |\n");
    printf("+------------+--------------------------------+--------------+-----------------+\n");
    for (int i = 0; i < count; i++) {
        printf("| %-10s | %-30s | %-12d | %-15s |\n", records[i].id, records[i].payerName, records[i].fineAmount, records[i].paymentDate);
    }
    printf("+------------+--------------------------------+--------------+-----------------+\n");
    printf("| Total Records: %-64d |\n", count);
    printf("+--------------------------------------------------------------------------------+\n");
}

// ฟังก์ชันหา ID ล่าสุดและสร้าง ID ใหม่ (ป้องกัน ID ซ้ำ)
void generateNextId(char* nextId) {
    FILE* file = fopen(FILENAME, "r");
    char line[MAX_LINE_LENGTH];
    int maxIdNum = 0;

    if (file != NULL) {
        fgets(line, sizeof(line), file); // ข้าม Header
        while (fgets(line, sizeof(line), file)) {
            char* token = strtok(line, ",");
            if (token != NULL) {
                int currentIdNum = atoi(token + 1); // +1 เพื่อข้ามตัว 'F'
                if (currentIdNum > maxIdNum) {
                    maxIdNum = currentIdNum;
                }
            }
        }
        fclose(file);
    }
    sprintf(nextId, "F%03d", maxIdNum + 1);
}

// --- ➕ ฟังก์ชันเพิ่มข้อมูล (Choice 2) ---
// ปรับปรุง: ใช้ fgets รับชื่อ, สร้าง ID ที่ไม่ซ้ำซ้อน
void addFineData() {
    FILE* file = fopen(FILENAME, "a");
    if (file == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ %s เพื่อเขียนได้\n", FILENAME);
        return;
    }

    char newId[50];
    char payerName[100];
    int fineAmount;
    char paymentDate[20];
    char buffer[256];

    generateNextId(newId);
    printf("ReceiptID ใหม่คือ: %s\n", newId);

    printf("PayerName (First Last): ");
    fgets(payerName, sizeof(payerName), stdin);
    strcpy(payerName, trimWhitespace(payerName));

    // รับและตรวจสอบ Fine Amount
    while (1) {
        fineAmount = getIntegerInput("FineAmount: ");
        if (fineAmount > 0) break;
        printf("ค่าปรับต้องเป็นตัวเลขที่มากกว่า 0\n");
    }

    // รับและตรวจสอบ Payment Date
    while (1) {
        printf("PaymentDate (YYYY-MM-DD): ");
        fgets(paymentDate, sizeof(paymentDate), stdin);
        strcpy(paymentDate, trimWhitespace(paymentDate));
        if (strlen(paymentDate) == 10 && paymentDate[4] == '-' && paymentDate[7] == '-') break;
        printf("รูปแบบวันที่ไม่ถูกต้อง กรุณาใช้ YYYY-MM-DD\n");
    }

    fprintf(file, "%s,%s,%d,%s\n", newId, payerName, fineAmount, paymentDate);
    fclose(file);
    printf("เพิ่มข้อมูลค่าปรับเรียบร้อยแล้ว!\n");
}

// --- 🔍 ฟังก์ชันค้นหา (Choice 3) ---
// ปรับปรุง: ใช้ toLowerCase เพื่อให้ค้นหาแบบ case-insensitive
void searchFineData() {
    char searchTerm[100];
    printf("ป้อน ReceiptID หรือ PayerName เพื่อค้นหา: ");
    fgets(searchTerm, sizeof(searchTerm), stdin);
    strcpy(searchTerm, trimWhitespace(searchTerm));
    toLowerCase(searchTerm);

    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ %s ได้\n", FILENAME);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int found = 0;
    fgets(line, sizeof(line), file); // ข้าม Header

    printf("\n--- ผลการค้นหา ---\n");
    printf("+------------+--------------------------------+--------------+-----------------+\n");
    printf("| ReceiptID  | PayerName                      | FineAmount   | PaymentDate     |\n");
    printf("+------------+--------------------------------+--------------+-----------------+\n");

    while (fgets(line, sizeof(line), file)) {
        char originalLine[MAX_LINE_LENGTH];
        strcpy(originalLine, line);

        char *id = strtok(line, ",");
        char *name = strtok(NULL, ",");
        
        char tempId[100], tempName[100];
        strcpy(tempId, id);
        strcpy(tempName, name);
        toLowerCase(tempId);
        toLowerCase(tempName);

        if (strstr(tempId, searchTerm) || strstr(tempName, searchTerm)) {
            char *fine = strtok(NULL, ",");
            char *date = strtok(NULL, "\r\n");
            printf("| %-10s | %-30s | %-12s | %-15s |\n", trimWhitespace(id), trimWhitespace(name), trimWhitespace(fine), trimWhitespace(date));
            found = 1;
        }
    }
    printf("+------------+--------------------------------+--------------+-----------------+\n");

    if (!found) {
        printf("ไม่พบข้อมูลที่ตรงกับ '%s'\n", searchTerm);
    }
    fclose(file);
}

// --- 🔄 ฟังก์ชันอัปเดต (Choice 4) ---
// ปรับปรุง: แสดงข้อมูลเก่าก่อนแก้ไข และอนุญาตให้ใช้ค่าเดิมได้โดยการกด Enter
void updateFineData() {
    char updateId[50];
    printf("ป้อน ReceiptID ที่ต้องการอัปเดต: ");
    fgets(updateId, sizeof(updateId), stdin);
    strcpy(updateId, trimWhitespace(updateId));

    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ %s ได้\n", FILENAME);
        return;
    }

    FILE* tempFile = fopen(TEMP_FILENAME, "w");
    if (tempFile == NULL) {
        printf("Error: ไม่สามารถสร้างไฟล์ชั่วคราวได้\n");
        fclose(file);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int found = 0;

    // เขียน Header ลงในไฟล์ temp
    if (fgets(line, sizeof(line), file)) {
        fprintf(tempFile, "%s", line);
    }

    while (fgets(line, sizeof(line), file)) {
        char originalLine[MAX_LINE_LENGTH];
        strcpy(originalLine, line); // เก็บสำเนาบรรทัดเดิมไว้

        char parseLine[MAX_LINE_LENGTH];
        strcpy(parseLine, line); // สร้างสำเนาสำหรับแยกข้อมูล

        char* id = strtok(line, ",");

        if (strcmp(trimWhitespace(id), updateId) == 0) {
            found = 1;

            // --- ✨ ส่วนที่เพิ่มเข้ามา: แสดงข้อมูลเก่า ✨ ---
            strtok(parseLine, ","); // ข้าม ID
            char* oldName = trimWhitespace(strtok(NULL, ","));
            char* oldFineStr = trimWhitespace(strtok(NULL, ","));
            char* oldDate = trimWhitespace(strtok(NULL, "\r\n"));
            int oldFine = atoi(oldFineStr);

            printf("\n--- พบข้อมูล! ---\n");
            printf("ข้อมูลเดิม:\n");
            printf("  -> PayerName: %s\n", oldName);
            printf("  -> FineAmount: %d\n", oldFine);
            printf("  -> PaymentDate: %s\n", oldDate);
            printf("---------------------------------------------\n");
            printf("ป้อนข้อมูลใหม่ (หากไม่ต้องการเปลี่ยน กด Enter ได้เลย):\n");
            // ----------------------------------------------------

            char newName[100], fineBuffer[100], newDate[20];
            int newFine;

            // 1. รับชื่อใหม่
            printf("PayerName ใหม่ [%s]: ", oldName);
            fgets(newName, sizeof(newName), stdin);
            trimWhitespace(newName);
            if (strlen(newName) == 0) { // ถ้าผู้ใช้กด Enter
                strcpy(newName, oldName); // ใช้ชื่อเดิม
            }

            // 2. รับค่าปรับใหม่
            printf("FineAmount ใหม่ [%d]: ", oldFine);
            fgets(fineBuffer, sizeof(fineBuffer), stdin);
            trimWhitespace(fineBuffer);
            if (strlen(fineBuffer) == 0) { // ถ้าผู้ใช้กด Enter
                newFine = oldFine; // ใช้ค่าปรับเดิม
            } else {
                newFine = atoi(fineBuffer);
                while (newFine <= 0) { // ตรวจสอบค่าที่ป้อนใหม่
                    printf("  ค่าปรับต้องเป็นตัวเลขที่มากกว่า 0. กรุณาป้อนใหม่: ");
                    fgets(fineBuffer, sizeof(fineBuffer), stdin);
                    newFine = atoi(trimWhitespace(fineBuffer));
                }
            }

            // 3. รับวันที่ใหม่
            printf("PaymentDate ใหม่ [%s]: ", oldDate);
            fgets(newDate, sizeof(newDate), stdin);
            trimWhitespace(newDate);
            if (strlen(newDate) == 0) { // ถ้าผู้ใช้กด Enter
                strcpy(newDate, oldDate); // ใช้วันที่เดิม
            } else {
                while (strlen(newDate) != 10 || newDate[4] != '-' || newDate[7] != '-') {
                     printf("  รูปแบบวันที่ไม่ถูกต้อง (YYYY-MM-DD). กรุณาป้อนใหม่: ");
                     fgets(newDate, sizeof(newDate), stdin);
                     trimWhitespace(newDate);
                     if (strlen(newDate) == 0) { // อนุญาตให้ใช้ค่าเดิมได้หากป้อนผิด
                        strcpy(newDate, oldDate);
                        break;
                     }
                }
            }

            fprintf(tempFile, "%s,%s,%d,%s\n", updateId, newName, newFine, newDate);
            printf("\n✅ อัปเดตข้อมูลเรียบร้อยแล้ว!\n");
        } else {
            fprintf(tempFile, "%s", originalLine);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove(FILENAME);
        rename(TEMP_FILENAME, FILENAME);
    } else {
        remove(TEMP_FILENAME);
        printf("ไม่พบ ReceiptID '%s'\n", updateId);
    }
}


// --- 🗑️ ฟังก์ชันลบข้อมูล (Choice 5) ---
// ปรับปรุง: อ่านไฟล์รอบเดียวเพื่อประสิทธิภาพที่ดีกว่า
void deleteFineData() {
    char deleteId[50];
    printf("ป้อน ReceiptID ที่ต้องการลบ: ");
    fgets(deleteId, sizeof(deleteId), stdin);
    strcpy(deleteId, trimWhitespace(deleteId));
    
    char confirm[10];
    printf("คุณแน่ใจหรือไม่ที่จะลบ ID %s? (yes/no): ", deleteId);
    fgets(confirm, sizeof(confirm), stdin);
    strcpy(confirm, trimWhitespace(confirm));
    toLowerCase(confirm);

    if (strcmp(confirm, "yes") != 0) {
        printf("ยกเลิกการลบข้อมูล\n");
        return;
    }
    
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ %s ได้\n", FILENAME);
        return;
    }

    FILE* tempFile = fopen(TEMP_FILENAME, "w");
    if (tempFile == NULL) {
        printf("Error: ไม่สามารถสร้างไฟล์ชั่วคราวได้\n");
        fclose(file);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int found = 0;

    // เขียน Header ลงในไฟล์ temp
    if(fgets(line, sizeof(line), file)){
        fprintf(tempFile, "%s", line);
    }

    while (fgets(line, sizeof(line), file)) {
        char originalLine[MAX_LINE_LENGTH];
        strcpy(originalLine, line);
        char* id = strtok(line, ",");
        
        if (strcmp(trimWhitespace(id), deleteId) == 0) {
            found = 1; // เจอข้อมูลที่จะลบ, ไม่ต้องเขียนลง tempFile
        } else {
            fprintf(tempFile, "%s", originalLine);
        }
    }

    fclose(file);
    fclose(tempFile);
    
    if (found) {
        remove(FILENAME);
        rename(TEMP_FILENAME, FILENAME);
        printf("ลบข้อมูล ReceiptID '%s' เรียบร้อยแล้ว\n", deleteId);
    } else {
        remove(TEMP_FILENAME);
        printf("ไม่พบ ReceiptID '%s' ที่ต้องการลบ\n", deleteId);
    }
}

// ======================= Sorting Comparison Functions =======================
// ฟังก์ชันสำหรับ qsort เพื่อเปรียบเทียบข้อมูลแต่ละส่วน
int compareByID(const void* a, const void* b) {
    FineData* dataA = (FineData*)a;
    FineData* dataB = (FineData*)b;
    // เปรียบเทียบตัวเลขหลังตัว 'F'
    return atoi(dataA->id + 1) - atoi(dataB->id + 1);
}

int compareByName(const void* a, const void* b) {
    FineData* dataA = (FineData*)a;
    FineData* dataB = (FineData*)b;
    return strcmp(dataA->payerName, dataB->payerName);
}

int compareByFine(const void* a, const void* b) {
    FineData* dataA = (FineData*)a;
    FineData* dataB = (FineData*)b;
    return dataA->fineAmount - dataB->fineAmount;
}

int compareByDate(const void* a, const void* b) {
    FineData* dataA = (FineData*)a;
    FineData* dataB = (FineData*)b;
    return strcmp(dataA->paymentDate, dataB->paymentDate);
}