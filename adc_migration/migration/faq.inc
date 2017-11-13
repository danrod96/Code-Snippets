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

class FaqNodeMigration extends Migration {
	
  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('CSV feed (pull) of Nodes tipo faq');
    // No mapear campo de contratante
    // 
    $fields = array(
	    0  => array('id_faq', 'ID de faq'),
      1  => array('id_tema', 'Tema FAQ'),
	    2  => array('pregunta', 'Pregunta'),
	    3  => array('respuesta', 'Respuesta'),
      4  => array('estado', 'Estado A:Activo'),
      5  => array('usuario_crea', 'usuario crea'),
      6  => array('fecha_crea', 'fecha de creacion'),
      7  => array('usuario_modifica', 'usuario modifica'),
      8  => array('fecha_modificacion', 'fecha modifica'),
    );

    //The Source of the import
    $path = variable_get('faqs_csv_path', 'public://migration/faqs.csv');
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type contrato
    $this->destination = new MigrateDestinationNode('pregunta_frecuente');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_faq' => array('type' => 'varchar', 'length' => 255, 'description' => 'Id del faq.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());

    
    $this->addFieldMapping('field_id_anterior', 'id_faq');
    $this->addFieldMapping('title', 'pregunta'); // Field Title 
    $this->addFieldMapping('body', 'respuesta');
    $this->addFieldMapping('body:format', 'body_format');
    $this->addFieldMapping('created', 'fecha_crea');
    $this->addFieldMapping('changed', 'fecha_modificacion');
    $this->addFieldMapping('status', 'estado');
    $this->addFieldMapping('field_tema_preg_frecuente', 'tema');
    $this->addFieldMapping('field_tema_preg_frecuente:source_type')->defaultValue('tid');

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

    $current_row->body_format = "full_html";
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
    if(isset($current_row->fecha_crea) && !empty($current_row->fecha_crea)){
      $current_row->fecha_crea = strtotime($current_row->fecha_crea);
    }

    // Fecha de Modificación
    if(isset($current_row->fecha_modificacion) && !empty($current_row->fecha_modificacion)){
      $current_row->fecha_modificacion = strtotime($current_row->fecha_modificacion);
    }
    
    if(isset($current_row->id_tema) && !empty($current_row->id_tema)){
      $current_row->tema = get_tema_faq($current_row->id_tema);
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
