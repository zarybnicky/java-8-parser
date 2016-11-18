#!/bin/env bash

. test/assert.sh

assert_raises "./ifj16 test/1_unknown_char.java" 1
assert_raises "./ifj16 test/2_no_class.java" 2
assert_raises "./ifj16 test/3_class_redefinition.java" 3
assert_raises "./ifj16 test/3_ifj16_redefinition.java" 3
assert_raises "./ifj16 test/4_wrong_param_number.java" 4
#assert_raises "./ifj16 test/8_undefined_variable.java" 8
#assert_raises "./ifj16 test/9_division_by_zero.java" 9
#assert_raises "./ifj16 test/10_substr_start_over.java" 10
#assert_raises "./ifj16 test/11_substr_read_over.java" 10
#assert_raises "./ifj16 test/12_substr_ok.java" 0
assert_raises "./ifj16" 99
#assert_raises "./ifj16 test/0_fact_iter.java" 0
#exclude test
assert_raises "./test/symbol-table" 0
assert_raises "./test/parser" 0

#assert for packed project
make dist
assert_raises "bash test/is_it_ok.sh xzaryb00.tgz is_ok" 0
rm -rf is_ok
make clean

assert_end
