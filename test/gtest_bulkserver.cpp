/**
 * @file gtest_bulk_server
 *
 * @brief test bulk_server
 *  */

#include "bulkasync.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace {
using std::string;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

std::string get_is(const std::string &path_in, time_t t) {
  testing::internal::CaptureStdout();
  BulkAsync ba;
  auto *handler = ba.connect(3, [&t]() { return t++; });

  std::ifstream ifsin(path_in);
  EXPECT_TRUE(ifsin.is_open());
  for (std::string line; std::getline(ifsin, line);) {
    line+="\n";
    ba.receive(handler, line.data(), line.size());
  }
  EXPECT_FALSE(ifsin.bad());
  ba.disconnect(handler);
  ba.closeAll();
  return testing::internal::GetCapturedStdout();
}

std::string get_must(const std::string &path_out) {
  std::string output_must;
  std::ifstream ifsout(path_out);
  EXPECT_TRUE(ifsout.is_open());
  for (std::string line; std::getline(ifsout, line);) {
    output_must += line + "\n";
  }
  EXPECT_FALSE(ifsout.bad());
  return output_must;
}


auto file_contents(const std::string &fname) {
  std::ifstream fit(fname);
  return std::string((std::istreambuf_iterator<char>(fit)),
                     std::istreambuf_iterator<char>());
}


TEST(async, input1) {
  {
    std::filesystem::remove("bulk10000-000-000.log");
    std::filesystem::remove("bulk10001-000-000.log");
    EXPECT_EQ(get_is("testinput1.txt", 10000), get_must("testoutput1.txt"));
    EXPECT_EQ(file_contents("bulk10000-000-000.log")+file_contents("bulk10001-000-000.log"), file_contents("testoutput1.txt"));
  }
}

TEST(async, input2) {
  {
    std::filesystem::remove("bulk20000-000-000.log");
    std::filesystem::remove("bulk20001-001-000.log");
    std::filesystem::remove("bulk20002-001-000.log");
    EXPECT_EQ(get_is("testinput2.txt", 20000), get_must("testoutput2.txt"));
    EXPECT_EQ(
      file_contents("bulk20000-000-000.log")+file_contents("bulk20001-001-000.log")+file_contents("bulk20002-001-000.log"),
      file_contents("testoutput2.txt"));
  }
}


TEST(async, input_mix) {
  {

    std::filesystem::remove("bulk30000-000-000.log");
    std::filesystem::remove("bulk30001-000-000.log");
    std::filesystem::remove("bulk30002-001-000.log");
    std::filesystem::remove("bulk30003-000-000.log");
    std::filesystem::remove("bulk30004-000-000.log");
    std::filesystem::remove("bulk30005-000-000.log");
    std::filesystem::remove("bulk30006-000-000.log");
    std::filesystem::remove("bulk30007-000-000.log");
    std::filesystem::remove("bulk40000-002-000.log");
    std::filesystem::remove("bulk50000-003-000.log");

    time_t t1 = 30000;
    time_t t2 = 40000;
    time_t t3 = 50000;

    BulkAsync ba;
    auto *h1 = ba.connect(3, [&t1]() { return t1++; });
    auto *h2 = ba.connect(3, [&t2]() { return t2++; });
    auto *h3 = ba.connect(3, [&t3]() { return t3; });

    ba.receive(h1, "h1.str1\n");
    ba.receive(h2, "h2.str1\n");
    ba.receive(h3, "h3.str1\n");
    ba.receive(h1, "h1.str2\n{\nh1.str2.1\nh1.str2.2\nh1.str2.3\n}\n");
    ba.receive(h2, "h2.str2\n{\nh2.str2.1\nh2.str2.2\nh2.str2.3\n}\n");
    ba.receive(h3, "h3.str2\n{\nh3.str2.1\nh3.str2.2\nh3.str2.3\n}\n");
    ba.receive(h1, "h1.str3\n");
    ba.receive(h2, "h2.str3\n");
    ba.receive(h3, "h3.str3\n");

    ba.receive(h1, "h1.str4\n");
    ba.receive(h2, "h2.str4\n");
    ba.receive(h3, "h3.str4\n");
    ba.receive(h1, "h1.str5\n");
    ba.receive(h2, "h2.str5\n");
    ba.receive(h3, "h3.str5\n");

    ba.disconnect(h1);
    ba.disconnect(h2);
    ba.disconnect(h3);
    ba.closeAll();

    EXPECT_EQ(file_contents("bulk30000-000-000.log"), "bulk: h1.str1, h2.str1, h3.str1\n");
    EXPECT_EQ(file_contents("bulk30001-000-000.log"), "bulk: h1.str2\n");
    EXPECT_EQ(file_contents("bulk30002-001-000.log"), "bulk: h1.str2.1, h1.str2.2, h1.str2.3\n");
    EXPECT_EQ(file_contents("bulk40000-002-000.log"), "bulk: h2.str2.1, h2.str2.2, h2.str2.3\n");
    EXPECT_EQ(file_contents("bulk50000-003-000.log"), "bulk: h3.str2.1, h3.str2.2, h3.str2.3\n");
    EXPECT_EQ(file_contents("bulk30003-000-000.log"), "bulk: h2.str2\n");
    EXPECT_EQ(file_contents("bulk30004-000-000.log"), "bulk: h3.str2\n");
    EXPECT_EQ(file_contents("bulk30005-000-000.log"), "bulk: h1.str3, h2.str3, h3.str3\n");
    EXPECT_EQ(file_contents("bulk30006-000-000.log"), "bulk: h1.str4, h2.str4, h3.str4\n");
    EXPECT_EQ(file_contents("bulk30007-000-000.log"), "bulk: h1.str5, h2.str5, h3.str5\n");
  }
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
} // namespace
