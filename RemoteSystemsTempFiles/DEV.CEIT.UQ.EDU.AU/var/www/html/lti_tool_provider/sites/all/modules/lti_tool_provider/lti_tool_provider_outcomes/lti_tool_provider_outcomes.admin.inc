<?php

/**
 * @file
 * Admin forms and menu page callbacks for LTI Tool Provider module.
 */

/**
 * Form to manage outcomes.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 *
 * @return array
 *   The form array.
 */
function lti_tool_provider_outcomes_resource_manage_form($form, &$form_state, $current = FALSE) {
  $form = array();
  if (isset($_SESSION['lti_tool_provider_context_info'])) {
    $consumer = lti_tool_provider_get_consumer_by_key($_SESSION['lti_tool_provider_context_info']['oauth_consumer_key']);
    if ($current) {
      $value = lti_tool_provider_outcomes_get_resource($consumer -> lti_tool_provider_consumer_id, $_SESSION['lti_tool_provider_context_info']['context_id'] , $_SESSION['lti_tool_provider_context_info']['resource_link_id']);
      if (isset($value)) {
        $records = array();
        $records[] = $value;
      }
      else {
        $records = NULL;
      }
    }
    else {
      $records = lti_tool_provider_outcomes_retrieves_resource_records_by_context($consumer -> lti_tool_provider_consumer_id, $_SESSION['lti_tool_provider_context_info']['context_id']);
    }
    $form['title'] = array(
      '#type' => 'item',
      '#markup' => t('Consumer : <b>' . $consumer-> lti_tool_provider_consumer_consumer . '</b>'),
    );
    $form['context'] = array(
      '#type' => 'item',
      '#markup' => t('Context : <b>' . $_SESSION['lti_tool_provider_context_info']['context_id'] . '</b>'),
    );
    //     $form['debug'] = array(
    //       '#type' => 'item',
    //       '#prefix' => '<div id="debug-test">',
    //       '#suffix' => '</div>',
    //     );
    if (isset($records)) {
      $form['records'] = array(
        '#type' => 'value',
        '#value' => $records,
      );
      $form['table'] = array(
        '#prefix' => '<div id="replace-table">',
        '#suffix' => '</div>',
        '#tree' => TRUE,
        '#theme' => 'table',
        '#rows' => array(),
        '#header' => array(
          t('No'),
          t('Resource Title'),
          t('Score Datatype'),
          t('Date Created'),
          t('Operations'),
        ),
        '#empty' => t('There are no records found.'),
      );
      $rows = 0;
      foreach ($records as $record) {
        $entity = array(
          '#type' => 'value',
          '#value' => $record,
        );
        $no =  array(
          '#type' => 'item',
          '#markup' => $rows + 1,
        );
        $title = array(
          '#type' => 'item',
          '#markup' => t($record -> lti_tool_provider_outcomes_resource_resource_link_title),
        );
        $datatype_arr = lti_tool_provider_outcomes_datatype_array($record->lti_tool_provider_outcomes_resource_resultvalue_sourcedids);
        $def_value = isset($form_state['input']['table'][$rows]['container'][1]) ? $form_state['input']['table'][$rows]['container'][1] : array_search($record->lti_tool_provider_outcomes_resource_score_datatype, $datatype_arr);
        $datatype_container = array();
        $datatype_container_start = array(
          '#prefix' => '<div id=' . $record -> lti_tool_provider_outcomes_resource_id . '"_datatype_container" class="container-inline">',
        );
        $datatype = array(
          '#type' => 'select',
          '#options'  => $datatype_arr,
          '#default_value' => $def_value,
          '#ajax' => array(
            'callback' => 'lti_tool_provider_outcomes_resource_datatype_callback',
            'wrapper' => 'replace-table',
            'effect' => 'fade',
            'method' => 'replace',
          ),
        );
        if (strcasecmp($datatype_arr[$def_value], 'ratio') == 0) {
          $prefix = '<div id="' . $record -> lti_tool_provider_outcomes_resource_id . '_base_no_value" style="visibility:visible;">';
          $is_visible = TRUE;
        }
        else {
          $prefix = '<div id="' . $record -> lti_tool_provider_outcomes_resource_id . '_base_no_value" style="visibility:hidden;">';
          $is_visible = FALSE;
        }
        //         /$resource -> lti_tool_provider_outcomes_resource_score_datatype_base_value
        $baseno = array(
          '#prefix' => $prefix,
          '#type' => 'textfield',
          '#size' => 3,
          '#maxlength' => 3,
          '#required' => FALSE,
          '#suffix' => '</div>',
          '#is_visible' => $is_visible,
          '#default_value' => $record -> lti_tool_provider_outcomes_resource_score_datatype_base_value,
        );
        $datatype_container_end = array(
          '#prefix' => '</div>',
        );
        $datatype_container[] = $datatype_container_start;
        $datatype_container[] = $datatype;
        $datatype_container[] = $baseno;
        $datatype_container[] = $datatype_container_end;
        $date_created = array(
          '#type' => 'item',
          '#markup' =>  '<div style=" white-space:normal;">' . t(format_date($record -> lti_tool_provider_outcomes_resource_timestamp_created)) . "</div>",
        );
        $operations = array(
          '#type' => 'item',
          '#markup' => l(t('Outcomes') , 'lti-tool-provider-outcomes/outcomes/by-resource/' . $record -> lti_tool_provider_outcomes_resource_id) . '&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp' . l(t('Delete') , 'lti-tool-provider-outcomes/resource/' . $record -> lti_tool_provider_outcomes_resource_id . '/confirm_delete') . '&nbsp&nbsp' ,
        );
        $rows ++;
        $form['table'][] = array(
          'entity' => &$entity,
          'no' => $no,
          'title' => &$title,
          'container' => &$datatype_container,
          'date_created' => &$date_created,
          'operations' => &$operations,
        );
        $form['table']['#rows'][] = array(
          array('data' => &$no),
          array('data' => &$title),
          array('data' => &$datatype_container, 'width' => '160px'),
          array('data' => &$date_created),
          array('data' => &$operations),
        );
        unset($no);
        unset($title);
        unset($datatype_container);
        unset($date_created);
        unset($operations);
        unset($entity);
      }

      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
        '#submit' => array('lti_tool_provider_outcomes_resource_manage_form_submit'),
      );
      // $form['#theme'] = array('lti_tool_provider_outcomes_resource_manage_form');
    }
    else {
      $form['message'] = array(
        '#type' => 'item',
        '#markup' => t('<b> No records avaliable</b>'),
      );
    }
  }
  else {
    $form['message'] = array(
      '#type' => 'item',
      '#markup' => t('<b> This is not a valid LTI Session</b>'),
    );
  }
  return $form;
}

