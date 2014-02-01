<?php

/**
 * @file
 * Rota editing UI.
 */

/**
 * RotaUI controller.
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
 * Form callback wrapper: create or edit a rota.
 *
 * @param $rota
 *   The rota object being edited by this form.
 *
 * @see party_rota_edit_form()
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
 * @see party_rota_delete_form()
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
  // Add ajax modal popup dependencies.
  ctools_include('modal');
  ctools_include('ajax');
  ctools_modal_add_js();

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

  // Get the slots for this rota.
  $slots = party_rota_get_slots_from_rota($rota);

  $form['slots_positions']['slots'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rota Slots'),
    '#weight' => 1,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  if (!$slots) {
    $form['slots_positions']['slots']['no_slots'] = array(
      '#markup' => t('No slots have been created yet.')
    );

    // Add helper text to tell user that slots can be created after rota is saved.
    if (isset($rota->is_new)) {
      $form['slots_positions']['slots']['no_slots']['#markup'] .= '</br>' .
        t('Slots can be added after the Rota has been created.');
    }
  }
  else {
    $form_state['slots'] = $slots;

    $header = array(
      'date' => array('data' => t('Date')),
      'name' => array('data' => t('Slot')),
      'description' => array('data' => t('Description')),
      'ops' => array('data' => t('Operations')),
    );

    $rows = array();

    foreach ($slots as $sid => $slot) {
      $edit_link = ctools_modal_text_button(
        t('Edit'),
        'admin/party_rota/nojs/rota_slot/' . $sid . '/edit',
        t('Edit Slot'), 'ctools-modal-modal-popup-medium'
      );

      $delete_link = ctools_modal_text_button(
        t('Delete'),
        'admin/party_rota/nojs/rota_slot/' . $sid . '/delete',
        t('Delete Slot'), 'ctools-modal-modal-popup-medium'
      );

      $rows[$sid] = array(
        'date' => date('j M Y - H:m', $slot->date),
        'name' => $slot->name,
        'description' => $slot->description,
        'ops' => $edit_link . ', ' . $delete_link,
      );
    }

    $form['slots_positions']['slots']['table'] = array(
      '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
    );
  }
  if (!isset($rota->is_new)) {
    $add_link = ctools_modal_text_button(
      t('Add Slot'),
      'admin/party_rota/nojs/add/' . $rota->rota_id . '/rota_slot',
      t('Add a Slot to the current Rota'), 'ctools-modal-modal-popup-medium'
    ); 

    $form['slots_positions']['slots']['add_link'] = array(
      '#markup' => '</br>' . $add_link,
    );
  }


  // Get the positions for this rota.
  $positions = party_rota_get_positions_from_rota($rota);

  $form['slots_positions']['positions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rota positions'),
    '#weight' => 1,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  if (!$positions) {
    $form['slots_positions']['positions']['no_positions'] = array(
      '#markup' => t('No positions have been created yet.')
    );

    // Add helper text to tell user that positions can be created after rota is saved.
    if (isset($rota->is_new)) {
      $form['slots_positions']['positions']['no_positions']['#markup'] .= '</br>' .
        t('Positions can be added after the Rota has been created.');
    }
  }
  else {
    $form_state['positions'] = $positions;

    $header = array(
      'name' => array('data' => t('Position')),
      'description' => array('data' => t('Description')),
      'ops' => array('data' => t('Operations')),
    );

    $rows = array();

    foreach ($positions as $pid => $position) {
      $edit_link = ctools_modal_text_button(
        t('Edit'),
        'admin/party_rota/nojs/rota_position/' . $pid . '/edit',
        t('Edit Position'), 'ctools-modal-modal-popup-medium'
      );

      $delete_link = ctools_modal_text_button(
        t('Delete'),
        'admin/party_rota/nojs/rota_position/' . $pid . '/delete',
        t('Delete Position'), 'ctools-modal-modal-popup-medium'
      );

      $rows[$pid] = array(
        'name' => $position->name,
        'description' => $position->description,
        'ops' => $edit_link . ', ' . $delete_link,
      );
    }

    $form['slots_positions']['positions']['table'] = array(
      '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
    );
  }

  if (!isset($rota->is_new)) {
    $add_link = ctools_modal_text_button(
      t('Add Position'),
      'admin/party_rota/nojs/add/' . $rota->rota_id . '/rota_position',
      t('Add a Position to the current Rota'), 'ctools-modal-modal-popup-medium'
    );

    $form['slots_positions']['positions']['add_link'] = array(
      '#markup' => '</br>' . $add_link,
    );
  }

  $form['rota_layout'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rota Layout'),
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => isset($rota->is_new) ? TRUE : FALSE,
  );

  if (!empty($slots) && !empty($positions)) {
    $header = array(
      'slot' => array('data' => t('Slot')),
    );

    $rows = array();

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
          $delete_link = ctools_modal_text_button(
            t('Remove'),
            'admin/party_rota/nojs/delete/' . $assignment->assignment_id,
            t('Remove assignment and clear slot.'),
            'ctools-modal-modal-popup-medium'
          );

          $rows[$sid][$pid] = $assignment->name . '  (' . $delete_link . ')';
        }
        else {
          $add_link = ctools_modal_text_button(
            t('Add'),
            'admin/party_rota/nojs/assign/' . $pid . '/' . $sid,
            t('Add new'),
            'ctools-modal-modal-popup-medium'
          );

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
 * @param $form
 * @param $form_state
 * @param $rota
 *
 * @return mixed
 *
 * @see confirm_form()
 */
