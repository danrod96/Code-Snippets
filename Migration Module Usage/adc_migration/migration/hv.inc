<?php

error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('auto_detect_line_endings', TRUE);

/*
 * @file
 * The HvNodeMigration uses the  hoja_de_vida.csv files as source
 * and creates Drupal nodes of type 'hoja_de_vida' as destination.
 */

require_once dirname(__FILE__) . '/../adc_migration_helper.inc';

class HvNodeMigration extends Migration {
  
  public $debug = false;
  public $ignore_file = false;
  public $path_anexos = "";
  public $is_new = false;
  public $conteo_pdfs = 0;
  public $arreglo_pfs = array();

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->debug = variable_get('adc_debug_migration', FALSE);
    $this->description = t('CSV feed (pull) of Nodes tipo hoja_de_vida');

    $fields = array(
      0 => array('hojas_vida_id', 'hojas_vida_id'),
      1 => array('hojas_vida_nombre', 'hojas_vida_nombre'),
      2 => array('hojas_vida_aspiracion', 'hojas_vida_aspiracion'),
      3 => array('hojas_vida_anexo_id', 'hojas_vida_anexo_id'),
      //4 => array('nombre_fisico_del_archivo', 'Nombre fisico del archivo'), // Es el mismo Tipo de Clasificado
      4 => array('hojas_vida_activo', 'hojas_vida_activo'),
      5 => array('hojas_vida_usuario_id_new', 'hojas_vida_usuario_id_new'),
      6 => array('hojas_vida_fecha_new', 'hojas_vida_fecha_new'),
      7 => array('hojas_vida_usuario_id_add', 'hojas_vida_usuario_id_add'),
      8 => array('hojas_vida_fecha_add', 'hojas_vida_fecha_add'),
      9 => array('hojas_vida_fecha_estampado', 'hojas_vida_fecha_estampado'),
      10 => array('nombre_fisico_del_archivo', 'nombre_fisico_del_archivo'),
    );

    //The Source of the import
    //$path = variable_get('hojas_de_vidas_csv_path', 'public://migration/hojas_de_vidas.csv');
    $path = DRUPAL_ROOT . '/sites/default/files/migration/hv.csv';
    $this->path_anexos = DRUPAL_ROOT . '/sites/default/files/migration/anexos.csv';

    $this->source = new MigrateSourceCSV($path, $fields, array('header_rows' => 1));

    // Set up our destination - nodes of type hoja_de_vida
    $this->destination = new MigrateDestinationNode('hoja_de_vida');

    // The source ID here is the one retrieved from each data item in the XML
    // file, and used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName, array('hojas_vida_id' => array('type' => 'varchar', 'length' => 255, 'description' => 'ID DE LA HV.', 'not null' => TRUE)), MigrateDestinationNode::getKeySchema());
    
    // Field id de contrato <Texto>;
    $this->addFieldMapping('field_id_de_hv', 'hojas_vida_id');

    // Make the mappings
    // Field Titulo Clasificado <Texto>;
    $this->addFieldMapping('title', 'hojas_vida_nombre'); // Field Title 
    $this->addFieldMapping('field_primer_nombre', 'hojas_vida_nombre'); // Field Nombre 
    // $this->addFieldMapping('field_segundo_nombre', ''); // Field Segundo nombre 
    // $this->addFieldMapping('field_segundo_apellido', ''); // Field Segundo apellido
    
    // Descripción 
    $this->addFieldMapping('body', 'hojas_vida_aspiracion');

    // Tipo de Clasificado
    $this->addFieldMapping('field_tipo_vinculacion', 'nombre_clasificado'); // Tipo de Clasificado
    $this->addFieldMapping('field_tipo_clasificado:source_type')->defaultValue('tid');

    // Hoja de vida Fisica
    //$this->addFieldMapping('field_archivo_hv', 'nombre_fisico_del_archivo');
    //$this->addFieldMapping('field_archivo_hv:preserved_files')->defaultValue(TRUE);
    //$this->addFieldMapping('field_archivo_hv:file_replace')->defaultValue(MigrateFile::FILE_EXISTS_REUSE);
    //$this->addFieldMapping('field_archivo_hv:destination_dir')->defaultValue('sites/default/files/pdf');