/**
 * AJAX callback for datatype selector.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 *
 * @return array
 *   The form element to be replaced.
 */
function lti_tool_provider_outcomes_resource_datatype_callback($form, &$form_state) {
  $counter = 0;
  //   foreach($form_state['input']['table'] as $row) {
  //     $selected = $row['container'][2];
  //     $form_state['values']['table'][$counter]['container'][0] = $selected;
  //     $counter ++;
  //   }
  //   dpm($form_state, 'form_state');
  return $form['table'];
}

/**
 * Submit handler for the Resource form.
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 */
function lti_tool_provider_outcomes_resource_manage_form_validate($form, &$form_state) {

}

/**
 * Submit handler for the Resource form.
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 */
function lti_tool_provider_outcomes_resource_manage_form_submit($form, &$form_state) {
  $records = $form_state['values']['records'];
  // A string of resource id separated by ','
  $changed_ids = '';
  // A string of index selected on the datatype
  $selected_datatype ='';
  // A string of base numbers id:value
  $base_numbers = '';
  //   dpm($form_state);
  $counter = 0;
  foreach ($form_state['input']['table'] as $row) {
    $chosen_value = $row['container'][1];
    $entity = $form_state['values']['table'][$counter]['entity'];
    $datatype_arr = lti_tool_provider_outcomes_datatype_array($entity->lti_tool_provider_outcomes_resource_resultvalue_sourcedids);
    $changed_ids .= $entity -> lti_tool_provider_outcomes_resource_id . ',';
    $selected_datatype .= $chosen_value . ',';
    $row['container'][2] = lti_tool_provider_outcomes_set_base($datatype_arr[$chosen_value] , isset($row['container'][2]) ? NULL : $row['container'][2]);
    $base_numbers .= $row['container'][2] . ',';
    $counter ++;
  }
  //   foreach ($records as $record) {
  //     $chosen_value = $form_state['values'][$record -> lti_tool_provider_outcomes_resource_id . '_datatype_select'];
  //     $datatype_arr = lti_tool_provider_outcomes_datatype_array($record->lti_tool_provider_outcomes_resource_resultvalue_sourcedids);
  //     $ori_entity =   $form_state['values'][$record -> lti_tool_provider_outcomes_resource_id . '_entity'];
  //     if (!($ori_entity -> lti_tool_provider_outcomes_resource_score_datatype == $datatype_arr[$chosen_value])) {
  //       $changed_ids .= $record -> lti_tool_provider_outcomes_resource_id . ',';
  //       $selected_datatype .= $chosen_value . ',';
  //     }
  //   }
  if (drupal_strlen($changed_ids) > 0 && drupal_strlen($selected_datatype) > 0) {
    $new_changed_ids = drupal_substr($changed_ids, 0, -1);
    $new_selected_datatype = drupal_substr($selected_datatype, 0, -1);
    $base_numbers = drupal_substr($base_numbers, 0, -1);
    $params_arr = array();
    $params_arr['ids'] = $new_changed_ids;
    $params_arr['datatypes'] = $new_selected_datatype;
    $params_arr['base_numbers'] = $base_numbers;
    $params_arr['return_url'] = 'lti-tool-provider-outcomes/resource';
    $form_state['redirect'] = array(
      'lti-tool-provider-outcomes/resource/grades-mapping/confirm_change',
      array(
        'query' => $params_arr
      )
    );
  }
  else {
    drupal_set_message(t('Nothing has changed'));
  }
}

