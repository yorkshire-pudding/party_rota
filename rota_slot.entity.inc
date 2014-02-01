<?php

/**
 * @file
 */

/**
 * The class used for Slot entities
 */
class Slot extends Entity {

  protected $inlineEditForm = 'party_rota_slot_inline_edit_form';

  /**
   * Constructor for the position entity.
   */
  public function __construct($values = array()) {
    parent::__construct($values, 'rota_slot');
  }

  /**
   * Defines the entity label if the 'entity_class_label' callback is used.
   */
  protected function defaultLabel() {
    return $this->name;
  }

  /**
   * Defines the entity uri if the 'entity_class_uri' callback is used.
   */
  protected function defaultUri() {
    return array('path' => 'rota_slot/' . $this->slot_id);
  }

  /**
   * Returns to form callback for the inline edit form of the entity.
   */
  public function getInlineEditForm() {
    return $this->inlineEditForm;
  }

}

/**
 * The Controller for Slot entities.
 */
class SlotController extends EntityAPIController {

  public function __construct($entityType) {
    parent::__construct($entityType);
  }


  /**
   * Creates a Slot entity.
   *
   * @param $values
   *   The machine-readable type of the slot.
   *
   * @return object
   *   A slot object with all default fields initialized.
   */
  public function create(array $values = array()) {
    // Add values that are specific to our slot
    $values += array( 
      'slot_id' => '',
      'rota_id' => '',
      'is_new' => TRUE,
      'name' => '',
      'description' => '',
      'created' => time(),
      'changed' => time(),
      'date' => '',
    );
    
    $slot = parent::create($values);
    return $slot;
  }

  /**
   * Saves the Slot entity.
   *
   * @param $entity
   * @param DatabaseTransaction $transaction
   *
   * @return object
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $entity->changed = time();

    $entity = parent::save($entity, $transaction);
    return $entity;
  }

}