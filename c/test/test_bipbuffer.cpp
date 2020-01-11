/**
 * @file bipbuffer_test.c
 */

#include <gtest/gtest.h>
#include <stdio.h>

#include "bipbuffer.h"

namespace {

class BipBufferTest : public ::testing::Test {
public:
    virtual ~BipBufferTest() = default;

protected:
    virtual void SetUp() {
        buf_ = new bbuf_t;
        bbuf_init(100, buf_);
    }

    virtual void TearDown() {
        bbuf_destroy(buf_);
        delete buf_;
    }

    bbuf_t *buf_;
};

TEST_F(BipBufferTest, InitTest) {
    EXPECT_TRUE(buf_->buffer != nullptr);
}

TEST_F(BipBufferTest, ReserveFail) {
    uint8_t* buf;
    size_t size;

//    buf = bbuf_reserve(buf_, 0);
//    EXPECT_TRUE(buf == nullptr);

    buf = bbuf_reserve(buf_, 10);
    EXPECT_FALSE(buf == nullptr);
    buf = bbuf_reserve(buf_, 10);
    EXPECT_TRUE(buf == nullptr);

    size = bbuf_commit(buf_, 10);
    EXPECT_EQ(10u, size);
    buf = bbuf_reserve(buf_, 20);
    EXPECT_FALSE(buf == nullptr);
    buf = bbuf_reserve(buf_, 20);
    EXPECT_TRUE(buf == nullptr);
}

TEST_F(BipBufferTest, SimpleReserveCommit) {
    uint8_t* buf1;
    uint8_t* buf2;
    size_t size;

    buf1 = bbuf_reserve(buf_, 10);
    size = bbuf_commit(buf_, 10);
    EXPECT_EQ(10u, size);

    buf2 = bbuf_reserve(buf_, 20);
    size = bbuf_commit(buf_, 20);
    EXPECT_EQ(20u, size);

    EXPECT_NE(buf1, buf2);
}

TEST_F(BipBufferTest, ReserveCommitToExactEnd) {
    uint8_t* buf1;
    uint8_t* buf2;
    size_t size;

    buf1 = bbuf_reserve(buf_, 48);
    size = bbuf_commit(buf_, 48);
    EXPECT_TRUE(buf1 != nullptr);
    EXPECT_EQ(48u, size);

    buf2 = bbuf_reserve(buf_, 48);
    size = bbuf_commit(buf_, 48);
    EXPECT_TRUE(buf2 != nullptr);
    EXPECT_EQ(48u, size);
}

TEST_F(BipBufferTest, ReserveCommitOverEnd) {
    uint8_t* buf1;
    uint8_t* buf2;
    uint8_t* buf3;
    size_t size;

    buf1 = bbuf_reserve(buf_, 48);
    size = bbuf_commit(buf_, 48);
    EXPECT_TRUE(buf1 != nullptr);
    EXPECT_EQ(48u, size);

    buf2 = bbuf_reserve(buf_, 38);
    size = bbuf_commit(buf_, 38);
    EXPECT_NE(buf1, buf2);
    EXPECT_EQ(38u, size);

    buf3 = bbuf_reserve(buf_, 9);
    size = bbuf_commit(buf_, 9);
    EXPECT_TRUE(buf3 == nullptr);
    EXPECT_EQ(0u, size);
}

TEST_F(BipBufferTest, ReserveAndCommitWithDifferentSize) {
    uint8_t* buf;
    size_t size;

    // legal case
    buf = bbuf_reserve(buf_, 30);
    size = bbuf_commit(buf_, 20);
    EXPECT_TRUE(buf != nullptr);
    EXPECT_EQ(20u, size);

    // faulty case
    buf = bbuf_reserve(buf_, 20);
    size = bbuf_commit(buf_, 30);
    EXPECT_TRUE(buf != nullptr);
    EXPECT_EQ(20u, size);
}

TEST_F(BipBufferTest, ReadFromEmptyBuffer) {
    uint8_t data[20];
    size_t size = bbuf_read(buf_, data);
    EXPECT_EQ(0u, size);
}

TEST_F(BipBufferTest, SimpleWriteReadTest) {
    uint8_t* buf;
    size_t size;
    const char* str1 = "test string";
    size_t str1len = strlen(str1) + 1;

    buf = bbuf_reserve(buf_, str1len);
    size = bbuf_commit(buf_, str1len);
    EXPECT_TRUE(buf != nullptr);
    EXPECT_EQ(str1len, size);

    memcpy(buf, str1, str1len);
    char str2[30];
    size = bbuf_read(buf_, (uint8_t*) str2);
    EXPECT_STREQ(str1, str2);
    EXPECT_EQ(str1len, size);
}

TEST_F(BipBufferTest, ReadUntilEndOfData) {
    uint8_t* buf;
    size_t size;

    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf != nullptr);
    memcpy(buf, "123456789012345678901234567", 28);
    size = bbuf_commit(buf_, 28);
    EXPECT_EQ(28u, size);
    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf != nullptr);
    memcpy(buf, "234567890123456789012345678", 28);
    size = bbuf_commit(buf_, 28);
    EXPECT_EQ(28u, size);
    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf != nullptr);
    memcpy(buf, "345678901234567890123456789", 28);
    size = bbuf_commit(buf_, 28);
    EXPECT_EQ(28u, size);

    char str[30];

    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(28u, size);
    EXPECT_STREQ("123456789012345678901234567", str);
    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(28u, size);
    EXPECT_STREQ("234567890123456789012345678", str);
    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(28u, size);
    EXPECT_STREQ("345678901234567890123456789", str);
    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(0u, size);
}

