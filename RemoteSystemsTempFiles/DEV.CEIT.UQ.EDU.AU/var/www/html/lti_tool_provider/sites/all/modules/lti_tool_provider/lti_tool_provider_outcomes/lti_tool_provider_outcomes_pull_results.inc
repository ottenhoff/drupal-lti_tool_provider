<?php

/**
 * @file
 * Contains all function related to retreiving the score from LMS site.
 *
 * The functions are defaulted to setting the score into their
 * respective outcome records. Modify the codes as commented if the
 * retreived scores not not to be set into the drupal site.
 */

/**
 * Sends all the results belonging to the current user back to the LMS site.
 *
 * Function invoked when url is accessed.
 *
 * @param string $return_url
 *   To customise return page when URL is accessed
 */
function lti_tool_provider_outcomes_pull_current_user_results($return_url = '') {
  global $user;

  // Consumer_id based on the context
  $consumer = lti_tool_provider_outcomes_get_consumer_by_key($_SESSION['lti_tool_provider_context_info']['oauth_consumer_key']);
  $uid = $user -> uid;
  $context_id = $_SESSION['lti_tool_provider_context_info']['context_id'];
  $key = $_SESSION['lti_tool_provider_context_info']['oauth_consumer_key'];
  $bool = TRUE;
  // RETRIEVE ALL RESULTS
  $outcomes_entities = lti_tool_provider_outcomes_retrieve_results_by_user($consumer -> lti_tool_provider_consumer_id, $uid, $context_id);
  if (isset($outcomes_entities)) {
    // Construct params
    $url = $consumer ->lti_tool_provider_consumer_outcomes_url;
    if (!isset($url)) {
      drupal_set_message(t('Unable to send results. No Outcome URL is avaliable', 'error'));
      drupal_goto($return_url);
    }
    else{
      foreach ($outcomes_entities as $outcome) {
        if ($consumer -> lti_tool_provider_consumer_ext) {
          $params = lti_tool_provider_outcomes_construct_params_array_read($outcome -> lti_tool_provider_outcomes_result_sourcedid);
          $response = lti_tool_provider_outcomes_lti_service_post($params , $consumer -> lti_tool_provider_consumer_key , $consumer -> lti_tool_provider_consumer_outcomes_url);
          $score = lti_tool_provider_outcomes_extract_ext_xml_score_reply($response);
          $success = isset($score) ? TRUE : FALSE;
        }
        else{
          $response = lti_tool_provider_outcomes_lti_service_xml($consumer , $consumer -> lti_tool_provider_consumer_outcomes_url , 2 , $outcome -> lti_tool_provider_outcomes_result_sourcedid);
          $score = lti_tool_provider_outcomes_extract_xml_score_reply($response);
          $success = isset($score) ? TRUE : FALSE;
        }
        if ($success) {
          // ONCE SENT , UPDATE TIMESTAMP
          lti_tool_provider_outcomes_updated_pulled_score($outcome , $score);
        }
        else{
          // UNABLE TO UPDATE
          // DISPLAY ERROR
          drupal_set_message(check_plain('Unable to retreive result ID:' . $outcome->lti_tool_provider_outcomes_id, 'error'));
          $bool = FALSE;
        }
      }
    }
    if ($bool) {
      drupal_set_message(t('All results has been retrieved'));
    }
    drupal_goto($return_url);
  }
  else{
    drupal_set_message(t('There is nothing to retrieve.'));
    drupal_goto($return_url);
  }
}

/**
 * Retreive results by resource in domain.
 *
 * Sends all the results belonging to the current resource under the logged
 * in user's domain back to the LMS site.
 *
 * Function invoked when url is accessed.
 *
 * @param string $return_url
 *   To customise return page when URL is accessed
 */
function lti_tool_provider_outcomes_pull_all_results_by_resource_for_domain($return_url = '') {
  global $user;
  // Consumer_id based on the context
  $consumer = lti_tool_provider_outcomes_get_consumer_by_key($_SESSION['lti_tool_provider_context_info']['oauth_consumer_key']);
  $resource_link_id = $_SESSION['lti_tool_provider_context_info']['resource_link_id'];
  $key = $_SESSION['lti_tool_provider_context_info']['oauth_consumer_key'];
  $bool = TRUE;
  // RETRIEVE ALL RESULTS
  $outcomes_entities = lti_tool_provider_outcomes_retrieve_results_by_resource_for_domain($consumer -> lti_tool_provider_consumer_id, $resource_link_id);
  if (isset($outcomes_entities)) {
    // Construct params
    $url = $consumer -> lti_tool_provider_consumer_outcomes_url ;
    if (!isset($url)) {
      drupal_set_message(t('Unable to send results .  No Outcome URL is avaliable', 'error'));
      drupal_goto($return_url);
    }
    else{
      foreach ($outcomes_entities as $outcome) {
        if ($consumer -> lti_tool_provider_consumer_ext) {
          $params = lti_tool_provider_outcomes_construct_params_array_read($outcome -> lti_tool_provider_outcomes_result_sourcedid);
          $response = lti_tool_provider_outcomes_lti_service_post($params , $consumer -> lti_tool_provider_consumer_key , $consumer -> lti_tool_provider_consumer_outcomes_url);
          $score = lti_tool_provider_outcomes_extract_ext_xml_score_reply($response);
          $success = isset($score) ? TRUE : FALSE;
        }
        else{
          $response = lti_tool_provider_outcomes_lti_service_xml($consumer , $consumer -> lti_tool_provider_consumer_outcomes_url , 2 , $outcome -> lti_tool_provider_outcomes_result_sourcedid);
          $score = lti_tool_provider_outcomes_extract_xml_score_reply($response);
          $success = isset($score) ? TRUE : FALSE;
        }
        if ($success) {
          // ONCE SENT,  UPDATE TIMESTAMP
          lti_tool_provider_outcomes_updated_pulled_score($outcome , $score);
        }
        else{
          // UNABLE TO UPDATE
          // DISPLAY ERROR
          drupal_set_message(check_plain('Unable to retreive result ID:' . $outcome->lti_tool_provider_outcomes_id, 'error'));
          $bool = FALSE;
        }
      }
    }
    if ($bool) {
      drupal_set_message(t('All results has been retrieved'));
    }
    drupal_goto($return_url);
  }
  else{
    drupal_set_message(t('There is nothing to retrieve.'));
    drupal_goto($return_url);
  }
}

