#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
//#include "CUnit/Automated.h"
//#include "CUnit/Console.h"
#include "../src/create_socket.h"
#include "../src/packet_interface.h"
#include "../src/real_address.h"
#include "../src/various.h"
#include "../src/wait_for_client.h"
#include "../src/read_write_loop.h"
#include "../src/sender.h"
#include "../src/receiver.h"
#include <stdio.h>  // for printf
#include <stdlib.h>

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

void test_real_address(void){  //RAJOUTER TEST POUR 2e set pour verifier
  const char * str1 = "::1";
  const char * str2 = "google.com";
  struct sockaddr_in6 addr;
  struct sockaddr_in6 addr2;
  struct sockaddr_in6 addr3;

  const char * res1 = real_address(str1,&addr);
  const char * res2 = real_address(str2,&addr2);
  const char * res3 = real_address(NULL,&addr3);

  CU_ASSERT_PTR_NULL(res1);
  CU_ASSERT_PTR_NULL(res2);
  CU_ASSERT_PTR_NOT_NULL(res3);
}

void test_create_socket(void){
  struct sockaddr_in6 source_addr;
  struct sockaddr_in6 dst_addr;
  const char res1 = real_address("::1",&source_addr);
  const char res2 = real_address("::2",&dst_addr);
  CU_ASSERT_PTR_NULL(res1);
  CU_ASSERT_PTR_NULL(res2);

  int err = create_socket(&source_addr,1,&dst_addr,1);
  CU_ASSERT_NOT_EQUAL(err,-1);
}

void test_wait_for_client(void){
  struct sockaddr_in6 source_addr;
  struct sockaddr_in6 dst_addr;

  const char res1 = real_address("::1",&source_addr);
  const char res2 = real_address("::2",&dst_addr);
  CU_ASSERT_PTR_NULL(res1);
  CU_ASSERT_PTR_NULL(res2);

  int err = create_socket(&source_addr,1,&dst_addr,1);

  int err2 = wait_for_client(err);
  CU_ASSERT_NOT_EQUAL(err2,-1);
}

void test_read_write_loop(void){
  struct sockaddr_in6 source_addr;
  struct sockaddr_in6 dst_addr;

  const char res1 = real_address("::1",&source_addr);
  const char res2 = real_address("::2",&dst_addr);
  CU_ASSERT_PTR_NULL(res1);
  CU_ASSERT_PTR_NULL(res2);

  int err = create_socket(&source_addr,1,&dst_addr,1);

  int err2 = wait_for_client(err);

  read_write_loop(err);
}



 int main ( void )
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite( "matrix_test_suite", init_suite, clean_suite );
   if ( NULL == pSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ( (NULL == CU_add_test(pSuite, "test_real_address", test_real_address)) ||
        (NULL == CU_add_test(pSuite, "test_create_socket", test_create_socket)) ||
        (NULL == CU_add_test(pSuite, "test_wait_for_client", test_wait_for_client)) ||
        (NULL == CU_add_test(pSuite, "test_read_write_loop", test_read_write_loop))
      )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // Run all tests using the basic interface
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   printf("\n");
   CU_basic_show_failures(CU_get_failure_list());
   printf("\n\n");
/*
   // Run all tests using the automated interface
   CU_automated_run_tests();
   CU_list_tests_to_file();

   // Run all tests using the console interface
   CU_console_run_tests();
*/
   /* Clean up registry and return */
   CU_cleanup_registry();
   return CU_get_error();
 }
