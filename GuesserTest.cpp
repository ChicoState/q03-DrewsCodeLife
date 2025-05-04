/**
 * Unit Tests for the class
**/

#include <gtest/gtest.h>
#include "Guesser.h"

/* BEST PRACTICES (how to be a good boy)
1) Unit tests should serve as reliable regression tests
2) Unit tests should serve as documentation of expected behavior
3) Unit tests should test the behavior, not the current implementation 
4) A test suite should include both "happy" and "sad" tests 
5) Class/function design should not be altered just to make testing easier
6) Give each unit test a descriptive name 
7) Test only one case in each unit test (ambiguity is bad)
8) Unit test should focus on testing the results of one function if possible (over-protectiveness is bad)
9) Keep unit tests as simple as possible (complexity is bad)
*/

class GuesserTest : public ::testing::Test
{
	protected:
		GuesserTest(){} //constructor runs before each test
		virtual ~GuesserTest(){} //destructor cleans up after tests
		virtual void SetUp(){} //sets up before each test (after constructor)
		virtual void TearDown(){} //clean up after each test, (before destructor)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////// VALID PASSWORD ENTRY //////////

// User inputs valid password
TEST(GuesserTest, match_valid_no_lock)
{
  Guesser object("Secret");
  ASSERT_TRUE(object.match("Secret"));
}

// User inputs valid password after 1 failed attempt
TEST(GuesserTest, match_valid_with_repetition_1)
{
  Guesser object("Secret");
	object.match("Secrett");
  ASSERT_TRUE(object.match("Secret"));
}

// User inputs valid password after 2 failed attempts
TEST(GuesserTest, match_valid_with_repetition_2)
{
  Guesser object("Secret");
	object.match("Secrett");
	object.match("Secrett");
  ASSERT_TRUE(object.match("Secret"));
}

// User inputs valid password after 3 failed attempts (account is locked)
TEST(GuesserTest, match_valid_with_repetition_lock)
{
  Guesser object("Secret");
	object.match("Secrett");
	object.match("Secrett");
	object.match("Secrett");
  ASSERT_FALSE(object.match("Secret")); // Should now fail, the password is locked.
}

// User inputs valid password after inputting an invalid password that should have distance > 2
TEST(GuesserTest, match_valid_with_distance_lock)
{
  Guesser object("Secret");
	object.match("Secretttt"); // distance = len("Secret" - "Secretttt") = len("ttt") = 3 > 2, lock due to distance
  ASSERT_FALSE(object.match("Secret")); // Should now fail, the password is locked.
}

// User inputs one invalid password, enters valid password, enters two more invalid passwords, then enters valid password again
//  Checks that the repetition lock is properly reset after a valid password is input prior to a lock
TEST(GuesserTest, match_valid_early_voided_repetition_lock)
{
  Guesser object("Secret");
	object.match("Secrett");
  object.match("Secret"); // Should pass, repetition should be reset
  object.match("Secrett");
  object.match("Secrett");
  ASSERT_TRUE(object.match("Secret")); // If repetition was not reset, this will fail.
}

TEST(GuesserTest, match_valid_late_voided_repetition_lock)
{
  Guesser object("Secret");
	object.match("Secrett");
  object.match("Secrett");
  object.match("Secret"); // Should pass, repetition should be reset
  object.match("Secrett");
  ASSERT_TRUE(object.match("Secret")); // If repetition was not reset, this will fail.
}

// lock should not be lifted just because the user input a valid password
TEST(GuesserTest, match_valid_too_late_voided_repetition_lock)
{
  Guesser object("Secret");
	object.match("Secrett");
  object.match("Secrett");
  object.match("Secrett");
  object.match("Secret"); // Should fail, object is locked
  ASSERT_FALSE(object.match("Secret")); // If object was unlocked or not locked, this will fail
}

// Should be able to repeatedly enter the valid password,
//  as many times as desired, and it still succeeds.
TEST(GuesserTest, repeated_valid_match)
{
  Guesser object("Secret");
  int max_tries = 100;
  while (max_tries > 0) {
    object.match("Secret");
    max_tries--;
  }
  ASSERT_TRUE(object.match("Secret"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Base case, matched password is just wrong
TEST(GuesserTest, match_invalid_no_lock)
{
  Guesser object("Secret");
  ASSERT_FALSE(object.match("Secrett")); // Distance = 1, no lock
}

// Should fail, basic behavior
TEST(GuesserTest, match_invalid_once)
{
  Guesser object("Secret");
	object.match("Secrett");
  ASSERT_FALSE(object.match("Secrett")); // Should still be false
}

// Should still fail but now locks on the third attempt (silent)
TEST(GuesserTest, match_invalid_twice)
{
  Guesser object("Secret");
	object.match("Secrett");
	object.match("Secrett");
  ASSERT_FALSE(object.match("Secrett")); // Should still be false, now locks but after use
}

// False now not only because the password is wrong but additionally because it's been locked
TEST(GuesserTest, match_invalid_thrice)
{
  Guesser object("Secret");
	object.match("Secrett");
	object.match("Secrett");
	object.match("Secrett");
  ASSERT_FALSE(object.match("Secrett")); // Still false, but should be due to a lock now as well as the wrong answer
}

// Locks due to distance rather than repetition, should still fail.
TEST(GuesserTest, match_invalid_with_distance_lock)
{
  Guesser object("Secret");
	object.match("Secretttt"); // distance = len("Secret" - "Secretttt") = len("ttt") = 3 > 2, lock due to distance
  ASSERT_FALSE(object.match("Secrett")); // Should now fail, the password is locked & is wrong
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Other weird edge cases I thought of ///

// Never mentioned expected behavior of an empty password, but presumably it should work.
//  No minimum length is mentioned.
TEST(GuesserTest, empty_password)
{
  Guesser object("");
  ASSERT_TRUE(object.match(""));
}

// Minimal passwords should work
TEST(GuesserTest, one_letter_password)
{
  Guesser object("a");
  ASSERT_TRUE(object.match("a"));
}

// Passwords of exactly 32 characters should be accepted
TEST(GuesserTest, thirty_two_letter_password)
{
  Guesser object("aaaaaabbbbbbccccccddddddeeeeeeff");
  ASSERT_TRUE(object.match("aaaaaabbbbbbccccccddddddeeeeeeff"));
}

// Passwords of longer than 32 characters should be truncated
TEST(GuesserTest, thirty_two_letter_password_valid)
{
  Guesser object("aaaaaabbbbbbccccccddddddeeeeeeffffff");
  ASSERT_TRUE(object.match("aaaaaabbbbbbccccccddddddeeeeeeff")); // Should pass, first 32 characters
}

// Passwords of longer than 32 characters should be truncated
TEST(GuesserTest, thirty_two_letter_password_invalid_wrong_end)
{
  Guesser object("aaaaaabbbbbbccccccddddddeeeeeeffffff");
  ASSERT_FALSE(object.match("aabbbbbbccccccddddddeeeeeeffffff")); // Should fail, 'truncated' typically means the end, not the start.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Tests for Remaining() //

// Just a test for standard functionality (start with 3)
TEST(GuesserTest, remaining_valid_no_try)
{
  Guesser object("abcdefg");
  ASSERT_EQ(3, object.remaining()); // Should start with 3 tries left
}

// Just a test for standard functionality (3 left after successful)
TEST(GuesserTest, remaining_valid_first_try)
{
  Guesser object("abcdefg");
  object.match("abcdefg");
  ASSERT_EQ(3, object.remaining()); // Should have 3 tries left still
}

// Just a test for standard functionality (1 fail, 2 left)
TEST(GuesserTest, remaining_one_fail)
{
  Guesser object("abcdefg");
  object.match("abcdef");
  ASSERT_EQ(2, object.remaining()); // Should have 2 tries left still
}

// Just a test for standard functionality (2 fail, 1 left)
TEST(GuesserTest, remaining_two_fail)
{
  Guesser object("abcdefg");
  object.match("abcdef");
  object.match("abcdef");
  ASSERT_EQ(1, object.remaining()); // Should have 1 try left still
}

// Just a test for standard functionality (3 fail, 0 left)
TEST(GuesserTest, remaining_three_fail)
{
  Guesser object("abcdefg");
  object.match("abcdef");
  object.match("abcdef");
  object.match("abcdef");
  ASSERT_EQ(0, object.remaining()); // Should have no tries left
}

// Locked password due to distance, remaining guesses decrements to hide lock
TEST(GuesserTest, remaining_after_distance_lock_2_left)
{
  Guesser object("abcdefg");
  object.match("abcd");
  ASSERT_EQ(2, object.remaining()); // Still subtract 1 
}

// Locked password due to distance, remaining guesses decrements to hide lock
TEST(GuesserTest, remaining_after_distance_lock_1_left)
{
  Guesser object("abcdefg");
  object.match("abcd");
  object.match("abcd");
  ASSERT_EQ(1, object.remaining()); // Still subtract 2
}

// Locked password due to distance, remaining guesses decrements to hide lock
TEST(GuesserTest, remaining_after_distance_lock_0_left)
{
  Guesser object("abcdefg");
  object.match("abcd");
  object.match("abcd");
  object.match("abcd");
  ASSERT_EQ(0, object.remaining()); // Still subtract 3
}

// Testing for reset behavior, should always reset if not locked & match(guess) == true
TEST(GuesserTest, remaining_reset_one_fail)
{
  Guesser object("abcdefg");
  object.match("abcdef");
  object.match("abcdefg");
  ASSERT_EQ(3, object.remaining()); // Should reset to 3 remaining after successful match
}

// Testing for reset behavior, should always reset if not locked & match(guess) == true
TEST(GuesserTest, remaining_reset_two_fail)
{
  Guesser object("abcdefg");
  object.match("abcdef");
  object.match("abcdef");
  object.match("abcdefg");
  ASSERT_EQ(3, object.remaining()); // Should reset to 3 remaining after successful match
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Distance calculation tests //

// Guess 1 shorter than password
TEST(GuesserTest, distance_guess_less_pass_dist_1)
{
  Guesser object("abcdefg");
  object.match("abcdef");
  ASSERT_TRUE(object.match("abcdefg")); // Should pass, guess was only 1 shorter
}

// Guess 2 shorter than password
TEST(GuesserTest, distance_guess_less_pass_dist_2)
{
  Guesser object("abcdefg");
  object.match("abcde");
  ASSERT_TRUE(object.match("abcdefg")); // Should pass, guess was only 2 shorter
}

// Guess 3 shorter than password
TEST(GuesserTest, distance_guess_less_pass_dist_3)
{
  Guesser object("abcdefg");
  object.match("abcd");
  ASSERT_FALSE(object.match("abcdefg")); // Should fail, guess was 3 shorter, pass is locked
}

// Guess absurdly shorter than password
TEST(GuesserTest, distance_guess_way_less_pass)
{
  Guesser object("abcdefghijklmnopqrstuvabcdefghijklmnopqrstuv");
  object.match("abcdefg");
  ASSERT_FALSE(object.match("abcdefghijklmnopqrstuvabcdefghijklmnopqrstuv")); // Should fail, distance was massive on prior match
}

// Password 1 shorter than guess
TEST(GuesserTest, distance_pass_less_guess_dist_1)
{
  Guesser object("abcdefg");
  object.match("abcdefgh");
  ASSERT_TRUE(object.match("abcdefg")); // Should pass, guess was only 1 longer
}

// Password 2 shorter than guess
TEST(GuesserTest, distance_pass_less_guess_dist_2)
{
  Guesser object("abcdefg");
  object.match("abcdefghi");
  ASSERT_TRUE(object.match("abcdefg")); // Should pass, guess was only 2 longer
}

// Password 3 shorter than guess
TEST(GuesserTest, distance_pass_less_guess_dist_3)
{
  Guesser object("abcdefg");
  object.match("abcdefghij");
  ASSERT_FALSE(object.match("abcdefg")); // Should fail, guess was 3 longer, pass is locked
}

// Password absurdly shorter than guess
TEST(GuesserTest, distance_pass_way_less_guess)
{
  Guesser object("abcdefg");
  object.match("abcdefghijklmnopqrstuvabcdefghijklmnopqrstuv");
  ASSERT_FALSE(object.match("abcdefg")); // Should fail, distance was massive on prior match
}

// distance but only on characters not length //
TEST(GuesserTest, distance_char_only_1)
{
  Guesser object("aaaaaaaa");
  object.match("aaaaaaab");
  ASSERT_TRUE(object.match("aaaaaaaa")); // Should pass, last only had a distance of 1
}

// ^"" //
TEST(GuesserTest, distance_char_only_2)
{
  Guesser object("aaaaaaaa");
  object.match("aaaaaabb");
  ASSERT_TRUE(object.match("aaaaaaaa")); // Should pass, last only had a distance of 2
}

// ^"" //
TEST(GuesserTest, distance_char_only_3)
{
  Guesser object("aaaaaaaa");
  object.match("aaaaabbb");
  ASSERT_FALSE(object.match("aaaaaaaa")); // Should fail, last had a distance of 3
}

// From the .cpp implementation description: "in other words, if m_secret has a 10 characters and the guess has 100, the distance is 10."
// Best practice we don't write to the implementation, we write to the expected behavior. While I would disagree that the following behavior
//  is desired, it is precisely how the behavior is described. In theory, a password with a length of 0, 1, or 2 could never cause a distance-based
//  lock for any guess, since the max distance is len(m_secret) and len(m_secret) <= 2
TEST(GuesserTest, distance_short_password_0)
{
  Guesser object("");
  object.match("aaaaaaaaaaaaaaaa");
  ASSERT_TRUE(object.match("")); // Should pass, based on behavior description
}

// ^"" //
TEST(GuesserTest, distance_short_password_1)
{
  Guesser object("a");
  object.match("aaaaaaaaaaaaaaaa");
  ASSERT_TRUE(object.match("a")); // Should pass, based on behavior description
}

// ^"" //
TEST(GuesserTest, distance_short_password_2)
{
  Guesser object("aa");
  object.match("aaaaaaaaaaaaaaaa");
  ASSERT_TRUE(object.match("aa")); // Should pass, based on behavior description
}