/**
 * Retrieve results by domain.
 *
 * This function sends all the results under the logged in user's domain back to the LMS site.
 *
 * @param string $return_url
 *   To customise return page when URL is accessed
 */
function lti_tool_provider_outcomes_pull_all_results_by_domain($return_url = '') {
  global $user;
  // Consumer_id based on the context
  $consumer = lti_tool_provider_outcomes_get_consumer_by_key($_SESSION['lti_tool_provider_context_info']['oauth_consumer_key']);
  $key = $_SESSION['lti_tool_provider_context_info']['oauth_consumer_key'];
  $bool = TRUE;
  // RETRIEVE ALL RESULTS
  $outcomes_entities = lti_tool_provider_outcomes_retrieve_results_for_domain($consumer -> lti_tool_provider_consumer_id);
  if (isset($outcomes_entities)) {
    // Construct params
    $url = $consumer -> lti_tool_provider_consumer_id;

    if (!isset($url)) {
      drupal_set_message(t('Unable to send results .  No Outcome URL is avaliable', 'error'));
      drupal_goto($return_url);
    }
    else{
      foreach ($outcomes_entities as $outcome) {
        if ($consumer -> lti_tool_provider_consumer_ext) {
          $params = lti_tool_provider_outcomes_construct_params_array_read($outcome -> lti_tool_provider_outcomes_result_sourcedid);
          $response = lti_tool_provider_outcomes_lti_service_post($params , $consumer -> lti_tool_provider_consumer_key , $consumer -> lti_tool_provider_consumer_outcomes_url);
          $score = lti_tool_provider_outcomes_extract_ext_xml_score_reply($response);
          $success = isset($score) ? TRUE : FALSE;
        }
        else{
          $response = lti_tool_provider_outcomes_lti_service_xml($consumer , $consumer -> lti_tool_provider_consumer_outcomes_url , 2 , $outcome -> lti_tool_provider_outcomes_result_sourcedid);
          $score = lti_tool_provider_outcomes_extract_xml_score_reply($response);
          $success = isset($score) ? TRUE : FALSE;
        }
        if ($success) {
          // ONCE SENT , UPDATE TIMESTAMP
          lti_tool_provider_outcomes_updated_pulled_score($outcome , $score);
        }
        else{
          // UNABLE TO UPDATE
          // DISPLAY ERROR
          drupal_set_message(check_plain('Unable to retreive result ID:' . $outcome->lti_tool_provider_outcomes_id, 'error'));
          $bool = FALSE;
        }
      }
    }
    if ($bool) {
      drupal_set_message(t('All results has been retrieved'));
    }
    drupal_goto($return_url);
  }
  else{
    drupal_set_message(t('There is nothing to retrieve.'));
    drupal_goto($return_url);
  }
}
/**
 * Retrieves all results.
 *
 * The menu link is being commented. Use this function if only you
 * want a menu url to flush all the results back to the respective
 * LMS sites. Originally intended as a form of backup or restoration
 * of results on LMS sites.
 *
 * @param string $return_url
 *   To customise return page when URL is accessed
 */
function lti_tool_provider_outcomes_pull_all_results($return_url = '') {
  $bool = TRUE;
  // RETRIEVE ALL RESULTS
  $entities = lti_tool_provider_outcomes_retrieve_all_results();
  if (isset($entities)) {
    // Construct params
    foreach ($entities as $outcome) {
      $consumer = lti_tool_provider_outcomes_get_consumer_by_id($outcome -> lti_tool_provider_outcomes_consumer_id);
      if ($consumer -> lti_tool_provider_consumer_ext) {
        $params = lti_tool_provider_outcomes_construct_params_array_read($outcome -> lti_tool_provider_outcomes_result_sourcedid);
        $response = lti_tool_provider_outcomes_lti_service_post($params , $consumer -> lti_tool_provider_consumer_key , $consumer -> lti_tool_provider_consumer_outcomes_url);
        $score = lti_tool_provider_outcomes_extract_ext_xml_score_reply($response);
        $success = isset($score) ? TRUE : FALSE;
      }
      else{
        $response = lti_tool_provider_outcomes_lti_service_xml($consumer , $consumer -> lti_tool_provider_consumer_outcomes_url , 2 , $outcome -> lti_tool_provider_outcomes_result_sourcedid);
        $score = lti_tool_provider_outcomes_extract_xml_score_reply($response);
        $success = isset($score) ? TRUE : FALSE;
      }
      if ($success) {
        // ONCE SENT , UPDATE TIMESTAMP
        lti_tool_provider_outcomes_updated_pulled_score($outcome , $score);
      }
      else{
        // UNABLE TO UPDATE
        // DISPLAY ERROR
        drupal_set_message(check_plain('Unable to retreive result ID:' . $outcome->lti_tool_provider_outcomes_id, 'error'));
        $bool = FALSE;
      }
    }
    if ($bool) {
      drupal_set_message(t('All results has been retrieved'));
    }
    drupal_goto($return_url);
  }
  else{
    drupal_set_message(t('There is nothing to retrieve.'));
    drupal_goto($return_url);
  }
}