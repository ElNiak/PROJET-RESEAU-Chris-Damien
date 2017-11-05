#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "receiver.h"
#include "sender.h"
#include "CUnit/Basic.h"

int setup(void)  { return 0; }
int teardown(void) { return 0; }

void fake_test(){

CU_ASSERT_EQUAL(5, 5);
}

int main(int argc, char *argv[]){

  CU_pSuite pSuite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  pSuite = CU_add_suite("Suite", setup, teardown);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  if ((NULL == CU_add_test(pSuite, "Test matrix init", test_matrix_init)) 
    ){
      CU_cleanup_registry();
      return CU_get_error();
    }

    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    CU_cleanup_registry();
    printf("\n");
    return CU_get_error();
    return 0;
}