function party_rota_delete_form($form, &$form_state, $rota) {
  $form_state['rota'] = $rota;

  $form['#submit'][] = 'party_rota_delete_form_submit';

  return confirm_form($form,
    t('Are you sure you want to delete rota %name?', array('%name' => $rota->name)),
    'admin/content/rotas/rota',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );
}

/**
 * Submit callback for party_rota_delete_form
 */
function party_rota_delete_form_submit($form, &$form_state) {
  $rota = $form_state['rota'];

  party_rota_delete($rota);

  drupal_set_message(t('The rota %name has been deleted.', array('%name' => $rota->name)));
  watchdog('party_rota', 'Deleted rota %name.', array('%name' => $rota->name));

  $form_state['redirect'] = 'admin/content/rotas';
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

/**
 * Create/Edit form callback for the Slot entity.
 *
 * @param $form
 * @param $form_state
 * @param $slot
 *
 * @see party_rota_inline_ajax_edit()
 */
function party_rota_slot_inline_edit_form($form, &$form_state, $slot) {
  $rota = rota_load($slot->rota_id);
  $form_state['slot'] = $slot;

  if (isset($slot->is_new)) {
    drupal_set_title(t('Add Slot to the @name Rota', array('@name' => $rota->name)));
  }
  else {
    drupal_set_title(t('Edit the @name Slot', array('@name' => $slot->name)));
  }
  if (!isset($form_state['rota'])) {
    $form_state['rota'] = $rota;
  }

  $form['date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Slot Date'),
    '#default_value' => isset($slot->date) ? date('Y-m-d H:i', $slot->date) : '',
    '#required' => TRUE,
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => 'Slot Label',
    '#default_value' => isset($slot->name) ? $slot->name : '',
  );

  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Slot Description'),
    '#description' => t('Optional instructions to the Assignee about this Slot'),
    '#default_value' => isset($slot->description) ? $slot->description : '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => (isset($slot->is_new)) ? t('Add slot') : t('Save'),
    '#submit' => array('party_rota_slot_inline_edit_form_submit'),
    '#validate' => array('party_rota_slot_inline_edit_form_validate'),
  );

  return $form;
}

/**
 * Validation callback for the slot entity edit form.
 *
 * @see party_rota_slot_inline_edit_form()
 * @see party_rota_slot_inline_edit_form_submit()
 */
function party_rota_slot_inline_edit_form_validate(&$form, &$form_state) {
  if (!empty($form_state['values']['description']) && strlen($form_state['values']['description']) > 255) {
    form_set_error('slot_description', t('Your description is too long, please use less than 255 characters.'));
  }
}

/**
 * Submission callback for the slot entity edit form.
 *
 * @see party_rota_slot_inline_edit_form()
 * @see party_rota_slot_inline_edit_form_validate()
 */
function party_rota_slot_inline_edit_form_submit(&$form, &$form_state) {
  $slot = $form_state['slot'];

  foreach ($form_state['values'] as $key => $value) {
    $slot->{$key} = $value;
  }

  $slot->date = strtotime($form_state['values']['date']);

  if (isset($slot->is_new)) {
    drupal_set_message(t('New slot @name has been created.', array('@name' => $slot->name)), 'status');
  }
  else {
    drupal_set_message(t('Slot @name has been updated.', array('@name' => $slot->name)), 'status');
  }

  $slot->save();

  $form_state['redirect'] = 'admin/content/rotas/rota/' . $slot->rota_id . '/edit';
}

/**
 * Create/Edit form callback for the Position entity.
 *
 * @param $form
 * @param $form_state
 * @param $position
 *
 * @see party_rota_inline_ajax_edit()
 */
