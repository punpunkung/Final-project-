#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

// ประกาศให้ไฟล์อื่นรู้ว่า "มี" ตัวแปรนี้อยู่จริง ๆ
extern char csv_filename[50];

// ประกาศให้ไฟล์อื่นรู้ว่า "มี" ฟังก์ชันนี้ให้เรียกใช้
void run_all_tests();

#endif // UNIT_TESTS_H