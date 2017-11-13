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

class EncuestaNodeMigration extends Migration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = t('CSV feed (pull) of Nodes tipo faq');
    // No mapear campo de contratante
    //
    $fields = array(
	    0  => array('id_encuesta', 'ID de faq'),
      1  => array('pregunta', 'Tema FAQ'),
      2  => array('opciones', 'opciones'),
      3  => array('estado', 'Estado A:Activo'),
      4  => array('fecha_crea', 'fecha de creacion'),
      5  => array('fecha_modificacion', 'fecha modifica'),
      6  => array('fecha_pub', 'fecha modifica'),
      7  => array('fecha_ven', 'fecha modifica'),
    );

    //The Source of the import
    $path = variable_get('encuesta_csv_path', 'public://migration/encuestas.csv');
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type contrato
    $this->destination = new MigrateDestinationNode('poll');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_encuesta' => array('type' => 'varchar', 'length' => 255, 'description' => 'Id del faq.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());


    $this->addFieldMapping('field_id_anterior', 'id_encuenta');
    $this->addFieldMapping('title', 'pregunta'); // Field Title
    $this->addFieldMapping('created', 'fecha_crea');
    $this->addFieldMapping('changed', 'fecha_modificacion');
    $this->addFieldMapping('publish_on', 'fecha_pub');
    $this->addFieldMapping('unpublish_on', 'fecha_ven');
    $this->addFieldMapping('status', 'estado');
    $this->addFieldMapping('field_fecha_publicacion','fecha_pub');
    $this->addFieldMapping('runtime', 'seconds_to_run')->defaultValue(0);
    $this->addFieldMapping('active')->defaultValue(1);// Unmapped destination fields
    $this->addUnmigratedSources(array('active', 'runtime'));
    $this->addFieldMapping('choice', 'choice');
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

    if(isset($current_row->fecha_pub) && !empty($current_row->fecha_pub)){
      $current_row->fecha_pub               = strtotime($current_row->fecha_pub);
    }

    if(isset($current_row->fecha_ven) && !empty($current_row->fecha_ven)){
      $current_row->fecha_ven = strtotime($current_row->fecha_ven);
    }

    $current_row->choice = array();
    if (isset($current_row->opciones) && !empty($current_row->opciones)){
        $choices = explode("|", $current_row->opciones);
        foreach ($choices as $choice) {
          $current_row->choice[] = array(
            'chid' => 0,
            'chtext' => $choice,
            'chvotes' => 0,
            'weight' => 0,
          );
        }
    }

    $current_row->votes = array();

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
