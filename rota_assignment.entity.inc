<?php

/**
 * @file
 */

/**
 * The class used for model entities
 */
class Assignment extends Entity {
  
  public function __construct($values = array()) {
    parent::__construct($values, 'rota_assignment');
  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return array('path' => 'assignment/' . $this->assignment_id);
  } 

}

/**
 * The Controller for Model entities
 */
class AssignmentController extends EntityAPIController {

  public function __construct($entityType) {
    parent::__construct($entityType);
  }


  /**
   * Create a model - we first set up the values that are specific
   * to our model schema but then also go through the EntityAPIController
   * function.
   * 
   * @param $type
   *   The machine-readable type of the model.
   *
   * @return
   *   A model object with all default fields initialized.
   */
  public function create(array $values = array()) {
    // Add values that are specific to our Model
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

  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $entity->changed = time();

    $entity = parent::save($entity, $transaction);
    return $entity;
  }

}