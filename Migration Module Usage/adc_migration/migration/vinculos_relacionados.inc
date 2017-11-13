<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

/*
 * @file
 * The PublicacionesToNodeMigration uses the  NOTICIAS.CSV files as source
 * and creates Drupal nodes of type 'article' as destination.
 */

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class VinculoRelacionadoNodeMigration extends Migration {

  public $debug = false;
  public $ignore_file = false;

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('etc_debug_migration', FALSE);
    $this->description = t('CSV update estado');

    $fields = array(
      0 => array('id', 'Id'),
      1 => array('contenido_id', 'Id Contenido'),
      2 => array('nombre_link', 'texto a mostrar'),
      3 => array('mouse_over', 'texto a mouse over'),
      4 => array('url', 'url'),
    );
    
   $_SESSION['procesados'] = 0;
   $_SESSION['noencontrado'] = 0;

    //The Source of the import
    $path = file_create_url('public://migration/vinculos-relacionados.csv');
    //$path = variable_get('body_csv_path', 'public://migration/contenido-body.csv');
    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type page
    $this->destination = new MigrateDestinationNode('noticia');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('id' => array('type' => 'varchar', 'length' => 255, 'description' => 'Contenido ID.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());

    // Field Title <Texto>;
    $this->addFieldMapping('title', 'title'); // Field Title

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

    // Building Node Informatión From $row Data.
    
    
    $node = get_node_by_id_contenido($row->contenido_id);


   if($node != FALSE){
      if (isset($node->field_vinculos_relacionados['und'])){
          $cont = count($node->field_vinculos_relacionados['und']);
      }
      else{
          $cont = 0;
      }
      $node->field_vinculos_relacionados['und'][$cont] = array('url'=>$row->url,
                                                                   'title'=>$row->nombre_link,
                                                                   'attributes'=>array('title'=>$row->mouse_over));
      node_save($node);
      $_SESSION['procesados']++;

   }
   else {
    $_SESSION['noencontrado']++;
     drupal_set_message('Nodo no encontrado '.$row->contenido_id.' '.$_SESSION['noencontrado'].'-'.$_SESSION['procesados']);
   }

    return FALSE;

  }

}
