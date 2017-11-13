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

class GlosarioNodeMigration extends Migration {
	
  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('CSV feed (pull) of taxonomy glosario');
    // No mapear campo de contratante
    // 
    $fields = array(
	    0  => array('id_glosario', 'ID de faq'),
      1  => array('palabra', 'Tema FAQ'),
      2  => array('sinonimos', 'opciones'),
      3  => array('fuente', 'Estado A:Activo'),
      4  => array('significado', 'fecha de creacion'),
    );

    //The Source of the import
    $path = variable_get('glosario_csv_path', 'public://migration/glosario.csv');
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type contrato
     $this->destination = new MigrateDestinationTerm('terminos_glosario');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_glosario' => array('type' => 'varchar', 'length' => 255, 'description' => 'Id del glosario.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());

    
    $this->addFieldMapping('name', 'palabra');
    $this->addFieldMapping('description', 'significado'); // Field Title 
    $this->addFieldMapping('format', 'body_format');

    $this->addFieldMapping('field_id_anterior', 'id_glosario');
    $this->addFieldMapping('field_fuente_de_informacion', 'fuente');
    $this->addFieldMapping('field_fuente_de_informacion:format', 'body_format');
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

    // Status

    $current_row->body_format = "full_html";
      
    //dpm($row, 'row');
    //dpm($choices, 'choices');


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
