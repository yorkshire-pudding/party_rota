<?php
/**
 * @file
 * Default theme implementation of an Instagram image link.
 *
 * Available variables:
 * - post: The entire data array returned from the Instagram API request.
 * - href: The url to the Instagram post page.
 * - src: The source url to the instagram image.
 * - width: The display width of the image.
 * - height: The display height of the image.
 */
?>

<div class="party-rota-position position-<?php print $entity->position_id; ?>">
  <h4><?php print $entity->name; ?></h4>
  <span><?php print $entity->description; ?></span>
<div>
