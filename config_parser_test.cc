#include "gtest/gtest.h"
#include "config_parser.h"

/*****************************************************************************
    BASIC TESTS
*****************************************************************************/

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;
  EXPECT_TRUE(parser.Parse("example_config", &out_config));
}

TEST(NginxConfigParserTest, ToString) {
	NginxConfigStatement statement;
	statement.tokens_.push_back("foo");
	statement.tokens_.push_back("bar");
	EXPECT_EQ(statement.ToString(0), "foo bar;\n");
}

/*****************************************************************************
    PARSING STRINGS
*****************************************************************************/

class NginxStringConfigTest : public ::testing::Test {
protected:
	bool ParseString(const std::string config_string) {
		std::stringstream config_stream(config_string);
		return parser_.Parse(&config_stream, &out_config_);
	}
	NginxConfigParser parser_;
	NginxConfig out_config_;
};

TEST_F(NginxStringConfigTest, Statements) {
	EXPECT_TRUE(ParseString("foo bar;"));
	EXPECT_EQ(out_config_.statements_.size(), 1);
	EXPECT_EQ(out_config_.statements_.at(0)->tokens_.at(0), "foo");
	EXPECT_EQ(out_config_.statements_.at(0)->tokens_.at(1), "bar");
	EXPECT_EQ(out_config_.statements_.at(0)->child_block_,
		std::unique_ptr<NginxConfig>());
}

TEST_F(NginxStringConfigTest, InvalidConfig) {
	EXPECT_FALSE(ParseString("foo bar"));
}

TEST_F(NginxStringConfigTest, NestedConfig) {
	EXPECT_TRUE(ParseString("server { listen 80; }"));
	// TODO: test the contents of out_config_
}

TEST_F(NginxStringConfigTest, UnbalancedConfig) {
	EXPECT_FALSE(ParseString("server { listen 80;"));
}