function party_rota_position_inline_edit_form($form, &$form_state, $position) {
  $rota = rota_load($position->rota_id);
  $form_state['position'] = $position;

  if (isset($position->is_new)) {
    drupal_set_title(t('Add Position to the @name Rota', array('@name' => $rota->name)));
  }
  else {
    drupal_set_title(t('Edit the @name Position', array('@name' => $position->name)));
  }
  if (!isset($form_state['rota'])) {
    $form_state['rota'] = $rota;
  }

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => 'Position Label',
    '#default_value' => isset($position->name) ? $position->name : '',
  );

  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Position Description'),
    '#description' => t('Optional instructions to the Assignee about this Position'),
    '#default_value' => isset($position->description) ? $position->description : '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add position'),
    '#submit' => array('party_rota_position_inline_edit_form_submit'),
    '#validate' => array('party_rota_position_inline_edit_form_validate'),
  );

  return $form;
}

/**
 * Validation callback for the position entity edit form.
 *
 * @see party_rota_position_inline_edit_form()
 * @see party_rota_position_inline_edit_form_submit()
 */
function party_rota_position_inline_edit_form_validate(&$form, &$form_state) {
  // Make sure the position has a name.
  if (empty($form_state['values']['name'])) {
    form_set_error('position_name', t('Please choose a name for the new position.'));
  }

  if (!empty($form_state['values']['description']) && strlen($form_state['values']['description']) > 255) {
    form_set_error('position_description', t('Your description is too long, please use less than 255 characters.'));
  }
}

/**
 * Submission callback for the position entity edit form.
 *
 * @see party_rota_position_inline_edit_form()
 * @see party_rota_position_inline_edit_form_validate()
 */
function party_rota_position_inline_edit_form_submit(&$form, &$form_state) {
  $position = $form_state['position'];

  foreach ($form_state['values'] as $key => $value) {
    $position->{$key} = $value;
  }

  if (isset($position->is_new)) {
    drupal_set_message(t('New position @name has been created.', array('@name' => $position->name)), 'status');
  }
  else {
    drupal_set_message(t('Position @name has been updated.', array('@name' => $position->name)), 'status');
  }

  $position->save();

  $form_state['redirect'] = 'admin/content/rotas/rota/' . $position->rota_id . '/edit';
}

/**
 * Create/Edit form callback for the rota entities.
 *
 * @param $form
 * @param $form_state
 * @param $entity
 *
 * @see party_rota_inline_ajax_delete()
 */
function party_rota_entity_inline_delete_form($form, &$form_state, $entity) {
  $form_state['entity'] = $entity;

  $question = t('Are you sure you want to delete this @entity', array('@entity' => get_class($entity)));
  $path = 'admin/content/rotas/rota/' . $entity->rota_id . '/edit';
  $description = t('You are about to delete the @name @entity. This action cannot be undone.', array('@name' => $entity->name, '@entity' => get_class($entity)));
  $yes = t('Delete');

  return confirm_form($form, $question, $path, $description, $yes);
}

/**
 * Submission callback for the position entity edit form.
 *
 * @see party_rota_entity_inline_delete_form()
 */
function party_rota_entity_inline_delete_form_submit(&$form, &$form_state) {
  $entity = $form_state['entity'];

  entity_delete($entity->entityType(), $entity->identifier());

  $form_state['redirect'] = 'admin/content/rotas/rota/' . $entity->rota_id . '/edit';
  drupal_set_message(t('@entity @name was deleted successfully.', array('@name' => $entity->name, '@entity' => get_class($entity))), 'status');
}

/**
 * AJAX form builder
 * Adds a rota entity to a Rota.
 *
 * @param null $js
 * @param $position
 * @param $slot
 *
 * @return array|mixed
 */
function party_rota_inline_ajax_assign($js = NULL, $position, $slot) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('party_rota_inline_ajax_assign_form');
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

  $output = ctools_modal_form_wrapper('party_rota_inline_ajax_assign_form', $form_state);
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
}

/**
 * Form callback to assign a party to a rota slot.
 *
 * @param $form
 * @param $form_state
 * @param $position
 * @param $slot
 *
 * @return array
 *
 * @see party_rota_inline_ajax_assign_form_submit()
 */
function party_rota_inline_ajax_assign_form($form, &$form_state, $position, $slot) {
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
    '#description' => t('Select a Party to assign to this slot.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Assign'),
  );

  return $form;
}

/**
 * Submission callback for the party assign form.
 *
 * @see party_rota_inline_ajax_assign_form()
 */
function party_rota_inline_ajax_assign_form_submit(&$form, &$form_state) {
  $assignment = entity_load_single('rota_assignment', $form_state['values']['party_assignment']);
  $assignment->slot_id = $form_state['slot']->slot_id;
  $assignment->position_id = $form_state['position']->position_id;

  $assignment->save();
  $form_state['redirect'] = 'admin/content/rotas/rota/' . $assignment->rota_id . '/edit';
}

