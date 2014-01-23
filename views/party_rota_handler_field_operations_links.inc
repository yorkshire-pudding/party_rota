<?php
/**
 * @file
 * Contains a Views field handler to take care of displaying deletes links
 * as fields
 */

class party_rota_handler_field_operations_links extends views_handler_field {
  function construct() {
    parent::construct();

    $this->additional_fields['rota_id'] = 'rota_id';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    if (!party_rota_access('delete')) {
      return;
    }

    $rota_id = $values->{$this->aliases['rota_id']};
http://localhost/rotas/admin/content/rotas/rota/1/edit
    $links = '';
    $links .= l('View', 'rota/' . $rota_id) . '  |  ';
    $links .= l('Edit', 'admin/content/rotas/rota/' . $rota_id . '/edit',
      array('query' => array('destination' => $_GET['q']))) . '  |  ';
    $links .= l('Delete', 'admin/content/rotas/rota/' . $rota_id . '/delete',
      array('query' => array('destination' => $_GET['q'])));

    return $links;
  }
}
