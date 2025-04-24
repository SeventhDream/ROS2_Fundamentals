/**
 * @file test_publisher.cpp
 * @brief Unit tests for the ROS 2 minimal publisher node.
 *
 * These tests verify the functionality of a minimal ROS 2 publisher including
 * node creation, message formatting, and publishing behavior.
 *
 * @author Addison Sears-Collins
 * @date November 6, 2024
 */

#define TESTING_EXCLUDE_MAIN
#include <gtest/gtest.h>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "../../src/minimal_cpp_publisher.cpp"
 
/**
 * @class TestMinimalPublisher
 * @brief Test fixture for the MinimalCppPublisher tests
 *
 * This fixture handles the setup and teardown of ROS 2 infrastructure
 * needed for each test case.
 */
class TestMinimalPublisher : public ::testing::Test 
{
protected:
    void SetUp() override
    {
        rclcpp::init(0, nullptr);
        node = std::make_shared<MinimalCppPublisher>();
    }
 
    void TearDown() override
    {
        node.reset();
        rclcpp::shutdown();
    }
 
    std::shared_ptr<MinimalCppPublisher> node;
};
 
/**
 * @test TestNodeCreation
 * @brief Verify the publisher node is created correctly
 *
 * Tests:
 * - Node name is set correctly
 * - Publisher exists on the correct topic
 * - Message type is correct
 */
TEST_F(TestMinimalPublisher, TestNodeCreation) 
{
    // Verify the node name is set correctly
    EXPECT_STREQ(node->get_name(), "minimal_cpp_publisher");
 
    // Get all publisher endpoints
    auto pub_endpoints = node->get_publishers_info_by_topic("/cpp_example_topic");
     
    // Verify we have exactly one publisher on the topic
    EXPECT_EQ(pub_endpoints.size(), 1u);
     
    // Verify the topic name and message type
    EXPECT_EQ(node->get_node_topics_interface()->resolve_topic_name("/cpp_example_topic"), "/cpp_example_topic");
    EXPECT_EQ(pub_endpoints[0].topic_type(), "std_msgs/msg/String");
}
 
/**
 * @test TestMessageContent
 * @brief Verify the published message content is correct
 *
 * Creates a subscription to capture published messages and verifies
 * the message format matches expectations.
 */
TEST_F(TestMinimalPublisher, TestMessageContent) 
{
    // Create a subscription to capture the published message
    std::shared_ptr<std_msgs::msg::String> received_msg;
    auto subscription = node->create_subscription<std_msgs::msg::String>(
        "/cpp_example_topic", 10,
        [&received_msg](const std_msgs::msg::String::SharedPtr msg) {
            received_msg = std::make_shared<std_msgs::msg::String>(*msg);
        });
 
    // Trigger the timer callback
    node->timerCallback();
 
    // Spin until we receive the message or timeout
    rclcpp::spin_some(node);
     
    // Verify the message format
    ASSERT_NE(received_msg, nullptr);
    EXPECT_EQ(received_msg->data, "Hello World! 0");
}
 
/**
 * @brief Main function to run all tests
 */
int main(int argc, char** argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}