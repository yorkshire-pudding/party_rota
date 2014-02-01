<?php
/**
 * @file
 * Default theme implementation of a Slot entity.
 *
 * Available variables:
 * - entity: The fully loaded slot entity object.
 */
?>

<div class="party-rota-slot slot-<?php print $entity->slot_id; ?>">
  <h4><?php print $entity->name; ?> ( <?php print date('j M Y - H:m', $entity->date); ?> )</h4>
  <span><?php print $entity->description; ?></span>
<div>