/**
 * AJAX form builder
 * Adds a rota entity to a Rota.
 *
 * @param null $js
 * @param $assignment
 *
 * @return array|mixed
 */
function party_rota_inline_ajax_delete_assignment($js = NULL, $assignment) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('party_rota_inline_ajax_delete_form');
  }

  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    // 'title' => t('Remove the assignment'),
    'ajax' => TRUE,
    'build_info' => array(
      'args' => array(
        0 => $assignment,
      ),
    ),
  );

  $output = ctools_modal_form_wrapper('party_rota_inline_ajax_delete_assignment_form', $form_state);
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
}

/**
 * Form callback to unassign a party from a rota slot.
 *
 * @param $form
 * @param $form_state
 * @param $assignment
 *
 * @return array
 *
 * @see party_rota_inline_ajax_delete_assignment_form_submit()
 */
function party_rota_inline_ajax_delete_assignment_form($form, &$form_state, $assignment) {
  if (!$assignment) {
    return array(array(
      '#markup' => 'There was a problem. You are trying to delete an assigment that does not exist.',
    ));  
  }
  $form_state['assignment'] = $assignment;

  $question = t('Are you sure you want to delete this assignment?');
  $path = 'admin/content/rotas/rota/' . $assignment->rota_id . '/edit';

  return confirm_form($form, $question, $path, NULL, t('Delete'));
}

/**
 * Submission callback for the party unassign form.
 *
 * @see party_rota_inline_ajax_delete_assignment_form()
 */
function party_rota_inline_ajax_delete_assignment_form_submit(&$form, &$form_state) {
  $assignment = $form_state['assignment'];

  $form_state['redirect'] = 'admin/content/rotas/rota/' . $assignment->rota_id . '/edit';
  $assignment->delete();
}

/**
 * AJAX form builder
 * Adds a rota entity to a Rota.
 *
 * @param null $js
 * @param $rota
 * @param $entity_type
 *
 * @return array|mixed
 */
function party_rota_inline_ajax_add($js = NULL, $rota, $entity_type) {
  $values = array(
    'rota_id' => $rota->rota_id,
  );

  $create = 'party_' . $entity_type . '_create';
  $entity = $create($values);

  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form($entity->getInlineEditForm(), $entity);
  }

  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'ajax' => TRUE,
    'build_info' => array(
      'args' => array(
        0 => $entity,
      ),
    ),
  );

  $output = ctools_modal_form_wrapper($entity->getInlineEditForm(), $form_state);

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
}

/**
 * AJAX form builder
 * Edit a rota entity on a Rota.
 *
 * @param null $js
 * @param $entity_type
 * @param $entity_id
 *
 * @return array|mixed
 */
function party_rota_inline_ajax_edit($js = NULL, $entity_type, $entity_id) {
  // Fall back if $js is not set.
  $entity = entity_load_single($entity_type, $entity_id);

  if (!$js) {
    return drupal_get_form($entity->getInlineEditForm(), $entity);
  }

  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'ajax' => TRUE,
    'build_info' => array(
      'args' => array(
        0 => $entity,
      ),
    ),
  );

  $output = ctools_modal_form_wrapper($entity->getInlineEditForm(), $form_state);

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
}

/**
 * AJAX form builder
 * Deletes a rota entity from a Rota.
 *
 * @param null $js
 * @param $entity_type
 * @param $entity_id
 *
 * @return array|mixed|string
 */
function party_rota_inline_ajax_delete($js = NULL, $entity_type, $entity_id) {
  // Fall back if $js is not set.
  $entity = entity_load_single($entity_type, $entity_id);
  dpm($entity);


  if (!$js) {
    if ($entity_type == 'rota_slot') {
      return drupal_get_form('party_rota_entity_inline_delete_form', $entity);
    }
    if ($entity_type == 'rota_position') {
      return drupal_get_form('party_rota_entity_inline_delete_form', $entity);
    }
  }

  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'ajax' => TRUE,
    'build_info' => array(
      'args' => array(
        0 => $entity,
      ),
    ),
  );

  if ($entity_type == 'rota_slot') {
    $output = ctools_modal_form_wrapper('party_rota_entity_inline_delete_form', $form_state);
  }
  elseif ($entity_type == 'rota_position') {
    $output = ctools_modal_form_wrapper('party_rota_entity_inline_delete_form', $form_state);
  }
  else {
    // @TODO Needs better error handling.
    return '';
  }
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
}

function party_rota_form_party_rota_slot_inline_edit_form_alter(&$form, &$form_state) {

  //dpm($form);
}