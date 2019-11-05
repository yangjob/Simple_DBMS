#include <iostream>
#include <unistd.h>
#include "CLTable.h"
// #include "CLThread.h"
// #include "CLExecutiveFunctionProvider.h"
using namespace std;

#define THREAD_NUM 10
void Test();
void* TestThreadForCLTable(void *arg);
string atts_name[COLUMN_NUMS];

int main()
{
    Test();
   
    return 0;
}

void Test() {
    for(int i = 0; i < COLUMN_NUMS; i++) atts_name[i] = to_string(i);
    pthread_t tid[THREAD_NUM];
    for(long i = 0; i < THREAD_NUM; i++) pthread_create(&tid[i], 0, TestThreadForCLTable, (void *)i);
    for(long i = 0; i < THREAD_NUM; i++) pthread_join(tid[i], 0);
}

void* TestThreadForCLTable(void *arg) {
    long i = (long)arg;
    for(int j = 0; j < 10; j++) {
        if(i % 2 == 0) {
            SRow row;
            for(int k = 0; k < COLUMN_NUMS; k++) row.atts[k] = rand() % 200;
            CLStatus s = CLTable::GetInstance()->InsertDataLast(&row);
            if(s.IsSuccess()) cout << "插入成功" << endl;
        }else CLTable::SelectData(atts_name[rand() % 100], rand() % 100, 100 + rand() % 100);
    }
}