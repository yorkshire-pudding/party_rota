<?php
/**
 * @file Relationship for party data sets, relating parties to the entities in a set.
 */

class party_rota_handler_relationship_position_from_assignment extends views_handler_relationship {
  /**
   * Defines default values for options.
   */
  function option_definition() {
    $options = parent::option_definition();

    return $options;
  }

  /**
   * Extends the relationship's basic options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

  }

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    // Join to the party table.
    $join_party = new views_join();
    $join_party->definition = array(
      'table' => $this->definition['base'],
      'field' => $this->definition['base_field'],
      'left_table' => 'party_rota_assignment',
      'left_field' => 'position_id',
    );
    $join_party->construct();
    $join_party->adjusted = TRUE;

    $this->alias = $this->query->add_relationship('party_rota' . $this->relationship, $join_party, $this->definition['base'], $this->relationship);
  }
}

