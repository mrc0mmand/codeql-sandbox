/**
 * vi: sw=2 ts=2 et syntax=ql:
 *
 * Based on cpp/uninitialized-local.
 *
 * @name Potentially uninitialized local variable using the cleanup attribute
 * @description Running the cleanup handler on a possibly uninitialized variable
 *              is generally a bad idea.
 * @id cpp/uninitialized-local-with-cleanup
 * @kind problem
 * @problem.severity error
 * @precision high
 * @tags security
 */

import cpp
import semmle.code.cpp.controlflow.StackVariableReachability

/** Auxiliary predicate: List cleanup functions we want to explicitly ignore
  * since they don't do anything illegal even when the variable is uninitialized
  */
predicate cleanupFunctionDenyList(string fun) {
  fun = "erase_char"
}

/**
 * A declaration of a local variable using __attribute__((__cleanup__(x)))
 * that leaves the variable uninitialized.
 */
DeclStmt declWithNoInit(LocalVariable v) {
  result.getADeclaration() = v and
  not v.hasInitializer() and
  /* The variable has __attribute__((__cleanup__(...))) set */
  v.getAnAttribute().hasName("cleanup") and
  /* Check if the cleanup function is not on a deny list */
  not cleanupFunctionDenyList(v.getAnAttribute().getAnArgument().getValueText())
}

class UninitialisedLocalReachability extends StackVariableReachability {
  UninitialisedLocalReachability() { this = "UninitialisedLocal" }

  override predicate isSource(ControlFlowNode node, StackVariable v) { node = declWithNoInit(v) }

  /* Note: _don't_ use the `useOfVarActual()` predicate here (and a couple of lines
   * below), as it assumes that the callee always modifies the variable if
   * it's passed to the function.
   *
   * i.e.:
   * _cleanup_free char *x;
   * fun(&x);
   * puts(x);
   *
   * `useOfVarActual()` won't treat this an an uninitialized read even if the callee
   * doesn't modify the argument, however, `useOfVar()` will
   */
  override predicate isSink(ControlFlowNode node, StackVariable v) { useOfVar(v, node) }

  override predicate isBarrier(ControlFlowNode node, StackVariable v) {
    // only report the _first_ possibly uninitialized use
    useOfVar(v, node) or
    (
      /* If there's a return statement somewhere between the variable declaration
       * and a possible definition, don't accept is as a valid initialization.
       *
       * E.g.:
       * _cleanup_free_ char *x;
       * ...
       * if (...)
       *    return;
       * ...
       * x = malloc(...);
       *
       * is not a valid initialization, since we might return from the function
       * _before_ the actual iniitialization (emphasis on _might_, since we
       * don't know if the return statement might ever evaluate to true).
       */
      definitionBarrier(v, node) and
      not exists(ReturnStmt rs | v.getFunction() = rs.getEnclosingFunction() | rs.getLocation().isBefore(node.getLocation()))
    )
  }
}

from UninitialisedLocalReachability r, LocalVariable v, VariableAccess va
where r.reaches(_, v, va)
select va, "The variable $@ may not be initialized here, but has a cleanup handler.", v, v.getName()
