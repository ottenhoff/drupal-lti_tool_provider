<?php

/**
 * @file
 * Implementation of the CreaTeams team formation wizard.
 */

/**
 *
 * @param unknown $node
 */
function _uq_teams_generate_teams_title($node)  {
  return t($node->title.' - Generate Teams');
}

/**
 * Present a multistep wizard form for Teams formation
 *
 * @param unknown $node
 */
function uq_teams_wizard_page($node) {
  if ($node->type === 'course') {
    return drupal_get_form('uq_teams_wizard', $node->nid);
  } else {
    // If node is not of type 'course' then turn a 'Page Not Found' error message
    drupal_not_found();
  }
}

/**
 * Returns the list of steps and their associated forms
 *
 * @return multitype:multitype:string
 */
function _uq_teams_wizard_steps() {
  return array(
    1 => array(
      'form' => 'uq_teams_wizard_setup_form',
    ),
    2 => array(
      'form' => 'uq_teams_wizard_preview_form',
    ),
  );
}

/*
 * Present a multistep wizard form for Teams formation
* - Form render based on current step
*
* @param unknown $form
* @param unknown $form_state
* @param unknown $node
* @return unknown
*/
function uq_teams_wizard_form($form, &$form_state, $node) {
  if (empty($form_state['step'])) {
    $form_state['step'] = 1;
    $form_state['step_information'] = _uq_teams_wizard_steps();
  }

  $step = $form_state['step'];
  $form = $form_state['step_information'][$step]['form']($form, $form_state, $node);

  return $form;
}

