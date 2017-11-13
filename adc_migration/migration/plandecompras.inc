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

class PlanDeComprasNodeMigration extends Migration {
	
  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of User tipo Artesanos');

    $fields = array(
      0 => array('id_plan_compras', 'ID DEL PLAN DE COMPRAS'),
      1 => array('anho_plan_compras', 'AÑO PLAN DE COMPRAS'),
      2 => array('desc_plan_compras', 'DESCRIPCION DEL PLAN DE COMPRAS'),
      3 => array('anexo_id_plan_compras', 'ANEXO ID PLAN DE COMPRAS'),
      4 => array('pdf_plan_compras', 'PDF PLAN DE COMPRAS'),
      5 => array('estado_plan_compras', 'ESTADO PLAN DE COMPRAS'),
      6 => array('usuario_crea_plan_compras', 'USUARIO CREACION PLAN DE COMPRAS'),
      7 => array('fecha_crea_plan_compras', 'FECHA CREACION PLAN DE COMPRAS'),
      8 => array('usuario_modifica_plan_compras', 'USUARIO MODIFICA PLAN DE COMPRAS'),
      9 => array('fecha_modifica_plan_compras', 'FECHA MODIFICA PLAN DE COMPRAS'),
    );

    //The Source of the import
    $path = file_create_url('public://migration/plandecompras.csv');
    //$path = variable_get('artesanos_csv_path', 'public://migration/plandecompras.csv');
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type directorio
    $this->destination = new MigrateDestinationNode('plan_de_compra');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id_plan_compras' => array('type' => 'varchar', 'length' => 255, 'description' => 'ID DEL PLAN DE COMPRAS', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());
    $this->addFieldMapping('title', 'anho_plan_compras');
    // Make the mappings
    $this->addFieldMapping('field_anio_plan_compra', 'anho_plan_compras');
    // Field Body <Texto largo y resumen>;
    $this->addFieldMapping('body', 'desc_plan_compras');
    $this->addFieldMapping('body:summary', 'body_summary');
    $this->addFieldMapping('body:format', 'body_format');
    $this->addFieldMapping('field_archivo_plan_compra', 'pdf_plan_compras');
    $this->addFieldMapping('field_archivo_plan_compra:file_replace')->defaultValue(FILE_EXISTS_RENAME);
    $this->addFieldMapping('field_archivo_plan_compra:preserve_files')->defaultValue(TRUE);
    $this->addFieldMapping('field_archivo_plan_compra:destination_dir')->defaultValue('sites/default/files/pdf');
    // Field Created Date
    $this->addFieldMapping('created', 'fecha_crea_plan_compras');

    // Field Changed Date
    $this->addFieldMapping('changed', 'fecha_modifica_plan_compras');

    // Field Status
    $this->addFieldMapping('status', 'estado_plan_compras');
    
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
    dpm($current_row);
    // Always include this fragment at the beginning of every prepareRow()
    // implementation, so parent classes can ignore rows.

    if (parent::prepareRow($current_row) === FALSE) {
      return FALSE;
    }

      // 1 => array('anho_plan_compras', 'AÑO PLAN DE COMPRAS'),
      // 2 => array('desc_plan_compras', 'DESCRIPCION DEL PLAN DE COMPRAS'),
      // 3 => array('anexo_id_plan_compras', 'ANEXO ID PLAN DE COMPRAS'),
      // 4 => array('pdf_plan_compras', 'PDF PLAN DE COMPRAS'),
      // 5 => array('estado_plan_compras', 'ESTADO PLAN DE COMPRAS'),
      // 6 => array('usuario_crea_plan_compras', 'USUARIO CREACION PLAN DE COMPRAS'),
      // 7 => array('fecha_crea_plan_compras', 'FECHA CREACION PLAN DE COMPRAS'),
      // 8 => array('usuario_modifica_plan_compras', 'USUARIO MODIFICA PLAN DE COMPRAS'),
      // 9 => array('fecha_modifica_plan_compras', 'FECHA MODIFICA PLAN DE COMPRAS'),
    // Source ID
    $source_id = $current_row->id_plan_compras;

    // Status
    if(isset($current_row->estado_plan_compras) && !empty($current_row->estado_plan_compras)){
      if(($current_row->estado_plan_compras == 'A') || ($current_row->estado_plan_compras == 'V')){
        $current_row->estado_plan_compras = 1;
      }elseif($current_row->estado_plan_compras == 'I'){
        $current_row->estado_plan_compras = 0;
      }
    }

    // Año del plan de compras
    if(isset($current_row->anho_plan_compras) && !empty($current_row->anho_plan_compras)){
      $current_row->anho_plan_compras =  str_replace(".", "", $current_row->anho_plan_compras);
      $current_row->anho_plan_compras =  str_replace(",", "", $current_row->anho_plan_compras);
    }
    // Body
    $current_row->body_format = "full_html"; // Filtered HTML
    $current_row->body_summary = $current_row->body_summary; // Body Summary
    $current_row->body_formatted = '';
    $current_row->body = '';
    if(!empty($current_row->body_text)){
      $current_row->body_formatted = cleanHtmlEntities($current_row->body_text);
      $current_row->body = $current_row->body_formatted;
    }    

    // Files
    if( isset($current_row->pdf_plan_compras) && !empty($current_row->pdf_plan_compras) ){
      $file = file_create_url('public://pdf/'.$current_row->pdf_plan_compras);
       //$file = 'http://www.esinergia.co/adc/ftp/Documentos/' . $current_row->pdf_plan_compras; 
      $current_row->pdf_plan_compras = $file;
    }else{
      $current_row->pdf_plan_compras = '';
    }
    // Fecha de Creación
    if(isset($current_row->fecha_crea_plan_compras) && !empty($current_row->fecha_crea_plan_compras)){
      $current_row->fecha_crea_plan_compras = strtotime($current_row->fecha_crea_plan_compras);
    }

    // Fecha de Modificación
    if(isset($current_row->fecha_modifica_plan_compras) && !empty($current_row->fecha_modifica_plan_compras)){
      $current_row->fecha_modifica_plan_compras = strtotime($current_row->fecha_modifica_plan_compras);
    }
    $current_row->title = "Plan de Compras ".$current_row->title;

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
    dpm($entity);
    dpm($current_row);
   		$node = $entity;


   }

}