function lti_tool_provider_outcomes_outcomes_manage_form($form, &$form_state, $resource = NULL) {
  $form = array();
  if (isset($_SESSION['lti_tool_provider_context_info'])) {
    $consumer = lti_tool_provider_get_consumer_by_key($_SESSION['lti_tool_provider_context_info']['oauth_consumer_key']);
    if (!isset($resource)) {
      $resource = lti_tool_provider_outcomes_get_resource($consumer -> lti_tool_provider_consumer_id, $_SESSION['lti_tool_provider_context_info']['context_id'], $_SESSION['lti_tool_provider_context_info']['resource_link_id']);
    }
    $query = db_select('lti_tool_provider_outcomes', 'o')->extend('PagerDefault');
    $query->fields('o');
    $result = $query
    ->limit(15)
    ->condition('lti_tool_provider_outcomes_resource_entity_id_fk', $resource -> lti_tool_provider_outcomes_resource_id, '=')
    ->execute();

    $form['title'] = array(
      '#type' => 'item',
      '#markup' => t('Consumer : <b>' . $consumer-> lti_tool_provider_consumer_consumer . '</b>'),
    );
    $form['resource_title'] = array(
      '#type' => 'item',
      '#markup' => t('Outcomes record for resource : <b>' . $_SESSION['lti_tool_provider_context_info']['resource_link_title'] . '</b>'),
    );
    //     if (!isset($outcomes)) {
    //       $form['message'] = array(
    //         '#type' => 'item',
    //         '#markup' => t('No outcomes found.'),
    //       );
    //     }
    //     else {
    $outcomes = array();
    $form['resource'] = array(
      '#type' => 'value',
      '#value' => $resource,
    );
    $form['table'] = array(
      '#tree' => TRUE,
      '#theme' => 'table',
      '#rows' => array(),
      '#header' => array(
        t('No'),
        t('Username'),
        t('Current Score'),
        t('Date Joined'),
        t('Last sent'),
        t('Score Last updated'),
      ),
      '#empty' => t('There are no records found.'),
    );
    $page = isset($_GET['page']) ? $_GET['page'] * 15: 0;
    $rows = 1;
    $table_row_counter = 0;
    $rows_arr = array();
    foreach ($result as $outcome) {
      $no =  array(
        '#type' => 'item',
        '#markup' => $rows + $page,
      );
      $username =  array(
        '#type' => 'item',
        '#markup' => l(lti_tool_provider_outcomes_get_username($outcome->lti_tool_provider_outcomes_user_id), 'user/' . $outcome->lti_tool_provider_outcomes_user_id) ,
      );
      $score_arr = lti_tool_provider_outcomes_create_score_form_params($resource-> lti_tool_provider_outcomes_resource_score_datatype , $resource-> lti_tool_provider_outcomes_resource_score_datatype_base_value , $outcome);
      $date_joined =  array(
        '#type' => 'item',
        '#markup' => '<div style=" white-space:normal;">' . format_date($outcome -> lti_tool_provider_outcomes_date_joined) . "</div>",
      );
      $last_sent =  array(
        '#type' => 'item',
        '#markup' => '<div style=" white-space:normal;">' . (isset($outcome -> lti_tool_provider_outcomes_timestamp) ? format_date($outcome -> lti_tool_provider_outcomes_timestamp) : 'Score yet to be sent') . "</div>",
      );
      $last_updated =  array(
        '#type' => 'item',
        '#markup' => '<div style=" white-space:normal;">' . (isset($outcome -> lti_tool_provider_outcomes_last_updated) ? format_date($outcome -> lti_tool_provider_outcomes_last_updated) : 'Yet to be updated') . "</div>",
      );
      $form['table'][] = array(
        'no' => &$no,
        'username' => &$username,
        'score' => &$score_arr,
        'date_joined' => &$date_joined,
        'last_sent' => &$last_sent,
        'last_updated' => &$last_updated,
      );
      $form['table']['#rows'][] = array(
        array('data' => &$no),
        array('data' => &$username),
        array('data' => &$score_arr, "width" => '100px'),
        array('data' => &$date_joined),
        array('data' => &$last_sent),
        array('data' => &$last_updated),
      );
      unset($no);
      unset($username);
      unset($score_arr);
      unset($date_joined);
      unset($last_sent);
      unset($last_updated);
      $rows ++;
      $outcomes[] = $outcome;
    }
    $form['outcomes'] = array(
      '#type' => 'value',
      '#value' => $outcomes,
    );
    $form['table_pager'] = array('#theme' => 'pager');
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#submit' => array('lti_tool_provider_outcomes_outcomes_manage_form_submit'),
    );
    $form['back'] = array(
      '#type' => 'button',
      '#value' => t('Cancel'),
      '#executes_submit_callback' => TRUE,
      '#submit' => array('lti_tool_provider_outcomes_outcomes_manage_form_cancel'),
    );
  }
  else {
    $form['message'] = array(
      '#type' => 'item',
      '#markup' => t('<b> This is not a valid LTI session</b>'),
    );
  }
  return $form;
}

