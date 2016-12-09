#!/bin/env bash

. test/assert.sh

assert_raises "./ifj16 test/0_class_cooperation.java" 0
assert_raises "./ifj16 test/0_assign.java" 0
assert_raises "./ifj16 test/0_block_continue.java" 0
assert_raises "./ifj16 test/0_cycle_continue_break.java" 0
assert_raises "./ifj16 test/0_declare.java" 0
assert_raises "./ifj16 test/0_fact_rekur.java" 0
assert_raises "./ifj16 test/0_fib_recurs.java" 0
assert_raises "./ifj16 test/0_for_run.java" 0
assert_raises "./ifj16 test/0_if.java" 0
assert_raises "./ifj16 test/0_length_cmp.java" 0
assert_raises "./ifj16 test/0_static_var_assign.java" 0
assert_raises "./ifj16 test/0_work_with_double.java" 0
assert_raises "./ifj16 test/0_condition_check.java" 0
assert_raises "echo 10 | ./ifj16 test/0_fact_iter.java" 0
assert_raises "./ifj16 test/0_loops_etest_cond.java" 0
assert_raises "./ifj16 test/0_return_var_declared.java" 0
assert_raises "echo abcdefgh | ./ifj16 test/0_String\&IALString.java" 0
assert_raises "./ifj16 test/0_sort_try.java" 0
assert_raises "./ifj16 test/0_variable_bothscopes.java" 0
assert_raises "printf '8\n12e-10\nabcdefghijklm' |./ifj16 test/0_builtin_func.java" 0
#assert "./ifj16 test/0_priority_operation.java" "50"
assert_raises "./ifj16 test/0_work_with_double.java" 0

assert_raises "./ifj16 test/1_unknown_char.java" 1
assert_raises "./ifj16 test/1_lex_double.java" 1
assert_raises "./ifj16 test/1_simple_iden.java" 1

assert_raises "./ifj16 test/2_badvalue_assign.java" 2
assert_raises "./ifj16 test/2_no_class.java" 2

assert_raises "./ifj16 test/3_assign_to_undec_var.java" 3
assert_raises "./ifj16 test/3_bad_definition_of_run.java" 3
assert_raises "./ifj16 test/3_class_redefinition.java" 3
assert_raises "./ifj16 test/3_ifj16_redefinition.java" 3
assert_raises "./ifj16 test/3_class_nomain.java" 3
assert_raises "./ifj16 test/3_function_nodefinition.java" 3
assert_raises "./ifj16 test/3_undefined_variable.java" 3
assert_raises "./ifj16 test/3_redefinition_of_function.java" 3
assert_raises "./ifj16 test/3_redefinition_in_func.java" 3
assert_raises "./ifj16 test/3_redefinition_of_var.java" 3
assert_raises "./ifj16 test/3_redefinition_of_static_var.java" 3

assert_raises "./ifj16 test/4_wrong_param_type.java" 4
assert_raises "./ifj16 test/4_wrong_param_number.java" 4
assert_raises "./ifj16 test/4_assign_bool.java" 4
assert_raises "./ifj16 test/4_equals.java" 4
assert_raises "./ifj16 test/4_plus.java" 4
assert_raises "./ifj16 test/4_binaryval_cmpwithint.java" 4

assert_raises "./ifj16 test/8_print_uninitialized_value.java" 8
assert_raises "./ifj16 test/8_uninit_var.java" 8
assert_raises "./ifj16 test/8_no_return.java" 8
assert_raises "./ifj16 test/9_division_by_zero.java" 9

assert_raises "echo abcd | ./ifj16 test/7_wrong_input_readDouble.java" 7
assert_raises "echo abcd | ./ifj16 test/7_wrong_input_readInt.java" 7

assert_raises "./ifj16 test/10_substr_start_over.java" 10
assert_raises "./ifj16 test/10_substr_read_over.java" 10

assert_raises "./ifj16" 99
assert_raises "./ifj16 test/99_noncycle_break.java" 99
assert_raises "./ifj16 test/99_noncycle_continue.java" 99
#exclude test
assert_raises "./test/symbol-table" 0
#assert_raises "./test/parser" 0

#assert for packed project
make dist
assert_raises "bash test/is_it_ok.sh xzaryb00.tgz is_ok" 0
rm -rf is_ok

assert_end
