#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Constants ---
#define FILENAME "parking_fines_v6.csv" // ชื่อไฟล์สำหรับเวอร์ชันล่าสุด
#define MAX_LINE_LEN 256

// --- Data Structure ---
// เพิ่มฟิลด์ payerName เข้าไปใน struct
typedef struct {
    char receiptId[5];      // F001, F002,...
    char payerName[100];    // ชื่อผู้จ่ายเงิน
    int  amount;            // จำนวนเงิน
    char paymentDate[11];   // YYYY-MM-DD
} FineReceipt;

// --- Function Prototypes ---
void clearInputBuffer();
void pressEnterToContinue();
void GenerateNextReceiptID(char *nextId);

void AddNewReceipt();
void DisplayAllReceipts();
void DeleteReceipt();
void GenerateReport();

// --- Main Program ---
int main() {
    int choice;
    do {
        system("cls"); // หรือ "clear"
        printf("==========================================\n");
        printf("   ระบบบันทึกข้อมูลการจ่ายค่าปรับ (v6.0)\n");
        printf("==========================================\n");
        printf("1. เพิ่มข้อมูลใบเสร็จใหม่ (Add New Receipt)\n");
        printf("2. แสดงข้อมูลทั้งหมด (Display All Receipts)\n");
        printf("3. ลบข้อมูลใบเสร็จ (Delete Receipt)\n");
        printf("4. ดูรายงานสรุป (Summary Report)\n");
        printf("5. ออกจากโปรแกรม (Exit)\n");
        printf("------------------------------------------\n");
        printf("เลือกเมนู: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Error: กรุณาป้อนเป็นตัวเลขเท่านั้น\n");
            clearInputBuffer();
            pressEnterToContinue();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: AddNewReceipt(); break;
            case 2: DisplayAllReceipts(); break;
            case 3: DeleteReceipt(); break;
            case 4: GenerateReport(); break;
            case 5: printf("กำลังออกจากโปรแกรม...\n"); break;
            default: printf("ตัวเลือกไม่ถูกต้อง กรุณาลองใหม่อีกครั้ง\n"); break;
        }
        if (choice != 5) {
            pressEnterToContinue();
        }

    } while (choice != 5);
    
    return 0;
}

// ============== Helper Functions ==============
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pressEnterToContinue() {
    printf("\nกด Enter เพื่อดำเนินการต่อ...");
    getchar();
}

void GenerateNextReceiptID(char *nextId) {
    FILE *file = fopen(FILENAME, "r");
    char lastId[5] = "F000";

    if (file != NULL) {
        char line[MAX_LINE_LEN];
        while (fgets(line, sizeof(line), file)) {
            sscanf(line, "%4[^,],", lastId);
        }
        fclose(file);
    }
    
    int lastNum = atoi(lastId + 1);
    sprintf(nextId, "F%03d", lastNum + 1);
}

// ============== 1. เพิ่มข้อมูลใบเสร็จใหม่ ==============
void AddNewReceipt() {
    FineReceipt receipt;

    GenerateNextReceiptID(receipt.receiptId);
    
    printf("\n--- เพิ่มข้อมูลใบเสร็จใหม่ ---\n");
    printf("Receipt ID (Auto-generated): %s\n", receipt.receiptId);

    // รับชื่อผู้จ่ายเงิน (รองรับการเว้นวรรค)
    printf("ชื่อผู้จ่ายเงิน (Payer's Name): ");
    scanf(" %99[^\n]", receipt.payerName);
    clearInputBuffer();

    printf("จำนวนเงิน: ");
    scanf("%d", &receipt.amount);
    clearInputBuffer();
    
    printf("วันที่จ่ายเงิน (YYYY-MM-DD): ");
    scanf("%10s", receipt.paymentDate);
    clearInputBuffer();

    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ %s ได้\n", FILENAME);
        return;
    }
    
    // อัปเดต fprintf ให้บันทึก PayerName ด้วย
    fprintf(file, "%s,%s,%d,%s\n", 
            receipt.receiptId, receipt.payerName, receipt.amount, receipt.paymentDate);
    
    fclose(file);
    printf("\nSuccess: บันทึกข้อมูลใบเสร็จ %s สำหรับคุณ %s เรียบร้อยแล้ว\n", receipt.receiptId, receipt.payerName);
}

