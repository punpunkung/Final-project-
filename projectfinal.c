#include <stdio.h>
#include <stdlib.h>
#include <string.h> // เพิ่มเข้ามาเพื่อใช้ฟังก์ชันเกี่ยวกับสตริง เช่น strstr, strcmp

// --- Function Prototypes ---
void clearInputBuffer();
void ReadCsv();
void AddFineData();
void SearchFineData(); // ฟังก์ชันค้นหาข้อมูล
void UpdateFineData(); // ฟังก์ชันอัปเดตข้อมูล
void DeleteFineData(); // ฟังก์ชันลบข้อมูล

// ---------------------------

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ReadCsv() {
    FILE *Read = fopen("test.csv", "r");
    if (Read == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ test.csv ได้\n");
        return;
    }
    
    char data[1024];
    printf("\n---------- ข้อมูลการจ่ายค่าปรับ ----------\n");
    while (fgets(data, sizeof(data), Read)) {
        printf("%s", data);
    }
    printf("----------------------------------------\n\n");
    fclose(Read);
}

void AddFineData() {
    int Fine;
    char FirstName[50], LastName[50], Id[10], Date[11];
    
    printf("ReceiptID: ");
    scanf("%9s", Id);
    clearInputBuffer();

    printf("PayerName (First Last): ");
    scanf("%49s %49s", FirstName, LastName);
    clearInputBuffer();

    printf("Fineamount: ");
    scanf("%d", &Fine);
    clearInputBuffer();

    printf("PaymentDate (dd/mm/yyyy): ");
    scanf("%10s", Date);
    clearInputBuffer();

    FILE *Add = fopen("test.csv", "a");
    if (Add == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์เพื่อเขียนได้\n");
    } else {
        fprintf(Add, "%s,%s %s,%d,%s\n", Id, FirstName, LastName, Fine, Date);
        printf("Success: เพิ่มข้อมูลเรียบร้อยแล้ว\n\n");
        fclose(Add);
    }
}

// ============== 🔎 ฟังก์ชันค้นหาข้อมูล (Choice 3) ==============
void SearchFineData() {
    char searchId[10];
    char line[1024];
    int found = 0;

    printf("กรอก ReceiptID ที่ต้องการค้นหา: ");
    scanf("%9s", searchId);
    clearInputBuffer();

    FILE *file = fopen("test.csv", "r");
    if (file == NULL) {
        printf("Error: ไม่สามารถเปิดไฟล์ข้อมูลได้\n");
        return;
    }

    printf("\n--- ผลการค้นหา ---\n");
    while (fgets(line, sizeof(line), file)) {
        char tempLine[1024];
        strcpy(tempLine, line); // คัดลอก line ไว้ใช้กับ strtok

        char *token = strtok(tempLine, ","); // แยกข้อมูลส่วนแรก (ID) ด้วยเครื่องหมาย comma
        if (token != NULL && strcmp(token, searchId) == 0) {
            printf("%s", line); // ถ้า ID ตรงกัน ให้แสดงผลทั้งบรรทัด
            found = 1;
        }
    }
    
    if (!found) {
        printf("ไม่พบข้อมูลสำหรับ ReceiptID: %s\n", searchId);
    }
    printf("------------------\n\n");

    fclose(file);
}

// ============== 📝 ฟังก์ชันอัปเดตข้อมูล (Choice 4) ==============
void UpdateFineData() {
    char updateId[10];
    char line[1024];
    int found = 0;

    printf("กรอก ReceiptID ของข้อมูลที่ต้องการอัปเดต: ");
    scanf("%9s", updateId);
    clearInputBuffer();

    FILE *originalFile = fopen("test.csv", "r");
    FILE *tempFile = fopen("temp.csv", "w"); // สร้างไฟล์ชั่วคราวเพื่อเขียนข้อมูลใหม่

    if (originalFile == NULL || tempFile == NULL) {
        printf("Error: เกิดข้อผิดพลาดในการเปิดไฟล์\n");
        return;
    }

    while (fgets(line, sizeof(line), originalFile)) {
        char tempLine[1024];
        strcpy(tempLine, line);
        char *id = strtok(tempLine, ",");

        if (id != NULL && strcmp(id, updateId) == 0) {
            found = 1;
            printf("--- พบข้อมูล! กรุณากรอกข้อมูลใหม่ ---\n");
            
            // รับข้อมูลใหม่จากผู้ใช้
            int newFine;
            char newFirstName[50], newLastName[50], newDate[11];

            printf("New PayerName (First Last): ");
            scanf("%49s %49s", newFirstName, newLastName);
            clearInputBuffer();

            printf("New Fineamount: ");
            scanf("%d", &newFine);
            clearInputBuffer();

            printf("New PaymentDate (dd/mm/yyyy): ");
            scanf("%10s", newDate);
            clearInputBuffer();

            // เขียนข้อมูลที่อัปเดตแล้วลงในไฟล์ชั่วคราว
            fprintf(tempFile, "%s,%s %s,%d,%s\n", updateId, newFirstName, newLastName, newFine, newDate);
            printf("Success: อัปเดตข้อมูลเรียบร้อยแล้ว\n\n");
        } else {
            fprintf(tempFile, "%s", line); // ถ้า ID ไม่ตรงกัน ให้คัดลอกข้อมูลเดิมไปไฟล์ชั่วคราว
        }
    }

    fclose(originalFile);
    fclose(tempFile);

    if (!found) {
        printf("ไม่พบข้อมูล ReceiptID: %s เพื่อทำการอัปเดต\n\n", updateId);
        remove("temp.csv"); // ลบไฟล์ชั่วคราวทิ้งถ้าไม่เจอข้อมูล
    } else {
        remove("test.csv"); // ลบไฟล์เก่า
        rename("temp.csv", "test.csv"); // เปลี่ยนชื่อไฟล์ชั่วคราวเป็นไฟล์จริง
    }
}

// ============== 🗑️ ฟังก์ชันลบข้อมูล (Choice 5) ==============
void DeleteFineData() {
    char deleteId[10];
    char line[1024];
    int found = 0;

    printf("กรอก ReceiptID ของข้อมูลที่ต้องการลบ: ");
    scanf("%9s", deleteId);
    clearInputBuffer();

    FILE *originalFile = fopen("test.csv", "r");
    FILE *tempFile = fopen("temp.csv", "w");

    if (originalFile == NULL || tempFile == NULL) {
        printf("Error: เกิดข้อผิดพลาดในการเปิดไฟล์\n");
        return;
    }

    while (fgets(line, sizeof(line), originalFile)) {
        char tempLine[1024];
        strcpy(tempLine, line);
        char *id = strtok(tempLine, ",");

        if (id != NULL && strcmp(id, deleteId) == 0) {
            found = 1; // เจอข้อมูลที่ต้องการลบแล้ว ไม่ต้องทำอะไร (คือไม่เขียนลงไฟล์ใหม่)
            printf("Success: ลบข้อมูล ReceiptID: %s เรียบร้อยแล้ว\n\n", deleteId);
        } else {
            fprintf(tempFile, "%s", line); // เขียนเฉพาะข้อมูลที่ไม่ต้องการลบลงไฟล์ชั่วคราว
        }
    }

    fclose(originalFile);
    fclose(tempFile);

    if (!found) {
        printf("ไม่พบข้อมูล ReceiptID: %s เพื่อทำการลบ\n\n", deleteId);
        remove("temp.csv");
    } else {
        remove("test.csv");
        rename("temp.csv", "test.csv");
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