/*
 * Setup step form - for setting up team constraints and attributes
*
* @param unknown $form
* @param unknown $form_state
* @param unknown $node
* @return multitype:string NULL
*/
function uq_teams_wizard_setup_form($form, &$form_state, $node) {
  drupal_add_js(drupal_get_path('module', 'uq_teams') . '/js/constraint.js', array('cache' => FALSE));

  // Retrieve previously saved teams configuration
  $savedTeamsConfig = variable_get('uq_teams_config');
  $numMem = '';
  $minOrMax = 0;
  $setDefault = TRUE;

  if (!empty($savedTeamsConfig)) {
    if (!empty($savedTeamsConfig[$node->nid])) {
      // Get the saved teams config for current course
      $teamConfig = $savedTeamsConfig[$node->nid];
      $numMem = $teamConfig['num_mem'];
      $minOrMax = $teamConfig['min_or_max'];
      $userChecklist = $teamConfig['user_attrib'];
      $courseChecklist = $teamConfig['course_attrib'];
      $userSettingsJSON = $teamConfig['user'];
      $courseSettingsJSON = $teamConfig['course'];
      $userSettings = json_decode($teamConfig['user']);
      $courseSettings = json_decode($teamConfig['course']);

      // Previous config found so do not need to set to default
      $setDefault = FALSE;
    }
  }

  if ($setDefault) {
    // Assume default settings if no config found
    $numMem = '';
    $minOrMax = '0';
    $userChecklist = array();
    $courseChecklist = array();
    $userSettingsJSON = '[]';
    $courseSettingsJSON = '[]';
    $userSettings = array();
    $courseSettings = array();
  }

  $form = array();

  $form['step_1'] = array(
    '#type' => 'item',
    '#markup' => t('<h1>Step 1</h1><p>Specify the desired number of members and whether it is acceptable to have more or less members.</p>'),
  );

  $form['num_mem'] = array(
    '#type' => 'textfield',
    '#title' => t('Size'),
    '#maxlength' => 4,
    '#size' => 4,
    '#default_value' => $numMem,
  );

  $form['min_or_max'] = array(
    '#type' => 'select',
    '#options' => array('min' => 'Minimum', 'max' => 'Maximum'),
    '#default_value' => $minOrMax,
  );

  $form['step_2'] = array(
    '#type' => 'item',
    '#markup' => t('<br><h1>Step 2</h1><p>Select and configure the team constraints.</p>'),
  );

  $header = array(
    'const' => t('Constraint'),
    'dist' => t('Distribution'),
    //     'weight' => t('Weight'),
    'priority' => t('Priority'),
  );

  // Retrieve list of constraint fields attached to user profile
  $userAttribList = variable_get('uq_teams_user_attributes');
  // TODO: Only set initially - need a UI for adding new fields
  if (!$userAttribList) {
    $userAttribList = array('field_gender', 'field_international');
    variable_set('uq_teams_user_attributes', $userAttribList);
  }

  $courseAttribList = variable_get('uq_teams_course_attributes');

  if (!$courseAttribList) {
    $courseAttribList = array('field_math1051_grade', 'field_paf_score');
    variable_set('uq_teams_course_attributes', $courseAttribList);
  }

  $userOptions = array();
  $courseOptions = array();

  $userSettingsLen = count($userSettings);
  $courseSettingsLen = count($courseSettings);

  foreach ($userAttribList as $userAttrib) {
    $fieldInst = field_info_instance('user', $userAttrib, 'user');
    $fieldInfo = field_info_field($userAttrib);
    $fieldTypeExp = explode('_', $fieldInfo['type']);

    if ($fieldInst) {
      $userOptions[$userAttrib] = _uq_teams_form_constraint_row('user', $fieldInst, $fieldInfo, $fieldTypeExp);
    }
  }

  foreach ($courseAttribList as $courseAttrib) {
    $fieldInst = field_info_instance('og_membership', $courseAttrib, 'og_membership_type_default');
    $fieldInfo = field_info_field($courseAttrib);
    $fieldTypeExp = explode('_', $fieldInfo['type']);

    if ($fieldInst) {
      $courseOptions[$courseAttrib] = _uq_teams_form_constraint_row('course', $fieldInst, $fieldInfo, $fieldTypeExp);
    }
  }

  $form['user_attrib_title'] = array(
    '#type' => 'item',
    '#markup' => t('<label>User Attributes</label>'),
  );

  // User constraint table
  $form['user_attrib'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $userOptions,
    '#default_value' => $userChecklist,
    '#attributes' => array(
      'class' => array('user-table'),
    ),
  );

  $form['course_attrib_title'] = array(
    '#type' => 'item',
    '#markup' => t('<br><label>Course Attributes</label>'),
  );

  // Course constraint table
  $form['course_attrib'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $courseOptions,
    '#default_value' => $courseChecklist,
    '#attributes' => array(
      'class' => array('course-table'),
    ),
  );

  // Step 3 label
  $form['step_3'] = array(
    '#type' => 'item',
    '#markup' => t('<br><h1>Step 3</h1>'),
  );

  // Go to preview team page before committing formed teams to the system
  $form['button']['preview_teams'] = array(
    '#type' => 'submit',
    '#value' => t('Preview Teams'),
    '#submit' => array('uq_teams_create'),
  );

  // Hidden form element to store user constraint config
  $form['config']['user'] = array(
    '#type' => 'hidden',
    '#default_value' => $userSettingsJSON,
  );

  // Hidden form element to store course constraint config
  $form['config']['course'] = array(
    '#type' => 'hidden',
    '#default_value' => $courseSettingsJSON,
  );

  // Store the node id within form for later reference
  $form['store'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  return $form;
}

/*
 * Preview step form - for previewing formed teams before actually creating
* teams in Drupal
*
* @param unknown $form
* @param unknown $form_state
* @param unknown $node
* @return multitype:string NULL
*/
function uq_teams_wizard_preview_form($form, &$form_state, $node) {
  drupal_add_js(drupal_get_path('module', 'uq_teams') . '/js/collapse.js', array('cache' => FALSE));

  // Preview form description
  $form['preview_desc'] = array(
    '#type' => 'item',
    '#markup' => t('<h1>Preview Teams</h1><p>Check whether teams have fulfilled formation constraints before committing to system.</p>'),
  );

  // Variables for storing constraints and formed teams data structure
  $sizeConst = $form_state['storage']['size'];
  $constraintList = $form_state['storage']['constraints'];
  $teamList = $form_state['storage']['teams'];

  // Check for constraint satistfaction every time preview form is reloaded
  $constSatisfaction = array();

  foreach ($constraintList as $constraint) {
    $constField = $constraint->field;

    foreach ($teamList as $teamName => $value) {
      if ($teamName !== 'Unallocated') {
        // Obtain each constraint crieria to test formed teams
        $constSatisfaction[$teamName][$constField]['dist'] = $constraint->dist->val;
        $constSatisfaction[$teamName][$constField]['quant'] = $constraint->dist->quant;
        switch($constraint->dist->val) {
          case 0:
            $constSatisfaction[$teamName][$constField]['test'] = '';
            break;
          case 1:
            $constSatisfaction[$teamName][$constField]['test'] = array();
            break;
          case 2:
          case 3:
          case 4:
            $constSatisfaction[$teamName][$constField]['test'] = 0;
            $constSatisfaction[$teamName][$constField]['object'] = $constraint->dist->obj;
            if (isset($constraint->dist->op)) {
              $constSatisfaction[$teamName][$constField]['operation'] = $constraint->dist->op;
            }
            break;
        }

        $teamList[$teamName][$constField] = TRUE;
      }
    }
  }

  // Check each team member and determine whether previously defined
  // constraint/s has been satisfied
  foreach ($teamList  as $teamName => $teamMembers) {
    $constSatisfaction[$teamName]['count'] = 0;

    // Do not need constraint satisfaction for group containing unallocated
    // students
    if ($teamName !== 'Unallocated') {
      foreach ($teamMembers as $memberUid => $memberAttrib) {
        if (is_numeric($memberUid)) {
          $constSatisfaction[$teamName]['count']++;

          foreach ($constraintList as $constraint) {
            $constField = $constraint->field;

            switch ($constraint->dist->val) {
              case 0:
                if ($constSatisfaction[$teamName][$constField]['test'] === '') {
                  $constSatisfaction[$teamName][$constField]['test'] = $memberAttrib[$constField];
                } elseif ($memberAttrib[$constField] !== $constSatisfaction[$teamName][$constField]['test']) {
                  $teamList[$teamName][$constField] = FALSE;
                }

                break;
              case 1:
                if (in_array($constSatisfaction[$teamName][$constField]['test'], $memberAttrib[$constField])) {
                  $teamList[$teamName][$constField] = FALSE;
                } else {
                  array_push($constSatisfaction[$teamName][$constField]['test'], $memberAttrib[$constField]);
                }

                break;
              case 2:
              case 3:
              case 4:
                if (isset($constSatisfaction[$teamName][$constField]['operation'])) {
                  switch($constSatisfaction[$teamName][$constField]['operation']) {
                    case 'e':
                      if ($memberAttrib[$constField] === $constSatisfaction[$teamName][$constField]['object']) {
                        $constSatisfaction[$teamName][$constField]['test']++;
                      }

                      break;
                    case 'g':
                      if ($memberAttrib[$constField] > $constSatisfaction[$teamName][$constField]['object']) {
                        $constSatisfaction[$teamName][$constField]['test']++;
                      }

                      break;
                    case 'ge':
                      if ($memberAttrib[$constField] >= $constSatisfaction[$teamName][$constField]['object']) {
                        $constSatisfaction[$teamName][$constField]['test']++;
                      }

                      break;
                    case 'l':
                      if ($memberAttrib[$constField] < $constSatisfaction[$teamName][$constField]['object']) {
                        $constSatisfaction[$teamName][$constField]['test']++;
                      }

                      break;
                    case 'le':
                      if ($memberAttrib[$constField] <= $constSatisfaction[$teamName][$constField]['object']) {
                        $constSatisfaction[$teamName][$constField]['test']++;
                      }

                      break;
                  }
                } elseif ($memberAttrib[$constField] === $constSatisfaction[$teamName][$constField]['object']) {
                  $constSatisfaction[$teamName][$constField]['test']++;
                }

                break;
            }
          }
        }
      }
    }
  }

  foreach ($constSatisfaction as $teamName => $constraints) {
    switch ($sizeConst['min_max']) {
      case 'min':
        if (($constSatisfaction[$teamName]['count'] === (integer) $sizeConst['num']) ||
        ($constSatisfaction[$teamName]['count'] === ((integer) $sizeConst['num'] + 1))) {
          $teamList[$teamName]['size'] = TRUE;
        } else {
          $teamList[$teamName]['size'] = FALSE;
        }
        break;
      case 'max':
        if (($constSatisfaction[$teamName]['count'] === (integer) $sizeConst['num']) ||
        ($constSatisfaction[$teamName]['count'] === ((integer) $sizeConst['num'] - 1))) {
          $teamList[$teamName]['size'] = TRUE;
        } else {
          $teamList[$teamName]['size'] = FALSE;
        }
        break;
    }

    if ($teamName !== 'Unallocated') {
      foreach ($constraints as $constraint => $constSat) {
        switch ($constSat['dist']) {
          case 2:
            if ($constSatisfaction[$teamName][$constraint]['test'] === (integer) $constSat['quant']) {
              $teamList[$teamName][$constraint] = FALSE;
            }
            break;
          case 3:
            if ($constSatisfaction[$teamName][$constraint]['test'] > (integer) $constSat['quant']) {
              $teamList[$teamName][$constraint] = FALSE;
            }
            break;
          case 4:
            if ($constSatisfaction[$teamName][$constraint]['test'] < (integer) $constSat['quant']) {
              $teamList[$teamName][$constraint] = FALSE;
            }
            break;
        }
      }
    }
  }

  $header = 'Team Name';

  foreach ($constraintList as $constraint) {
    $constraintName = $constraint->const;
    $header = "$header,$constraintName";
  }

  $header = "$header,Id,Pid";

  $form['preview_header'] = array(
    '#type' => 'hidden',
    '#default_value' => $header,
  );

  $form['preview_items']['#attributes']['class'] = array('team-table');
  $form['preview_items']['#tree'] = TRUE;

  foreach ($teamList as $teamName => $teamSpec) {
    // Convert to proper key i.e. lowercase with underscores instead of spaces
    $teamKey = str_replace(' ', '_', $teamName);
    $teamKey = strtolower($teamKey);

    // Instantiate team row array
    $form['preview_items'][$teamKey] = array();

    foreach ($constraintList as $constraint) {
      $constField = $constraint->field;

      if ($teamName !== 'Unallocated') {
        if ($teamSpec[$constField]) {
          $form['preview_items'][$teamKey][$constField]['#markup'] = '<div style="background-color:#04B507; -moz-border-radius:10px; border-radius:10px; width:20px; height:20px; margin-left: auto; margin-right: auto;" id="fulfilled"></div>';
        } else {
          $form['preview_items'][$teamKey][$constField]['#markup'] = '<div style="background-color:#E31414; -moz-border-radius:10px; border-radius:10px; width:20px; height:20px; margin-left: auto; margin-right: auto;" id="unfulfilled"></div>';
        }
      } else {
        $form['preview_items'][$teamKey][$constField]['#markup'] = '';
      }
    }

    $form['preview_items'][$teamKey]['id'] = array(
      '#type' => 'textfield',
      '#value' => $teamKey,
      '#size' => 8,
    );

    $form['preview_items'][$teamKey]['pid'] = array(
      '#type' => 'textfield',
      '#default_value' => 0,
      '#size' => 8,
    );

    $form['preview_items'][$teamKey]['depth'] = array(
      '#type' => 'value',
      '#value' => 0,
    );

    // Used to count the number of members
    $numMem = 0;

    foreach ($teamSpec as $memberUid => $memberAttrib) {
      if (is_numeric($memberUid)) {
        $form['preview_items'][$memberUid] = array(
          'name' => array(
            '#markup' => $memberAttrib['name'],
          ),
        );

        foreach ($constraintList as $constraint) {
          $constField = $constraint->field;
          $form['preview_items'][$memberUid][$constField]['#markup'] = $memberAttrib[$constField];
        }

        $form['preview_items'][$memberUid]['id'] = array(
          '#type' => 'textfield',
          '#default_value' => $memberUid,
          '#size' => 8,

        );

        $form['preview_items'][$memberUid]['pid'] = array(
          '#type' => 'textfield',
          '#default_value' => $teamKey,
          '#size' => 8,
          '#ajax' => array(
            'callback' => 'uq_teams_preview_tabledrag_callback',
            'wrapper' => 'uq-teams-wizard-form',
            'method' => 'html',
            'event' => 'change',
            'progress' => array('type' => 'none'),
          ),
        );

        $form['preview_items'][$memberUid]['depth'] = array(
          '#type' => 'value',
          '#value' => 1,
        );

        $numMem++;
      }
    }

    // Create the name row after number of members have been counted
    if ($teamSpec['size'] || ($teamName === 'Unallocated')) {
      $form['preview_items'][$teamKey]['name'] = array(
        '#markup' => "<b>$teamName</b>&nbsp;&nbsp&nbsp;&nbsp;<font color=\"black\">($numMem)</font>",
      );
    } else {
      $form['preview_items'][$teamKey]['name'] = array(
        '#markup' => "<b>$teamName</b>&nbsp;&nbsp&nbsp;&nbsp;<font color=\"red\">($numMem)</font>",
      );
    }
  }

  $form['button'] = array(
    '#type' => 'actions',
  );

  $form['button']['preview_previous'] = array(
    '#type' => 'submit',
    '#value' => t('Previous'),
    '#submit' => array('uq_teams_wizard_previous_submit'),
  );

  $form['button']['preview_export_user'] = array(
    '#type' => 'submit',
    '#value' => t('Export'),
  );

  $form['button']['preview_commit_user'] = array(
    '#type' => 'submit',
    '#value' => t('Commit'),
  );

  return $form;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 * @return multitype:string multitype:NULL
 */
function uq_teams_preview_tabledrag_callback ($form, &$form_state) {
  $teamList = $form_state['storage']['teams'];


  // Get the list of students without being in formed teams
  $studentList = array();
  $newTeamList = array();

  foreach ($teamList as $teamName => $teamMembers) {
    foreach ($teamMembers as $teamMember => $memberStats) {
      if (is_numeric($teamMember)) {
        $studentList[$teamMember] = $memberStats;
      }
    }

    $newTeamList[$teamName] = array();
  }

  $currentTeam = '';
  $newUnalloc = array();

  foreach ($form_state['input']['preview_items'] as $key => $value) {
    if (is_numeric($key)) {
      if ($value['pid'] !== '0') {
        $newTeamList[$currentTeam][$key] = $studentList[$key];
      } else {
        $form_state['input']['preview_items'][$key]['pid'] = 'unallocated';
        $newUnalloc[$key] = $studentList[$key];
      }
    } else {
      $teamName = ucwords($key);
      $teamName = str_replace('_', ' ', $teamName);

      $currentTeam = $teamName;
    }
  }

  foreach ($newUnalloc as $unallocUid => $unallocProp) {
    $newTeamList['Unallocated'][$unallocUid] = $unallocProp;
  }

  $form_state['storage']['teams'] = $newTeamList;

  $new_state = array();
  $new_state['build_info'] = $form_state['build_info'];
  $new_state['rebuild'] = TRUE;
  $new_state['storage'] = $form_state['storage'];
  $new_state['values'] = array();
  $new_state['step'] = $form_state['step'];
  $new_state['step_information'] = $form_state['step_information'];
  $new_state += form_state_defaults();

  $new_form_array = drupal_rebuild_form('uq_teams_wizard_form', $new_state);
  $new_form = drupal_render($new_form_array);

  $commands = array();
  $commands[] = ajax_command_html('#uq-teams-wizard-form', $new_form);

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 *
 * @param unknown $variables
 * @return unknown
 */
function theme_uq_teams_wizard_form($variables) {
  $form = $variables['form'];

  $rows = array();
  if (isset($form['preview_items'])) {
    foreach (element_children($form['preview_items']) as $id) {
      foreach ($form['preview_items'][$id] as $propName => $propVal) {
        if (substr($propName, 0, 1) !==  '#') {
          $form['preview_items'][$id][$propName]['#attributes']['class'] = array("preview-item-$propName");
        }
      }

      $class = array('draggable');

      if (is_numeric($id)) {
        $class[] = 'tabledrag-leaf';
      } else {
        $class[] = 'tabledrag-root';
      }

      $indent = theme('indentation', array('size' => $form['preview_items'][$id]['depth']['#value']));
      unset($form['preview_items'][$id]['depth']);

      $rows[] = array(
        'data' => array(
          $indent . drupal_render($form['preview_items'][$id]['name']),
        ),
        'class' => $class,
      );

      foreach ($form['preview_items'][$id] as $propName => $propVal) {
        if ((substr($propName, 0, 1) !==  '#') && ($propName !== 'name')) {
          array_push($rows[count($rows) - 1]['data'], drupal_render($form['preview_items'][$id][$propName]));
        }
      }
    }


    $header = explode(',', $form['preview_header']['#value']);
    $tableId = 'preview-items-table';
    $output = drupal_render_children($form);
    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => $tableId)));

    drupal_add_tabledrag($tableId, 'match', 'parent', 'preview-item-pid', 'preview-item-pid', 'preview-item-id', FALSE);
    drupal_add_js(drupal_get_path('module', 'uq_teams') . '/js/tabledrag_ovr.js', array('cache' => FALSE));

    return $output;
  }
}


