#!/bin/env bash

# testing on memory leaks

valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_class_cooperation.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_assign.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_block_continue.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_cycle_continue_break.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_declare.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_fact_rekur.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_fib_recurs.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_for_run.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_if.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_length_cmp.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_static_var_assign.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_work_with_double.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_condition_check.java
valgrind --tool=memcheck --leak-check=full printf '10' | ./ifj16 test/0_fact_iter.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_loops_etest_cond.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_return_var_declared.java
valgrind --tool=memcheck --leak-check=full printf abcdefgh | ./ifj16 test/0_String\&IALString.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_sort_try.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_variable_bothscopes.java
valgrind --tool=memcheck --leak-check=full printf '8\n12e-10\nabcdefghijklm' |./ifj16 test/0_builtin_func.java
#assert "./ifj16 test/0_priority_operation.java" "50"
valgrind --tool=memcheck --leak-check=full ./ifj16 test/0_work_with_double.java

valgrind --tool=memcheck --leak-check=full ./ifj16 test/1_unknown_char.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/1_lex_double.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/1_simple_iden.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/1_badvalue_assign.java

valgrind --tool=memcheck --leak-check=full ./ifj16 test/2_no_class.java

valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_assign_to_undec_var.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_bad_definition_of_run.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_class_redefinition.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_ifj16_redefinition.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_class_nomain.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_function_nodefinition.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_undefined_variable.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_redefinition_of_function.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_redefinition_in_func.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_redefinition_of_var.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/3_redefinition_of_static_var.java

valgrind --tool=memcheck --leak-check=full ./ifj16 test/4_wrong_param_type.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/4_wrong_param_number.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/4_assign_bool.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/4_equals.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/4_plus.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/4_binaryval_cmpwithint.java

valgrind --tool=memcheck --leak-check=full ./ifj16 test/8_print_uninitialized_value.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/8_uninit_var.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/8_no_return.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/9_division_by_zero.java

valgrind --tool=memcheck --leak-check=full printf abcd | ./ifj16 test/7_wrong_input_readDouble.java
valgrind --tool=memcheck --leak-check=full printf abcd | ./ifj16 test/7_wrong_input_readInt.java

valgrind --tool=memcheck --leak-check=full ./ifj16 test/10_substr_start_over.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/10_substr_read_over.java

valgrind --tool=memcheck --leak-check=full ./ifj16
valgrind --tool=memcheck --leak-check=full ./ifj16 test/99_noncycle_break.java
valgrind --tool=memcheck --leak-check=full ./ifj16 test/99_noncycle_continue.java