    // Field Created Date
    $this->addFieldMapping('created', 'hojas_vida_fecha_add');

    // Field Changed Date
    $this->addFieldMapping('changed', 'hojas_vida_fecha_add');

    // Field Status
    $this->addFieldMapping('status', 'estado');

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

    $current_row->hojas_vida_id = str_replace(".", "", $current_row->hojas_vida_id);
    $current_row->hojas_vida_id = str_replace(",", "", $current_row->hojas_vida_id);

    // Source ID
    $source_id = $current_row->hojas_vida_id;

    //revisar si $row->contenido_id ya esta en la BD
    $result = db_query("SELECT entity_id FROM field_data_field_id_de_hv WHERE bundle = 'hoja_de_vida' AND field_id_de_hv_value = :id_anterior", array(":id_anterior" => $current_row->hojas_vida_id))->fetchAssoc();

    if (!empty($result['entity_id'])){
      drupal_set_message(t("El nodo ya existe, el id es @id y su nid es: @nid", array("@id" => $current_row->hojas_vida_id, "@nid" => $result['entity_id'])), "status");
      return FALSE;  
    } 

    // Descripción
    if(isset($current_row->hojas_vida_aspiracion) && !empty($current_row->hojas_vida_aspiracion)){
      $current_row->hojas_vida_aspiracion = cleanHtmlEntities($current_row->hojas_vida_aspiracion);
    }

    // Status
    $current_row->estado = 1;

    // Fecha de Modificación | Fecha de Creación
    if(isset($current_row->hojas_vida_fecha_add) && !empty($current_row->hojas_vida_fecha_add)){
      $current_row->hojas_vida_fecha_add = strtotime($current_row->hojas_vida_fecha_add);
    }
    
    // Files
    /* if( isset($current_row->nombre_fisico_del_archivo) && !empty($current_row->nombre_fisico_del_archivo) ){
      $file = file_create_url('public://pdf/'.$current_row->nombre_fisico_del_archivo);
       //$file = 'http://www.esinergia.co/adc/ftp/Documentos/' . $current_row->nombre_fisico_del_archivo; 
      $current_row->nombre_fisico_del_archivo = $file;
    }else{
      $current_row->nombre_fisico_del_archivo = '';
    } */

    $file_anexos = fopen($this->path_anexos, 'r');

    while (($line_anexos = fgetcsv($file_anexos)) !== FALSE) {
      $anexo_id = $line_anexos[0];
      $anexo_nombre = $line_anexos[1];

      if ($anexo_id == $current_row->hojas_vida_anexo_id){
        $current_row->nombre_fisico_del_archivo = $anexo_nombre;
        break;
      }
    }

    fclose($file_anexos);
    
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
    if (file_exists('public://pdf/'.$current_row->nombre_fisico_del_archivo)){
      if(isset($current_row->nombre_fisico_del_archivo) && !empty($current_row->nombre_fisico_del_archivo)){
        file_put_contents("/tmp/pdf.json", json_encode($current_row->nombre_fisico_del_archivo));
        $pdf = (object) array( 'uid' => 1, 
                               'uri' => 'public://pdf/'.$current_row->nombre_fisico_del_archivo, 
                               'filemime' => file_get_mimetype('public://pdf/'.$current_row->nombre_fisico_del_archivo), 
                               'fid' => null,
                               'status' => 1, );        
        $data = file_save($pdf);
           
        $entity->field_archivo_hv = array(LANGUAGE_NONE => array('0' => (array) $data));           
      }
      drupal_set_message(t("NUEVO NODO CREADO CON PDF @id y su nid es: @nid", array("@id" => $current_row->hojas_vida_id, "@nid" => $result['entity_id'])), "status");
    } else {
      drupal_set_message(t("NUEVO NODO CREADO SIN PDF @id y su nid es: @nid", array("@id" => $current_row->hojas_vida_id, "@nid" => $result['entity_id'])), "status");
      drupal_set_message(t("No existe el archivo @id", array("@id" => $current_row->nombre_fisico_del_archivo)), "status");      
    }
   }

  /*
   * The Migration class complete() method is analogous to prepare(), but is called immediately after the complete
   * destination object is saved.
   */
   public function complete($entity, stdClass $current_row) {
      //dpm($entity);
      $node = $entity;
   }

}
