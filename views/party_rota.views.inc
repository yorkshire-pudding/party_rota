<?php

/**
 * @file
 * Contains Views hooks.
 */

/**
 * Implements hook_views_data().
 *
 * We can implement this *and* get entity module's views data for free because
 * we set our entity's 'views controller class' in hook_entity_info().
 */
function party_rota_views_data() {
  $data = array();

  // Rota fields.
  $data['party_rota']['operations'] = array(
    'title' => t('Operations'),
    'help' => t('Provide CRUD links for Party Rotas.'),
    'field' => array(
      'real field' => 'rota_id',
      'handler' => 'party_rota_handler_field_operations_links',
    ),
  );

  // Rota relationships.
  $data['party_rota']['rota_assignment'] = array(
    'title' => t('Rota Assignments'),
    'relationship' => array(
      'label' => t('Assignments from Rota'),
      'help' => t('Relates a rota to its assignments.'),
      'handler' => 'party_rota_handler_relationship_assignments_from_rota',
      'base' => 'party_rota_assignment',
      'base_field' => 'rota_id',
      'entity_type' => 'rota_assignment',
    ),
  );

  $data['party_rota']['rota_position'] = array(
    'title' => t('Rota Positions'),
    'relationship' => array(
      'label' => t('Positons from Rota'),
      'help' => t('Relates a rota to its positions.'),
      'handler' => 'party_rota_handler_relationship_positions_from_rota',
      'base' => 'party_rota_position',
      'base_field' => 'rota_id',
      'entity_type' => 'rota_position',
    ),
  );

  $data['party_rota']['rota_slot'] = array(
    'title' => t('Rota Slots'),
    'relationship' => array(
      'label' => t('Slots from Rota'),
      'help' => t('Relates a rota to its slots.'),
      'handler' => 'party_rota_handler_relationship_slots_from_rota',
      'base' => 'party_rota_slot',
      'base_field' => 'rota_id',
      'entity_type' => 'rota_slot',
    ),
  );

  // Rota Assignment relationships.
  $data['party_rota_assignment']['rota'] = array(
    'title' => t('Rota'),
    'relationship' => array(
      'label' => t('Rota from Assignment'),
      'help' => t('Relates an assignment to its rota.'),
      'handler' => 'party_rota_handler_relationship_rota_from_assignment',
      'base' => 'party_rota',
      'base_field' => 'rota_id',
      'entity_type' => 'rota',
    ),
  );

  // Rota Position relationships.

  // Rota Slot relationships.


  return $data;
}

function party_rota_views_data_alter(&$data) {
  $data['party_rota_slot']['date'] = array(
    'title' => t('Slot date'), // The item it appears as on the UI,
    'help' => t('The date slot is scheduled for.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $entities = array('rota', 'rota_slot', 'rota_position', 'rota_assignment');

  foreach ($entities as $entity) {
    $data['party_' . $entity]['created'] = array(
      'title' => t('Created date'), // The item it appears as on the UI,
      'help' => t('The date the ' . $entity . ' was created.'), // The help that appears on the UI,
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
    );

    $data['party_' . $entity]['changed'] = array(
      'title' => t('Changed date'), // The item it appears as on the UI,
      'help' => t('The date the ' . $entity . ' was last updated.'), // The help that appears on the UI,
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
    );
  }
}
