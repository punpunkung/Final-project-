#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =======================================================
// UI/UX HELPER FUNCTIONS (ฟังก์ชันช่วยเสริมหน้าตาโปรแกรม)
// =======================================================

// ฟังก์ชันล้างหน้าจอ (ทำงานได้ทั้ง Windows และ Linux/macOS)
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ฟังก์ชันหยุดรอผู้ใช้กด Enter
void pressEnterToContinue() {
    printf("\nกดปุ่ม Enter เพื่อดำเนินการต่อ...");
    while (getchar() != '\n'); // รอรับ Enter
    getchar(); // รออีกครั้งสำหรับบาง Terminal
}

// ฟังก์ชันแสดงหัวตาราง
void printTableHeader() {
    printf("+-----------------+---------------------------+--------------+-----------------+\n");
    printf("| %-15s | %-25s | %-12s | %-15s |\n", "ReceiptID", "PayerName", "FineAmount", "PaymentDate");
    printf("+-----------------+---------------------------+--------------+-----------------+\n");
}

// ฟังก์ชันแสดงข้อมูลในรูปแบบแถวของตาราง
void printTableRow(char* line) {
    char tempLine[1024];
    strcpy(tempLine, line); // คัดลอก line เพื่อป้องกัน strtok ทำลายข้อมูลดั้งเดิม

    // แยกข้อมูลด้วยเครื่องหมาย comma (,)
    char* id = strtok(tempLine, ",\n");
    char* name = strtok(NULL, ",\n");
    char* amountStr = strtok(NULL, ",\n");
    char* date = strtok(NULL, ",\n");

    if (id && name && amountStr && date) {
        printf("| %-15s | %-25s | %-12s | %-15s |\n", id, name, amountStr, date);
    }
}

// =======================================================
// CORE LOGIC FUNCTIONS (ฟังก์ชันหลักของโปรแกรม)
// =======================================================

void InitializeDataFile();
void display_menu();
void clearInputBuffer();
void ReadCsv();
void AddFineData();
void SearchFineData();
void UpdateFineData();
void DeleteFineData();

int main() {
    InitializeDataFile();
    int choice = 0;
    do {
        clearScreen();
        display_menu();
        
        if (scanf("%d", &choice) != 1) {
            printf("\n[ERROR] กรุณาป้อนข้อมูลเป็นตัวเลขเท่านั้น!\n");
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
                printf("ขอบคุณที่ใช้บริการโปรแกรม\n"); 
                break;
            default: 
                printf("\n[ERROR] ตัวเลือกไม่ถูกต้อง กรุณาลองใหม่อีกครั้ง\n"); 
                break;
        }

        if (choice != 6) {
            pressEnterToContinue();
        }

    } while (choice != 6);
    
    return 0;
}

// (ปรับปรุง) ฟังก์ชันแสดงเมนู
void display_menu() {
    printf("╔════════════════════════════════════════╗\n");
    printf("║      ระบบบันทึกข้อมูลการจ่ายค่าปรับ          ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║  1. แสดงข้อมูลค่าปรับทั้งหมด (Read)         ║\n");
    printf("║  2. เพิ่มข้อมูลค่าปรับใหม่ (Add)             ║\n");
    printf("║  3. ค้นหาข้อมูลค่าปรับ (Search)            ║\n");
    printf("║  4. อัพเดทข้อมูลค่าปรับ (Update)           ║\n");
    printf("║  5. ลบข้อมูลค่าปรับ (Delete)              ║\n");
    printf("║  6. ออกจากโปรแกรม                      ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("กรุณาเลือกเมนู: ");
}

// (ปรับปรุง) ฟังก์ชันอ่านไฟล์ CSV
void ReadCsv() {
    FILE *Read = fopen("test.csv", "r");
    if (Read == NULL) {
        printf("\n[ERROR] ไม่สามารถเปิดไฟล์ test.csv ได้\n");
        return;
    }
    char line[1024];
    
    printf("\n");
    printTableHeader(); // แสดงหัวตาราง
    
    // ข้าม Header ของไฟล์ CSV
    fgets(line, sizeof(line), Read);

    while (fgets(line, sizeof(line), Read)) {
        printTableRow(line); // แสดงแต่ละแถวในรูปแบบตาราง
    }
    printf("+-----------------+---------------------------+--------------+-----------------+\n"); // เส้นปิดตาราง
    fclose(Read);
}

// (ปรับปรุง) ฟังก์ชันค้นหาข้อมูล
void SearchFineData() {
    char searchTerm[100];
    char line[1024];
    int found = 0;

    printf("\nกรอกคำค้นหา (ReceiptID หรือ PayerName): ");
    scanf("%99[^\n]", searchTerm);
    clearInputBuffer();

    FILE *file = fopen("test.csv", "r");
    if (file == NULL) {
        printf("\n[ERROR] ไม่สามารถเปิดไฟล์ข้อมูลได้\n"); return;
    }

    printf("\n--- ผลการค้นหาสำหรับ '%s' ---\n", searchTerm);
    
    fgets(line, sizeof(line), file); // ข้าม Header

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, searchTerm) != NULL) {
            if (!found) { // ถ้าเป็นการค้นเจอครั้งแรก ให้แสดงหัวตารางก่อน
                printTableHeader();
                found = 1;
            }
            printTableRow(line);
        }
    }
    
    if (found) {
        printf("+-----------------+---------------------------+--------------+-----------------+\n");
    } else {
        printf("ไม่พบข้อมูลที่ตรงกับคำค้นหา\n");
    }
    fclose(file);
}

