<?php

/**
 * @file
 * Model type editing UI.
 */

/**
 * UI controller.
 */
class RotaTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
		$items[$this->path]['description'] = 'Manage rota entity types, including adding
		and removing fields and the display of fields.';

    return $items;
  }
}

/**
 * Generates the rota type editing form.
 */
function rota_type_form($form, &$form_state, $rota_type, $op = 'edit') {

  if ($op == 'clone') {
    $rota_type->label .= ' (cloned)';
    $rota_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $rota_type->label,
    '#description' => t('The human-readable name of this rota type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($rota_type->type) ? $rota_type->type : '',
    '#maxlength' => 32,
//    '#disabled' => $rota_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'party_rota_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this rota type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save rota type'),
    '#weight' => 40,
  );

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function rota_type_form_submit(&$form, &$form_state) {
  $rota_type = entity_ui_form_submit_build_entity($form, $form_state);
  $rota_type->save();
  $form_state['redirect'] = 'admin/structure/rota_types';
}

/**
 * Form API submit callback for the delete button.
 */
function rota_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/rota_types/manage/' . $form_state['rota_type']->type . '/delete';
}
