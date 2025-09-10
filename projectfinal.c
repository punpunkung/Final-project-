#include <stdio.h>

int main(){
    int choice;
    do
    {
        printf("1)อ่านข้อมูลการจ่ายค่าปรับ\n");
        printf("2)เพิ่มข้อมูลการจ่ายค่าปรับ\n");
        printf("3)ค้นหาข้อมูลการจ่ายค่าปรับ\n");
        printf("4)อัพเดทข้อมูลการจ่ายค่าปรับ\n");
        printf("5)ลบข้อมูลการจ่ายค่าปรับ\n");
        printf("6)ออกจากโปรแกรม\n");
        printf("เลือกดำเนินการ: ");
        scanf("%d", &choice);
        if(choice == 1){
            FILE *Read = fopen("test.csv","r");
            if(Read == NULL){
            printf("Error");
            return 1;
            }
            char data[1024];
            while (fgets(data, sizeof(data), Read))
            {
            printf("%s\n",data);
            }
            fclose(Read);
        }
        else if(choice == 2){
            int Fine;
            char FistName[50],LastName[50],Id[5],Date[9];
            printf("ReceiptID: ");
            scanf("%s",Id);
            printf("\nPayerName: ");
            scanf("%s %s",FistName,LastName);
            printf("\nFineamount: ");
            scanf("%d",&Fine);
            printf("\nPaymentDate: ");
            scanf("%s", Date);

            FILE *Add = fopen("test.csv","a");
            if(Add == NULL){
            printf("Error");
            return 1;
            }
            fprintf(Add,"%s, %s %s, %d, %s\n",Id,FistName,LastName,Fine,Date);
            printf("Success\n");
            fclose(Add);
            
        }
        else if(choice == 3){
            printf("3\n");
            
        }
        else if(choice == 4){
            printf("4\n");
            
        }   
        else if(choice == 5){
            printf("5\n");
            
        }
        else if(choice == 6){
            printf("ออกจากโปรแกรม\n");
            break;
        }
        else{
            printf("ตัวเลือกไม่ถูกต้อง\n");
            
        }


    }while (choice != 6);
    return 0;
}