// (ปรับปรุง) ฟังก์ชันเพิ่มข้อมูล
void AddFineData() {
    printf("\n--- เพิ่มข้อมูลค่าปรับใหม่ ---\n");
    // ... โค้ดส่วนรับข้อมูลเหมือนเดิม ...
    int Fine; char FirstName[50], LastName[50], Id[10], Date[11];
    printf("ReceiptID: ");
    scanf("%9s", Id); clearInputBuffer();
    printf("PayerName (First Last): ");
    scanf("%49s %49s", FirstName, LastName); clearInputBuffer();
    printf("Fineamount: ");
    scanf("%d", &Fine); clearInputBuffer();
    printf("PaymentDate (yyyy-mm-dd): ");
    scanf("%10s", Date); clearInputBuffer();

    FILE *Add = fopen("test.csv", "a");
    if (Add == NULL) {
        printf("\n[ERROR] ไม่สามารถเปิดไฟล์เพื่อเขียนได้\n");
    } else {
        fprintf(Add, "%s,%s %s,%d,%s\n", Id, FirstName, LastName, Fine, Date);
        printf("\n[SUCCESS] เพิ่มข้อมูลเรียบร้อยแล้ว!\n");
        fclose(Add);
    }
}

// ฟังก์ชันอื่นๆ (Update, Delete, Initialize, clearInputBuffer) ใช้โค้ดเดิมได้เลย
// แต่ปรับแก้ข้อความตอบกลับเล็กน้อยเพื่อความสอดคล้องกัน

void UpdateFineData() {
    printf("\n--- อัพเดทข้อมูลค่าปรับ ---\n");
    char updateId[10];
    char line[1024];
    int found = 0;

    printf("กรอก ReceiptID ของข้อมูลที่ต้องการอัปเดต: ");
    scanf("%9s", updateId); clearInputBuffer();

    FILE *originalFile = fopen("test.csv", "r");
    FILE *tempFile = fopen("temp.csv", "w");

    if (originalFile == NULL || tempFile == NULL) {
        printf("\n[ERROR] เกิดข้อผิดพลาดในการเปิดไฟล์\n"); return;
    }

    // คัดลอก Header ไปยังไฟล์ชั่วคราว
    if (fgets(line, sizeof(line), originalFile) != NULL) {
        fprintf(tempFile, "%s", line);
    }

    while (fgets(line, sizeof(line), originalFile)) {
        char tempLine[1024]; strcpy(tempLine, line);
        char *id = strtok(tempLine, ",");
        if (id != NULL && strcmp(id, updateId) == 0) {
            found = 1;
            printf("--- พบข้อมูล! กรุณากรอกข้อมูลใหม่ ---\n");
            int newFine; char newFirstName[50], newLastName[50], newDate[11];
            printf("New PayerName (First Last): ");
            scanf("%49s %49s", newFirstName, newLastName); clearInputBuffer();
            printf("New Fineamount: ");
            scanf("%d", &newFine); clearInputBuffer();
            printf("New PaymentDate (yyyy-mm-dd): ");
            scanf("%10s", newDate); clearInputBuffer();
            fprintf(tempFile, "%s,%s %s,%d,%s\n", updateId, newFirstName, newLastName, newFine, newDate);
        } else {
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(originalFile); fclose(tempFile);

    if (!found) {
        printf("\n[INFO] ไม่พบข้อมูล ReceiptID: %s\n", updateId);
        remove("temp.csv");
    } else {
        remove("test.csv");
        rename("temp.csv", "test.csv");
        printf("\n[SUCCESS] อัปเดตข้อมูลเรียบร้อยแล้ว!\n");
    }
}

void DeleteFineData() {
    printf("\n--- ลบข้อมูลค่าปรับ ---\n");
    char deleteId[10];
    char line[1024];
    int found = 0;

    printf("กรอก ReceiptID ของข้อมูลที่ต้องการลบ: ");
    scanf("%9s", deleteId); clearInputBuffer();

    FILE *originalFile = fopen("test.csv", "r");
    FILE *tempFile = fopen("temp.csv", "w");

    if (originalFile == NULL || tempFile == NULL) {
        printf("\n[ERROR] เกิดข้อผิดพลาดในการเปิดไฟล์\n"); return;
    }

    // คัดลอก Header
    if (fgets(line, sizeof(line), originalFile) != NULL) {
        fprintf(tempFile, "%s", line);
    }
    
    while (fgets(line, sizeof(line), originalFile)) {
        char tempLine[1024]; strcpy(tempLine, line);
        char *id = strtok(tempLine, ",");
        if (id != NULL && strcmp(id, deleteId) == 0) {
            found = 1; // เจอข้อมูลแล้ว ไม่ต้องทำอะไร (ข้ามการเขียนลงไฟล์ใหม่)
        } else {
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(originalFile); fclose(tempFile);

    if (!found) {
        printf("\n[INFO] ไม่พบข้อมูล ReceiptID: %s\n", deleteId);
        remove("temp.csv");
    } else {
        remove("test.csv");
        rename("temp.csv", "test.csv");
        printf("\n[SUCCESS] ลบข้อมูลเรียบร้อยแล้ว!\n");
    }
}


// โค้ดส่วนที่เหลือเหมือนเดิม
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void InitializeDataFile() {
    FILE *file = fopen("test.csv", "r");
    if (file == NULL) {
        printf("ไม่พบไฟล์ test.csv, กำลังสร้างไฟล์พร้อมข้อมูลเริ่มต้น...\n");
        file = fopen("test.csv", "w");
        if (file == NULL) {
            printf("Error: ไม่สามารถสร้างไฟล์ test.csv ได้\n");
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
        printf("สร้างไฟล์สำเร็จ!\n");
        pressEnterToContinue();
    }
    if (file != NULL) fclose(file);
}