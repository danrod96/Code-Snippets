<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);


require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class ProductosTallerNodeMigration extends Migration {

  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of User tipo Producto Taller Artesano');

    $fields = array(
      0 => array('id_producto_taller', 'id_producto'),
      1 => array('id_taller', 'id_taller'),
      2 => array('nombre_producto_taller', 'nombre_producto'),
      3 => array('desc_producto_taller', 'descricion_producto'),
      4 => array('estado_wrf_producto_taller', 'estado_publicacion_producto_taller'),
      5 => array('desc_wrf_estado_producto_taller', 'descripcion_estado_publicacion_taller'),
      6 => array('estado_producto_taller', 'estado'),
      7 => array('usuario_crea_producto_taller', 'usuario_creacion'),
      8 => array('fecha_crea_producto_taller', 'fecha_creacion'),
      9 => array('usuario_modifica_producto_taller', 'usuario_modificacion'),
      10 => array('fecha_modifica_producto_taller', 'fecha_modificacion'),
    );

    //The Source of the import
    $path = DRUPAL_ROOT . '/sites/default/files/migration/productos_taller.csv';

    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type directorio
    $this->destination = new MigrateDestinationNode('producto_artesano');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_producto_taller' => array('type' => 'varchar', 'length' => 255, 'description' => 'ID DEL PRODUCTO TALLER', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());

    $this->addFieldMapping('field_id_anterior_prod_artes', 'id_producto_taller');
    $this->addFieldMapping('title', 'nombre_producto_taller');
    $this->addFieldMapping('body', 'desc_producto_taller');
    $this->addFieldMapping('body:summary', 'body_summary');
    $this->addFieldMapping('body:format', 'body_format');
    $this->addFieldMapping('field_estado_producto', 'estado_wrf_producto_taller');
    $this->addFieldMapping('created', 'fecha_crea_producto_taller');
    $this->addFieldMapping('changed', 'fecha_modifica_producto_taller');
    $this->addFieldMapping('status', 'estado_producto_taller');
    $this->addFieldMapping('uid', 'usuario_crea_producto_taller');
    $this->addFieldMapping('field_categoria_del_producto', 'field_categoria_del_producto');
    $this->addFieldMapping('field_materiales_artesanales', 'field_materiales_artesanales');
    $this->addFieldMapping('field_oficio_artesanal', 'field_oficio_artesanal');
    $this->addFieldMapping('field_tecnicas_artesanales', 'field_tecnicas_artesanales');
    $this->addFieldMapping('field_id_taller_relacionado', 'id_taller');

    // Unmapped destination fields
    // Some conventions we use here: with a long list of fields to ignore, we
    // arrange them alphabetically, one distinct field per line (although
    // subfields of the same field may be grouped on the same line), and indent
    // subfields to distinguish them from top-level fields.
    $this->addUnmigratedDestinations(array(
      'is_new',
      'log',
    ));

  }

  // The prepareRow() method is called by the source class next() method, after loading the data row. The argument $row
  // is a stdClass object containing the raw data as provided by the source. There are two primary reasons to implement
  // prepareRow():
  // 1) To modify the data row before it passes through any further methods and handlers: for example, fetching related
  // data, splitting out source fields, combining or creating new source fields based on some logic.
  // 2) To conditionally skip a row (by returning FALSE).
  public function prepareRow($current_row) {
    // Always include this fragment at the beginning of every prepareRow()
    // implementation, so parent classes can ignore rows.

    if (parent::prepareRow($current_row) === FALSE) {
      return FALSE;
    }

    //Mapear Id del usuario artesano (portal viejo) a UID usuario artesano
    $current_row->usuario_crea_producto_taller = get_uid_usuario($current_row->usuario_crea_producto_taller);

    $current_row->id_producto_taller = str_replace(".", "", $current_row->id_producto_taller);
    $current_row->id_producto_taller = str_replace(",", "", $current_row->id_producto_taller);

    $source_id = $current_row->id_producto_taller;

    // Estado producto Taller
    if(isset($current_row->estado_producto_taller) && !empty($current_row->estado_producto_taller)){
      if(($current_row->estado_producto_taller == 'A') || ($current_row->estado_producto_taller == 'V')){
        $current_row->estado_producto_taller = 1;
      }elseif($current_row->estado_producto_taller == 'I'){
        $current_row->estado_producto_taller = 0;
      }
    }

    // Descripcion producto
    $current_row->body_format = "full_html"; // Filtered HTML
    $current_row->body_summary = $current_row->body_summary; // Body Summary
    $current_row->body_formatted = '';
    $current_row->body = '';

    if(!empty($current_row->desc_producto_taller)){
      $current_row->body_formatted = cleanHtmlEntities($current_row->desc_producto_taller);
      $current_row->body = $current_row->body_formatted;
    }

    //Fecha creacion producto taller
    if(!empty($current_row->fecha_crea_producto_taller) && ($current_row->fecha_crea_producto_taller != '#N/A')){
      $create_date = $current_row->fecha_crea_producto_taller;
      $current_row->fecha_crea_producto_taller = strtotime($create_date);
    }else{
      $current_row->fecha_crea_producto_taller = time();
    }

    // Fecha de Modificación producto taller
    if(!empty($current_row->fecha_modifica_producto_taller) && ($current_row->fecha_modifica_producto_taller != '#N/A')){
      $edit_date = $current_row->fecha_modifica_producto_taller;
      $current_row->fecha_modifica_producto_taller = strtotime($edit_date);
    }else{
      $current_row->fecha_modifica_producto_taller = time();
    }

    //Almacenar estado workflow
    if(isset($current_row->estado_wrf_producto_taller) && !empty($current_row->estado_wrf_producto_taller)){
      $retval = get_estado_publicacion_producto_taller($current_row->estado_wrf_producto_taller);

      if ($retval){
          $current_row->estado_wrf_producto_taller = $retval;
      } else {
          $current_row->estado_wrf_producto_taller = get_estado_publicacion_producto_taller('N');
      }
    }
    
    return TRUE;
  }
  /**
   * Runs before an import starts.
   * Code to execute before first article row is imported
   */
  public function preImport() {
    parent::preImport();
  }

  /*
   * The Migration class complete() method is analogous to prepare(), but is called immediately after the complete
   * destination object is saved.
   */
   public function prepare(stdClass $entity, stdClass $current_row) {

    //Mapear fecha creacion
    if(isset($current_row->fecha_crea_producto_taller) && !empty($current_row->fecha_crea_producto_taller)){
      $entity->created = $current_row->fecha_crea_producto_taller;
    }

    //Mapear fecha modificacion
    if(isset($current_row->fecha_modifica_producto_taller) && !empty($current_row->fecha_modifica_producto_taller)){
      $entity->changed = $current_row->fecha_modifica_producto_taller;
    }

    //Estado
    if(isset($current_row->estado) && !empty($current_row->estado)){
      $entity->status = $current_row->estado;
    }

    //Mapear ID taller
    if(isset($current_row->id_taller) && !empty($current_row->id_taller)){
      //$entity->field_id_taller_rel['und'][0]['target_id'] = $current_row->id_taller;
      //mirar si ID del taller esta asociado a un nid de taller ya existente
      $nid_taller = get_id_anterior_taller($current_row->id_taller);

      if ($nid_taller){
        $entity->field_id_taller_rel['und'][0]['target_id'] = $nid_taller;
      }
    }

    //leer csv oficio y obtener el ID de oficio
    //The Source of the import
    $path_oficios = DRUPAL_ROOT . '/sites/default/files/migration/productos_oficios.csv';

    //read csv file
    $file_oficios = fopen($path_oficios, 'r');

    while (($line_oficios = fgetcsv($file_oficios)) !== FALSE) {
      $id = $line_oficios[0];
      $id_producto = $line_oficios[1];
      $id_oficio = $line_oficios[2];

      $id_producto = str_replace(".", "", $id_producto);
      $id_producto = str_replace(",", "", $id_producto);

      if ($current_row->id_producto_taller == $id_producto){
        $entity->field_oficio_artesanal['und'][0]['tid'] = get_tid_oficio_artesanal($id_oficio);
        break;
      } else {
        $entity->field_oficio_artesanal['und'][0]['tid'] = get_tid_oficio_artesanal('Otros'); //Otros
      }
    }

    fclose($file_oficios); 

    //leer csv categoria del producto y obtener ID de categoria
    $path_lineas = DRUPAL_ROOT . '/sites/default/files/migration/linea.csv';

    //read csv file
    $file_lineas = fopen($path_lineas, 'r');

    while (($line_lineas = fgetcsv($file_lineas)) !== FALSE) {
      $id = $line_lineas[0];
      $id_producto = $line_lineas[1];
      $id_linea = $line_lineas[2];

      $id_producto = str_replace(".", "", $id_producto);
      $id_producto = str_replace(",", "", $id_producto);

      if ($current_row->id_producto_taller == $id_producto){
        $entity->field_categoria_del_producto['und'][0]['tid'] = get_tid_categoria_productos($id_linea);
        break;
      } else {
        $entity->field_categoria_del_producto['und'][0]['tid'] = get_tid_categoria_productos('Otros');
      }
    }

    //leer csv materias y obtener ID de material
    $path_materiales = DRUPAL_ROOT . '/sites/default/files/migration/materiales.csv';

    //read csv file
    $file_materiales = fopen($path_materiales, 'r');

    while (($line_materiales = fgetcsv($file_materiales)) !== FALSE) {
      $id = $line_materiales[0];
      $id_producto = $line_materiales[1];
      $id_material = $line_materiales[2];

      $id_producto = str_replace(".", "", $id_producto);
      $id_producto = str_replace(",", "", $id_producto);  

      if ($current_row->id_producto_taller == $id_producto){
        $entity->field_materiales_artesanales['und'][0]['tid'] = get_tid_materiales_artesanales($id_material);
        break;
      } else {
        $entity->field_materiales_artesanales['und'][0]['tid'] = get_tid_materiales_artesanales("Otros");
      }
    } 

    //leer csv tecnicas y obtener ID de material
    $path_tecnicas = DRUPAL_ROOT . '/sites/default/files/migration/tecnicas.csv';

    //read csv file
    $file_tecnicas = fopen($path_tecnicas, 'r');

    while (($line_tecnicas = fgetcsv($file_tecnicas)) !== FALSE) {
      $id = $line_tecnicas[0];
      $id_producto = $line_tecnicas[1];
      $id_tecnica = $line_tecnicas[2];

      $id_producto = str_replace(".", "", $id_producto);
      $id_producto = str_replace(",", "", $id_producto);

      if ($current_row->id_producto_taller == $id_producto){
        $entity->field_tecnicas_artesanales['und'][0]['tid'] = get_tid_tecnicas_artesanales($id_tecnica);
        break;
      } else {
        $entity->field_tecnicas_artesanales['und'][0]['tid'] = get_tid_tecnicas_artesanales("Otros"); 
      }
    }


    //workflow
    if(isset($current_row->estado_wrf_producto_taller) && !empty($current_row->estado_wrf_producto_taller)){
      $entity->field_estado_producto['und'][0]['value'] = $current_row->estado_wrf_producto_taller;
    }

   }

  /*
   * The Migration class complete() method is analogous to prepare(), but is called immediately after the complete
   * destination object is saved.
   */
   public function complete($entity, stdClass $current_row) {
   		$node = $entity;

   }

}
