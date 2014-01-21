<?php

/**
 * @file
 * Model editing UI.
 *
 * We make very little use of the EntityAPI interface for this - preferring instead to use
 * views. That offers more flexibility to change a UI that will, more often than not,
 * be end-user facing.
 */

/**
 * UI controller.
 */
class RotaUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {
    
    $items = array();
    $id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%' . $this->entityType;

    $items[$this->path] = array(
      'title' => 'Rotas',
      'description' => 'Add edit and update rotas.',
      'page callback' => 'system_admin_menu_block_page',
      'access arguments' => array('access administration pages'),
      'file path' => drupal_get_path('module', 'system'),
      'file' => 'system.admin.inc',
    );
    
    // Change the overview menu type for the list of rotas.
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    
    // Change the add page menu to multiple types of entities
    $items[$this->path . '/add'] = array(
      'title' => 'Add a rota',
      'description' => 'Add a new rota',
      'page callback' => 'party_rota_form_wrapper',
      'access callback'  => 'party_rota_access',
      'access arguments' => array('edit'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => 20,
      'file' => 'party_rota.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])

    );

    // Loading and editing rota entities
    $items[$this->path . '/rota/' . $wildcard] = array(
      'page callback' => 'party_rota_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'party_rota_access',
      'access arguments' => array('edit'),
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'party_rota.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    $items[$this->path . '/rota/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    );
    
    $items[$this->path . '/rota/' . $wildcard . '/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'party_rota_delete_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'party_rota_access',
      'access arguments' => array('edit', $id_count + 1),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
      'weight' => 10,
      'file' => 'party_rota.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    
    // Menu item for viewing rotas
    $items['rota/' . $wildcard] = array(
      //'title' => 'Title',
      'title callback' => 'party_rota_page_title',
      'title arguments' => array(1),
      'page callback' => 'party_rota_page_view',
      'page arguments' => array(1),
      'access callback' => 'party_rota_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );

    return $items;
  }
  
}


/**
 * Form callback wrapper: create or edit a model.
 *
 * @param $model
 *   The model object being edited by this form.
 *
 * @see model_edit_form()
 */
function party_rota_form_wrapper($rota = NULL) {
  if (!$rota) {
    $rota = party_rota_create();
  }

  // Add the breadcrumb for the form's location.
  party_rota_set_breadcrumb();
  return drupal_get_form('party_rota_edit_form', $rota);
}


/**
 * Form callback wrapper: delete a rota.
 *
 * @param $rota
 *   The rota object being edited by this form.
 *
 * @see party_rota_edit_form()
 */
function party_rota_delete_form_wrapper($rota) {
  return drupal_get_form('party_rota_delete_form', $rota);
}


/**
 * Form callback: create or edit a rota.
 *
 * @param $rota
 *   The rota object to edit or for a create form an empty rota object
 *     with only a rota type defined.
 */
function party_rota_edit_form($form, &$form_state, $rota) {
  // Add the default field elements.
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Rota Name'),
    '#default_value' => isset($rota->name) ? $rota->name : '',
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );

  // Add the field related form elements.
  $form_state['rota'] = $rota;
  field_attach_form('rota', $rota, $form, $form_state);

  $form['slots_positions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Slots and Positions'),
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => isset($rota->is_new) ? TRUE : FALSE,
  );

  $form['slots_positions']['slots'] = array();

  $form['slots_positions']['slots'] = party_rota_slot_inline_edit_form($form['slots_positions']['slots'], $form_state, $rota);

  $form['slots_positions']['positions'] = array();

  $form['slots_positions']['positions'] = party_rota_position_inline_edit_form($form['slots_positions']['positions'], $form_state, $rota);

  /*$form['add_to_dock'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add to Rota'),
    '#weight' => 3,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['add_to_dock']['add_assignment'] = array();

  $form['add_to_dock']['add_assignment'] = party_rota_assignment_inline_edit_form($form['add_to_dock']['add_assignment'], $form_state, $rota);

  $form['dock_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rota members'),
    '#weight' => 4,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );*/

