<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

/*
 * @file
 * The PublicacionessToNodeMigration uses the  NOTICIAS.CSV files as source
 * and creates Drupal nodes of type 'article' as destination.
 */

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class PublicacionNodeMigration extends Migration {

  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of Contenido tipo Publicacioness');
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
    $this->destination = new MigrateDestinationNode('page');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('contenido_id' => array('type' => 'varchar', 'length' => 255, 'description' => 'Contenido ID.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());

    // Field Title <Texto>;
    $this->addFieldMapping('title', 'title'); // Field Title

    // Field Body <Texto largo y resumen>;
    $this->addFieldMapping('body', 'body_text');
    $this->addFieldMapping('body:format', 'body_format');
    $this->addFieldMapping('body:summary', 'body_summary');

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
    $this->addFieldMapping('field_seccion_publicacion', 'menu_item'); // Field publicacion (List (text))
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

    //revisar si es nodo noticia, ignorar
    if ($row->es_noticia == 'S'){
      drupal_set_message(t("El nodo @id es noticia, ignorar", array("@id" => $row->contenido_id)), "status");
      return FALSE;  
    }

    //revisar si $row->contenido_id ya esta en la BD
    $result = db_query("SELECT entity_id FROM field_data_field_contenido_id WHERE bundle IN ('article', 'page') AND field_contenido_id_value = :id_anterior", array(":id_anterior" => $row->contenido_id))->fetchAssoc();

    if (!empty($result['entity_id'])){
      drupal_set_message(t("El nodo ya existe, el id es @id y su nid es: @nid", array("@id" => $row->contenido_id, "@nid" => $result['entity_id'])), "status");
      return FALSE;  
    }

    $allowed_values = array(
      "Catalogo / Almacenes" => "129",
      "Ferias y Eventos / Expoartesanías" => "135",
      "¿Quiénes somos? / Sobre la entidad" => "104",
      "SobreEntidadFix" => "104",
      "Ferias y Eventos / Maestría Artesanal" => "137",
      "¿Quienes somos? / Programas" => "14576",
      "Home / Transparencia /  Contratación" => "14582",
      "No aplica" => "14580",
      "Home / Transparencia / Planeación y Gestión" => "14278",
      "¿Quiénes somos? / Proyectos Inversión Nacional" => "107",
      "Home / Transparencia / Sistema de Gestión de Calidad" => "14579",
      "Ferias y Eventos / Expoartesano" => "134",
      "Catalogo / Servicio para proveedores" => "130",
      "Catalogo / Muestras Comerciales" => "131",
      "Ferias y Eventos / Bienal de Diseño para la Artesanía" => "136",
      "Catalogo / Politica Comercial" => "132",
      "Laboratorios \ Atlántico" => "12824",
      "Laboratorios \ Bolívar" => "145",
      "Laboratorios \ Caldas" => "12826",
      "Laboratorios \ Córdoba" => "12823",
      "Laboratorios \ Tolima" => "12827",
      "Laboratorios \ La Guajira" => "12825",
      "Laboratorios \ Nariño" => "12836",
      "Laboratorios \ Quindío" => "12829",
      "Laboratorios \ Estrategia" => "144",
      "Laboratorios \ Cauca" => "12835",
      "Laboratorios \ Amazonas" => "12838",
      "Laboratorios \ Santander" => "12831",
      "Laboratorios \ Bogotá" => "12821",
      "Laboratorios \ Huila" => "12828",
      "Laboratorios \ Cundinamarca" => "12820",
      "Laboratorios \ Chocó" => "12833",
      "Laboratorios \ Valle del Cauca" => "12834",
      "Laboratorios \ Risaralda" => "12830",
      "Laboratorios \ Putumayo" => "12837",
      "Laboratorios \ Norte de Santander" => "12832",
      "Laboratorios \ Boyacá" => "12822",
      "Laboratorios \ Novedades" => "14679",
      "0" => "14581",
    );

   if(isset($row->menu_item) && (!empty($row->menu_item) || $row->menu_item == 0)  && array_key_exists($row->menu_item, $allowed_values)){
      $row->menu_item = trim($row->menu_item);
      $val = "";
      foreach ($allowed_values as $key => $value) {
        if ($key == strpos($row->menu_item,$key) && empty($val)) {
          $row->menu_item =  $value;
          $val = $value;
        }
      }
      if(!is_numeric($row->menu_item)){
        dpm("ERROR SECCION NO NUMERICO");
        return FALSE;
      }
    } else {
        dpm("ERROR SECCION");
        dpm(array_key_exists($row->menu_item, $allowed_values));
        dpm("FALSE : ".$row->menu_item);
        dpm($allowed_values);
        return FALSE;
    }
    // Building Node Informatión From $row Data.

    $this->fieldMappingToPublicaciones($row);
  }

  /**
   * Runs before an import starts.
   * Code to execute before first article row is imported
   */
  public function preImport() {
    parent::preImport();
  }

  public function fieldMappingToPublicaciones($row){
    global $base_url;

    //si el titul está duplicado
    $row->title = $row->title;
    $result = db_select('node', 'n')
      ->fields('n', array('title'))
      ->condition('title',$row->title,'=')
      ->execute()
    ->fetchAssoc();
    if(!empty($result)){
      $row->title = "Duplicado - ".$row->title;
    }

    //Poner ese body por ahora
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
    // Update Date
    if(!empty($row->update_date) && ($row->update_date != '#N/A')){
      $update_date = str_replace('/', '-', $row->update_date);
      $row->changed = strtotime($update_date);
    }else{
      $row->changed  = time();
    }

    
      // Menu Id
      //$menu_id = str_replace(".", "", $row->menu_item);
      // $menu_id = intval($menu_id);
      // $row->menu_id = $menu_id;
    

    // Node Status
    $row->status = get_status($row->status_type);
    if(!empty($result)){
      $row->status = 0;
    }
    
    // Now Time.
    $now = strtotime('now');

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
    }
  }
  /*
   * The Migration class complete() method is analogous to prepare(), but is called immediately after the complete
   * destination object is saved.
   */
   public function complete($entity, stdClass $row) {
    dpm("Publicacion: ".$row->contenido_id.",".$entity->nid.",".$row->title.",".$row->menu_item.",");
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
