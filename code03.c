#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =======================================================
// COLOR DEFINITIONS (ชุดโค้ดสี ANSI)
// =======================================================
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"

// =======================================================
// UI/UX HELPER FUNCTIONS
// =======================================================

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnterToContinue() {
    printf(COLOR_YELLOW "\n+--------------------------------------------------+\n" COLOR_RESET);
    printf(COLOR_YELLOW "|     กดปุ่ม Enter เพื่อกลับสู่เมนูหลัก...        |"     COLOR_RESET);
    printf(COLOR_YELLOW "\n+--------------------------------------------------+\n" COLOR_RESET);
    // ล้าง buffer ที่อาจมีค่าค้างอยู่ก่อนรับ Enter
    while (getchar() != '\n');
    getchar();
}

void printTableHeader() {
    printf("+-----------------+---------------------------+--------------+-----------------+\n");
    printf("| " COLOR_BLUE "%-15s" COLOR_RESET " | " COLOR_BLUE "%-25s" COLOR_RESET " | " COLOR_BLUE "%-12s" COLOR_RESET " | " COLOR_BLUE "%-15s" COLOR_RESET " |\n", "ReceiptID", "PayerName", "FineAmount", "PaymentDate");
    printf("+-----------------+---------------------------+--------------+-----------------+\n");
}

void printTableRow(char* line) {
    char tempLine[1024];
    strcpy(tempLine, line);

    char* id = strtok(tempLine, ",\n");
    char* name = strtok(NULL, ",\n");
    char* amountStr = strtok(NULL, ",\n");
    char* date = strtok(NULL, ",\n");

    if (id && name && amountStr && date) {
        printf("| %-15s | %-25s | %-12s | %-15s |\n", id, name, amountStr, date);
    }
}

// =======================================================
// CORE LOGIC FUNCTIONS
// =======================================================

// Function Prototypes
void InitializeDataFile();
void display_menu();
void clearInputBuffer();
void ReadCsv();
void AddFineData();
void SearchFineData();
void UpdateFineData();
void DeleteFineData();

// Main Program
int main() {
    InitializeDataFile();
    int choice = 0;
    do {
        clearScreen();
        display_menu();
        
        if (scanf("%d", &choice) != 1) {
            printf(COLOR_RED "\n[ERROR] กรุณาป้อนข้อมูลเป็นตัวเลขเท่านั้น!\n" COLOR_RESET);
            clearInputBuffer();
            pressEnterToContinue();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: ReadCsv(); break;
            case 2: AddFineData(); break;
            case 3: SearchFineData(); break;
            case 4: UpdateFineData(); break;
            case 5: DeleteFineData(); break;
            case 6: 
                clearScreen();
                printf(COLOR_CYAN "ขอบคุณที่ใช้บริการโปรแกรม\n" COLOR_RESET); 
                break;
            default: 
                printf(COLOR_RED "\n[ERROR] ตัวเลือกไม่ถูกต้อง กรุณาลองใหม่อีกครั้ง\n" COLOR_RESET); 
                break;
        }

        if (choice > 0 && choice < 6) {
            pressEnterToContinue();
        }

    } while (choice != 6);
    
    return 0;
}

