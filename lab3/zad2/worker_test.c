#include "../../../../Unity/src/unity.h"
#include "matrix_worker.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_cmp(void){
    const int n_rows = 20;
    const int n_cols = 30;

    Matrix *first = malloc(sizeof(Matrix));
    first->matrix = malloc(n_rows * sizeof(int *));
    
    Matrix *second = malloc(sizeof(Matrix));
    second->matrix = malloc(n_rows * sizeof(int *));

    for(int r = 0; r < n_rows; r++){
        first->matrix[r] = malloc(n_cols * sizeof(int));
        second->matrix[r] = malloc(n_cols * sizeof(int));

        for(int c = 0; c < n_cols; c++){
            first->matrix[r][c] = second->matrix[r][c] = r*n_cols + c - 20;
        }
    }

    first->n_cols = second->n_cols = n_cols;
    first->n_rows = second->n_rows = n_rows;

    TEST_ASSERT_TRUE(cmp_matrices(first, second));

    free_matrix(first);
    free_matrix(second);
}


int main(void){
    UNITY_BEGIN();
    RUN_TEST(test_cmp);
    return UNITY_END();
}