/**
 * Submit handler for the outcomes form.
 *
 * @param array $form
 *   Form array
 * @param array $form_state
 *   Form state array
 */
function lti_tool_provider_outcomes_outcomes_manage_form_submit($form , &$form_state) {
  $outcomes = $form_state['values']['outcomes'];
  dpm($outcomes);
  foreach ($outcomes as $outcome) {
    $outcome -> lti_tool_provider_outcomes_score = $form_state['values'][$outcome -> lti_tool_provider_outcomes_id . '_formatted_score'];
    $outcome -> lti_tool_provider_outcomes_last_updated = REQUEST_TIME;
    lti_tool_provider_outcomes_save($outcome);
  }
  drupal_set_message(t('Configurations saved'));
  $form_state['redirect'] = current_path();
}

/**
 * Cancel button return for outcomes manage form.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 */
function lti_tool_provider_outcomes_outcomes_manage_form_cancel($form , &$form_state) {
  drupal_goto('lti-tool-provider-outcomes/resource');
}

/**
 * Confirms the delete action
 *
 * @param array $form_state
 *   Form state array
 * @return array
 *   Confirm form array
 */
function lti_tool_provider_outcomes_confirm_delete_form($form, &$form_state, $resource) {
  $form['resource_delete'] = array(
    '#type' => 'value',
    '#value' => $resource,
  );
  $affected_outcomes = lti_tool_provider_outcomes_get_affected_outcomes($resource);
  $form['affected_outcomes'] = array(
    '#type' => 'value',
    '#value' => $affected_outcomes,
  );
  return confirm_form($form,
    t('Are you sure you want to delete this resource record?'),
    'lti-tool-provider-outcomes/resource',
    t('This action cannot be undone. <br/><b>:X</b> outcomes record(s) will be removed', array(':X' => isset($affected_outcomes) ? count($affected_outcomes) : 0)),
    t('Delete'),
    t('Cancel'));
}

