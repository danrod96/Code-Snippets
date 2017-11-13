<?php 

	if(!empty($content['field_link']['#items'])){
		
		$link = drupal_render($content['field_link']);
	}

    $tab_title = normalize_string($node->title);

    $tabs_markup = build_tabs($node,$tab_title);
    
    print $link . $tabs_markup;
?>
