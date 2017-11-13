<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

/*
 * @file
 * The NoticiasToNodeMigration uses the  NOTICIAS.CSV files as source
 * and creates Drupal nodes of type 'article' as destination.
 */

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class NoticiaNodeMigration extends Migration {

  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of Contenido tipo Noticias');
    $fields = array(
      0 => array('contenido_id', 'Id Contenido'),
      2 => array('menu_id', 'identificador_contenido'),
      6 => array('menu_item', 'Menu Item'),
      7 => array('title', 'Title'),
      8 => array('body_text', 'Body'),
      9 => array('autor_contenido', 'Autor de Contenido'),
      10 => array('body_summary', 'Summary'),
      11 => array('es_noticia', 'Es noticia'),
      13 => array('image_name', 'Image Name'),
      15 => array('status_type', 'Status node'),
      16 => array('publish_date', 'Publish Date'),
      17 => array('unpublish_date', 'UnPublish Date'),
      19 => array('create_date', 'Create Date'),
      20 => array('mapeo_uid', 'User create'),
      21 => array('update_date', 'Update Date'),
      22 => array('tags_content', 'Tags'),
    );

    //The Source of the import
    //$path = variable_get('contenido_csv_path', 'public://migration/contenido.csv');
    $path = DRUPAL_ROOT . '/sites/default/files/migration/contenidos-noticias.csv';
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type article
    $this->destination = new MigrateDestinationNode('article');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('contenido_id' => array('type' => 'varchar', 'length' => 255, 'description' => 'Contenido ID.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());

    // Field Title <Texto>;
    $this->addFieldMapping('title', 'title'); // Field Title

    // Field Body <Texto largo y resumen>;
    $this->addFieldMapping('body', 'body_text');
    $this->addFieldMapping('body:summary', 'body_summary');
    $this->addFieldMapping('body:format', 'body_format');

    $this->addFieldMapping('uid', 'mapeo_uid');

    // Contenido ID
    $this->addFieldMapping('field_contenido_id', 'contenido_id');

    // Field Elaborado por
    $this->addFieldMapping('field_elaborado_por', 'autor_contenido');


    $this->addFieldMapping('field_tags', 'tags');
    $this->addFieldMapping('field_tags:create_term')
         ->defaultValue(TRUE);
    $this->addFieldMapping('field_tags:ignore_case')
         ->defaultValue(TRUE);

    // Field Created Date
    $this->addFieldMapping('created', 'created');

    // Field Changed Date
    $this->addFieldMapping('changed', 'changed');

    // Sección
    $this->addFieldMapping('field_seccion_publicacion', 'seccion_publicacion'); // Field publicacion (List (text))
    $this->addFieldMapping('field_seccion_publicacion:source_type')->defaultValue('tid');

      // Field Imágen
    $this->addFieldMapping('field_image', 'image_name');
    $this->addFieldMapping('field_image:file_replace')->defaultValue(FILE_EXISTS_RENAME);
    $this->addFieldMapping('field_image:preserve_files')->defaultValue(TRUE);
    // $this->addFieldMapping('field_image:destination_dir')->defaultValue('sites/default/files/imagenes');
        // // Field Documentos relacionados
        // $this->addFieldMapping('field_docs_relacionados', 'documents');

    /*
      $this->addFieldMapping('publish_on', 'publish_on'); // Publish On (Timestamp)
      $this->addFieldMapping('unpublish_on', 'unpublish_on'); // Publish On (Timestamp)
    */

    $this->addFieldMapping('language', 'language'); // Field Language
    $this->addFieldMapping('comment', 'comment'); // Field Comment

    $this->addFieldMapping('status', 'status'); // Field Comment
    // Unmapped destination fields
    // Some conventions we use here: with a long list of fields to ignore, we
    // arrange them alphabetically, one distinct field per line (although
    // subfields of the same field may be grouped on the same line), and indent
    // subfields to distinguish them from top-level fields.
    $this->addUnmigratedDestinations(array(
      'tnid',
      'body:language',
      'is_new',
      'log',
      'promote',
      'revision',
      'revision_uid',
      'sticky',
      'translate',
      'publish_on',
      'unpublish_on',
    ));

  }

  // The prepareRow() method is called by the source class next() method, after loading the data row. The argument $row
  // is a stdClass object containing the raw data as provided by the source. There are two primary reasons to implement
  // prepareRow():
  // 1) To modify the data row before it passes through any further methods and handlers: for example, fetching related
  // data, splitting out source fields, combining or creating new source fields based on some logic.
  // 2) To conditionally skip a row (by returning FALSE).
  public function prepareRow($row) {
    // Always include this fragment at the beginning of every prepareRow()
    // implementation, so parent classes can ignore rows.
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // Source ID
    $row->contenido_id = str_replace(".", "", $row->contenido_id);
    $row->contenido_id = str_replace(",", "", $row->contenido_id);

    //revisar si no es nodo noticia, ignorar
    if ($row->es_noticia == 'N'){
      drupal_set_message(t("El nodo @id NO es noticia, ignorar", array("@id" => $row->contenido_id)), "status");
      return FALSE;  
    }

    //revisar si $row->contenido_id ya esta en la BD
    $result = db_query("SELECT entity_id FROM field_data_field_contenido_id WHERE bundle IN ('article', 'page') AND field_contenido_id_value = :id_anterior", array(":id_anterior" => $row->contenido_id))->fetchAssoc();

    if (!empty($result['entity_id'])){
      drupal_set_message(t("El nodo ya existe, el id es @id y su nid es: @nid", array("@id" => $row->contenido_id, "@nid" => $result['entity_id'])), "status");
      return FALSE;  
    }

   if(isset($row->menu_item) && !empty($row->menu_item)){
    dpm($row->menu_item);
      $row->menu_item = trim($row->menu_item);
      if($row->menu_item != "Novedades"){
        dpm("");
        return FALSE;
      }
    }else{
      return FALSE;
    }
    // Building Node Informatión From $row Data.
    $this->fieldMappingToNoticias($row);
    //dpm($row);
  }

  /**
   * Runs before an import starts.
   * Code to execute before first article row is imported
   */
  public function preImport() {
    parent::preImport();
  }

  public function fieldMappingToNoticias($row){
    global $base_url;
    $row->title = $row->title;
    $result = db_select('node', 'n')
      ->fields('n', array('title'))
      ->condition('title',$row->title,'=')
      ->execute()
    ->fetchAssoc();
    if(!empty($result)){
      $row->title = "Duplicado - ".$row->title;
    }

    $row->body_format = "full_html"; // Filtered HTML
    $row->body_summary = $row->body_summary; // Body Summary
    $row->image_desc = (!empty($row->image_desc))? $row->image_desc : '';

    $row->body_formatted = '';
    $row->body = '';
    if(!empty($row->body_text)){
      $row->body_formatted = cleanHtmlEntities2($row->body_text);
      $row->body = $row->body_formatted;
    }

    if (!empty($row->image_name) && ($row->image_name != '#N/A')) {
      $row->image_name = validate_file('imagenes',$row->image_name);
    }

    // if (!empty($row->image_name) && ($row->image_name != '#N/A')) {
    //   $image[] = $row->image_name;
    //   $images_files = load_images($image);
    //   if(count($images_files)>0){
    //     $row->image_name = $images_files;
    //   }else{
    //     $row->image_name = "";
    //   }
    // }else{
    //   $row->image_url = '';
    //   $row->image_name = '';
    // }

    // $row->images_files = array();
    // if(!empty($row->image_url)){
    //  $row->images_files[] = $row->image_url;
    //  $row->images_alt[] = $row->image_desc;
    //  $row->images_title[] = $row->image_desc;
    //  $row->field_caption[] = array('value' => '', 'format' => 'filtered_html'); // Image Field Caption
    // }

    // Doct list
    $document_list = $row->document_list;
    $documents = array();
    $no_found_documents = array();
    if(!empty($document_list) && ($document_list != '#N/A')){
      $docs = explode(';', $document_list);
      foreach ($docs as $name_doc) {
        $name_doc = trim($name_doc);
        $url_file = 'http://www.esinergia.co/adc/documents/'.$name_doc;
        if(file_exists($url_file)){
        $documents[] = $url_file;
        }else{
          $no_found_documents[] = $url_file;
        }
      }
    }

    $row->no_found_documents = $no_found_documents;
    $row->documents = $documents;

    $row->update_date = ($row->create_date) ? $row->update_date : time();
    $row->create_date = ($row->update_date) ? $row->create_date : time();

    // Node UID
    $row->mapeo_uid = get_uid_usuario($row->uid_anterior);

    // Tags
    $tags_content = $row->tags_content;
    $tags = array();
    if(!empty($tags_content) && ($tags_content != '#N/A')){
      $tags_items = explode(';', $tags_content);
      foreach ($tags_items as $key => $tag) {
        if (!empty($tag)) {
          $tags[] = trim($tag);
        }
      }
    }
    $row->tags = $tags;
    // Create Date
    if(!empty($row->create_date) && ($row->create_date != '#N/A')){
      $create_date = str_replace('/', '-', $row->create_date);
      $row->created = strtotime($create_date);
    }else{
      $row->created = time();
    }
    // Upadte Date
    if(!empty($row->update_date) && ($row->update_date != '#N/A')){
      $update_date = str_replace('/', '-', $row->update_date);
      $row->changed = strtotime($update_date);
    }else{
      $row->changed  = time();
    }

    /*
      // Menu Id
      $menu_id = str_replace(".", "", $row->menu_item);
      $menu_id = intval($menu_id);
      $row->menu_id = $menu_id;
      $row->seccion_publicacion = _menu_item($menu_id);
    */

    // Node Status
    $row->status = get_status($row->status_type);

    // Now Time.
    $now = strtotime('now');
    if(!empty($result)){
      $row->status = 0;
    }
    /*
    if($row->status){
    // Publish Date
      $publish_date = ($row->publish_date) ? $row->publish_date : '';
      if(!empty($publish_date)){
        $publish_date = str_replace('/', '-', $publish_date);
        $publish_date = strtotime($publish_date);
        $row->publish_on = $publish_date;
      }else{
        $row->publish_on = $now;
      }
      if($row->publish_on <= $now){
        $row->publish_on = '';
      }
    }else{
      // Publish Date
      $publish_date = ($row->publish_date) ? $row->publish_date : '';
      if(!empty($publish_date)){
        $publish_date = str_replace('/', '-', $publish_date);
        $publish_date = strtotime($publish_date);
        $row->publish_on = $publish_date;
      }
    }

    // UnPublish Date
    $unpublish_date = ($row->unpublish_date) ? $row->unpublish_date : '';
    if(!empty($unpublish_date)){
      $unpublish_date = str_replace('/', '-', $unpublish_date);
      $unpublish_date = strtotime($unpublish_date);
      $row->unpublish_on = $unpublish_date;
    }
    */

  }
  public function prepare(stdClass $entity, stdClass $current_row) {
    if (isset($current_row->body_text) && !empty($current_row->body_text)) {
      $entity->body['und'][0]['value'] = cleanHtmlEntities2($current_row->body_text);
      $entity->body['und'][0]['value'] = cleanHtmlEntities2($entity->body['und'][0]['value']);
      $entity->body['und'][0]['value'] = cleanHtmlEntities2($entity->body['und'][0]['value']);
      $entity->body['und'][0]['value'] = str_replace('&nbsp;', ' ', $entity->body['und'][0]['value']);
    } else {
      # code...
    }
  }
  /*
   * The Migration class complete() method is analogous to prepare(), but is called immediately after the complete
   * destination object is saved.
   */
   public function complete($entity, stdClass $row) {
    dpm("Noticia: ".$row->contenido_id.",".$entity->nid.",".$row->title.",".$row->menu_item.",");
    //dpm($entity);
      $node = $entity;
  //      if (isset($node->nid)) {
  //        if(!empty($row->no_found_documents)){
  //          $non_migrated_files = variable_get('noticias_non_migrated_files', array());
  //          $non_migrated_files[$node->nid] = implode(", ", $row->no_found_documents);
  //          variable_set('noticias_non_migrated_files', $non_migrated_files);
  //        }
  //      }

    // if( ($row->mapeo_uid == 1) && (!empty($row->uid_anterior)) ){
    //  $users_no_found = variable_get('noticias_users_no_found', array());
    //  $new_user_no_found = array($row->uid_anterior);
    //  $users_no_found = array_unique(array_merge($users_no_found, $new_user_no_found), SORT_REGULAR);
    //  variable_set('noticias_users_no_found', $users_no_found );
    // }

   }


}