  $form['rota_layout'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rota Layout'),
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => isset($rota->is_new) ? TRUE : FALSE,
  );



  if (!isset($form_state['slots'])) {
    $slots = FALSE;
  }
  else {
    $slots = $form_state['slots'];
  }

  if (!isset($form_state['positions'])) {
    $positions = FALSE;
  }
  else {
    $positions = $form_state['positions'];
  }

  if ($slots && $positions) {
    $header = array(
      'slot' => array('data' => t('Slot')),
    );
//admin/party_rota/%ctools_js/add/%rota_position/%rota_slot
    $rows = array();

    ctools_include('modal');
    ctools_include('ajax');
    ctools_modal_add_js();

    foreach ($slots as $sid => $slot) {
      $label = '';
      if (!empty($slot->name)) {
        $label = $slot->name . ' ( ' . date('d/m/Y H:i', $slot->date) . ' )';
      }
      else {
        $label = date('d/m/Y H:i', $slot->date);
      }
      $rows[$sid] = array(
        'slot' => $label,
      );

      foreach ($positions as $pid => $position) {
        $header[$pid] = array('data' => $position->name);
        if ($assignment = party_rota_get_assignment_from_context($rota, $pid, $sid)) {
          $delete_link = ctools_modal_text_button(t('Remove'), 'admin/party_rota/nojs/delete/' . $assignment->assignment_id, t('Remove'), 'ctools-modal-modal-popup-medium');
          $rows[$sid][$pid] = $assignment->name . '  (' . $delete_link . ')';
        }
        else {
          $add_link = ctools_modal_text_button(t('Add'), 'admin/party_rota/nojs/add/' . $pid . '/' . $sid, t('Add new'), 'ctools-modal-modal-popup-medium');
          $rows[$sid][$pid] = $add_link;
        }      
      }
    }

    $form['rota_layout']['table'] = array(
      '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
    );

  }
  else {
    $form['rota_layout']['dock'] = array(
      '#markup' => t('Please make sure this Rota has both Slots and Positions.'),
    );
  }

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save rota'),
    '#submit' => $submit + array('party_rota_edit_form_submit'),
  );
  

  if (!empty($rota->name)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete rota'),
      '#suffix' => l(t('Cancel'), 'admin/content/rotas'),
      '#submit' => $submit + array('party_rota_form_submit_delete'),
      '#weight' => 45,
    );
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'party_rota_edit_form_validate';
  return $form;
}


/**
 * Form API validate callback for the rota form
 */
function party_rota_edit_form_validate(&$form, &$form_state) {
  $rota = $form_state['rota'];
  
  // Notify field widgets to validate their data.
  field_attach_form_validate('rota', $rota, $form, $form_state);
}


/**
 * Form API submit callback for the rota form.
 * 
 * @todo remove hard-coded link
 */
function party_rota_edit_form_submit(&$form, &$form_state) {
  
  $rota = entity_ui_controller('rota')->entityFormSubmitBuildEntity($form, $form_state);
  // Save the rota and go back to the list of rotas
  
  // Add in created and changed times.
  if ($rota->is_new = isset($rota->is_new) ? $rota->is_new : 0){
    $rota->created = time();
  }

  $rota->changed = time();
  
  $rota->save();
  $form_state['redirect'] = 'admin/content/rotas';
}

/**
 * Form API submit callback for the delete button.
 * 
 * @todo Remove hard-coded path
 */
function party_rota_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/content/rotas/rota/' . $form_state['rota']->rota_id . '/delete';
}


/**
 * Form callback: confirmation form for deleting a rota.
 *
 * @param $rota
 *   The rota to delete
 *
 * @see confirm_form()
 */
