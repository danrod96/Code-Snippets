<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

/*
 * @file
 * The ContratossNodeMigration uses the  contratos.csv files as source
 * and creates Drupal nodes of type 'contrato' as destination.
 */

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class ForosNodeMigration extends Migration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('CSV feed (pull) of Nodes tipo foro');
    // No mapear campo de contratante
    //
    $fields = array(
	    0  => array('id_foro', 'Id del foro'),
      1  => array('nombre_foro', 'Nombre del foro'),
	    2  => array('descripcion_foro', 'Descripción del foro'),
	    3  => array('estado', 'Estado A:Activo'),
      4  => array('fecha_pub', 'Fecha de publicación'),
      5  => array('fecha_ven', 'Fecha de vencimiento'),
      6  => array('usuario_crea', 'usuario crea'),
      7  => array('fecha_crea', 'fecha de creacion'),
      8  => array('usuario_modifica', 'usuario modifica'),
      9  => array('fecha_modificacion', 'fecha modifica'),
      10 => array('img_id', 'Anexo id'),
      11 => array('img_nombre', 'Nombre documento anexo'),
      12 => array('link_url', 'Link URL'),
      13 => array('anexo_id', 'Anexo documento'),
      14 => array('anexo_nombre', 'Nombre del documento'),
    );

    //The Source of the import
    $path = variable_get('foros_csv_path', 'public://migration/foros.csv');
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type contrato
    $this->destination = new MigrateDestinationNode('foro');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_foro' => array('type' => 'varchar', 'length' => 255, 'description' => 'Id del foro.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());


    $this->addFieldMapping('field_id_anterior', 'id_foro');
    $this->addFieldMapping('title', 'nombre_foro'); // Field Title
    $this->addFieldMapping('uid', 'author'); // Field Title
    $this->addFieldMapping('body', 'descripcion_foro');
    $this->addFieldMapping('body:format', 'body_format');
    $this->addFieldMapping('created', 'fecha_crea');
    $this->addFieldMapping('changed', 'fecha_modificacion');
    $this->addFieldMapping('status', 'estado');

    $this->addFieldMapping('field_image', 'imagen');
    $this->addFieldMapping('field_image:file_replace')->defaultValue(FILE_EXISTS_RENAME);
    $this->addFieldMapping('field_image:preserve_files')->defaultValue(TRUE);


    $this->addFieldMapping('field_archivo_foro', 'anexo');
    $this->addFieldMapping('field_archivo_foro:file_replace')->defaultValue(FILE_EXISTS_REPLACE);
    $this->addFieldMapping('field_archivo_foro:preserve_files')->defaultValue(TRUE);
    $this->addFieldMapping('field_archivo_foro:file_class')->defaultValue('MigrateFileUri');


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
    // Source ID
    $current_row->author = get_uid($current_row->usuario_crea);

    $current_row->body_format = "full_html";
    // Status
    if(isset($current_row->estado_contrato) && !empty($current_row->estado_contrato)){
      $current_row->estado_contrato = strtoupper($current_row->estado_contrato);
      if(($current_row->estado_contrato == 'A') || ($current_row->estado_contrato == 'V')){
        $current_row->estado_contrato = 1;
      }else{
        $current_row->estado_contrato = 0;
      }
    }

    // Fecha de Creación
    if(isset($current_row->fecha_crea) && !empty($current_row->fecha_crea)){
      $current_row->fecha_crea = strtotime($current_row->fecha_crea);
    }

    // Fecha de Modificación
    if(isset($current_row->fecha_modificacion) && !empty($current_row->fecha_modificacion)){
      $current_row->fecha_modificacion = strtotime($current_row->fecha_modificacion);
    }

    if (isset($current_row->img_nombre)  && !empty($current_row->img_nombre)){
        $current_row->imagen  = validate_file('imagenes',$current_row->img_nombre);
    }

    if (isset($current_row->anexo_nombre)  && !empty($current_row->anexo_nombre)){
       $current_row->anexo  = validate_file('pdf',$current_row->anexo_nombre);
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

   }

  /*
   * The Migration class complete() method is analogous to prepare(), but is called immediately after the complete
   * destination object is saved.
   */
   public function complete($entity, stdClass $current_row) {
   		$node = $entity;

   }

}