TEST_F(BipBufferTest, ReserveAfterReadWithFullBuffer) {
    uint8_t* buf;
    size_t size;

    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf != nullptr);
    memcpy(buf, "123456789012345678901234567", 28);
    size = bbuf_commit(buf_, 28);
    EXPECT_EQ(28u, size);
    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf != nullptr);
    memcpy(buf, "234567890123456789012345678", 28);
    size = bbuf_commit(buf_, 28);
    EXPECT_EQ(28u, size);
    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf != nullptr);
    memcpy(buf, "345678901234567890123456789", 28);
    size = bbuf_commit(buf_, 28);
    EXPECT_EQ(28u, size);
    buf = bbuf_reserve(buf_, 30);
    EXPECT_TRUE(buf == nullptr);
    // region A full

    char str[30];

    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(28u, size);
    EXPECT_STREQ("123456789012345678901234567", str);

    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf != nullptr);
    memcpy(buf, "456789012345678901234567890", 28);
    size = bbuf_commit(buf_, 28);
    EXPECT_EQ(28u, size);

    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf == nullptr);

    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(28u, size);
    EXPECT_STREQ("234567890123456789012345678", str);

    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf != nullptr);
    memcpy(buf, "567890123456789012345678901", 28);
    size = bbuf_commit(buf_, 28);
    EXPECT_EQ(28u, size);

    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf == nullptr);

    // reading last string from region A
    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(28u, size);
    EXPECT_STREQ("345678901234567890123456789", str);
    // region A empty

    // here region B should be moved to region A and 0ed

    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf != nullptr);
    memcpy(buf, "678901234567890123456789012", 28);
    size = bbuf_commit(buf_, 28);
    EXPECT_EQ(28u, size);

    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf == nullptr);

    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(28u, size);
    EXPECT_STREQ("456789012345678901234567890", str);

    // this is a full cycle, the next string should go to region B again
    buf = bbuf_reserve(buf_, 28);
    EXPECT_TRUE(buf != nullptr);
    memcpy(buf, "789012345678901234567890123", 28);
    size = bbuf_commit(buf_, 28);
    EXPECT_EQ(28u, size);

    // read everything
    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(28u, size);
    EXPECT_STREQ("567890123456789012345678901", str);
    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(28u, size);
    EXPECT_STREQ("678901234567890123456789012", str);
    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(28u, size);
    EXPECT_STREQ("789012345678901234567890123", str);
    size = bbuf_read(buf_, (uint8_t*) str);
    EXPECT_EQ(0u, size);
}

} // namespace
