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

class GeografiaCodigoDepartamentoMigration extends Migration {
	
  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('Sync Codigo de Departamento');

    $fields = array(
      0 => array('codigo_departamento', 'Código de departamento'), 
	    1 => array('codigo_pais', 'Código de pais'),
      2 => array('nombre_departamento', 'Nombre del departamento')
    );

    //The Source of the import
    $path = variable_get('geografia_codigo_departamento_csv_path', 'public://migration/geografia_codigo_departamento.csv');
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    $destination_options = array(
      'allow_duplicate_terms' => FALSE,
    );

    // Set up our destination - nodes of type article
    $this->destination = new MigrateDestinationTerm('ubicacion', $destination_options);

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('codigo_departamento' => array('type' => 'varchar', 'length' => 255, 'description' => 'Código de pais.', 'not null' => TRUE)), MigrateDestinationTerm::getKeySchema());

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

    $codigo_pais = '';
    // Source Código de pais
    if(isset($current_row->codigo_pais) && !empty($current_row->codigo_pais)){
      $codigo_pais = $current_row->codigo_pais;
    }

    $codigo_departamento = '';
    // Codig de departamento
    if(isset($current_row->codigo_departamento) && !empty($current_row->codigo_departamento)){
      $codigo_departamento = $current_row->codigo_departamento;
    }

    $nombre_departamento = '';
    // Nombre de departamento
    if(isset($current_row->nombre_departamento) && !empty($current_row->nombre_departamento)){
      $nombre_departamento = $current_row->nombre_departamento;
    }

    if(!empty($codigo_pais) && $codigo_pais == '57' && !empty($nombre_departamento) && !empty($codigo_departamento)){

      $colombia_tid = 196;
      $terms = taxonomy_term_contains_name($nombre_departamento, $vid = 21, $colombia_tid);

      if(!empty($terms)){
        if(isset($terms[0]['tid'])){
          $tid = $terms[0]['tid'];
          $taxonomy_term = taxonomy_term_load($tid);
          if($taxonomy_term != FALSE){
            $taxonomy_term->field_codigo_de_departamento['und'][0]['value'] = $codigo_departamento;
            taxonomy_term_save($taxonomy_term); 
          }
        }
        dpm($terms);
        $departamentos_encontrados = variable_get('departamentos_encontrados', 0);
        $departamentos_encontrados = $departamentos_encontrados + 1;
        // variable_set('departamentos_encontrados', $departamentos_encontrados);
      }else{
        $departamentos_no_encontrados = variable_get('departamentos_no_encontrados', 0);
        $departamentos_no_encontrados = $departamentos_no_encontrados + 1;
        // variable_set('departamentos_no_encontrados', $departamentos_no_encontrados);
      }
    }
    
    return FALSE;
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
   public function complete($entity, stdClass $row) {
   		$node = $entity;

   }

}