// Function Definitions
void display_menu() {
    printf(COLOR_CYAN "╔════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_YELLOW "      ระบบบันทึกข้อมูลการจ่ายค่าปรับ     " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╠════════════════════════════════════════╣\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "  1. แสดงข้อมูลค่าปรับทั้งหมด (Read)      " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "  2. เพิ่มข้อมูลค่าปรับใหม่ (Add)         " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "  3. ค้นหาข้อมูลค่าปรับ (Search)        " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "  4. อัพเดทข้อมูลค่าปรับ (Update)      " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "  5. ลบข้อมูลค่าปรับ (Delete)          " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RED "  6. ออกจากโปรแกรม                    " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╚════════════════════════════════════════╝\n" COLOR_RESET);
    printf(COLOR_MAGENTA "กรุณาเลือกเมนู: " COLOR_RESET);
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void InitializeDataFile() {
    FILE *file = fopen("test.csv", "r");
    if (file == NULL) {
        clearScreen();
        printf(COLOR_YELLOW "ไม่พบไฟล์ test.csv, กำลังสร้างไฟล์พร้อมข้อมูลเริ่มต้น...\n" COLOR_RESET);
        file = fopen("test.csv", "w");
        if (file == NULL) {
            printf(COLOR_RED "Error: ไม่สามารถสร้างไฟล์ test.csv ได้\n" COLOR_RESET);
            exit(1);
        }
        fprintf(file, "ReceiptID,PayerName,FineAmount,PaymentDate\n");
        fprintf(file, "F001,Somchai Jaidee,300,2025-08-11\n");
        fprintf(file, "F002,Jane Smith,500,2025-08-12\n");
        fprintf(file, "F003,Peter Jones,1000,2025-08-13\n");
        fprintf(file, "F004,Somsri Rakthai,300,2025-08-14\n");
        fprintf(file, "F005,David Copper,200,2025-08-15\n");
        fprintf(file, "F006,Maria Garcia,750,2025-08-16\n");
        fprintf(file, "F007,John Wick,2000,2025-08-17\n");
        fprintf(file, "F008,Tony Stark,5000,2025-08-18\n");
        fprintf(file, "F009,Lisa Manoban,400,2025-08-19\n");
        fprintf(file, "F010,Keanu Reeves,500,2025-08-20\n");
        fprintf(file, "F011,Suda Makmee,300,2025-08-21\n");
        fprintf(file, "F012,Chris Evans,1200,2025-08-22\n");
        fprintf(file, "F013,Pita Limjaroenrat,600,2025-08-23\n");
        fprintf(file, "F014,Elon Musk,10000,2025-08-24\n");
        fprintf(file, "F015,Taylor Swift,800,2025-08-25\n");
        printf(COLOR_GREEN "สร้างไฟล์สำเร็จ!\n" COLOR_RESET);
        pressEnterToContinue();
    }
    if (file != NULL) fclose(file);
}

void ReadCsv() {
    FILE *Read = fopen("test.csv", "r");
    if (Read == NULL) {
        printf(COLOR_RED "\n[ERROR] ไม่สามารถเปิดไฟล์ test.csv ได้\n" COLOR_RESET); return;
    }
    char line[1024];
    printf("\n");
    printTableHeader();
    fgets(line, sizeof(line), Read); // Skip Header
    while (fgets(line, sizeof(line), Read)) {
        printTableRow(line);
    }
    printf("+-----------------+---------------------------+--------------+-----------------+\n");
    fclose(Read);
}

void AddFineData() {
    printf(COLOR_YELLOW "\n--- เพิ่มข้อมูลค่าปรับใหม่ ---\n" COLOR_RESET);
    int Fine; char FirstName[50], LastName[50], Id[10], Date[11];
    printf(COLOR_MAGENTA "ReceiptID: " COLOR_RESET); scanf("%9s", Id); clearInputBuffer();
    printf(COLOR_MAGENTA "PayerName (First Last): " COLOR_RESET); scanf("%49s %49s", FirstName, LastName); clearInputBuffer();
    printf(COLOR_MAGENTA "Fineamount: " COLOR_RESET); scanf("%d", &Fine); clearInputBuffer();
    printf(COLOR_MAGENTA "PaymentDate (yyyy-mm-dd): " COLOR_RESET); scanf("%10s", Date); clearInputBuffer();

    FILE *Add = fopen("test.csv", "a");
    if (Add == NULL) {
        printf(COLOR_RED "\n[ERROR] ไม่สามารถเปิดไฟล์เพื่อเขียนได้\n" COLOR_RESET);
    } else {
        fprintf(Add, "%s,%s %s,%d,%s\n", Id, FirstName, LastName, Fine, Date);
        printf(COLOR_GREEN "\n[SUCCESS] เพิ่มข้อมูลเรียบร้อยแล้ว!\n" COLOR_RESET);
        fclose(Add);
    }
}

void SearchFineData() {
    char searchTerm[100]; char line[1024]; int found = 0;
    printf(COLOR_MAGENTA "\nกรอกคำค้นหา (ReceiptID หรือ PayerName): " COLOR_RESET);
    scanf("%99[^\n]", searchTerm); clearInputBuffer();
    FILE *file = fopen("test.csv", "r");
    if (file == NULL) {
        printf(COLOR_RED "\n[ERROR] ไม่สามารถเปิดไฟล์ข้อมูลได้\n" COLOR_RESET); return;
    }
    printf(COLOR_YELLOW "\n--- ผลการค้นหาสำหรับ '%s' ---\n" COLOR_RESET, searchTerm);
    fgets(line, sizeof(line), file); // Skip Header
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, searchTerm) != NULL) {
            if (!found) { printTableHeader(); found = 1; }
            printTableRow(line);
        }
    }
    if (found) {
        printf("+-----------------+---------------------------+--------------+-----------------+\n");
    } else {
        printf(COLOR_YELLOW "ไม่พบข้อมูลที่ตรงกับคำค้นหา\n" COLOR_RESET);
    }
    fclose(file);
}

