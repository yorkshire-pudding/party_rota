<?php

/**
 * @file
 */

/**
 * The class used for model entities
 */
class Position extends Entity {
  
  public function __construct($values = array()) {
    parent::__construct($values, 'rota_position');
  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return array('path' => 'rota_position/' . $this->position_id);
  } 

}

/**
 * The Controller for Model entities
 */
class PositionController extends EntityAPIController {

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
      'position_id' => '',
      'rota_id' => '',
      'is_new' => TRUE,
      'title' => '',
      'created' => '',
      'changed' => '',
    );
    
    $position = parent::create($values);
    return $position;
  }
  
}