function party_rota_delete_form($form, &$form_state, $rota) {
  $form_state['rota'] = $rota;

  $form['#submit'][] = 'party_rota_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete rota %name?', array('%name' => $rota->name)),
    'admin/content/rotas/rota',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );
  
  return $form;
}

/**
 * Submit callback for party_rota_delete_form
 */
function party_rota_delete_form_submit($form, &$form_state) {
  $rota = $form_state['rota'];

  party_rota_delete($rota);

  drupal_set_message(t('The rota %name has been deleted.', array('%name' => $rota->name)));
  watchdog('rota', 'Deleted rota %name.', array('%name' => $rota->name));

  $form_state['redirect'] = 'admin/content/rotas';
}

/**
 * Displays the list of available rota types for rota creation.
 *
 * @ingroup themeable
 */
function theme_party_rota_add_list($variables) {
  $content = $variables['content'];
  $output = '';
  if ($content) {
    $output = '<dl class="rota-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    if (user_access('administer rota types')) {
      $output = '<p>' . t('Rota Entities cannot be added because you have not created any rota types yet. Go to the <a href="@create-rota-type">rota type creation page</a> to add a new rota type.', array('@create-rota-type' => url('admin/structure/rota_types/add'))) . '</p>';
    }
    else {
      $output = '<p>' . t('No rota types have been created yet for you to use.') . '</p>';
    }
  }

  return $output;
}





/**
 * Sets the breadcrumb for administrative rota pages.
 */
function party_rota_set_breadcrumb() {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Content'), 'admin/content'),
    l(t('Rotas'), 'admin/content/rotas'),
  );

  drupal_set_breadcrumb($breadcrumb);
}



function party_rota_slot_inline_edit_form($form, &$form_state, $rota) {
  if (!isset($form_state['rota'])) {
    $form_state['rota'] = $rota;
  }

  // Get the slots for this rota.
  $slots = party_rota_get_slots_from_rota($rota);

  $form['slots'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rota Slots'),
    '#weight' => 1,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  if (!$slots) {
    $form['slots']['no_slots'] = array('#markup' => t('No slots have been created yet.'));
  }
  else {
    $form_state['slots'] = $slots;

    foreach ($slots as $slot) {
      $form['slots']['slot_id_' . $slot->slot_id] = array(
        '#markup' => $slot->name,
      );
    }
  }

  $form['add_slot'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Slots to Rota'),
    '#weight' => 2,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['add_slot']['slot_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Name',
    //'#required' => TRUE,
  );

  $form['add_slot']['slot_description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
  );

  $form['add_slot']['slot_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Date'),
    //'#required' => TRUE,
  );

  $form['add_slot']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add slot'),
    '#submit' => array('party_rota_slot_inline_edit_form_submit'),
    '#validate' => array('party_rota_slot_inline_edit_form_validate'),
  );

  return $form;
}

function party_rota_slot_inline_edit_form_validate(&$form, &$form_state) {
  if (!empty($form_state['values']['slot_description']) && strlen($form_state['values']['slot_description']) > 255) {
    form_set_error('slot_description', t('Your description is too long, please use less than 255 characters.'));
  }
}

function party_rota_slot_inline_edit_form_submit(&$form, &$form_state) {
  $values = array(
    'rota_id' => $form_state['rota']->rota_id,
    'name' => $form_state['values']['slot_name'],
    'description' => $form_state['values']['slot_description'],
    'date' => strtotime($form_state['values']['slot_date']),
  );

  $slot = party_rota_slot_create($values);
  $slot->save();
  drupal_set_message(t('New slot @name has been created.', array('@name' => $slot->name)), 'status');
}