void UpdateFineData() {
    printf(COLOR_YELLOW "\n--- อัพเดทข้อมูลค่าปรับ ---\n" COLOR_RESET);
    char updateId[10]; char line[1024]; int found = 0;
    printf(COLOR_MAGENTA "กรอก ReceiptID ของข้อมูลที่ต้องการอัปเดต: " COLOR_RESET);
    scanf("%9s", updateId); clearInputBuffer();

    FILE *originalFile = fopen("test.csv", "r");
    FILE *tempFile = fopen("temp.csv", "w");

    if (originalFile == NULL || tempFile == NULL) {
        printf(COLOR_RED "\n[ERROR] เกิดข้อผิดพลาดในการเปิดไฟล์\n" COLOR_RESET); return;
    }

    if (fgets(line, sizeof(line), originalFile) != NULL) fprintf(tempFile, "%s", line); // Copy Header

    while (fgets(line, sizeof(line), originalFile)) {
        char tempLine[1024]; strcpy(tempLine, line);
        char *id = strtok(tempLine, ",");
        if (id != NULL && strcmp(id, updateId) == 0) {
            found = 1;
            printf(COLOR_YELLOW "--- พบข้อมูล! กรุณากรอกข้อมูลใหม่ ---\n" COLOR_RESET);
            int newFine; char newFirstName[50], newLastName[50], newDate[11];
            printf(COLOR_MAGENTA "New PayerName (First Last): " COLOR_RESET); scanf("%49s %49s", newFirstName, newLastName); clearInputBuffer();
            printf(COLOR_MAGENTA "New Fineamount: " COLOR_RESET); scanf("%d", &newFine); clearInputBuffer();
            printf(COLOR_MAGENTA "New PaymentDate (yyyy-mm-dd): " COLOR_RESET); scanf("%10s", newDate); clearInputBuffer();
            fprintf(tempFile, "%s,%s %s,%d,%s\n", updateId, newFirstName, newLastName, newFine, newDate);
        } else {
            fprintf(tempFile, "%s", line);
        }
    }
    fclose(originalFile); fclose(tempFile);

    if (!found) {
        printf(COLOR_YELLOW "\n[INFO] ไม่พบข้อมูล ReceiptID: %s\n" COLOR_RESET, updateId);
        remove("temp.csv");
    } else {
        remove("test.csv");
        rename("temp.csv", "test.csv");
        printf(COLOR_GREEN "\n[SUCCESS] อัปเดตข้อมูลเรียบร้อยแล้ว!\n" COLOR_RESET);
    }
}

void DeleteFineData() {
    printf(COLOR_YELLOW "\n--- ลบข้อมูลค่าปรับ ---\n" COLOR_RESET);
    char deleteId[10]; char line[1024]; int found = 0;
    printf(COLOR_MAGENTA "กรอก ReceiptID ของข้อมูลที่ต้องการลบ: " COLOR_RESET);
    scanf("%9s", deleteId); clearInputBuffer();

    FILE *originalFile = fopen("test.csv", "r");
    FILE *tempFile = fopen("temp.csv", "w");

    if (originalFile == NULL || tempFile == NULL) {
        printf(COLOR_RED "\n[ERROR] เกิดข้อผิดพลาดในการเปิดไฟล์\n" COLOR_RESET); return;
    }

    if (fgets(line, sizeof(line), originalFile) != NULL) fprintf(tempFile, "%s", line); // Copy Header
    
    while (fgets(line, sizeof(line), originalFile)) {
        char tempLine[1024]; strcpy(tempLine, line);
        char *id = strtok(tempLine, ",");
        if (id != NULL && strcmp(id, deleteId) == 0) {
            found = 1; // Skip this line
        } else {
            fprintf(tempFile, "%s", line);
        }
    }
    fclose(originalFile); fclose(tempFile);

    if (!found) {
        printf(COLOR_YELLOW "\n[INFO] ไม่พบข้อมูล ReceiptID: %s\n" COLOR_RESET, deleteId);
        remove("temp.csv");
    } else {
        remove("test.csv");
        rename("temp.csv", "test.csv");
        printf(COLOR_GREEN "\n[SUCCESS] ลบข้อมูลเรียบร้อยแล้ว!\n" COLOR_RESET);
    }
}