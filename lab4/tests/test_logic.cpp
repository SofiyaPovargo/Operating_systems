#include <gtest/gtest.h>
#include <fstream>
#include "../common.h"

TEST(BufferTest, FIFO_Logic) {
    const char* fName = "test.bin";
    // Создаем файл на 2 записи
    std::ofstream out(fName, std::ios::binary);
    FileHeader h = { 2, 0, 0 };
    out.write((char*)&h, sizeof(h));
    Message m = {""};
    out.write((char*)&m, sizeof(m)); out.write((char*)&m, sizeof(m));
    out.close();

    // Симуляция записи
    std::fstream f(fName, std::ios::binary | std::ios::in | std::ios::out);
    f.read((char*)&h, sizeof(h));
    Message m1 = {"msg1"};
    f.seekp(sizeof(h) + h.tail * sizeof(Message));
    f.write((char*)&m1, sizeof(m1));
    h.tail = (h.tail + 1) % h.maxMessages;
    
    EXPECT_EQ(h.tail, 1);
    f.close();
}