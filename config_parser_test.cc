#include "gtest/gtest.h"
#include "config_parser.h"

/*****************************************************************************
    BASIC TESTS
*****************************************************************************/

// Tests basic config with a single statement
TEST(NginxConfigParserTest, ParseSimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;
  EXPECT_TRUE(parser.Parse("example_config", &out_config));
}

// Tests ToString method on simple config
TEST(NginxConfigParserTest, ToStringSimple) {
	NginxConfigStatement statement;
	statement.tokens_.push_back("foo");
	statement.tokens_.push_back("bar");
	EXPECT_EQ(statement.ToString(0), "foo bar;\n");
}

// Tests ToString method on config with a block
TEST(NginxConfigParserTest, ToStringBlock) {
	NginxConfigStatement statement;
	statement.tokens_.push_back("server");
	statement.tokens_.push_back("{");
	statement.tokens_.push_back("listen");
	statement.tokens_.push_back("80");
	statement.tokens_.push_back(";");
	statement.tokens_.push_back("}");
	EXPECT_EQ(statement.ToString(0), "server { listen 80 ; };\n");
}

/*****************************************************************************
    PARSING STRINGS
*****************************************************************************/

// Test fixture for parsing strings
class NginxStringConfigTest : public ::testing::Test {
protected:
	bool ParseString(const std::string config_string) {
		std::stringstream config_stream(config_string);
		return parser_.Parse(&config_stream, &out_config_);
	}
	NginxConfigParser parser_;
	NginxConfig out_config_;
};

// Verify config parser correctly separates a simple statement into tokens
TEST_F(NginxStringConfigTest, SimpleConfigValid) {
	EXPECT_TRUE(ParseString("foo bar;"));
	EXPECT_EQ(out_config_.statements_.size(), 1); // config has one statement
	EXPECT_EQ(out_config_.statements_.at(0)->tokens_.at(0), "foo");
	EXPECT_EQ(out_config_.statements_.at(0)->tokens_.at(1), "bar");
	EXPECT_EQ(out_config_.statements_.at(0)->child_block_,
		std::unique_ptr<NginxConfig>()); // child block is empty
}

// Statements must end in semicolons
TEST_F(NginxStringConfigTest, SimpleConfigInvalid) {
	EXPECT_FALSE(ParseString("foo bar"));
}

// Blocks with matching { and } brackets are valid
TEST_F(NginxStringConfigTest, BlockConfigValid) {
	EXPECT_TRUE(ParseString("server { listen 80; }"));
}

// Blocks with unmatching brackets are invalid
TEST_F(NginxStringConfigTest, BlockConfigInvalid) {
	EXPECT_FALSE(ParseString("server { listen 80;"));
	EXPECT_FALSE(ParseString("server listen 80;}"));
}

// Tests nested blocks
TEST_F(NginxStringConfigTest, NestedConfigValid) {
	EXPECT_TRUE(ParseString("server { foo {bar 80;}}"));
}

// Blocks that are incorrectly nested are invalid
TEST_F(NginxStringConfigTest, NestedConfigInvalid) {
	EXPECT_FALSE(ParseString("server { foo {bar 80;}"));
	EXPECT_FALSE(ParseString("server { foo } bar 80;{"));
	EXPECT_FALSE(ParseString("} foo { bar 80;}{"));
}

// Comments following a statement on the same line are valid
TEST_F(NginxStringConfigTest, CommentOnLine) {
	EXPECT_TRUE(ParseString("foo bar; # this is a comment."));
	EXPECT_EQ(out_config_.statements_.size(), 1); // config has one statement
}

// Comments on separate lines are ignored by the parser
TEST_F(NginxStringConfigTest, CommentOnLineAfter) {
	EXPECT_TRUE(ParseString("foo bar; \n # this is a comment."));
	EXPECT_EQ(out_config_.statements_.size(), 1); // config still has one statement
}

// Comments on separate lines are ignored by the parser
TEST_F(NginxStringConfigTest, CommentOnLineBefore) {
	EXPECT_TRUE(ParseString("# this is a comment. \n foo bar;"));
	EXPECT_EQ(out_config_.statements_.size(), 1); // config has one statement
}

// A single comment in the file will result in a failed parsing
// because there are no statements in the file
TEST_F(NginxStringConfigTest, CommentAlone) {
	EXPECT_FALSE(ParseString("# this is a comment."));
	EXPECT_EQ(out_config_.statements_.size(), 0);
}




