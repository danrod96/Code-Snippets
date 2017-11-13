<?php 

    $tab_title = normalize_string($node->title);

    $tabs_markup = build_tabs($node, $tab_title);
    
    print $tabs_markup;
?>
