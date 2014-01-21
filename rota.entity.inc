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
      'created' => time(),
      'changed' => time(),
    );
    
    $rota = parent::create($values);
    return $rota;
  }
  
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $entity->changed = time();

    $entity = parent::save($entity, $transaction);
    return $entity;
  }


  /**
   * Overriding the buldContent function to add entity specific fields
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $content = parent::buildContent($entity, $view_mode, $langcode, $content);

    $positions = party_rota_get_positions_from_rota($entity);
    $slots = party_rota_get_slots_from_rota($entity);

    if ($slots && $positions) {
      $header = array(
        'slot' => array('data' => t('Slot')),
      );
      $rows = array();

      $empty = 'Not assigned';
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
          if ($assignment = party_rota_get_assignment_from_context($entity, $pid, $sid)) {
            $rows[$sid][$pid] = $assignment->name;
          }
          else {
            $rows[$sid][$pid] = $empty;
          }      
        }
      }

      $content['table'] = array(
        '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
      );
    }

    return $content;
  }


/*

*/
}