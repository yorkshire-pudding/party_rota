<?php

/**
 * @file
 * Bulk export of views_default objects generated by Bulk export module.
 */

/**
 * Implementation of hook_views_default_views()
 */
function party_rota_views_default_views() {
  $views = array();

  $view = new view();
  $view->name = 'party_rotas';
  $view->description = 'Manage fields and displays for Party Rotas.';
  $view->tag = 'default';
  $view->base_table = 'party_rota';
  $view->human_name = 'Party Rotas';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Party Rotas';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'administer rotas';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  /* Field: Rota: Rota ID */
  $handler->display->display_options['fields']['rota_id']['id'] = 'rota_id';
  $handler->display->display_options['fields']['rota_id']['table'] = 'party_rota';
  $handler->display->display_options['fields']['rota_id']['field'] = 'rota_id';
  /* Field: Rota: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'party_rota';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  /* Field: Rota: Members */
  $handler->display->display_options['fields']['party_rota_parties']['id'] = 'party_rota_parties';
  $handler->display->display_options['fields']['party_rota_parties']['table'] = 'field_data_party_rota_parties';
  $handler->display->display_options['fields']['party_rota_parties']['field'] = 'party_rota_parties';
  $handler->display->display_options['fields']['party_rota_parties']['settings'] = array(
    'link' => 0,
  );
  $handler->display->display_options['fields']['party_rota_parties']['delta_offset'] = '0';
  /* Field: Rota: Operations */
  $handler->display->display_options['fields']['operations']['id'] = 'operations';
  $handler->display->display_options['fields']['operations']['table'] = 'party_rota';
  $handler->display->display_options['fields']['operations']['field'] = 'operations';

  /* Display: Structure */
  $handler = $view->new_display('page', 'Structure', 'page');
  $handler->display->display_options['path'] = 'admin/structure/rotas';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Party Rotas';
  $handler->display->display_options['menu']['description'] = 'Manage fields and displays for Party Rotas.';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  /* Display: Content */
  $handler = $view->new_display('page', 'Content', 'page_1');
  $handler->display->display_options['path'] = 'admin/content/rotas';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Party Rotas';
  $handler->display->display_options['menu']['description'] = 'Manage fields and displays for Party Rotas.';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;

  $views[$view->name] = $view;

  return $views;
}