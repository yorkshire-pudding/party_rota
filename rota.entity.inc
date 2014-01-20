<?php

/**
 * @file
 */

/**
 * The class used for model entities
 */
class Rota extends Entity {
  
  public function __construct($values = array()) {
    parent::__construct($values, 'rota');
  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    return array('path' => 'rota/' . $this->rota_id);
  } 

}

/**
 * The Controller for Model entities
 */
class RotaController extends EntityAPIController {

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
      'rota_id' => '',
      'is_new' => TRUE,
      'title' => '',
      'created' => '',
      'changed' => '',
    );
    
    $rota = parent::create($values);
    return $rota;
  }
  
  /**
   * Overriding the buldContent function to add entity specific fields
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $content = parent::buildContent($entity, $view_mode, $langcode, $content);

    $content['rota_sample_data'] =  array(
      '#markup' => 'Sampe data',
    );

    return $content;
  }
  
}