<?php

$tab_title = normalize_string($node->title);

$tabs_markup = build_tabs($node,$tab_title);

print (!empty($content['field_link']['#items'])) ? drupal_render($content['field_link']) . $tabs_markup : $tabs_markup;
