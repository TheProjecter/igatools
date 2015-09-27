# Testing policy #
We divide our tests in two categories:
  * Unit test
  * Consistency test

## Unit tests ##
Each new feature that is added to the library should provide some unit tests that will be included in the library testsuite.
As a matter of fact we use and encourage the test driven development (TDD) model.
First write a test that uses the feature that wants to be included and then add the feature itself.

Each test is a small monolithic program that generates a text output file.
After the output file is checked and accepted as correct it becomes the **"expected"** output for this test.

Every time the library is modified the test suite is run and the output generated by the tests is compared with the expected output.
This allows to catch in an automatic way undesired effects of  the changes just introduced.

The unit tests should:
  * be simple, small and easy to understand by any developer;
  * take little time to run.


### Running the test suite ###
After the library has been installed
```
cd $IGATOOLS_WS/igatools_lib/tests
ctest -jN
```

### Failing tests ###
A test may fail at three levels:
  * compile/link time error
  * runtime error
  * different than expected output error

For the purpose of illustrating the process to find the cause of the failure we assume that the failing test is base/tensor\_multiplication02.cpp.

When running ctest (without any addition option), the only information shown is that the test failed. To find out the reason we need to re run it with the following command:
```
ctest -V -R base-tensor_multiplication_02
```
This produces a verbose output that will identify the reason for the failing, you need to carefully read the information printed on the screen.


### Adding a new test ###
  1. First, in the tests directory, decide and select a pertinent subdirectory for your test. For example if you are creating a second test to check tensor multiplication a reasonable name for your test would be tensor\_multiplication02.cpp and a reasonable subdirectory for your test would be tests/base.
  1. Then you need to create a monolithic file for your test with the .cpp extension and a directory with the same name (and no extension), for example in the previous case we would do
```
cd tests/base
mkdir tensor_multiplication02
emacs tensor_multiplication02.cpp
```
  1. At this point to be able to execute the new test using the testsuite framework it is necessary to re-configure
```
cd $IGATOOLS_WS/igatools_lib
cmake  $IGATOOLS_SOURCE
```
  1. The tests programs should create some text output. This output must be generated with the specialized output stream object out.
  1. The test should minimize (nothing if possible)  any additional output (to the console for example)
  1. The test source code should follow the  general structure:
```
/*
 *  Test for ...
 *  
 *  author: ....
 *  date: Feb 2, 2013
 *
 */

  #include "../tests.h"

  //The specific test includes
  
  int main ()
  {
 
    out.depth_console(10); //to be removed after test finished

    //The test itself with some out <<

    return 0;
  }
```
  1. Once you are happy with the output of your new test you need to set it as the expected output for that test. This is done by copying output.txt onto expected.txt
```
cp output.txt $IGATOOLS_SOURCE/tests/base/tensor_multiplication02/expected.txt
```
  1. If your test requires to read some input file (not really encourage) you should put it in your test directory. For now only xml files are allowed.