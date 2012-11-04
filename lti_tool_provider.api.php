<?php

/**
 * @file
 * Hooks provided by the LTI Tool Provider module.
 */

/**
 * Implements hook_lti_tool_provider_launch().
 */
function hook_lti_tool_provider_launch() {
  /*
   *  Do stuff at the time of an LTI launch.
   * Invoked after user provisioned, authenticated and authorized,
   *   and course group provisioned, but before redirect to landing page.
   * LTI context variables are available in
   * $_SESSION['lti_tool_provider_context_info'].
   */
}

/**
 * Implements hook_lti_tool_provider_return().
 */
function hook_lti_tool_provider_return() {
  /*
   * Do stuff at the time of an LTI return.
   * Invoked before user logged out and session is destroyed.
   * LTI context variables are available in
   * $_SESSION['lti_tool_provider_context_info'].
   */
}
