<?php
/**
 * @file
 * Default theme implementation of a Position entity.
 *
 * Available variables:
 * - entity: The fully loaded position entity object.
 */
?>

<div class="party-rota-position position-<?php print $entity->position_id; ?>">
  <h4><?php print $entity->name; ?></h4>
  <span><?php print $entity->description; ?></span>
<div>

