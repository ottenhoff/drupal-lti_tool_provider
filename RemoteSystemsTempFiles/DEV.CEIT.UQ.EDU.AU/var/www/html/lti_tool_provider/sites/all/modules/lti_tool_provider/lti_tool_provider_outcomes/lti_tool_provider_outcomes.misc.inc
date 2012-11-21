<?php
/**
 * @file
 *
 * Contains functions for minor operations like retreiving
 * name of consumer etc.
 *
 */

/**
 * Validate the score if it conforms to the selected datatype.
 *
 * @param string $datatype
 *   The selected datatype
 * @param string $score
 *   The input score
 *
 * @return boolean
 *   Success or failure
 */
function lti_tool_provider_outcomes_validate_score($datatype, $score) {
  try{
    $value = floatval($score);
  }catch(Exception $e) {
    return FALSE;
  }
  if (strcasecmp($datatype, 'decimal') == 0) {
    if ($value >= 0 && $value <= 1.0 ) {
      return TRUE;
    }
  }
  elseif (strcasecmp($datatype, 'percentage') == 0) {
    if ($value >= 0 && $value <= 100 && is_numeric($score)) {
      if (!strpos($value, '.')) {
        return TRUE;
      }
    }
  }
  elseif (strcasecmp($datatype, 'ratio') == 0) {
    $slash_pos = strpos($score, '/');
    if ($slash_pos) {
      $first_string = drupal_substr($score, 0, $slash_pos);
      $new_start_index = ++$slash_pos;
      $second_string = drupal_substr($score, $new_start_index);
      if (is_numeric($first_string) && is_numeric($second_string)) {
        $pos_first = strpos($first_string, '.');
        $pos_second = strpos($second_string, '.');
        if (!$pos_first && !$pos_second) {
          return TRUE;
        }
      }
    }
  }
  elseif (strcasecmp($datatype, 'letteraf') == 0) {
    if (drupal_strlen($score) == 1 && ctype_alpha($score)) {
      return TRUE;
    }
  }
  elseif (strcasecmp($datatype, 'letterafplus') == 0) {
    if (drupal_strlen($score) == 1) {
      if (ctype_alpha($score)) {
        return TRUE;
      }
    }
    elseif (drupal_strlen($score) == 2) {
      $first_letter = drupal_substr($score, 0, 1);
      $second_letter = drupal_substr($score, 1, 1);
      if ((($first_letter == '-' || $first_letter == '+') && ctype_alpha($second_letter)) xor (($second_letter == '-' || $second_letter == '+') && ctype_alpha($first_letter))) {
        return TRUE;
      }
    }
  }
  elseif (strcasecmp($datatype, 'passfail') == 0) {
    if (strcasecmp($score, 'pass') xor strcasecmp($score, 'fail') ) {
      return TRUE;
    }
  }
  elseif (strcasecmp($datatype, 'freetext') == 0) {
    if (drupal_strlen($score) <= 1024 ) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Validates the name of the consumer.
 *
 * Check if it exists and is valid.
 *
 * @param string $name
 *   Consumer Entity name
 *
 * @return boolean
 *   Success or failure
 */
function lti_tool_provider_outcomes_validate_consumer_name($name) {
  $results_arr = array();
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_consumer')
  ->propertyCondition('lti_tool_provider_consumer_consumer',  $name);
  $result = $query->execute();
  $entity_type = 'lti_tool_provider_consumer';
  if (empty($result[$entity_type])) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

/**
 * Check if there is any existing records in the database.
 *
 * This is typically used to alter the display for the grades mapping,
 * such that an approriate content will be displayed.
 *
 * @return boolean
 *   Success or failure.
 */
function lti_tool_provider_outcomes_resource_is_empty() {
  $results_arr = array();
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_outcomes_resource');
  $result = $query->execute();
  $entity_type = 'lti_tool_provider_outcomes_resource';
  if (!empty($result[$entity_type])) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

/**
 * Retrieve the Consumer Entity by name.
 *
 * @param string $name
 *   Consumer entity name.
 *
 * @return object $entity
 *   Consumer_Entity.
 */
function lti_tool_provider_outcomes_get_consumer_by_name($name) {
  $results_arr = array();
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_consumer')
  ->propertyCondition('lti_tool_provider_consumer_consumer',  $name);
  $result = $query->execute();
  $entity_type = 'lti_tool_provider_consumer';
  if (!empty($result[$entity_type])) {
    $entities = entity_load($entity_type,  array_keys($result[$entity_type]));
    foreach ($entities as $entity) {
      return $entity;
    }
  }
}

/**
 * Retrieve lti_tool_provider_consumer entity key by id.
 *
 * @param int $consumer_id
 *   Consumer entity id.
 *
 * @return string $key
 *   Consumer key.
 */
function lti_tool_provider_outcomes_get_consumer_key($consumer_id) {
  $results_arr = array();
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_consumer')
  ->propertyCondition('lti_tool_provider_consumer_id',  $consumer_id);
  $result = $query->execute();
  $entity_type = 'lti_tool_provider_consumer';
  if (!empty($result[$entity_type])) {
    $outcomes = entity_load($entity_type,  array_keys($result[$entity_type]));
    $key = NULL;
    foreach ($outcomes as $outcome) {
      $key = $outcome->lti_tool_provider_consumer_key;
    }
    return $key;
  }
}

/**
 * Retrieves the consumer entity via its consumer id.
 *
 * @param int $consumer_id
 *   Consumer entity id.
 *
 * @return object
 *   Consumer entity, null if record is unable to be retrieved.
 */
function lti_tool_provider_outcomes_get_consumer_by_id($consumer_id) {
  if (isset($consumer_id)) {
    $entities = entity_load('lti_tool_provider_consumer', array($consumer_id));
    foreach ($entities as $entity) {
      $entity_final = $entity;
    }
    if ( isset($entity_final) ) {
      return $entity_final;
    }
    else {
      return NULL;
    }
  }
  else{
    return NULL;
  }
}

/**
 * Retrieves the lis outcome service url for the particular consumer.
 *
 * @param int $consumer_id
 *   Consumer entity id.
 *
 * @return string
 *   The lis outcome service url.
 */
function lti_tool_provider_outcomes_get_outcomes_url($consumer_id) {
  $results_arr = array();
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_consumer')
  ->propertyCondition('lti_tool_provider_consumer_id',  $consumer_id);
  $result = $query->execute();
  $entity_type = 'lti_tool_provider_consumer';
  if (!empty($result[$entity_type])) {
    $outcomes = entity_load($entity_type,  array_keys($result[$entity_type]));
    $url = NULL;
    foreach ($outcomes as $outcome) {
      $url = $outcome->lti_tool_provider_consumer_outcomes_url;
    }
    return $url;
  }
}

/**
 * Retreives the current outcome record
 *
 * @param string $result_sourcedid
 *   Current result source did
 * @param string $context_id
 *   Current session context id
 * @param string $resource_link_id
 *   Resource link id of the current session
 * @param string $consumer_id
 *   Consumer entity id
 * @param int $uid
 *   Current user id
 *
 * @return object
 *   Outcome entity
 */
function lti_tool_provider_outcomes_get_current_outcome($result_sourcedid, $context_id, $resource_link_id, $consumer_id, $uid) {
  $resource = lti_tool_provider_outcomes_get_resource($consumer_id, $context_id, $resource_link_id);
  if (isset($resource)) {
    $query =new EntityFieldQuery();
    $query
    ->entityCondition('entity_type',  'lti_tool_provider_outcomes')
    ->propertyCondition('lti_tool_provider_outcomes_user_id',  $uid)
    ->propertyCondition('lti_tool_provider_outcomes_resource_entity_id_fk',  $resource -> lti_tool_provider_outcomes_resource_id)
    ->propertyCondition('lti_tool_provider_outcomes_result_sourcedid',  $result_sourcedid);
    $result = $query->execute();
    $entity_type = 'lti_tool_provider_outcomes';
    if (!empty($result[$entity_type])) {
      $outcomes = entity_load($entity_type,  array_keys($result[$entity_type]));
      return $outcomes;
    }
    else{
      drupal_set_message(t('No records found'));
    }
  }
  else{
    drupal_set_message(t('No records found'));
  }
}

/**
 * Retrieves Resource records via the Consumer Id.
 *
 * @param int $consumer_id
 *   Consumer entity id.
 *
 * @return array
 *   Resource_Entities. All the retrieved entities.
 */
function lti_tool_provider_outcomes_retrieves_resource_records($consumer_id) {
  $results_arr = array();
  $query =new EntityFieldQuery();
  if (isset($consumer_id)) {
    $query
    ->entityCondition('entity_type',  'lti_tool_provider_outcomes_resource')
    ->propertyCondition('lti_tool_provider_outcomes_resource_consumer_id',  $consumer_id);
  }
  else {
    $query
    ->entityCondition('entity_type',  'lti_tool_provider_outcomes_resource');
  }
  $result = $query->execute();
  $entity_type = 'lti_tool_provider_outcomes_resource';
  if (!empty($result[$entity_type])) {
    $resources = entity_load($entity_type,  array_keys($result[$entity_type]));
    $loopcounter = 0;
    $arr = array();
    foreach ($resources as $resource) {
      $arr[$loopcounter] = $resource;
      $loopcounter ++;
    }
    return $arr;
  }
  else {
    return NULL;
  }
}

/**
 * Retrieves Resource records via the Consumer Id.
 *
 * @param int $consumer_id
 *   Consumer entity id.
 *
 * @return array
 *   Resource_Entities. All the retrieved entities.
 */
function lti_tool_provider_outcomes_retrieves_resource_records_by_context($consumer_id, $context_id) {
  $results_arr = array();
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_outcomes_resource')
  ->propertyCondition('lti_tool_provider_outcomes_resource_consumer_id',  $consumer_id)
  ->propertyCondition('lti_tool_provider_outcomes_resource_context_id',  $context_id);
  $result = $query->execute();
  $entity_type = 'lti_tool_provider_outcomes_resource';
  if (!empty($result[$entity_type])) {
    $resources = entity_load($entity_type,  array_keys($result[$entity_type]));
    return $resources;
  }
  else {
    return NULL;
  }
}

/**
 * Retrieve the resource title for display, based on its id.
 *
 * @param int $resource_id
 *   Resource id of the resource record.
 *
 * @return string
 *   The resource link title.
 */
function lti_tool_provider_outcomes_retrieves_resource_title($resource_id) {
  $results_arr = array();
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_outcomes_resource')
  ->propertyCondition('lti_tool_provider_outcomes_resource_id',  $resource_id);
  $result = $query->execute();
  $entity_type = 'lti_tool_provider_outcomes_resource';
  if (!empty($result[$entity_type])) {
    $resources = entity_load($entity_type,  array_keys($result[$entity_type]));
    foreach ($resources as $resource) {
      return $resource -> lti_tool_provider_outcomes_resource_resource_link_title;
    }
  }
  else {
    return NULL;
  }
}

/**
 * Retrieve all corresponding outcomes
 *
 * @param int $resource_id
 *   Resource id of the resource record.
 *
 * @return array
 *   Outcomes entities
 */
function lti_tool_provider_outcomes_retrieves_outcomes_by_resource($resource_id) {
  $results_arr = array();
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_outcomes')
  ->propertyCondition('lti_tool_provider_outcomes_resource_entity_id_fk',  $resource_id);
  $result = $query->execute();
  $entity_type = 'lti_tool_provider_outcomes';
  if (!empty($result[$entity_type])) {
    $outcomes = entity_load($entity_type,  array_keys($result[$entity_type]));
    return $outcomes;
  }
  else {
    return NULL;
  }
}

/**
 * Retrieve the associated Datatype of the resource record.
 *
 * @param int $resource_id
 *   Resource entity id.
 *
 * @return string
 *   Datatype associated to that resource.
 */
function lti_tool_provider_outcomes_get_datatype_by_resource($resource_id) {
  // return the selected datatype
  $results_arr = array();
  $query = new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_outcomes_resource')
  ->propertyCondition('lti_tool_provider_outcomes_resource_id',  $resource_id);
  $result = $query->execute();
  if (isset($result['lti_tool_provider_outcomes_resource'])) {
    $outcome = reset(entity_load('lti_tool_provider_outcomes_resource',  array_keys($result['lti_tool_provider_outcomes_resource'])));
    return $outcome->lti_tool_provider_outcomes_resource_score_datatype;
  }
  drupal_set_message(t('Resource not found.'), 'error');
  return '';
}

/**
 * Checks update timestamp.
 *
 * Validates if the timestamp is set correctly and returns
 * the approriate string of display.
 *
 * @param string $timestamp
 *   Timestamp of the record.
 *
 * @return string
 *   Message.
 */
function lti_tool_provider_outcomes_trans_last_updated($timestamp) {
  if (isset($timestamp)) {
    return format_date($timestamp);
  }
  else{
    return 'Yet to be updated';
  }
}

/**
 * Validates if the timestamp(for date of result sent).
 *
 * Check if its set correctly and returns the approriate string of display.
 *
 * @param string $timestamp
 *   Timestamp of record.
 *
 * @return string
 *   Message.
 */
function lti_tool_provider_outcomes_trans_last_sent($timestamp) {
  if (isset($timestamp)) {
    return format_date($timestamp);
  }
  else{
    return 'Score yet to be sent.';
  }
}

/**
 * Updates the Outcome Service Url of the Consumer Record.
 *
 * @param string $key
 *   Consumer key.
 * @param string $url
 *   Consumer outcomes url.
 * @param boolean $ext_bool
 *   Extension check.
 */
function lti_tool_provider_outcomes_update_outcome_service_url($consumer_id, $url, $ext_bool) {
  $consumer = lti_tool_provider_outcomes_get_consumer_by_id($consumer_id);
  if ($consumer -> lti_tool_provider_consumer_outcomes_url!= $url ) {
    $consumer -> lti_tool_provider_consumer_outcomes_url = $url;
  }
  if ($consumer -> lti_tool_provider_consumer_ext != $ext_bool) {
    $consumer -> lti_tool_provider_consumer_ext = $ext_bool;
  }
  entity_get_controller('lti_tool_provider_consumer')->save($consumer);
}

/**
 * Checks if the corresponding resource record exists.
 *
 * @param int $cid
 *   Consumer entity id.
 *
 * @return boolean
 *   Success or failure.
 */
function lti_tool_provider_outcomes_check_if_result_record_exist($cid) {
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_outcomes_resource')
  ->propertyCondition('lti_tool_provider_outcomes_resource_consumer_id',  $cid);
  $result = $query->execute();
  if (!empty($result)) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Retreives all consumer.
 *
 * This function purpose is to get all resource for display
 * in grades mapping form.
 *
 * @return array
 *   Consumer entities.
 */
function lti_tool_provider_outcomes_get_all_consumer() {
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_consumer');
  $rs = $query->execute();
  $entity_type = 'lti_tool_provider_consumer';
  $result = array();
  if (!empty($rs[$entity_type])) {
    $consumers = entity_load($entity_type,  array_keys($rs[$entity_type]));
    foreach ($consumers as $consumer) {
      if (lti_tool_provider_outcomes_check_if_result_record_exist($consumer->lti_tool_provider_consumer_id)) {
        $result[] = $consumer;
      };
    };
  }
  return $result;
}

/**
 * Simply shorten the string for display.
 *
 * Prevent the display table from stretching too much.
 *
 * @param string $str
 *   Original string.
 * @param int $cap
 *   Maximum length.
 *
 * @return string
 *   The modified string.
 */
function lti_tool_provider_outcomes_shorten_string($str , $cap = 30) {
  if ( drupal_strlen($str) >= $cap ) {
    $new_str = drupal_drupal_substr($str, 0 , $cap );
    $new_str .= '...';
    return $new_str;
  }
  else {
    return $str;
  }
}

/**
 * Save the score into its respectively outcomes record.
 *
 * @param object $outcome
 *   Outcome entity.
 *
 * @param string $score
 *   Score.
 */
function lti_tool_provider_outcomes_updated_pulled_score($outcome , $score) {
  $outcome -> lti_tool_provider_outcomes_score = $score;
  $outcome -> lti_tool_provider_outcomes_last_updated = REQUEST_TIME;
  entity_get_controller('lti_tool_provider_outcomes')->save($outcome);
}

/**
 * Count all affected outcomes.
 *
 * @param object $entity
 *   Resource entity.
 *
 * @return array
 *   All the affected outcomes record.
 */
function lti_tool_provider_outcomes_get_affected_outcomes($entity) {
  $query =new EntityFieldQuery();
  $query
  ->entityCondition('entity_type',  'lti_tool_provider_outcomes')
  ->propertyCondition('lti_tool_provider_outcomes_resource_entity_id_fk',  $entity -> lti_tool_provider_outcomes_resource_id);
  $result = $query->execute();
  if (!empty($result)) {
    $outcomes = entity_load('lti_tool_provider_outcomes',  array_keys($result['lti_tool_provider_outcomes']));
    return $outcomes;
  }
  return NULL;
}

/**
 * Returns the selection array for passfail
 *
 * @return array
 *   passfail array
 */
function lti_tool_provider_outcomes_passfail_datatype_arr() {
  return array(
    'pass',
    'fail',
  );
}

/**
 * Returns the selection array for letteraf
 *
 * @return array
 *   letteraf array
 */
function lti_tool_provider_outcomes_letteraf_datatype_arr() {
  return array(
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
  );
}

/**
 * Returns the selection array for letterafplus
 *
 * @return array
 *   letterafplus array
 */
function lti_tool_provider_outcomes_letterafplus_datatype_arr() {
  return array(
    'A+',
    'A',
    'A-',
    'B+',
    'B',
    'B-',
    'C+',
    'C',
    'C-',
    'D+',
    'D',
    'D-',
    'E+',
    'E',
    'E-',
    'F+',
    'F',
    'F-',
  );
}

/**
 * Returns the default value of datatype
 *
 * @param string $datatype
 *   The datatype
 * @return string
 *   The default value of the datatype
 */
function lti_tool_provider_outcomes_get_datatype_default_value($datatype) {
  //   if (strcasecmp($datatype, 'decimal') == 0) {
  //     return '0.0';
  //   }
  //   elseif (strcasecmp($datatype, 'percentage') == 0) {
  //     return '0.00';
  //   }
  //   elseif (strcasecmp($datatype, 'ratio') == 0) {
  //     return '0/1';
  //   }
  //   elseif (strcasecmp($datatype, 'passfail') == 0) {
  //     return 'fail';
  //   }
  //   elseif (strcasecmp($datatype, 'letteraf') == 0) {
  //     return 'F';
  //   }
  //   elseif (strcasecmp($datatype, 'letterafplus') == 0) {
  //     return 'F-';
  //   }
  //   elseif (strcasecmp($datatype, 'freetext') == 0) {
  //     return 'None';
  //   }
  //   else {
  //     return '0.0';
  //   }
    return NULL;
}

/**
 * Create the score form parameters
 *
 * @param string $datatype
 *   The datatype
 * @param object $outcome
 *   Outcome object
 * @return array
 *   The form data
 */
function lti_tool_provider_outcomes_create_score_form_params($datatype, $base_no , $outcome ) {
  $result = array();
  if (strcasecmp($datatype, 'decimal') == 0) {
    $result[] = array(
      '#prefix' => '<div align="center">',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#default_value' => $outcome -> lti_tool_provider_outcomes_score,
      '#size' => 9,
      '#maxlength' => 9,
      '#required' => TRUE ,
    );
  }
  elseif (strcasecmp($datatype, 'percentage') == 0) {
    $value =  $outcome -> lti_tool_provider_outcomes_score * $base_no;
    $result[] = array(
      '#prefix' => '<div class="container-inline" align="center">',
      '#type' => 'textfield',
      '#default_value' => $value,
      '#size' => 3,
      '#maxlength' => 3,
      '#suffix' => '%</div>',
      '#required' => TRUE ,
    );
  }
  elseif (strcasecmp($datatype, 'ratio') == 0) {
    //     $slash_pos = strpos($outcome -> lti_tool_provider_outcomes_score, '/');
    //     $first_string = drupal_substr($outcome -> lti_tool_provider_outcomes_score, 0, $slash_pos);
    //     $new_start_index = ++$slash_pos;
    //     $second_string = drupal_substr($outcome -> lti_tool_provider_outcomes_score, $new_start_index);
    $value =  $outcome -> lti_tool_provider_outcomes_score * $base_no;
    $result[] =  array(
      '#prefix' => '<div class="container-inline" align="center">',
      '#type' => 'textfield',
      '#default_value' => $value,
      '#size' => 1,
      '#maxlength' => 3,
      '#required' => TRUE,
      '#suffix' => '/'
    );
    $result[] =  array(
      '#type' => 'textfield',
      '#default_value' => $base_no,
      '#size' => 1,
      '#maxlength' => 3,
      '#attributes' => array('readonly' => 'readonly'),
      '#required' => TRUE,
      '#suffix' => '</div>'
    );
  }
  elseif (strcasecmp($datatype, 'passfail') == 0) {
    $value =  $outcome -> lti_tool_provider_outcomes_score * $base_no;
    $result[] =  array(
      '#prefix' => '<div align="center">',
      '#suffix' => '</div>',
      '#type' => 'select',
      '#options' => lti_tool_provider_outcomes_passfail_datatype_arr(),
      //       '#default_value' => array_search($outcome -> lti_tool_provider_outcomes_score , lti_tool_provider_outcomes_passfail_datatype_arr())
      '#default_value' => floor($value),
    );
  }
  elseif (strcasecmp($datatype, 'letteraf') == 0) {
    $value =  $outcome -> lti_tool_provider_outcomes_score * $base_no;
    $result[] = array(
      '#prefix' => '<div align="center">',
      '#suffix' => '</div>',
      '#type' => 'select',
      '#options' => lti_tool_provider_outcomes_letteraf_datatype_arr(),
      '#default_value' => floor($value),
      // '#default_value' => array_search($outcome -> lti_tool_provider_outcomes_score , lti_tool_provider_outcomes_letteraf_datatype_arr()),
    );
  }
  elseif (strcasecmp($datatype, 'letterafplus') == 0) {
    $value =  $outcome -> lti_tool_provider_outcomes_score * $base_no;
    $result[] =  array(
      '#prefix' => '<div align="center">',
      '#suffix' => '</div>',
      '#type' => 'select',
      '#options' => lti_tool_provider_outcomes_letterafplus_datatype_arr(),
      '#default_value' => floor($value),
      //       '#default_value' => array_search($outcome -> lti_tool_provider_outcomes_score , lti_tool_provider_outcomes_letterafplus_datatype_arr()),
    );
  }
  elseif (strcasecmp($datatype, 'freetext') == 0) {
    $result[] =  array(
      '#prefix' => '<div align="center">',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#default_value' => $outcome -> lti_tool_provider_outcomes_score,
      '#size' => 9,
      '#maxlength' => 1024,
      '#required' => TRUE,
    );
  }
  else {
    $result[] = array(
      '#prefix' => '<div align="center">',
      '#suffix' => '</div>',
      '#type' => 'textfield',
      '#default_value' => $outcome -> lti_tool_provider_outcomes_score,
      '#size' => 9,
      '#maxlength' => 9,
      '#required' => TRUE,
    );
  }
  return $result;
}

/**
 * Retrieves the score from the form
 *
 * @param int $row_no
 *   The row identifier
 * @param array $form_state
 *   The form state array
 * @param object $outcome
 *   The outcome object
 * @param string $datatype
 *   The corresponding datatype
 *
 * @return string
 *   The formatted score
 */
function lti_tool_provider_outcomes_format_score($row_no, $form_state , $outcome , $datatype, $base_no) {
  if (strcasecmp($datatype, 'decimal') == 0) {
    return reset($form_state['values']['table'][$row_no]['score']);
  }
  elseif (strcasecmp($datatype, 'percentage') == 0) {
    $value = (reset($form_state['values']['table'][$row_no]['score']) + 1/2) / $base_no;
    return $value;
  }
  elseif (strcasecmp($datatype, 'ratio') == 0) {
    $str_arr = $form_state['values']['table'][$row_no]['score'];
    $return_str = $str_arr[0] . '/' . $str_arr[1];
    $value = (reset($form_state['values']['table'][$row_no]['score']) + 1/2) / $base_no;
    return $value;
  }
  elseif (strcasecmp($datatype, 'passfail') == 0) {
    $passfail_arr = lti_tool_provider_outcomes_passfail_datatype_arr();
    //     return $passfail_arr[reset($form_state['values']['table'][$row_no]['score'])];
    $value = (reset($form_state['values']['table'][$row_no]['score']) + 1/2) / $base_no;
    return $value;
  }
  elseif (strcasecmp($datatype, 'letteraf') == 0) {
    $letteraf_arr = lti_tool_provider_outcomes_letteraf_datatype_arr();
    //     return $letteraf_arr[reset($form_state['values']['table'][$row_no]['score'])];
    $value = (reset($form_state['values']['table'][$row_no]['score']) + 1/2) / $base_no;
    return $value;
  }
  elseif (strcasecmp($datatype, 'letterafplus') == 0) {
    $letteraf_arr = lti_tool_provider_outcomes_letterafplus_datatype_arr();
    //     return $letteraf_arr[reset($form_state['values']['table'][$row_no]['score'])];
    $value = (reset($form_state['values']['table'][$row_no]['score']) + 1/2) / $base_no;
    return $value;
  }
  elseif (strcasecmp($datatype, 'freetext') == 0) {
    return reset($form_state['values']['table'][$row_no]['score']);
  }
  else {
    return reset($form_state['values']['table'][$row_no]['score']);
  }
}

/**
 * Set the base or denominator for a score.
 *
 * @param string $datatype
 *   The datatype of the score.
 * @param string $base_no
 *   The denominator or base of the ratio.
 *
 * @return string|number
 */
function lti_tool_provider_outcomes_set_base($datatype , $base_no = NULL) {
  if (strcasecmp($datatype, 'decimal') == 0) {
    return 'EMPTY';
  }
  elseif (strcasecmp($datatype, 'percentage') == 0) {
    return 100;
  }
  elseif (strcasecmp($datatype, 'ratio') == 0) {
    return isset($base_no) ? $base_no : 100;
  }
  elseif (strcasecmp($datatype, 'passfail') == 0) {
    return 2;
  }
  elseif (strcasecmp($datatype, 'letteraf') == 0) {
    return 6;
  }
  elseif (strcasecmp($datatype, 'letterafplus') == 0) {
    return 18;
  }
  elseif (strcasecmp($datatype, 'freetext') == 0) {
    return 'EMPTY';
  }
  else {
    return 'EMPTY';
  }
}

/**
 * Change the visibility of the prefix.
 *
 * @param string $prefix
 */
function lti_tool_provider_outcomes_datatype_prefix_string_change_visibility($prefix) {
  $style_hidden = 'hidden';
  $style_visible = 'visible';
  if (strpos($prefix, $style_hidden)) {
    return str_replace($style_hidden, $style_visible, $prefix);
  }
  if (strpos($prefix, $style_visible)) {
    return str_replace($style_visible, $style_hidden, $prefix);
  }
}