/*
 * Submit handler for the "previous" button
* - Decrements the step counter
*
* @param unknown $form
* @param unknown $form_state
*/
function uq_teams_wizard_previous_submit($form, &$form_state) {
  $current_step = &$form_state['step'];

  if ($current_step > 1) {
    $current_step--;
  }

  $form_state['rebuild'] = TRUE;
}

/*
 * Defines the Form layout and content for User and Course Attributes Tables
*
* @param unknown $name
* @param unknown $instance
* @param unknown $info
* @param unknown $type
* @return multitype:unknown multitype:multitype:string number multitype:multitype:string     multitype:multitype:string multitype:multitype:string   multitype:NULL    multitype:multitype:multitype:string unknown multitype:multitype:string    multitype:string number multitype:multitype:string    multitype:string multitype:multitype:string   multitype:NULL
*/
function _uq_teams_form_constraint_row($name, $instance, $info, $type) {
  $distOptions = array(
    0 => t('Homogeneous'),
    1 => t('Heterogeneous'),
    2 => t('Not Equal To'),
    3 => t('Fewer Than'),//'No More Than'),
    4 => t('At Least'),//'More Than Or Equal To'),
  );

  $priorityOptions = array(
    1 => t('1'),
    2 => t('2'),
    3 => t('3'),
    4 => t('4'),
    5 => t('5'),
    6 => t('6'),
    7 => t('7'),
    8 => t('8'),
    9 => t('9'),
  );

  $opOptions = array(
    'e' => t('='),
    'g' => t('>'),
    'ge' => t('>='),
    'l' => t('<'),
    'le' => t('<='),
  );

  $constOpt = array(
    'const' => $instance['label'],
    'dist' => array(
      'data' => array(
        0 => array(
          '#type' => 'hidden',
          '#value' => $info['module'],
          '#attributes' => array(
            'class' => array('field-mod'),
          ),
        ),
        1 => array(
          '#type' => 'hidden',
          '#value' => $type[1],
          '#attributes' => array(
            'class' => array('field-type'),
          ),
        ),
        2 => array(
          '#name' => 'dist-select',
          '#type' => 'select',
          '#options' => $distOptions,
          '#attributes' => array(
            'class' => array('dist-select', "$name-dist"),
          ),
        ),
        3 => array(
          '#type' => 'textfield',
          '#size' => 4,
          '#maxlength' => 4,
          '#attributes' => array(
            'class' => array('quant-field', "$name-quant"),
          ),
        ),
      ),
    ),
    'weight' => array(
      'data' => array(
        '#type' => 'textfield',
        '#size' => 1,
        '#maxlength' => 1,
        '#attributes' => array(
          'class' => array("$name-weight-field"),
        ),
      )
    ),/*
    'priority' => array(
      'data' => array(
        '#type' => 'select',
        '#size' => 1,
        '#maxlength' => 1,
        '#attributes' => array(
          'class' => array("$name-priority-field"),
        ),
      )
    ) */
    'priority' => array(
      'data' => array(
        '#name' => 'priority-select',
        '#type' => 'select',
        '#options' => $priorityOptions,
        '#attributes' => array(
          'class' => array("$name-priority-select"),
        ),
      ),
    ),
  );

  if ($info['module'] === 'number') {
    array_push($constOpt['dist']['data'], array(
    '#type' => 'select',
    '#options' => $opOptions,
    '#attributes' => array(
    'class' => array('op-select', "$name-op"),
    ),
    ));

    array_push($constOpt['dist']['data'], array(
    '#type' => 'textfield',
    '#size' => 4,
    '#maxlength' => 4,
    '#attributes' => array(
    'class' => array('obj-field', "$name-obj"),
    ),
    ));
  } else if ($info['module'] === 'list') {
    $allowedVal = $info['settings']['allowed_values'];

    array_push($constOpt['dist']['data'], array(
    '#type' => 'select',
    '#options' => $allowedVal,
    '#attributes' => array(
    'class' => array('obj-select', "$name-obj"),
    ),
    ));
  } else {
    drupal_set_message('Field type other than number or list.', 'error');
  }

  return $constOpt;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 * @return boolean
 */
function uq_teams_create($form, &$form_state) {
  $numMem = $form_state['values']['num_mem'];
  $minOrMax = $form_state['values']['min_or_max'];
  $userChecklist = $form_state['values']['user_attrib'];
  $courseChecklist = $form_state['values']['course_attrib'];
  $userSettingsJSON = $form_state['values']['user'];
  $courseSettingsJSON = $form_state['values']['course'];
  $userSettings = json_decode($userSettingsJSON);
  $courseSettings = json_decode($courseSettingsJSON);

  // Get the course node ID from current url and load in all students
  // who have enrolled in the course
  $urlPath = explode('/', current_path());

  if (($urlPath[0] === 'node') && (!empty($urlPath[1]))) {
    $courseNid = $urlPath[1];
  }

  if (!$courseNid) {
    drupal_set_message('Unable to obtain the course node ID.', 'error');
    return FALSE;
  }

  $savedTeamsConfig = variable_get('uq_teams_config');

  if (!$savedTeamsConfig) {
    $savedTeamsConfig = array(
      $courseNid => array(
        'num_mem' => $numMem,
        'min_or_max' => $minOrMax,
        'user_attrib' => $userChecklist,
        'course_attrib' => $courseChecklist,
        'user' => $userSettingsJSON,
        'course' => $courseSettingsJSON
      )
    );
  } else {
    unset($savedTeamsConfig[$courseNid]);
    $savedTeamsConfig[$courseNid] = array(
      'num_mem' => $numMem,
      'min_or_max' => $minOrMax,
      'user_attrib' => $userChecklist,
      'course_attrib' => $courseChecklist,
      'user' => $userSettingsJSON,
      'course' => $courseSettingsJSON
    );
  }

  // Store the current state of teams formation config form
  variable_set('uq_teams_config', $savedTeamsConfig);

  // Initialise the variables for storing constraint details used to form teams
  $userConstList = array();
  $courseConstList = array();

  // Determine the config for user attributes
  foreach ($userChecklist as $checked) {
    if (is_string($checked)) {
      foreach ($userSettings as $setting) {
        if ($checked === $setting->field) {
          $userConstList[$checked] = $setting;
        }
      }
    }
  }

  // Determine the config for course attributes
  foreach ($courseChecklist as $checked) {
    if (is_string($checked)) {
      foreach ($courseSettings as $setting) {
        if ($checked === $setting->field) {
          $courseConstList[$checked] = $setting;
        }
      }
    }
  }

  // Check if a copy of the DLV solver binary was placed in the libraries folder
  $binPath = $_SERVER['DOCUMENT_ROOT'] . base_path() . libraries_get_path('dlv'); // . '/./*.bin -filter=member test.dl';

  if (!is_dir($binPath)) {
    drupal_set_message('DLV binary missing or not installed correctly.', 'error');
    return FALSE;
  }

  // Check if the public file system contains a folder for storing DLV scripts
  $filePath = $_SERVER['DOCUMENT_ROOT'] . base_path() . variable_get('file_public_path', conf_path() . '/files') . '/dlv';

  if (!is_dir($filePath)) {
    if (!mkdir($filePath)) {
      drupal_set_message('Unable to create folder to store DLV scripts.', 'error');
      return FALSE;
    }
  }

  $fileNameNotChosen = TRUE;
  $fileNameFailCount = 0;

  // Find an unused filename by appending a randomly generated string
  // Must ensure that the file does not already exist on the server
  // Exit function and throw an error after 10 failed tries
  while ($fileNameNotChosen) {
    // @ - error supression
    $fileName = 'team-script-' . @_uq_teams_generate_random_string(10);
    $fileHandler = @fopen($filePath . '/' . $fileName . '.dl', 'r');

    if (!$fileHandler) {
      $fileNameNotChosen = FALSE;
    } else if (!fclose($fileHandler)) {
      drupal_set_message('Failed to close the DLV script file.', 'error');
      return FALSE;
    } else {
      $fileNameFailCount++;
    }

    if ($fileNameFailCount >= 10) {
      drupal_set_message('Failed to find an unused filename after 10 tries.', 'error');
      return FALSE;
    }
  }

  // Attempt to open the file for writing and return error if fail
  $fileHandler = fopen($filePath . '/' . $fileName . '.dl', 'w');

  if (!$fileHandler) {
    drupal_set_message('Failed to open script file for writing.', 'error');
    return FALSE;
  }

  // Need following to determine which fields are user and course attributes
  $userAttribList = variable_get('uq_teams_user_attributes');
  $courseAttribList = variable_get('uq_teams_course_attributes');

  // Merge user and course contraint lists and rank in terms of importance
  $constraintList = array_merge($userConstList, $courseConstList);
  $sortedConstList = array();

  for ($i = 9; $i > 0; $i--) {
    foreach ($constraintList as $constraint) {
      if ((integer) $constraint->priority === $i) {
        array_push($sortedConstList, $constraint);
      }
    }
  }

  // Get all the enrolled students in the course
  $enrolledStudentList = get_course_enrolled_students($courseNid);

  foreach ($enrolledStudentList as $student) {
    fwrite($fileHandler, 'student("' . $student->uid);

    if ($student) {
      foreach ($sortedConstList as $const) {
        $fieldValid = FALSE;
        if (in_array($const->field, $userAttribList)) {
          $userFieldName = $const->field;
          $studentField = $student->$userFieldName;
          $fieldValid = TRUE;
        } elseif (in_array($const->field, $courseAttribList)) {
          $courseFieldName = $const->field;
          $studentField = og_get_membership('node', $courseNid, 'user', $student->uid)->$courseFieldName;
          $fieldValid = TRUE;
        }

        if ($fieldValid) {
          $studentFieldValue = $studentField['und'][0]['value'];

          if (isset($const->dist->op)) {
            switch ($const->dist->val) {
              case 2:
              case 3:
              case 4:
                switch ($const->dist->op) {
                  case 'e':
                    fwrite($fileHandler, '","' . $studentFieldValue);
                    break;
                  case 'g':
                    if ($studentFieldValue > $const->dist->obj) {
                      fwrite($fileHandler, '","T');
                    } else {
                      fwrite($fileHandler, '","F');
                    }
                    break;
                  case 'ge':
                    if ($studentFieldValue >= $const->dist->obj) {
                      fwrite($fileHandler, '","T');
                    } else {
                      fwrite($fileHandler, '","F');
                    }
                    break;
                  case 'l':
                    if ($studentFieldValue < $const->dist->obj) {
                      fwrite($fileHandler, '","T');
                    } else {
                      fwrite($fileHandler, '","F');
                    }
                    break;
                  case 'le':
                    if ($studentFieldValue <= $const->dist->obj) {
                      fwrite($fileHandler, '","T');
                    } else {
                      fwrite($fileHandler, '","F');
                    }
                    break;
                }
                break;
              default:
                fwrite($fileHandler, '","' . $studentFieldValue);
            }
          } else {
            fwrite($fileHandler, '","' . $studentFieldValue);
          }
        }
      }
    }

    fwrite($fileHandler, "\").\n");
  }

  // Write the number of desired members to script
  fwrite($fileHandler, 'nmembers(' . $numMem . ").\n");

  // Ensure that members are evenly spreadout in the case where there are
  // remaining members to be sorted but too few (or many) to be placed
  // in a new team
  // Determine the number of teams required and add to script

  // Keep a list of team names
  $teamList = array();

  if ($minOrMax === 'min') {
    fwrite($fileHandler, 'nmemmax(' . ($numMem + 1) . ").\n");

    $numTeams = (integer) floor(count($enrolledStudentList) / $numMem);

    for ($i = 1; $i <= $numTeams; $i++) {
      $teamList["Team $i"] = array();
      fwrite($fileHandler, 'team("Team ' . $i . "\").\n");
    }
  } else {
    fwrite($fileHandler, 'nmemmin(' . ($numMem - 1) . ").\n");

    $numTeams = (integer) ceil(count($enrolledStudentList) / $numMem);

    for ($i = 1; $i <= $numTeams; $i++) {
      $teamList["Team $i"] = array();
      fwrite($fileHandler, 'team("Team ' . $i . "\").\n");
    }
  }

  // Write to script the basic constraints:
  // - Student is either a member or a non-member of a group
  // - Student can only be in one group
  // - Each group must have at least or at most a specified number of students
  //   (depends on teams formation configuration)

  // E = Email
  // F, G, ... M, N = Reserved for Attributes
  $attribVarList = array('F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N');

  // T = Team
  // A, B, C, D = Reserved for Mappers
  $mapVarList = array('A', 'B', 'C', 'D');

  // Store next unused attribute variable
  $attribVarIdx = 0;

  // Store next unused mapper variable
  $mapVarIdx = 0;

  // Create the member script string
  $memberStr = 'member(E';
  $studentStr = 'student(E';

  foreach ($constraintList as $constraint) {
    $memberStr = $memberStr . ',' . $attribVarList[$attribVarIdx];
    $studentStr = $studentStr . ',' . $attribVarList[$attribVarIdx];
    $attribVarIdx++;
  }

  $studentStr = $studentStr . ')';
  $memberStr = $memberStr . ',T)';

  fwrite($fileHandler, $memberStr . ' v not_' . $memberStr . ' :- ' . $studentStr . ", team(T).\n");
  fwrite($fileHandler, ':- ' . str_replace($attribVarList, '_', $studentStr) . ', not #count{T : ' . str_replace($attribVarList, '_', $memberStr) . "} = 1.\n");

  if ($minOrMax === 'min') {
    fwrite($fileHandler, ':- team(T), nmembers(C), not #count{E : ' . str_replace($attribVarList, '_', $memberStr) . "} >= C.\n");
    fwrite($fileHandler, ':- team(T), nmemmax(C), not #count{E : ' . str_replace($attribVarList, '_', $memberStr) . "} <= C.\n");
  } else {
    fwrite($fileHandler, ':- team(T), nmembers(C), not #count{E : ' . str_replace($attribVarList, '_', $memberStr) . "} <= C.\n");
    fwrite($fileHandler, ':- team(T), nmemmin(C), not #count{E : ' . str_replace($attribVarList, '_', $memberStr) . "} >= C.\n");
  }

  // Return the attribute variable index back to 0
  $attribVarIdx = 0;
  $costBound = array();
  $costBoundStr = "";
  $priorityLen = $sortedConstList[0]->priority;

  for ($i = 0; $i < $priorityLen; $i++) {
    $costBound[$i] = '_';
  }

  // Write to script the config constraints
  foreach ($sortedConstList as $constraint) {
    switch($constraint->dist->val) {
      case 0: // Homogeneous
        $repNum = 1;
        $varPos = strpos($memberStr, $attribVarList[$attribVarIdx]);
        $memberStrUnder = str_replace($attribVarList, '_', $memberStr);
        $memberStrVar = substr_replace($memberStrUnder, 'C', $varPos, 1);
        $memberStrMod = str_replace('E', 'A', $memberStrVar, $repNum);

        $compMemStrUnder = str_replace($attribVarList, '_', $memberStr);
        $compMemStrVar = substr_replace($compMemStrUnder, 'D', $varPos, 1);
        $compMemStrMod = str_replace('E', 'B', $compMemStrVar, $repNum);

        fwrite($fileHandler, ":~ team(T), $memberStrMod, " . $compMemStrMod . ", " . $mapVarList[0] . " != " . $mapVarList[1] . ", " . $mapVarList[2] . " != " . $mapVarList[3] . ". [" . $constraint->weight . ":" . $constraint->priority . "]\n");

        break;
      case 1: // Heterogeneous
        $repNum = 1;
        $varPos = strpos($memberStr, $attribVarList[$attribVarIdx]);
        $memberStrUnder = str_replace($attribVarList, '_', $memberStr);
        $memberStrVar = substr_replace($memberStrUnder, 'C', $varPos, 1);
        $memberStrMod = str_replace('E', 'A', $memberStrVar, $repNum);

        $compMemStrUnder = str_replace($attribVarList, '_', $memberStr);
        $compMemStrVar = substr_replace($compMemStrUnder, 'D', $varPos, 1);
        $compMemStrMod = str_replace('E', 'B', $compMemStrVar, $repNum);

        fwrite($fileHandler, ":~ team(T), " . $memberStrMod . ", " . $compMemStrMod . ", " . $mapVarList[0] . " != " . $mapVarList[1] . ", " . $mapVarList[2] . " = " . $mapVarList[3] . ". [" . $constraint->weight . ":" . $constraint->priority . "]\n");

        break;
      case 2: // Not Equal To
        $varPos = strpos($memberStr, $attribVarList[$attribVarIdx]);
        $memberStrMod = str_replace($attribVarList, '_', $memberStr);

        if ((isset($constraint->dist->op)) && ($constraint->dist->op !== 'e')) {
          fwrite($fileHandler, ":~ team(T), #count{E : " . substr_replace($memberStrMod, "\"T\"", $varPos, 1) . "} = " . (integer) $constraint->dist->quant  . ". [" . $constraint->weight . ":" . $constraint->priority . "]\n");
        } else {
          fwrite($fileHandler, ":~ team(T), #count{E : " . substr_replace($memberStrMod, "\"" . $constraint->dist->obj . "\"", $varPos, 1) . "} = " . (integer) $constraint->dist->quant  . ". [" . $constraint->weight . ":" . $constraint->priority . "]\n");
        }

        break;
      case 3: // No More Than
        $varPos = strpos($memberStr, $attribVarList[$attribVarIdx]);
        $memberStrMod = str_replace($attribVarList, '_', $memberStr);

        if ((isset($constraint->dist->op)) && ($constraint->dist->op !== 'e')) {
          fwrite($fileHandler, ":~ team(T), not #count{E : " . substr_replace($memberStrMod, "\"T\"", $varPos, 1) . "} <= " . (integer) $constraint->dist->quant . ". [" . $constraint->weight . ":" . $constraint->priority . "]\n");
        } else {
          fwrite($fileHandler, ":~ team(T), not #count{E : " . substr_replace($memberStrMod, "\"" . $constraint->dist->obj . "\"", $varPos, 1) . "} <= " . (integer) $constraint->dist->quant . ". [" . $constraint->weight . ":" . $constraint->priority . "]\n");
        }

        break;
      case 4: // More Than Or Equal To
        $varPos = strpos($memberStr, $attribVarList[$attribVarIdx]);
        $memberStrMod = str_replace($attribVarList, '_', $memberStr);

        if ((isset($constraint->dist->op)) && ($constraint->dist->op !== 'e')) {
          fwrite($fileHandler, ":~ team(T), not #count{E : " . substr_replace($memberStrMod, "\"T\"", $varPos, 1) . "} >= " . (integer) $constraint->dist->quant . ". [" . $constraint->weight . ":" . $constraint->priority . "]\n");
        } else {
          fwrite($fileHandler, ":~ team(T), not #count{E : " . substr_replace($memberStrMod, "\"" . $constraint->dist->obj . "\"", $varPos, 1) . "} >= " . (integer) $constraint->dist->quant . ". [" . $constraint->weight . ":" . $constraint->priority . "]\n");
        }

        break;
    }

    $priorityIdx = $constraint->priority - 1;

    $costBound[$priorityIdx] = 0;
    $shellOut = FALSE;
    $attemptNum = 1;

    // Execute the binary with the script as an input
    // Only output the first best model and filter on member
    while (empty($shellOut)) {
      for ($i = 0; $i < $priorityLen; $i++) {
        if ($i === 0) {
          $costBoundStr = $costBound[0];
        } else {
          $costBoundStr = "$costBoundStr,$costBound[$i]";
        }
      }

      // Reverse expontential function to calculate wait time
      $waitTime = ceil(-1.4 * $attemptNum + 21.4);
      if ($waitTime <= 6) {
        $waitTime = 6;
      }

      $shellOut = shell_exec("perl -e \"alarm $waitTime; exec @ARGV\" \"$binPath/./dlv.x86-64-linux-elf-static.bin -costbound=$costBoundStr -n=1 -silent -filter=member $filePath/$fileName.dl\"");
      dsm("perl -e \"alarm $waitTime; exec @ARGV\" \"$binPath/./dlv.x86-64-linux-elf-static.bin -costbound=$costBoundStr -n=1 -silent -filter=member $filePath/$fileName.dl\"");

      if ($costBound[$priorityIdx] < 1) {
        $costBound[$priorityIdx] = 1;
      } elseif ($costBound[$priorityIdx] < 8) {
        $costBound[$priorityIdx] = $costBound[$priorityIdx] * 2;
      } elseif ($costBound[$priorityIdx] < 12) {
        $costBound[$priorityIdx] = $costBound[$priorityIdx] + 4;
      } elseif ($costBound[$priorityIdx] < 20) {
        $costBound[$priorityIdx] = $costBound[$priorityIdx] + 8;
      } else {
        $costBound[$priorityIdx] = $costBound[$priorityIdx] + 10;
      }

      $attemptNum++;
    }

    dsm($shellOut);

    $currentCost = _uq_teams_parse_dlv_get_cost($shellOut, $priorityIdx);
    $costBound[$priorityIdx] = $currentCost;
    if ($attribVarIdx === 0) {
      $costBoundStr = $currentCost;
    } else {
      $costBoundStr = "$costBoundStr,$currentCost";
    }
    $attribVarIdx++;
  }
  if (!fclose($fileHandler)) {
    drupal_set_message('Failed to close the DLV script file.', 'error');
    return FALSE;
  }
  // Convert the shell output into an array of students
  $processString = explode('{', $shellOut);
  $processString = explode('}', $processString[1]);
  // Remove member tag around each student
  $processString = explode('member(', $processString[0]);
  array_shift($processString);
  foreach ($processString as $key => $user) {
    $user = explode(')', $user);
    $processString[$key] = $user[0];
  }
  // Determine the number of constraints
  $constNum = count($sortedConstList);
  // Convert from DLV output string into structured data
  foreach ($processString as $key => $student) {
    $student = str_replace('"', '', $student);
    $studentDetails = explode(',', $student);
    $studentLoad = user_load($studentDetails[0]);
    $studentUid = $studentDetails[0];
    $studentTeam = $studentDetails[$constNum + 1];
    $detailCount = 1;
    $teamList[$studentTeam][$studentUid]['name'] = $studentLoad->field_fullname['und'][0]['value'];
    foreach ($sortedConstList as $constraint) {
      $constField = $constraint->field;
      if (in_array($constField, $userAttribList)) {
        $studentField = $studentLoad->$constField;
      } elseif (in_array($constField, $courseAttribList)) {
        $studentField = og_get_membership('node', $courseNid, 'user', $studentUid)->$constField;
      }
      $teamList[$studentTeam][$studentUid][$constField] = $studentField['und'][0]['value'];
      $detailCount++;
    }
  }
  $teamList['Unallocated'] = array();
  $form_state['storage']['size']['num'] = $numMem;
  $form_state['storage']['size']['min_max'] = $minOrMax;
  $form_state['storage']['teams'] = $teamList;
  $form_state['storage']['constraints'] = $sortedConstList;
  // Move to the next step
  $form_state['step'] = 2;
  $form_state['rebuild'] = TRUE;
}

/**
 * Gets the cost of a computed model
 *  - If priority is specified, returns numeric cost value
 *  - If priority is NULL, return entire cost list
 *
 * @param unknown $dlv_out
 * @param string $priority
 * @return boolean
 */
function _uq_teams_parse_dlv_get_cost($dlv_out, $priority = NULL) {
  // Example: <[1:1],[1:2]>
  $costReport = explode('<', $dlv_out);
  $costReport = explode('>', $costReport[1]);
  // Example: [1:1],[1:2]
  if (count($costReport) === 2) {
    if (is_int($priority)) {
      $costStr = explode(',', $costReport[0]);
      $costStr = explode(':', $costStr[$priority]);
      $costStr = explode('[', $costStr[0]);
      $parseOut = $costStr[1];
    }
    else {
      $costList = array();
      $costStr = explode(':', $costReport[0]);
      foreach ($costStr as $priCost) {
        $cost = explode('[', $priCost);
        if (count($cost) === 2) {
          array_push($costList, explode('[', $cost[1]));
        }
      }
      $parseOut = $costList;
    }
  }
  else {
    drupal_set_message('DLV output contains more than one model.', 'error');
    $parseOut = FALSE;
  }
  return $parseOut;
}

/**
 *
 * @param unknown $length
 * @return string
 */
function _uq_teams_generate_random_string($length) {
  $characters = '0123456789abcdefghijklmnopqrstuvwxyz';
  $string = '';

  for ($i = 0; $i < $length; $i++) {
    $string .= $characters[mt_rand(0, strlen($characters))];
  }
  return $string;
}

/**
 * Retrieve a list of user profiles of students that are enrolled in $courseid
 *
 * @param unknown $courseid
 * @return multitype:
 */
function get_course_enrolled_students($courseid) {
  $studentList = get_all_students();
  $enrolledList = array();
  foreach ($studentList as $student) {
    foreach ($student->og_user_group_ref['und'] as $ref) {
      if ($ref['target_id'] === (string) $courseid) {
        array_push($enrolledList, $student);
      }
    }
  }
  return $enrolledList;
}

/*
 * Retrieve a list of all students in the system
*
* @return multitype:
*/
function get_all_students() {
  $users = entity_load('user');
  $selectedUsers = array();

  foreach($users as $user) {
    if (array_key_exists(array_search('Student', user_roles(TRUE)), $user->roles)) {
      array_push($selectedUsers, $user);
    }
  }
  return $selectedUsers;
}