// ============== 2. แสดงข้อมูลทั้งหมด ==============
void DisplayAllReceipts() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("ยังไม่มีข้อมูลในระบบ\n");
        return;
    }
    
    char line[MAX_LINE_LEN];
    printf("\n---------------------------------- ข้อมูลใบเสร็จทั้งหมด ----------------------------------\n");
    printf("%-15s | %-25s | %-10s | %-15s\n", 
           "Receipt ID", "Payer Name", "Amount", "Payment Date");
    printf("-------------------------------------------------------------------------------------\n");
    
    while (fgets(line, sizeof(line), file)) {
        FineReceipt receipt;
        // อัปเดต sscanf ให้อ่าน PayerName ด้วย
        sscanf(line, "%4[^,],%99[^,],%d,%10[^\n]",
               receipt.receiptId, receipt.payerName, &receipt.amount, receipt.paymentDate);
               
        printf("%-15s | %-25s | %-10d | %-15s\n",
               receipt.receiptId, receipt.payerName, receipt.amount, receipt.paymentDate);
    }
    printf("-------------------------------------------------------------------------------------\n");
    fclose(file);
}

// ============== 3. ลบข้อมูลใบเสร็จ ==============
void DeleteReceipt() {
    char targetId[5];
    char line[MAX_LINE_LEN];
    int found = 0;

    printf("\n--- ลบข้อมูลใบเสร็จ ---\n");
    printf("กรอก Receipt ID ที่ต้องการลบ (เช่น F001): ");
    scanf("%4s", targetId);
    clearInputBuffer();

    FILE *originalFile = fopen(FILENAME, "r");
    FILE *tempFile = fopen("temp.csv", "w");

    if (originalFile == NULL || tempFile == NULL) {
        printf("Error: เกิดข้อผิดพลาดในการเปิดไฟล์\n");
        return;
    }

    while (fgets(line, sizeof(line), originalFile)) {
        char currentId[5];
        sscanf(line, "%4[^,],", currentId);

        if (strcmp(currentId, targetId) == 0) {
            found = 1;
        } else {
            fprintf(tempFile, "%s", line);
        }
    }

    fclose(originalFile);
    fclose(tempFile);

    if (!found) {
        printf("Error: ไม่พบข้อมูล Receipt ID: %s\n", targetId);
        remove("temp.csv");
    } else {
        remove(FILENAME);
        rename("temp.csv", FILENAME);
        printf("Success: ลบข้อมูลใบเสร็จ %s เรียบร้อยแล้ว\n", targetId);
    }
}

// ============== 4. ดูรายงานสรุป ==============
void GenerateReport() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("ยังไม่มีข้อมูลเพื่อสร้างรายงาน\n");
        return;
    }

    char line[MAX_LINE_LEN];
    int totalReceipts = 0;
    long totalRevenue = 0;

    while (fgets(line, sizeof(line), file)) {
        FineReceipt receipt;
        // อัปเดต sscanf ให้อ่านข้อมูลได้ถูกต้องครบทุกฟิลด์
        sscanf(line, "%4[^,],%99[^,],%d,%10[^\n]",
               receipt.receiptId, receipt.payerName, &receipt.amount, receipt.paymentDate);
        
        totalReceipts++;
        totalRevenue += receipt.amount;
    }
    fclose(file);

    printf("\n=============== รายงานสรุป ===============\n");
    printf("จำนวนใบเสร็จทั้งหมด:   %d\n", totalReceipts);
    printf("ยอดเงินรวมทั้งหมด:      %ld บาท\n", totalRevenue);
    printf("==========================================\n");
}