/**
 * Submit handler for the Resource form delete.
 *
 * @param array $form
 *   Form array
 * @param array $form_state
 *   Form state array
 */
function lti_tool_provider_outcomes_confirm_delete_form_submit($form, &$form_state) {
  $entity = $form_state['values']['resource_delete'];
  $affected_outcomes = lti_tool_provider_outcomes_get_affected_outcomes($form_state['values']['resource_delete']);
  if (isset($affected_outcomes)) {
    foreach ($affected_outcomes as $outcome) {
      lti_tool_provider_outcomes_delete($outcome);
    }
  }
  lti_tool_provider_outcomes_resource_delete($entity);
  drupal_set_message(t('The entity :(ID %lti_tool_provider_outcomes_resource_id) has been deleted. <b>%NO</b> outcomes record(s) has been removed as well', array(
  '%lti_tool_provider_outcomes_resource_id' => $entity->lti_tool_provider_outcomes_resource_id,
  '%NO' => isset($affected_outcomes) ? count($affected_outcomes) : 0,
  )));
  $form_state['redirect'] = 'lti-tool-provider-outcomes/resource';
}

/**
 * Form for confirming changes to resource datatypes
 *
 * @param array $form
 *   Form array
 * @param array $form_state
 *   Form state array
 */
function lti_tool_provider_outcomes_resource_confirm_change_form($form, &$form_state) {
  $ids = $_GET['ids'];
  $datatype_selection = $_GET['datatypes'];
  $base_numbers = $_GET['base_numbers'];
  $message = 'This action cannot be undone. <br/>';
  $ids_array = explode(',',  $ids);
  $datatype_selected_array = explode(',',  $datatype_selection);
  $base_numbers_array = explode(',',  $base_numbers);
  $counter = 0;
  $affected_resource = array();
  foreach ($ids_array as $id) {
    $resource = lti_tool_provider_outcomes_resource_load($id);
    $affected_outcomes = lti_tool_provider_outcomes_get_affected_outcomes($resource);
    $datatype_arr = lti_tool_provider_outcomes_datatype_array($resource -> lti_tool_provider_outcomes_resource_resultvalue_sourcedids);
    $selected_datatype = $datatype_arr[$datatype_selected_array[$counter]];
    //     dpm($base_numbers_array);
    //     dpm($base_no);
    $base_no = $base_numbers_array[$counter];
    $counter++;
    $str = 'Changing resource <b>' . $resource -> lti_tool_provider_outcomes_resource_resource_link_title . '</b> datatype to <b>' . $selected_datatype . '</b> will affect <b>' . (isset($affected_outcomes) ? count($affected_outcomes) : 0) . '</b> of outcomes record(s)! </br><br/>';
    $message .= $str;
    $form['row_' . $counter][$resource -> lti_tool_provider_outcomes_resource_id . '_selected_datatype'] = array(
      '#type' => 'value',
      '#value' => $selected_datatype,
    );
    $form['row_' . $counter][$resource -> lti_tool_provider_outcomes_resource_id . '_affected_outcomes'] = array(
      '#type' => 'value',
      '#value' => $affected_outcomes,
    );
    $form['row_' . $counter][$resource -> lti_tool_provider_outcomes_resource_id . '_base_value'] = array(
      '#type' => 'value',
      '#value' => (strcasecmp($base_no, 'EMPTY') == 0) ? NULL : $base_no,
    );
    $affected_resource[] = $resource;
  }
  $form['affected_resource'] = array(
    '#type' => 'value',
    '#value' => $affected_resource,
  );
  $form['return_url'] = array(
    '#type' => 'value',
    '#value' => isset($_GET['return_url']) ? $_GET['return_url'] : '',
  );
  return confirm_form($form,
    t('Are you sure you want to make changes to the resource record(s)?'),
    $_SERVER['HTTP_REFERER'],
    t($message),
    t('Change'),
    t('Cancel'));
}

