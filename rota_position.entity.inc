<?php

/**
 * @file
 */

/**
 * The class used for Position entities
 */
class Position extends Entity {

  protected $inlineEditForm = 'party_rota_position_inline_edit_form';

  /**
   * Constructor for the position entity.
   */
  public function __construct($values = array()) {
    parent::__construct($values, 'rota_position');
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
    return array('path' => 'rota_position/' . $this->position_id);
  }

  /**
   * Returns to form callback for the inline edit form of the entity.
   */
  public function getInlineEditForm() {
    return $this->inlineEditForm;
  }

}

/**
 * The Controller for Position entities.
 */
class PositionController extends EntityAPIController {

  public function __construct($entityType) {
    parent::__construct($entityType);
  }

  /**
   * Creates a Position entity.
   * 
   * @param $values
   *   The machine-readable type of the position.
   *
   * @return object
   *   A position object with all default fields initialized.
   */
  public function create(array $values = array()) {
    // Add values that are specific to our position
    $values += array( 
      'position_id' => '',
      'rota_id' => '',
      'is_new' => TRUE,
      'name' => '',
      'description' => '',
      'created' => time(),
      'changed' => time(),
    );
    
    $position = parent::create($values);
    return $position;
  }

  /**
   * Saves the Position entity.
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