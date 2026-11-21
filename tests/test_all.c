#include "test_dijkstra.c"
#include "test_localsearch.c"
#include "test_meta.c"
#include "test_ranking.c"
#include "test_spanning_tree.c"
#include "test_wsp.c"

int tests_run = 0;

static const char *all_tests() {
    test_wsp();
    test_dijkstra();
    test_spanning_tree();
    test_localsearch();

    test_ranking();
    // test_meta.c
    test_meta();

    return 0;
}

int main() {
    const char *result = all_tests();
    if (result != 0) {
        printf("FAILED: %s\n", result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