function party_rota_position_inline_edit_form($form, &$form_state, $rota) {
  if (!isset($form_state['rota'])) {
    $form_state['rota'] = $rota;
  }

  // Get the slots for this rota.
  $positions = party_rota_get_positions_from_rota($rota);

  $form['positions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rota positions'),
    '#weight' => 1,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  if (!$positions) {
    $form['positions']['no_positions'] = array('#markup' => t('No positions have been created yet.'));
  }
  else {
    $form_state['positions'] = $positions;

    foreach ($positions as $position) {
      $form['positions']['position_id_' . $position->position_id] = array(
        '#markup' => $position->name,
      );
    }
  }

  $form['add_position'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Positions to Rota'),
    '#weight' => 2,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['add_position']['position_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    //'#required' => TRUE,
  );

  $form['add_position']['position_description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
  );


  $form['add_position']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add position'),
    '#submit' => array('party_rota_position_inline_edit_form_submit'),
    '#validate' => array('party_rota_position_inline_edit_form_validate'),
  );

  return $form;
}

function party_rota_position_inline_edit_form_validate(&$form, &$form_state) {
  // Make sure the position has a name.
  if (empty($form_state['values']['position_name'])) {
    form_set_error('position_name', t('Please choose a name for the new position.'));
  }

  if (!empty($form_state['values']['position_description']) && strlen($form_state['values']['position_description']) > 255) {
    form_set_error('position_description', t('Your description is too long, please use less than 255 characters.'));
  }
}

function party_rota_position_inline_edit_form_submit(&$form, &$form_state) {
  $values = array(
    'rota_id' => $form_state['rota']->rota_id,
    'name' => $form_state['values']['position_name'],
    'description' => $form_state['values']['position_description'],
  );

  $position = party_rota_position_create($values);
  $position->save();
  drupal_set_message(t('New position @name has been created.', array('@name' => $slot->name)), 'status');
}




/*function party_rota_assignment_inline_edit_form($form, &$form_state, $rota) {
  if (!isset($form_state['rota'])) {
    $form_state['rota'] = $rota;
  }

  // Get the slots for this rota.
  $assignments = party_rota_get_assignments_from_rota($rota);


  if ($assignments) {
    $form_state['assignments'] = $assignments;
  }

  $form['add_assignment'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add assignments to Rota'),
    '#weight' => 2,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['add_assignment']['assignment_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Name',
  );

  $form['add_assignment']['assignment_pid'] = array(
    '#type' => 'textfield',
    '#title' => 'Party ID',
  );

  $form['add_assignment']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add assignment'),
    '#submit' => array('party_rota_assignment_inline_edit_form_submit'),
    '#validate' => array('party_rota_assignment_inline_edit_form_validate'),
  );

  return $form;
}

function party_rota_assignment_inline_edit_form_validate(&$form, &$form_state) {
  // Make sure the assignment has a name.
  if (empty($form_state['values']['assignment_name'])) {
    form_set_error('assignment_name', t('Please choose a name for the new assignment.'));
  }

  if (!empty($form_state['values']['assignment_pid'])) {
    $party = party_load($form_state['values']['assignment_pid']);

    if (!$party) {
      form_set_error('assignment_pid', t('Party could not be found.'));
    }
    else {
      if (isset($form_state['assignments'])) {
        $assignments = $form_state['assignments'];
        $pids = party_rota_get_pids_from_assignments($assignments);

        if (in_array($party->pid, $pids)) {
          form_set_error('assignment_pid', t('Party @name has already been added.', array('@name' => $party->label)));
        }
      }
      $form_state['values']['#party'] = $party;
    }
  }
}

function party_rota_assignment_inline_edit_form_submit(&$form, &$form_state) {
  if (isset($form_state['values']['#party'])) {
    $party = $form_state['values']['#party'];
  }
  else {
    $party = FALSE;
  }

  $values = array(
    'rota_id' => $form_state['rota']->rota_id,
    'name' => $form_state['values']['assignment_name'],
  );

  if ($party) {
    $values += array(
      'party_id' => $party->pid,
    );

    $values['name'] = $party->label;

  }

  $assignment = party_rota_assignment_create($values);
  $assignment->save();
  drupal_set_message(t('New assignment @name has been created.', array('@name' => $assignment->name)), 'status');
}


*/


