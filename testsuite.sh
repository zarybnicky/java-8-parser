#!/bin/env bash

. test/assert.sh

assert_raises "./ifj16 test/1_unknown_char.java" 1
#assert_raises "./ifj16 test/2_no_class.java" 2
#assert_raises "./ifj16 test/3_class_redefinition.java" 3
#assert_raises "./ifj16 test/3_ifj16_redefinition.java" 3
#assert_raises "./ifj16 test/4_wrong_param_number.java" 4
#assert_raises "./ifj16 test/8_undefined_variable.java" 8
#assert_raises "./ifj16 test/9_division_by_zero.java" 9
assert_raises "./ifj16" 99

assert_end
