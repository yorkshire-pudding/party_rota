<?php

/**
 * @file
 */

/**
 * The class used for Assignment entities
 */
class Assignment extends Entity {

  /**
   * Constructor for the assignment entity.
   */
  public function __construct($values = array()) {
    parent::__construct($values, 'rota_assignment');
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
    return array('path' => 'assignment/' . $this->assignment_id);
  } 

}

/**
 * The Controller for Assignment entities
 */
class AssignmentController extends EntityAPIController {

  /**
   * Constructor for the assignment controller entity.
   */
  public function __construct($entityType) {
    parent::__construct($entityType);
  }


  /**
   * Creates a Assignment entity.
   *
   * @param $values
   *   The machine-readable type of the assignment.
   *
   * @return object
   *   A assignment object with all default fields initialized.
   */
  public function create(array $values = array()) {
    // Add values that are specific to our assignment
    $values += array( 
      'assignment_id' => '',
      'rota_id' => 0,
      'party_id' => 0,
      'position_id' => 0,
      'slot_id' => 0,
      'is_new' => TRUE,
      'name' => '',
      'created' => time(),
      'changed' => time(),
    );
    
    $assignment = parent::create($values);
    return $assignment;
  }

  /**
   * Saves the Assignment entity.
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