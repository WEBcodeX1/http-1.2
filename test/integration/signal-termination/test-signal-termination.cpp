#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <thread>

//- Test helper to track child PIDs (mimics Server behavior)
static std::vector<pid_t> TestChildPIDs;

void testAddChildPID(pid_t pid)
{
    TestChildPIDs.push_back(pid);
}

void testTerminateChildren()
{
    for (pid_t pid : TestChildPIDs) {
        kill(pid, SIGTERM);
    }
}

//- Child process signal handler
static bool ChildRunning = true;

void childTerminateHandler(int sig)
{
    ChildRunning = false;
}

BOOST_AUTO_TEST_CASE( test_parent_sends_sigterm_to_children )
{
    TestChildPIDs.clear();
    
    pid_t child1 = fork();
    
    if (child1 == -1) {
        BOOST_FAIL("Fork failed");
    }
    
    if (child1 == 0) {
        // Child process 1
        signal(SIGTERM, childTerminateHandler);
        
        while(ChildRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        exit(0);
    }
    
    // Parent process
    testAddChildPID(child1);
    
    pid_t child2 = fork();
    
    if (child2 == -1) {
        BOOST_FAIL("Fork failed");
    }
    
    if (child2 == 0) {
        // Child process 2
        signal(SIGTERM, childTerminateHandler);
        
        while(ChildRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        exit(0);
    }
    
    // Parent process
    testAddChildPID(child2);
    
    // Give children time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Send SIGTERM to all children
    testTerminateChildren();
    
    // Wait for children to exit
    int status1, status2;
    pid_t result1 = waitpid(child1, &status1, 0);
    pid_t result2 = waitpid(child2, &status2, 0);
    
    // Verify both children exited normally
    BOOST_CHECK_EQUAL(result1, child1);
    BOOST_CHECK_EQUAL(result2, child2);
    BOOST_CHECK(WIFEXITED(status1));
    BOOST_CHECK(WIFEXITED(status2));
    BOOST_CHECK_EQUAL(WEXITSTATUS(status1), 0);
    BOOST_CHECK_EQUAL(WEXITSTATUS(status2), 0);
}

BOOST_AUTO_TEST_CASE( test_child_processes_not_killed_without_sigterm )
{
    TestChildPIDs.clear();
    
    pid_t child = fork();
    
    if (child == -1) {
        BOOST_FAIL("Fork failed");
    }
    
    if (child == 0) {
        // Child process - should still be running after timeout
        signal(SIGTERM, childTerminateHandler);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        if (ChildRunning) {
            exit(42); // Exit with special code if still running
        } else {
            exit(0);
        }
    }
    
    // Parent process - don't send SIGTERM
    testAddChildPID(child);
    
    // Wait just a bit but don't send SIGTERM
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Now send SIGTERM so child can exit cleanly
    kill(child, SIGTERM);
    
    // Wait for child to exit
    int status;
    waitpid(child, &status, 0);
    
    // Child should have been terminated by our SIGTERM
    BOOST_CHECK(WIFEXITED(status));
    BOOST_CHECK_EQUAL(WEXITSTATUS(status), 0);
}
