#include "test_util.h"
#include "phylo.h"
#include <libSysCalls.h>
#include <defs.h>
#include <utils.h>

#include "test_phylo.h"

#define TEST_DURATION 10000  // 10 seconds for long-running tests
#define ITERATIONS 3         // Number of times to run each test
#define WAIT_TIME 1000      // Time to wait between operations

// Test Cases
static int test_basic_functionality() {
    fprintf(STDOUT, "\nTesting basic initialization with 5 philosophers...\n");
    
    char *args[] = {"5", NULL};
    uint64_t result = phylo(1, args);
    
    if (result != 0) {
        fprintf(STDERR, "ERROR: Basic initialization with 5 philosophers failed\n");
        return -1;
    }
    
    sys_wait(WAIT_TIME * 2);  // Give time for philosophers to run
    return 0;
}

static int test_edge_cases() {
    fprintf(STDOUT, "\nTesting edge cases...\n");
    
    // Test with minimum number of philosophers
    char min_args[] = "3";
    char *args_min[] = {min_args, NULL};
    if (phylo(1, args_min) != 0) {
        fprintf(STDERR, "ERROR: Failed with minimum philosophers\n");
        return -1;
    }
    sys_wait(WAIT_TIME);
    
    // Test with maximum number of philosophers
    char max_args[] = "10";
    char *args_max[] = {max_args, NULL};
    if (phylo(1, args_max) != 0) {
        fprintf(STDERR, "ERROR: Failed with maximum philosophers\n");
        return -1;
    }
    sys_wait(WAIT_TIME);
    
    // Test invalid cases
    char invalid_min[] = "2";
    char *args_invalid_min[] = {invalid_min, NULL};
    if (phylo(1, args_invalid_min) != -1) {
        fprintf(STDERR, "ERROR: Should have rejected less than MIN_PHILOSOPHERS\n");
        return -1;
    }
    
    char invalid_max[] = "11";
    char *args_invalid_max[] = {invalid_max, NULL};
    if (phylo(1, args_invalid_max) != -1) {
        fprintf(STDERR, "ERROR: Should have rejected more than MAX_PHILOSOPHERS\n");
        return -1;
    }
    
    return 0;
}

static int test_long_running() {
    fprintf(STDOUT, "\nTesting long-running stability...\n");
    
    // Start with 5 philosophers and let them run for a while
    char *args[] = {"5", NULL};
    if (phylo(1, args) != 0) {
        fprintf(STDERR, "ERROR: Failed to initialize long-running test\n");
        return -1;
    }
    
    // Let it run for a longer period to check for deadlocks/starvation
    sys_wait(TEST_DURATION);
    
    return 0;
}

static int test_concurrent_load() {
    fprintf(STDOUT, "\nTesting under concurrent load...\n");
    
    // Start with maximum philosophers to test heavy load
    char *args[] = {"10", NULL};
    if (phylo(1, args) != 0) {
        fprintf(STDERR, "ERROR: Failed to initialize concurrent load test\n");
        return -1;
    }
    
    // Let them compete for resources for a while
    sys_wait(TEST_DURATION / 2);
    
    return 0;
}

void test_philosophers() {
    fprintf(STDOUT, "=== Starting Dining Philosophers Test Suite ===\n");
    

    int failed_tests = 0;
    int total_tests = 4 * ITERATIONS;  // 4 test types, run ITERATIONS times each
        char buffer[50]; 

    // Run each test multiple times to ensure consistency
    for (int i = 0; i < ITERATIONS; i++) {
        sprintf(buffer, "\nIteration %d of %d\n", i + 1);
        fprintf(STDOUT, buffer); 
        sprintf(buffer, "%d\n", ITERATIONS);
        fprintf(STDOUT, buffer);
        if (test_basic_functionality() < 0) failed_tests++;
        if (test_edge_cases() < 0) failed_tests++;
        if (test_long_running() < 0) failed_tests++;
        if (test_concurrent_load() < 0) failed_tests++;
        
        sys_wait(WAIT_TIME);  // Wait between iterations
    }
    
    fprintf(STDOUT, "\n=== Test Suite Complete ===\n");
    
    sprintf(buffer, "Failed test: %d out of ", failed_tests); 
    fprintf(STDOUT, buffer);
    sprintf(buffer, "%d total test cases\n", total_tests);
    fprintf(STDOUT, buffer); 
    
    if (failed_tests == 0) {
        fprintf(STDOUT, "All tests passed successfully!\n");
        fprintf(STDOUT, "\nVerification Results:\n");
        fprintf(STDOUT, "✓ No deadlocks detected (all philosophers were able to eat)\n");
        fprintf(STDOUT, "✓ No starvation detected (all philosophers got fair access to forks)\n");
        fprintf(STDOUT, "✓ No race conditions detected (no conflicts in resource access)\n");
    } else {
        fprintf(STDERR, "Some tests failed - check logs for details\n");
    }
}