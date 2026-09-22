# CMake generated Testfile for 
# Source directory: /home/lyf67/Webserver
# Build directory: /home/lyf67/Webserver/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(http_test "/home/lyf67/Webserver/build/http_test")
set_tests_properties(http_test PROPERTIES  _BACKTRACE_TRIPLES "/home/lyf67/Webserver/CMakeLists.txt;29;add_test;/home/lyf67/Webserver/CMakeLists.txt;0;")
add_test(thread_pool_test "/home/lyf67/Webserver/build/thread_pool_test")
set_tests_properties(thread_pool_test PROPERTIES  _BACKTRACE_TRIPLES "/home/lyf67/Webserver/CMakeLists.txt;54;add_test;/home/lyf67/Webserver/CMakeLists.txt;0;")
add_test(notify_test "/home/lyf67/Webserver/build/notify_test")
set_tests_properties(notify_test PROPERTIES  TIMEOUT "10" _BACKTRACE_TRIPLES "/home/lyf67/Webserver/CMakeLists.txt;76;add_test;/home/lyf67/Webserver/CMakeLists.txt;0;")
add_test(static_file_test "/home/lyf67/Webserver/build/static_file_test")
set_tests_properties(static_file_test PROPERTIES  _BACKTRACE_TRIPLES "/home/lyf67/Webserver/CMakeLists.txt;91;add_test;/home/lyf67/Webserver/CMakeLists.txt;0;")
add_test(timer_test "/home/lyf67/Webserver/build/timer_test")
set_tests_properties(timer_test PROPERTIES  TIMEOUT "10" _BACKTRACE_TRIPLES "/home/lyf67/Webserver/CMakeLists.txt;103;add_test;/home/lyf67/Webserver/CMakeLists.txt;0;")
