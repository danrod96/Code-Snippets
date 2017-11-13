<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

/*
 * @file
 * The ClasificadosNodeMigration uses the  clasificados.csv files as source
 * and creates Drupal nodes of type 'clasificado' as destination.
 */

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class ClasificadosNodeMigration extends Migration {
	
  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of Nodes tipo clasificado');

    $fields = array(
	    0 => array('id_clasificado', 'Id de Clasificado'),
      1 => array('id_usuario_que_solicita_clasificado', 'Usuario que solicita el clasificado'),
	    2 => array('login_de_usuario', 'Login de usuario que solicita clasificado'),
	    3 => array('id_tipo_de_clasificado', 'ID tipo de clasificado'),
	    4 => array('nombre_clasificado', 'Nombre del clasificado'), // Es el mismo Tipo de Clasificado
      5 => array('titulo_clasificado', 'Titulo del clasificado'),
      6 => array('descripcion_clasificado', 'Descripción del clasificado'),
      7 => array('pais', 'Pais'),
      8 => array('departamento', 'Departamento'),
      9 => array('ciudad', 'Ciudad'),
      10 => array('direccion', 'Direccion'),
      11 => array('telefono', 'Telefono'),
      12 => array('movil', 'Movil'),
      13 => array('fax', 'Fax'),
      14 => array('correo_electronico', 'Correo electrónico'),
      15 => array('url', 'URL'),
      16 => array('fecha_inicio', 'Fecha de inicio'),
      17 => array('fecha_final', 'Fecha Final'),
      18 => array('anexo_id', 'Anexo ID'),
      19 => array('id_estado', 'Id del estado'),
      20 => array('nombre_estado_clasificado', 'Nombre del estado clasificado'),
      21 => array('estado', 'Estado A:Activo'),
      22 => array('usuario_crea', 'Usuario crea'),
      23 => array('fecha_creacion', 'Fecha de creacion'),
      24 => array('usuario_modifica', 'Usuario modifica'),
      25 => array('fecha_modificacion', 'Fecha de modificacion'),
      26 => array('etiquetas_clasificado', 'Palabras clave'),
    );

    //The Source of the import
    //$path = variable_get('clasificado_csv_path', 'public://migration/clasificado.csv');
    $path = file_create_url('public://migration/clasificado.csv');
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type clasificado
    $this->destination = new MigrateDestinationNode('clasificado');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_clasificado' => array('type' => 'varchar', 'length' => 255, 'description' => 'ID DEL DIRECTORIO.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());

    
    // Field id de clasificado <Texto>;
    $this->addFieldMapping('field_id_clasificado', 'id_clasificado');

    // Make the mappings
    // Field Titulo Clasificado <Texto>;
    $this->addFieldMapping('title', 'titulo_clasificado'); // Field Title 

    // Descripción 
    $this->addFieldMapping('body', 'descripcion_clasificado');
    $this->addFieldMapping('body:format', 'body_format');

    // Tipo de Clasificado
    $this->addFieldMapping('field_tipo_clasificado', 'nombre_clasificado'); // Tipo de Clasificado
    $this->addFieldMapping('field_tipo_clasificado:source_type')->defaultValue('tid');

    // Field Imágen
    $this->addFieldMapping('field_imagenes', 'images_files');
    $this->addFieldMapping('field_imagenes:file_replace')->defaultValue(FILE_EXISTS_RENAME);
    $this->addFieldMapping('field_imagenes:alt', 'images_alt');
    $this->addFieldMapping('field_imagenes:title', 'images_title');
    $this->addFieldMapping('field_imagenes:preserve_files')->defaultValue(TRUE);

    // Palabras Claves
    $this->addFieldMapping('field_etiquetas_clasificado', 'etiquetas_clasificado');
    $this->addFieldMapping('field_etiquetas_clasificado:create_term')
         ->defaultValue(TRUE);
    $this->addFieldMapping('field_etiquetas_clasificado:ignore_case')
         ->defaultValue(TRUE);

    // Estado del directorio 
    $this->addFieldMapping('field_estado_clasificado', 'nombre_estado_clasificado');

    // Field Created Date
    $this->addFieldMapping('created', 'fecha_creacion');

    // Field Changed Date
    $this->addFieldMapping('changed', 'fecha_modificacion');

    // Field Status
    $this->addFieldMapping('status', 'estado');
    
    // Field UID
    $this->addFieldMapping('uid', 'uid');

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
    if(!valid_email_address($current_row->login_de_usuario)){
      
      return FALSE;
    }else{
      $user = user_load_by_mail($current_row->login_de_usuario);
      if($user == FALSE){
        drupal_set_message('No encontro el correo:'.$current_row->id_clasificado); 
        return FALSE;
      }else{
        $current_row->uid = $user->uid; // User ID
      }
    }


    $current_row->body_format = "full_html";

    // Source ID
    $source_id = $current_row->id_clasificado;

    // Descripción
//    if(isset($current_row->descripcion_clasificado) && !empty($current_row->descripcion_clasificado)){
 //     $current_row->descripcion_clasificado = cleanHtmlEntities($current_row->descripcion_clasificado);
 //   }

    // Tipo de Clasificado
    if(isset($current_row->id_tipo_de_clasificado) && !empty($current_row->id_tipo_de_clasificado)){
      $current_row->nombre_clasificado = get_categoria_clasificado($current_row->id_tipo_de_clasificado);
    }

    // Etiquetas de Clasificado
    $tags = array();
    if(isset($current_row->etiquetas_clasificado) && !empty($current_row->etiquetas_clasificado)){
      $tags_items = explode(',', $current_row->etiquetas_clasificado);
      foreach ($tags_items as $key => $tag) {
        $item = trim($tag);
        if(!empty($item)){
          $tags[] = $item;
        }
      }
    }
    $current_row->etiquetas_clasificado = $tags;

    // Estado de Clasificado
    if(isset($current_row->id_estado) && !empty($current_row->id_estado)){
      $current_row->nombre_estado_clasificado = get_estado_de_publicacion_clasificado($current_row->id_estado);
    }

    // Status
    if(isset($current_row->estado) && !empty($current_row->estado)){
      $current_row->estado = strtoupper($current_row->estado);
      if(($current_row->estado == 'A') || ($current_row->estado == 'V')){
        $current_row->estado = 1;
      }else{
        $current_row->estado = 0;
      }
    }
    
    // Fecha de Creación
    if(isset($current_row->fecha_creacion) && !empty($current_row->fecha_creacion)){
      $current_row->fecha_creacion = strtotime($current_row->fecha_creacion);
    }

    // Fecha de Modificación
    if(isset($current_row->fecha_modificacion) && !empty($current_row->fecha_modificacion)){
      $current_row->fecha_modificacion = strtotime($current_row->fecha_modificacion);
    }
    
    // dpm($current_row);
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

   }

  /*
   * The Migration class complete() method is analogous to prepare(), but is called immediately after the complete
   * destination object is saved.
   */
   public function complete($entity, stdClass $current_row) {
   		$node = $entity;

   }

}
