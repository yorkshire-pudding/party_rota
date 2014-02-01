<?php

/**
 * @file
 */

/**
 * The class used for Rota entities
 */
class Rota extends Entity {

  /**
   * Constructor for the assignment entity.
   */
  public function __construct($values = array()) {
    parent::__construct($values, 'rota');
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
    return array('path' => 'rota/' . $this->rota_id);
  } 

}

/**
 * The Controller for Rota entities
 */
class RotaController extends EntityAPIController {

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
    // Add values that are specific to our Rota
    $values += array( 
      'rota_id' => '',
      'is_new' => TRUE,
      'name' => '',
      'created' => time(),
      'changed' => time(),
    );
    
    $rota = parent::create($values);
    return $rota;
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

  /**
   * Builds a structured array representing the entity's content.
   *
   * @param $entity
   * @param string $view_mode
   * @param null $langcode
   * @param $content
   *   Optionally. Allows pre-populating the built content to ease overridding
   *   this method.
   *
   * @return array
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

}