function party_rota_inline_ajax_add($js = NULL, $position, $slot) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('party_rota_inline_ajax_add_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    // 'title' => t('Log in'),
    'ajax' => TRUE,
    'build_info' => array(
      'args' => array(
        0 => $position,
        1 => $slot,
      ),
    ),
  );

  $output = ctools_modal_form_wrapper('party_rota_inline_ajax_add_form', $form_state);
  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.
    $output = array();
    ctools_add_js('ajax-responder');
    $output[] = ctools_modal_command_dismiss(t('Add success'));
    if (isset($_GET['destination'])) {
      $output[] = ctools_ajax_command_redirect($_GET['destination']);
    }
    else {
      $output[] = ctools_ajax_command_reload();
    }
  }
  print ajax_render($output);

  //return 'test';
}

function party_rota_inline_ajax_add_form($form, &$form_state, $position, $slot) {
  if ($position->rota_id != $slot->rota_id) {
    return array(array(
      '#markup' => 'There was a problem. You are trying to add an assigment to a slot and position that attach to seperate rotas.',
    ));  
  }

  $form_state['position'] = $position;
  $form_state['slot'] = $slot;
  $rota = entity_load_single('rota', $slot->rota_id);
  $form_state['rota'] = $rota;
  $parties = party_rota_get_parties_from_rota($rota);
  $assignments = party_rota_get_assignments_from_parties($parties, $rota);

  $form_state['assignments'] = $assignments;

  $options = array();
  foreach ($assignments as $assignment) {
    $party = party_load($assignment->party_id);
    $options[$assignment->assignment_id] = $party->label;
  }

  $form['party_assignment'] = array(
    '#type' => 'select',
    '#title' => t('Party'),
    '#options' => $options,
    //'#default_value' => $options[],
    '#description' => t('Select a Party to assign to this slot.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Assign'),
  );

  return $form;
}

function party_rota_inline_ajax_add_form_validate(&$form, &$form_state) {
}

function party_rota_inline_ajax_add_form_submit(&$form, &$form_state) {
  $assignment = entity_load_single('rota_assignment', $form_state['values']['party_assignment']);
  $assignment->slot_id = $form_state['slot']->slot_id;
  $assignment->position_id = $form_state['position']->position_id;

  $assignment->save();
}

function party_rota_inline_ajax_delete($js = NULL, $assignment) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('party_rota_inline_ajax_delete_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    // 'title' => t('Log in'),
    'ajax' => TRUE,
    'build_info' => array(
      'args' => array(
        0 => $assignment,
      ),
    ),
  );

  $output = ctools_modal_form_wrapper('party_rota_inline_ajax_delete_form', $form_state);
  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.
    $output = array();
    ctools_add_js('ajax-responder');
    $output[] = ctools_modal_command_dismiss(t('Delete success'));
    if (isset($_GET['destination'])) {
      $output[] = ctools_ajax_command_redirect($_GET['destination']);
    }
    else {
      $output[] = ctools_ajax_command_reload();
    }
  }
  print ajax_render($output);

  //return 'test';
}

function party_rota_inline_ajax_delete_form($form, &$form_state, $assignment) {
  if (!$assignment) {
    return array(array(
      '#markup' => 'There was a problem. You are trying to delete an assigment that does not exist.',
    ));  
  }

  $form_state['assignment'] = $assignment;

  $question = t('Are you sure you want to delete this assignment?');

  return confirm_form($form, $question, $path, NULL, t('Delete'));
}

function party_rota_inline_ajax_delete_form_validate(&$form, &$form_state) {
}

function party_rota_inline_ajax_delete_form_submit(&$form, &$form_state) {
  $assignment = $form_state['assignment'];

  $assignment->delete();
}