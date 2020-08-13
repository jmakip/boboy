#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "unity.h"
#include "mem.h"


void setUp(void)
{
  /* This is run before EACH TEST */
}

void tearDown(void)
{
}


void test_Simple(void)
{
  /* All of these should pass */
  TEST_ASSERT_EQUAL(0, 0);
}

//EXAMPLE MOCK sys call wrapped with linker
int __wrap_execv(const char *path, char *const argv[])
{
  //this call should only return if error has occured
  errno = EINVAL;
  return -1;
}
extern void __real_exit(int status);
//EXAMPLE MOCK sys call wrapped with linker
void __wrap_exit(int status) 
{
  //even need to wrap exit if error_behavior is >0
  printf("MOCK: exit() called\n");
  //if (!error_behavior) __real_exit(status);
}

//MOCK
void irq_request(uint8_t val )
{
    val = val;
}
//MOCK
void print_reg()
{

}

void test_mem_read()
{
    uint8_t tmp;
    mem_write(0x0000, 0x55);
    tmp = mem_read(0x0000);
    TEST_ASSERT_EQUAL(tmp, 0x55);

    mem_write16(0x0000, 0x0055);
    tmp = mem_read(0x0000);
    TEST_ASSERT_EQUAL(tmp, 0x55);
    tmp = mem_read(0x0001);
    TEST_ASSERT_EQUAL(tmp, 0x00);
}