/**
 * Submit handler for confirming resource changes.
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 */
function lti_tool_provider_outcomes_resource_confirm_change_form_submit($form, &$form_state) {
  $affected_resource = $form_state['values']['affected_resource'];
  foreach ($affected_resource as $resource) {
    $datatype = $form_state['values'][$resource -> lti_tool_provider_outcomes_resource_id . '_selected_datatype'];
    $base_no = $form_state['values'][$resource -> lti_tool_provider_outcomes_resource_id . '_base_value'];
    $affected_outcomes = $form_state['values'][$resource -> lti_tool_provider_outcomes_resource_id . '_affected_outcomes'];
    // set all affected outcomes score to zero
    foreach ($affected_outcomes as $outcome) {
      $outcome -> lti_tool_provider_outcomes_score = lti_tool_provider_outcomes_get_datatype_default_value($datatype);
      $outcome -> lti_tool_provider_outcomes_last_updated = NULL;
      lti_tool_provider_outcomes_save($outcome);
    }
    $resource -> lti_tool_provider_outcomes_resource_score_datatype = $datatype;
    $resource -> lti_tool_provider_outcomes_resource_score_datatype_base_value = $base_no;
    $resource -> lti_tool_provider_outcomes_resource_timestamp_last_updated = REQUEST_TIME;
    lti_tool_provider_outcomes_resource_save($resource);
  }
  drupal_set_message(t('Configurations saved'));
  $form_state['redirect'] = $form_state['values']['return_url'];
}

/**
 * Theme for Grades Mapping Form.
 *
 * @param array $variables
 *   Theme variables
 *
 * @return array
 *   Themed Form
 *
 function theme_lti_tool_provider_outcomes_resource_manage_form($variables) {
 $form = $variables['form'];
 $output = '';
 if (isset($_SESSION['lti_tool_provider_context_info'])) {
 $output .= drupal_render($form['title']);
 $output .= drupal_render($form['context']);
 $records = isset($form['records']['#value']) ? $form['records']['#value'] : NULL;
 if (isset($records)) {
 $counter = 1;
 $rows = array();
 foreach ($records as $record) {
 $rows[] = array(
 array('data' => $counter ),
 array('data' => drupal_render($form['row_' . $counter][$record -> lti_tool_provider_outcomes_resource_id . '_title'])),
 array('data' => drupal_render($form['row_' . $counter][$record -> lti_tool_provider_outcomes_resource_id . '_datatype_select']) ),
 array('data' => drupal_render($form['row_' . $counter][$record -> lti_tool_provider_outcomes_resource_id . '_data_created']) ),
 array('data' => drupal_render($form['row_' . $counter][$record -> lti_tool_provider_outcomes_resource_id . '_operations']) ),
 );
 $a = drupal_render($form['row_' . $counter][$record -> lti_tool_provider_outcomes_resource_id . '_datatype_container_start']);
 $b = drupal_render($form['row_' . $counter][$record -> lti_tool_provider_outcomes_resource_id . '_datatype_select']);
 $c = drupal_render($form['row_' . $counter][$record -> lti_tool_provider_outcomes_resource_id . '_datatype_container_end']);
 dpm($a, 'a' );
 dpm($b, 'b' );
 dpm($c, 'c' );
 $counter ++;
 }
 $header = array(
 t('No'),
 t('Resource Title'),
 t('Score Datatype'),
 t('Date Created'),
 t('Operations'),
 );
 $output .= theme('table', array('header' => $header, 'rows' => $rows));
 }
 else {
 $output .= drupal_render($form['message']);
 }
 }
 else {
 $output .= drupal_render($form['message']);
 }

 $output .= drupal_render_children($form);
 return $output;
 } */